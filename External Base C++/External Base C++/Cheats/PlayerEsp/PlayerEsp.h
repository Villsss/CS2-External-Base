#pragma once

namespace PlayerEsp
{
	inline std::mutex playerMutex;

	ImVec4 CalcBox(float hPosX, float hPosY, float sPosX, float sPosY);
	void RunPlayerEsp();
	void EachPlayer(std::vector<DrawObject_t>* vecDrawData, uintptr_t pawn, uintptr_t controller, int index, std::string playerName, view_matrix_t matrix);
}