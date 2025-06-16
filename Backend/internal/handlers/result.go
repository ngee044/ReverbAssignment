package handlers

import (
	"net/http"
	"os"
	"path/filepath"
	"reverb/backend-gin/internal/jobs"

	"github.com/gin-gonic/gin"
)

func GetResult(c *gin.Context) {
	id := c.Param("id")
	j, ok := jobs.GetJob(id)
	if !ok {
		c.JSON(http.StatusNotFound, gin.H{"error": "not found"})
		return
	}
	if !j.Done {
		c.Status(http.StatusTooEarly) // 425
		return
	}
	if _, err := os.Stat(j.OutPath); err != nil {
		c.JSON(500, gin.H{"error": "result file not found"})
		return
	}
	c.FileAttachment(j.OutPath, filepath.Base(j.OutPath))
}
