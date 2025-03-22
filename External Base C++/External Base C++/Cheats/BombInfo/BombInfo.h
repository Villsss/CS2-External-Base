#pragma once

namespace BombInfo
{
	inline uintptr_t bomb;
	inline bool planted;
	inline int site;
	inline bool isDefusing;
	inline float defuseTime;
	inline bool isTicking;
	inline Vec3 sOrigin;
	inline bool defused;
	inline bool exploded;

	void GetBombInfo();
	void DrawBombInfo();
}