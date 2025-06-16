package handlers

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

// Healthz returns 200 OK for load-balancer / k8s probe
func Healthz(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{"status": "ok"})
}
