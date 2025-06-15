# Audio Reverb Renderer

React 기반 Next.js 13 (app directory) 프로젝트로, MP3/WAV 파일 업로드 후 리버브(Reverb) 효과 파라미터를 설정하여 서버에 렌더링 요청을 보내고, 처리된 오디오를 실시간 진행률과 함께 확인 및 다운로드할 수 있습니다.

---

## 프로젝트 설치 및 실행

1. 저장소 클론

```bash
git clone <your-repo-url>
cd <your-repo-folder>
```
2. 패키지 설치
```bash
npm install
# 또는
yarn
# 또는
pnpm install
# 또는
bun install
```
3. 개발 서버 실행
```bash
npm run dev
# 또는
yarn dev
# 또는
pnpm dev
# 또는
bun dev

```
4. 브라우저에서 열기: http://localhost:3000

## 주요 기능
- MP3, WAV 파일 업로드 지원

- 리버브 효과 파라미터 실시간 슬라이더 설정 (RoomSize, Damping, WetLevel, DryLevel, Width)

- 업로드 및 렌더링 진행률 실시간 표시 (shadcn/ui Progress 컴포넌트 사용)

- 렌더링 완료 후 오디오 재생 및 다운로드 기능 제공

## 사용 기술 및 라이브러리
- Next.js 13 (app directory 기반)

- React (Client Component)

- shadcn/ui (Button, Slider, Progress 등 UI 컴포넌트)

- Fetch API를 이용한 REST API 통신 (POST /api/render, GET /api/render/{job_id}, GET /api/render/{job_id}/result)

## 프로젝트 구조
- app/ : Next.js 페이지 및 레이아웃

- components/ : UI 컴포넌트 모음 (AudioUploader, ReverbSlider, RenderResultPlayer 등)

- lib/api.js : API 요청 함수 모음

## 참고 자료
- Next.js 공식 문서

- shadcn/ui GitHub

