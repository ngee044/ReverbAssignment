"use client";

import { Slider } from "@/components/ui/slider";

export default function ReverbSlider({ params, onChange }) {
  const updateParam = (key, value) => {
    const newParams = {
      ...params,
      [key]: value,
    };
    onChange(newParams);
  };

  return (
    <div className="mt-8 bg-white p-4 rounded-xl shadow">
      <h2 className="text-lg font-semibold mb-4">Reverb Parameters</h2>
      {Object.keys(params).map((key) => (
        <div key={key} className="mb-4">
          <div className="flex justify-between mb-1">
            <span className="text-sm font-medium">{key}</span>
            <span className="text-sm text-gray-500">{params[key]}</span>
          </div>
          <Slider
            defaultValue={[params[key]]}
            max={100}
            step={1}
            onValueChange={(value) => updateParam(key, value[0])}
          />
        </div>
      ))}
    </div>
  );
}
