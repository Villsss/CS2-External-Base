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

		if (playerController == localPlayerController) continue;

		if (!memory.Read<bool>(playerController + Offsets::client::m_bPawnIsAlive)) continue;

		uintptr_t playerPawnHandle = memory.Read<uintptr_t>(playerController + Offsets::client::m_hPawn);
		if (!playerPawnHandle) continue;

		uintptr_t listEntry2 = memory.Read<uintptr_t>(entityList + 0x8 * ((playerPawnHandle & 0x7FFF) >> 9) + 16);
		if (!listEntry2) continue;

		uintptr_t playerPawn = memory.Read<uintptr_t>(listEntry2 + 120 * (playerPawnHandle & 0x1FF));
		if (!playerPawn) continue;

		std::string playerName = memory.ReadStr<32>(memory.Read<uintptr_t>(playerController + Offsets::client::m_sSanitizedPlayerName));
		if (playerName.empty()) continue;

		uintptr_t node = memory.Read<uintptr_t>(playerPawn + Offsets::client::m_pGameSceneNode);
		uintptr_t bonearray = memory.Read<uintptr_t>(node + Offsets::client::m_modelState + 0x80);

		Vec3 pawnPos = memory.Read<Vec3>(playerPawn + Offsets::client::m_vOldOrigin);
		Vec3 headPos = memory.Read<Vec3>(bonearray + 6 * 32);

		Vec3 sPawnPos = pawnPos.worldToScreen(viewMatrix); if (sPawnPos.z < 0.1f) continue;
		Vec3 sHeadPos = headPos.worldToScreen(viewMatrix); if (sHeadPos.z < 0.1f) continue;

		ImVec4 Box = CalcBox(sHeadPos.x, sHeadPos.y, sPawnPos.x, sPawnPos.y);

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		if(Config::boxToggle)
			drawList->AddRect({ Box.x, Box.y }, { Box.x + Box.z,Box.y + Box.w }, ImColor(255,0,0), 0, 0, 0.1f);

		ImVec2 nameSize = ImGui::CalcTextSize(playerName.c_str());

		if (Config::nameToggle) {
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					drawList->AddText({ Box.x + (Box.z / 2) - (nameSize.x / 2) - i, Box.y - 1 - nameSize.y - j }, ImColor(0.f, 0.f, 0.f, 1.f), playerName.c_str());
				}
			}
			drawList->AddText({ Box.x + (Box.z / 2) - (nameSize.x / 2), Box.y - 1 - nameSize.y }, ImColor(255, 255, 255), playerName.c_str());
		}

		if (Config::healthBarToggle) {
			int pawnHealth = memory.Read<int>(playerPawn + Offsets::client::m_iHealth);
			float healthBarYOffset = ((int)(Box.w * pawnHealth * 0.01f));
			float red = (255.f - (pawnHealth * 2.55f)) - 100.f;
			float green = (pawnHealth * 2.55f) / 100.f;
			drawList->AddRectFilled({ Box.x - 5.f, Box.y - 0.5f}, { Box.x - 4.5f + 3, Box.y + Box.w + 0.5f }, ImColor(0.f, 0.f, 0.f, 0.6f));
			drawList->AddRectFilled({ Box.x - 4.5f, Box.y - healthBarYOffset + Box.w }, { Box.x - 4.5f + 2, Box.y - healthBarYOffset + Box.w + healthBarYOffset }, ImColor(red, green, 0.f, 1.f));

			std::string health = std::to_string(pawnHealth);

			if (pawnHealth < 100) {
				ImVec2 size = ImGui::CalcTextSize(health.c_str());
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						drawList->AddText(Globals::ESPFont, 9, { Box.x - (size.x / 2) - 8 - i, Box.y - healthBarYOffset + Box.w - j - 0.7f }, ImColor(0.f, 0.f, 0.f, 1.f), health.c_str());
					}
				}
				drawList->AddText(Globals::ESPFont, 9, { Box.x - (size.x / 2) - 8, Box.y - healthBarYOffset + Box.w - 0.7f }, ImColor(red, green, 0.f, 1.f), health.c_str());
			}
		}
	}
}