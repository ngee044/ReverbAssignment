package handlers

import (
	"net/http"
	"reverb/backend-gin/internal/jobs"

	"github.com/gin-gonic/gin"
)

func PostRender(c *gin.Context) {
	jobID, err := jobs.StartJob(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}
	c.JSON(http.StatusOK, gin.H{"job_id": jobID})
}
