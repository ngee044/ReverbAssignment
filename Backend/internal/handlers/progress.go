package handlers

import (
	"net/http"
	"reverb/backend-gin/internal/jobs"

	"github.com/gin-gonic/gin"
)

type progressRes struct {
	Progress int `json:"progress"`
}

func GetProgress(c *gin.Context) {
	id := c.Param("id")

	if j, ok := jobs.GetJob(id); ok {
		c.JSON(http.StatusOK, progressRes{Progress: j.Progress})
		return
	}

	c.JSON(http.StatusNotFound, gin.H{"error": "not found"})
}
