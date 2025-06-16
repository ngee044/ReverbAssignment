"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";
import { UploadCloud } from "lucide-react";
import ReverbSlider from "./reverb-slider";
import RenderProgress from "./render-progress";
import { postRenderApi } from "./api";
import RenderResultPlayer from "./render-result-player";

export default function AudioUploader() {
  const [files, setFiles] = useState([]);
  const [reverbParams, setReverbParams] = useState({
    roomSize: 50,
    damping: 50,
    wetLevel: 50,
    dryLevel: 40,
    width: 50,
  });
  const [jobId, setJobId] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleFiles = (fileList) => {
    if (!fileList) return;
    const newFiles = [];

    Array.from(fileList).forEach((file) => {
      if (file.type === "audio/mpeg" || file.type === "audio/wav") {
        newFiles.push({
          file,
          url: URL.createObjectURL(file),
        });
      } else {
        alert(`지원하지 않는 파일 형식입니다: ${file.name}`);
      }
    });

    setFiles((prev) => [...prev, ...newFiles]);
  };

  const onFileInputChange = (e) => {
    handleFiles(Array.from(e.target.files));
  };

  const onDrop = (e) => {
    e.preventDefault();
    handleFiles(e.dataTransfer.files);
  };

  const handleRender = async () => {
    if (files.length === 0) {
      alert("파일을 선택해주세요.");
      return;
    }

    setLoading(true);
    setError(null);

    try {
      const job_id = await postRenderApi(
        files.map((f) => f.file),
        reverbParams
      );
      setJobId(job_id);
    } catch (e) {
      setError(e.message);
    } finally {
      setLoading(false);
    }
  };

  // 렌더 작업 완료 시 호출해서 loading 해제
  const handleRenderComplete = () => {
    setLoading(false);
  };

  return (
    <div className="w-full max-w-md">
      <div
        onDrop={onDrop}
        onDragOver={(e) => e.preventDefault()}
        className="border-2 border-dashed border-gray-300 rounded-xl p-6 text-center bg-white"
      >
        <UploadCloud className="w-10 h-10 mx-auto mb-2 text-gray-500" />
        <p className="text-gray-500 text-sm">MP3/WAV 파일을 드래그하거나 클릭하여 업로드</p>

        <input
          id="audio-uploader"
          type="file"
          accept=".mp3,.wav"
          multiple
          onChange={onFileInputChange}
          className="hidden"
        />
        <label htmlFor="audio-uploader">
          <Button className="mt-4 cursor-pointer" onClick={() => document.getElementById("audio-uploader").click()}>파일 선택</Button>
        </label>
      </div>

      <ul className="mt-6 space-y-4">
        {files.map((audio, index) => (
          <li key={index} className="border rounded-lg p-2 bg-white shadow-sm">
            <p className="text-sm truncate">
              {audio.file.name} ({(audio.file.size / 1024).toFixed(1)} KB)
            </p>
            <audio controls src={audio.url} className="w-full mt-1" />
          </li>
        ))}
      </ul>
      <ReverbSlider
        params={reverbParams}
        onChange={(newParams) => {
          setReverbParams(newParams);
          console.log("Reverb 파라미터:", newParams);
        }}
      />
      <div className="mt-6 w-full">
        <Button className="w-full h-12 cursor-pointer" onClick={handleRender} disabled={loading || files.length === 0}>
          {loading ? "작업 중..." : "리버브 적용 렌더링 시작"}
        </Button>
      </div>

      {error && <p className="mt-2 text-red-600">에러: {error}</p>}

      <RenderProgress jobId={jobId} onComplete={handleRenderComplete} />

      <RenderResultPlayer jobId={jobId} />

    </div>
  );
}
