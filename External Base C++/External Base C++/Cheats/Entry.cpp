#include "../include.h"

void __fastcall Run::Cheats() 
{
	uintptr_t client = memory.clientDLL;

	SpectatorList::DrawSpectatorList();

	std::vector<DrawObject_t> playerEspDataTemp;
	{
		std::unique_lock<std::shared_mutex> lock(Draw::PlayerEspMutex);
		playerEspDataTemp = Draw::playerEspData;
	}
	Draw::RenderDrawData(ImGui::GetBackgroundDrawList(), playerEspDataTemp);

	std::vector<DrawObject_t> aimbotDataTemp;
	{
		std::unique_lock<std::shared_mutex> lock(Draw::AimbotMutex);
		aimbotDataTemp = Draw::aimbotData;
	}
	Draw::RenderDrawData(ImGui::GetBackgroundDrawList(), aimbotDataTemp);

	std::vector<DrawObject_t> itemDataTemp;
	{
		std::unique_lock<std::shared_mutex> lock(Draw::ItemMutex);
		itemDataTemp = Draw::ItemData;
	}
	Draw::RenderDrawData(ImGui::GetBackgroundDrawList(), itemDataTemp);
}