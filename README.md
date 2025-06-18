# 🎛️ Reverb Assignment

> Drag-and-drop an audio file → tweak Reverb → get a processed WAV in a browser **or** a cross-platform Qt desktop app.

Frontend (Qt, React) ⇆ Go API (Gin) ⇆ CLI DSP (JUCE ReverbApp)

---
## 🚀 Quick Build & Run
### 1.Build CPP Project
```
mkdir build
cd build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="${vcpkg}"
cmake --build build --config Release     # → build/out/Release/ReverbApp
```
Automatic creation when using VSCode (with CPP Extension Pack installed)

### 2.Start the Go API server
```
cd backend
go mod tidy
go build -o ../build/out/Release/backend
```
The backend service execution path and ReverbApp path exist in the same path. Or, add ReverbApp to the environment variable

### 3. Launch the React web-client
```
cd Frontend-web
npm install
npm run dev
```

## 🌐 REST API
| Endpoint                          | Purpose           | Request                                                                                                   | Success 200                                                               | Errors                      |
| --------------------------------- | ----------------- | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------- | --------------------------- |
| **POST `/api/render`**            | enqueue job       | multipart:<br>• `files[]` WAV/MP3<br>• `params` JSON `{roomSize,damping,wetLevel,dryLevel,width}` (0-100) | `{ "job_id": "uuid" }`                                                    | 400 bad form<br>413 too big |
| **GET `/api/render/{id}`**        | poll progress     | —                                                                                                         | `{ "progress": 0-100 }` (`-1` = failed)                                   | 404                         |
| **GET `/api/render/{id}/result`** | download / stream | —                                                                                                         | 1 file → `audio/wav` *inline*<br>N files → `application/zip` *attachment* | 404 not ready<br>409 failed |

### Slider ↔︎ JUCE Mapping
| Field      | `juce::dsp::Reverb::Parameters` |
| ---------- | ------------------------------- |
| `roomSize` | `roomSize = v / 100.f`          |
| `damping`  | `damping  = v / 100.f`          |
| `wetLevel` | `wetLevel = v / 100.f`          |
| `dryLevel` | `dryLevel = v / 100.f`          |
| `width`    | `width    = v / 100.f`          |


## 🔧 Config
| Env Var      | Default           | Description         |
| ------------ | ----------------- | --------------------|
| `REVERB_BIN` | `./bin/ReverbApp` | DSP executable      |
| `TEMP_ROOT`  | `./tmp/reverb`    | per-job scratch     |
| `THREADS`    | `2 ~ 5`           | ReverbApp pool size |
| `PORT`       | `8080`            | API listen port     |

## Performance Profiling Report
* PC Hardware: intel i7-10700K 3.80GHz (16CPU), RAM 32GB, GTX 2060 super, SSD
  
### 1. ReverbApp
* input sample: 120s, 24bit 44.1kHz stero WAV (≈30MB)
* thread pool count: min thread = 2, max trhead = 5

| Files | Batch time (ms)    | Avg / file (ms)    | Min | Max |   Through-put (files / s)  |
| ----: | -----------------: | -----------------: | --: | --: | -------------------------: |
|     1 |                427 |                421 | 421 | 421 |                       2.3  |
|     2 |                455 |                433 | 429 | 436 |                       4.4  |
|     5 |                573 |                548 | 536 | 557 |                       8.7  |
|    10 |              1,010 |                470 | 435 | 510 |                   **9.9**  |

### 2. API-layer
* localhost
* backend: go lang(gin)
* frontend: qt, react
| Request Files | POST `/api/render` | GET `/…/result` | API 오버헤드 합계 |
| :------------ | -----------------: | --------------: | ----------: |
| 1             |             118 ms |          112 ms |      230 ms |
| 2             |             284 ms |        1 844 ms |    2 128 ms |
| 5             |             770 ms |        4 458 ms |    5 228 ms |
| 10            |           1 627 ms |        9 067 ms |   10 694 ms |



