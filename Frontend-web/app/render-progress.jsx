"use client";

import { useState, useEffect, useRef } from "react";
import { Button } from "@/components/ui/button";
import { downloadRenderResult, getRenderProgress } from "./api";
import { Progress } from "@/components/ui/progress";

export default function RenderProgress({ jobId, onComplete }) {
  const [progress, setProgress] = useState(12);
  const [error, setError] = useState(null);
  const intervalRef = useRef(null);

  useEffect(() => {
    if (!jobId) return;

    intervalRef.current = setInterval(async () => {
      try {
        const prog = await getRenderProgress(jobId);
        setProgress(prog);

        if (prog >= 100) {
          clearInterval(intervalRef.current);
          intervalRef.current = null;
          if (onComplete) onComplete();
        }
      } catch (e) {
        clearInterval(intervalRef.current);
        intervalRef.current = null;
        setError("진행률 조회 중 오류 발생");
      }
    }, 2000);

    return () => {
      if (intervalRef.current) clearInterval(intervalRef.current);
    };
  }, [jobId, onComplete]);

  if (!jobId) return null;

  return (
    <div className="mt-4">
      {error && <p className="text-red-600 mb-2">{error}</p>}
      {progress !== null && <div>
            <p>진행률: {progress}%</p>
            <Progress value={progress} className="w-full" /> 
        </div>}
    </div>
  );
}
