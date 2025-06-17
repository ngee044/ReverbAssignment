#pragma once

#include <QJsonObject>
#include <QJsonDocument>

struct ReverbSetting 
{
	int room_size_;
	int damping_;
	int wet_level_;
	int dry_level_;
	int width_;

	auto to_json() const -> QJsonObject
	{
		QJsonObject json;
		json["roomSize"] = room_size_;
		json["damping"] = damping_;
		json["wetLevel"] = wet_level_;
		json["dryLevel"] = dry_level_;
		json["width"] = width_;
		return json;
	}

	ReverbSetting()
		: room_size_(0.5f), damping_(0.5f), wet_level_(0.5f), dry_level_(0.5f), width_(1.0f) {}


};