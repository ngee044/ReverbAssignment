// 1. POST /api/render - 파일 + 파라미터 전송 후 job_id 반환
export async function postRenderApi(files, reverbParams) {
    const formData = new FormData();
  
    files.forEach((file) => {
      formData.append("files", file);
    });
  
    formData.append("params", JSON.stringify(reverbParams));
  
    const res = await fetch("/api/render", {
      method: "POST",
      body: formData,
    });
  
    if (!res.ok) {
      throw new Error("API 요청 실패");
    }
  
    const data = await res.json();
    return data.job_id;
  }
  
  // 2. GET /api/render/{job_id} - 진행률 조회
  export async function getRenderProgress(job_id) {
    const res = await fetch(`/api/render/${job_id}`, {
      method: "GET",
    });
  
    if (!res.ok) {
      throw new Error("진행률 조회 실패");
    }
  
    const data = await res.json();
    // 예: { progress: 0~100 } 반환 예상
    return data.progress;
  }
  
  // 3. GET /api/render/{job_id}/result - 처리된 WAV 다운로드
  export async function downloadRenderResult(job_id) {
    const res = await fetch(`/api/render/${job_id}/result`, {
      method: "GET",
    });
  
    if (!res.ok) {
      throw new Error("결과 다운로드 실패");
    }
  
    // 바이너리 Blob으로 받기
    const blob = await res.blob();
  
    // 다운로드 링크 생성
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = `render_result_${job_id}.wav`;
    document.body.appendChild(a);
    a.click();
    a.remove();
    window.URL.revokeObjectURL(url);
  }
  