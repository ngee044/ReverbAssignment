package jobs

import (
	"sync"
)

type ReverbParams struct {
	RoomSize float64 `json:"roomSize"`
	Damping  float64 `json:"damping"`
	WetLevel float64 `json:"wetLevel"`
	DryLevel float64 `json:"dryLevel"`
	Width    float64 `json:"width"`
}

type Job struct {
	Progress int    `json:"progress"`
	Done     bool   `json:"done"`
	OutPath  string `json:"-"`
	Err      string `json:"error,omitempty"`
}

var (
	workDir = "./tmp/reverb"
	mu      sync.RWMutex
	table   = make(map[string]*Job)
)

func newJob(id, out string) *Job {
	j := &Job{Progress: 0, Done: false, OutPath: out}
	mu.Lock()
	table[id] = j
	mu.Unlock()
	return j
}

func GetJob(id string) (*Job, bool) {
	mu.RLock()
	j, ok := table[id]
	mu.RUnlock()
	return j, ok
}

func updateProgress(id string, pct int) {
	mu.Lock()
	if j, ok := table[id]; ok {
		j.Progress = pct
	}
	mu.Unlock()
}

func markDone(id string, errStr string) {
	mu.Lock()
	if j, ok := table[id]; ok {
		j.Progress = 100
		j.Done = true
		j.Err = errStr
	}
	mu.Unlock()
}
