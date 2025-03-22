#pragma once

namespace RadarEsp
{
	inline Vec3 localPlayerPos;
	inline float localPlayerYaw;
	inline std::vector<std::pair<Vec3, float>> enemies;

	void GetRadarPoints();
	void DrawRadar();
}