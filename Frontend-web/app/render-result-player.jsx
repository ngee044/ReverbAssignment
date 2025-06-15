"use client";

import React, { useState, useRef } from "react";
import { Button } from "@/components/ui/button";

export default function RenderResultPlayer({ jobId }) {
  const [audioUrl, setAudioUrl] = useState(null);
  const audioRef = useRef();

  async function handleDownloadAndPlay() {
    try {
      const res = await fetch(`/api/render/${jobId}/result`);
      if (!res.ok) throw new Error("결과 다운로드 실패");

      const blob = await res.blob();
      const url = URL.createObjectURL(blob);

      setAudioUrl(url);

      if (audioRef.current) {
        audioRef.current.load();
        audioRef.current.play();
      }
    } catch (error) {
      alert(error.message);
    }
  }

  return (
    <div className="space-y-4 mt-4">
      <Button className="w-full h-12" onClick={handleDownloadAndPlay} disabled={!jobId}>
        결과 듣기 & 다운로드
      </Button>

      
        <audio
          ref={audioRef}
          controls
          src={audioUrl}
          disabled={!audioUrl}
          className="w-full rounded-md border"
        />
      
    </div>
  );
}
