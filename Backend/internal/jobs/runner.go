package jobs

import (
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"mime/multipart"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/gin-gonic/gin"
	"github.com/google/uuid"
)

func StartJob(c *gin.Context) (string, error) {
	form, err := c.MultipartForm()
	if err != nil {
		return "", err
	}

	files := form.File["files"]

	if len(files) == 0 {
		files = form.File["files[]"]
	}

	if len(files) == 0 {
		return "", errors.New("files field missing")
	}

	var p ReverbParams
	if err := json.Unmarshal([]byte(form.Value["params"][0]), &p); err != nil {
		return "", fmt.Errorf("invalid params: %w", err)
	}

	id := uuid.NewString()
	inDir := filepath.Join(workDir, id, "in")
	outDir := filepath.Join(workDir, id, "out")
	_ = os.MkdirAll(inDir, 0755)
	_ = os.MkdirAll(outDir, 0755)

	var savedPaths []string
	for i, fh := range files {
		ext := strings.ToLower(filepath.Ext(fh.Filename))
		base := id
		if len(files) > 1 {
			base = fmt.Sprintf("%s_%d", id, i+1)
		}
		dst := filepath.Join(inDir, base+ext)
		if err := saveFile(fh, dst); err != nil {
			return "", err
		}
		savedPaths = append(savedPaths, dst)
	}

	if len(savedPaths) == 1 {
		outFile := filepath.Join(outDir, id+".wav")
		newJob(id, outFile)
		go runSingle(id, savedPaths[0], outFile, p)
	} else {
		newJob(id, outDir)
		go runBatch(id, inDir, outDir, p)
	}

	return id, nil
}

func runSingle(id, inFile, outFile string, p ReverbParams) {
	args := []string{
		"--input_file", inFile, "--output_file", outFile,
		"--room", f(p.RoomSize), "--damp", f(p.Damping),
		"--wet", f(p.WetLevel), "--dry", f(p.DryLevel), "--width", f(p.Width),
	}
	execAndTrack(id, args)
}

func runBatch(id, inDir, outDir string, p ReverbParams) {
	args := []string{
		"--input_folder", inDir, "--output_folder", outDir,
		"--room", f(p.RoomSize), "--damp", f(p.Damping),
		"--wet", f(p.WetLevel), "--dry", f(p.DryLevel), "--width", f(p.Width),
	}
	execAndTrack(id, args)
}

func execAndTrack(id string, args []string) {
	cmd := exec.Command("./ReverbApp", args...)
	_ = cmd.Start()

	go parseProgress(id)

	var errStr string
	err := cmd.Wait()
	if err != nil {
		errStr = err.Error()
	}

	markDone(id, errStr)
}

func parseProgress(id string) {
	inDir := filepath.Join("tmp", "reverb", id, "in")
	outDir := filepath.Join("tmp", "reverb", id, "out")

	inFiles, _ := filepath.Glob(filepath.Join(inDir, "*"))
	outFiles, _ := filepath.Glob(filepath.Join(outDir, "*.wav"))

	total := len(inFiles)
	if total == 0 {
		updateProgress(id, 0)
		return
	}

	done := len(outFiles)
	pct := int(float64(done) / float64(total) * 100)
	if pct > 100 {
		pct = 100
	}

	updateProgress(id, pct)
}

func saveFile(fh *multipart.FileHeader, dst string) error {
	src, err := fh.Open()
	if err != nil {
		return err
	}
	defer src.Close()
	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer out.Close()
	_, err = io.Copy(out, src)
	return err
}

func f(x float64) string { return strconv.FormatFloat(x, 'f', -1, 64) }
