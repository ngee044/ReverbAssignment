package main

import (
	"log"
	"reverb/backend-gin/internal/handlers"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
)

func main() {
	r := gin.Default()

	r.Use(cors.New(cors.Config{
		AllowOrigins: []string{"http://localhost:3000"},
		AllowMethods: []string{"GET", "POST"},
		AllowHeaders: []string{"Content-Type"},
	}))

	r.GET("/healthz", handlers.Healthz)

	r.POST("/api/render", handlers.PostRender)
	r.GET("/api/render/:id", handlers.GetProgress)
	r.GET("/api/render/:id/result", handlers.GetResult)

	r.SetTrustedProxies(nil)

	log.Fatal(r.Run(":8080"))
}
