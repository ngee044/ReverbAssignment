package handlers

import (
	"errors"
	"net/http"
	"os"
	"path/filepath"
	"reverb/backend-gin/internal/jobs"
	"reverb/backend-gin/internal/util"

	"github.com/gin-gonic/gin"
)

func GetResult(c *gin.Context) {
	id := c.Param("id")
	job, ok := jobs.GetJob(id)
	if !ok || !job.Done {
		c.JSON(http.StatusNotFound, gin.H{"error": "not found"})
		return
	}
	if job.Err != "" {
		c.JSON(http.StatusConflict, gin.H{"error": job.Err})
		return
	}

	outDir := filepath.Join("tmp", "reverb", id, "out")
	wavs, _ := filepath.Glob(filepath.Join(outDir, "*.wav"))

	switch len(wavs) {
	case 1:
		file := wavs[0]

		f, err := os.Open(file)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": err.Error()})
			return
		}
		defer f.Close()

		c.Header("Content-Type", "audio/wav")
		c.Header("Content-Disposition", `inline; filename="`+filepath.Base(file)+`"`)
		c.Header("Access-Control-Expose-Headers", "Content-Disposition")

		stat, _ := f.Stat()
		http.ServeContent(c.Writer, c.Request, file, stat.ModTime(), f)
		return

	default:
		zipPath := filepath.Join("tmp", "reverb", id, id+"_result.zip")
		if _, err := os.Stat(zipPath); errors.Is(err, os.ErrNotExist) {
			if err := util.ZipFolder(outDir, zipPath); err != nil {
				c.JSON(http.StatusInternalServerError, gin.H{"error": err.Error()})
				return
			}
		}

		c.Header("Access-Control-Expose-Headers", "Content-Disposition")
		c.FileAttachment(zipPath, filepath.Base(zipPath))
	}
}
