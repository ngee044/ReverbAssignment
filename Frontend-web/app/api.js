
const API_BASE = process.env.NEXT_PUBLIC_API_BASE || "http://localhost:8080"; 

export async function postRenderApi(files, reverbParams) {
    const formData = new FormData();

    files.forEach((item, idx) => {
      let file = item;

      if (!(file instanceof File) && item.originFileObj) {
        file = item.originFileObj;
      }

      if (file instanceof File) {
        formData.append("files", file, file.name);
      } else {
        console.warn("skip non-file item", idx, item);
      }
    });

    formData.append("params", JSON.stringify(reverbParams));

    const res = await fetch("/api/render", { method: "POST", body: formData });

    if (!res.ok) {
      const msg = await res.text();
      throw new Error(`API 요청 실패: ${res.status} / ${msg}`);
    }
    return (await res.json()).job_id;
  }
  
  // 2. GET /api/render/{job_id} - 진행률 조회
  export async function getRenderProgress(job_id) {
    const res = await fetch(`${API_BASE}/api/render/${job_id}`, {
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
    const res = await fetch(`${API_BASE}/api/render/${job_id}/result`, {
      method: "GET",
    });
  
    if (!res.ok) {
      throw new Error("결과 다운로드 실패");
    }

    const cd = res.headers.get("contnent-disposition") || "";
    const filename =- cd.match(/filename="(.+?)"/)?.[1] || `result_${job_id}.wav`;
    const blob = await res.blob();
  
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = filename;
    // document.body.appendChild(a);
    a.click();
    a.remove();
    window.URL.revokeObjectURL(url);
    
    return filename;
  }
  