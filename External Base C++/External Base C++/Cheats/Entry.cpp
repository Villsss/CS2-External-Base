#include "../include.h"

ImVec4 CalcBox(float hPosX, float hPosY, float sPosX, float sPosY)
{
	Vec2 Size, Pos;
	Size.y = (sPosY - hPosY) * 1.26f;
	Size.x = Size.y * 0.57f;

	Pos = ImVec2(hPosX - Size.x / 2, hPosY - Size.y * 0.13f);

	return ImVec4{ Pos.x,Pos.y,Size.x,Size.y };
}

void __fastcall Run::Cheats() 
{
	uintptr_t client = memory.clientDLL;

	const auto& localPlayerPawn = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerPawn);
	if (!localPlayerPawn) return;

	const auto& localPlayerController = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController);
	if (!localPlayerController) return;

	const auto& entityList = memory.Read<uintptr_t>(client + Offsets::dwEntityList);
	if (!entityList) return;

	view_matrix_t viewMatrix = memory.Read<view_matrix_t>(client + Offsets::dwViewMatrix);

	for (int i = 1; i < 64; i++)
	{
		uintptr_t listEntry1 = memory.Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
		if (!listEntry1) continue;

		uintptr_t playerController = memory.Read<uintptr_t>(listEntry1 + 120 * (i & 0x1FF));
		if (!playerController) continue;

		if (playerController == localPlayerController)  continue;

		uintptr_t playerPawnHandle = memory.Read<uintptr_t>(playerController + Offsets::client::m_hPawn);
		if (!playerPawnHandle) continue;

		uintptr_t listEntry2 = memory.Read<uintptr_t>(entityList + 0x8 * ((playerPawnHandle & 0x7FFF) >> 9) + 16);
		if (!listEntry2) continue;

		uintptr_t playerPawn = memory.Read<uintptr_t>(listEntry2 + 120 * (playerPawnHandle & 0x1FF));
		if (!playerPawn) continue;

		uintptr_t node = memory.Read<uintptr_t>(playerPawn + Offsets::client::m_pGameSceneNode);
		uintptr_t bonearray = memory.Read<uintptr_t>(node + Offsets::client::m_modelState + 0x80);

		Vec3 pawnPos = memory.Read<Vec3>(playerPawn + Offsets::client::m_vOldOrigin);
		Vec3 headPos = memory.Read<Vec3>(bonearray + 6 * 32);

		Vec3 sPawnPos = pawnPos.worldToScreen(viewMatrix); if (sPawnPos.z < 0.1f) continue;
		Vec3 sHeadPos = headPos.worldToScreen(viewMatrix); if (sHeadPos.z < 0.1f) continue;

		ImVec4 Box = CalcBox(sHeadPos.x, sHeadPos.y, sPawnPos.x, sPawnPos.y);

		if(Config::boxToggle)
			ImGui::GetBackgroundDrawList()->AddRect({ Box.x, Box.y }, { Box.x + Box.z,Box.y + Box.w }, ImColor(255,0,0), 0, 0, 0.1f);

	}
}