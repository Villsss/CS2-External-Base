#include "../../include.h"

void ItemEsp::RunItemEsp() 
{
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		uintptr_t client = memory.clientDLL;
		std::vector<DrawObject_t> vecDrawDataTemp;

		const auto& localPlayerController = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController);
		if (!localPlayerController) return;

		view_matrix_t viewMatrix = memory.Read<view_matrix_t>(client + Offsets::dwViewMatrix);

		std::vector<desiredOtherValues> itemListTemp = List.otherList;
		for (auto& item : itemListTemp)
		{
			ItemEsp::EachItem(&vecDrawDataTemp, item.handle, item.otherName, item.ownerID, viewMatrix);
		}

		{
			std::unique_lock<std::shared_mutex> lock(Draw::ItemMutex);
			Draw::ItemData.swap(vecDrawDataTemp);
		}
	}
}

void ItemEsp::EachItem(std::vector<DrawObject_t>* vecDrawData, uintptr_t pawn, std::string playerName, int owner, view_matrix_t matrix)
{
	const auto& localPlayerPawn = memory.Read<uintptr_t>(memory.clientDLL + Offsets::dwLocalPlayerPawn);
	if (!localPlayerPawn) return;

	Vec3 localOrigin = memory.Read<Vec3>(pawn + Offsets::client::m_vOldOrigin);

	uintptr_t gameSceneNode = memory.Read<uintptr_t>(pawn + Offsets::client::m_pGameSceneNode);
	Vec3 origin = memory.Read<Vec3>(gameSceneNode + Offsets::client::m_vecAbsOrigin);
	Vec3 screenOrigin = origin.worldToScreen(matrix); if (screenOrigin.z < 0.1f) return;

	std::string weaponName = GunIcon(playerName);

	if (owner != -1)
	{
		if(Config::nadeEsp)
			Draw::AddText(vecDrawData, Globals::WEAPONFont, 10, { screenOrigin.x, screenOrigin.y }, weaponName, ImColor(255, 0, 0), DRAW_TEXT_OUTLINE);
	}
	else
	{
		float distance = localOrigin.distanceTo(origin);
		if (distance / 100 >= Config::maxitemDistance)
			return;

		if(Config::droppedItemEsp)
			Draw::AddText(vecDrawData, Globals::WEAPONFont, 10, { screenOrigin.x, screenOrigin.y }, weaponName, ImColor(255, 255, 255), DRAW_TEXT_OUTLINE);
	}
}