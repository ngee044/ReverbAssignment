
const API_BASE = "http://localhost:8080"; 

export async function postRenderApi(files, reverbParams) {
    const formData = new FormData();

    files.forEach((item, idx) => {
      const file = item instanceof File ? item : item.file || item.originFileObj;

      if (file instanceof File) {
        formData.append("files", file, file.name);
      } else {
        console.warn("skip non-file item", idx, item);
      }
    });

    formData.append("params", JSON.stringify(reverbParams));

    const res = await fetch(`${API_BASE}/api/render`, { method: "POST", body: formData });
    if (!res.ok) throw new Error(await res.text());
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
  
    const { progress } = await res.json();
    return progress ?? 0;
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
    const match = cd.match(/filename="?(.+?)"?$/);
    const filename = match ? match[1] : `result_${job_id}.wav`;

    const blob = await res.blob();
    return { blob, filename };
  }
  