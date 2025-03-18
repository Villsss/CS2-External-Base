#include "../include.h"

void EntityList::StartEntityLoop() 
{
	Sleep(400);

	client = memory.clientDLL;

	for (;;)
	{
		ReadEntityList();
		Sleep(555);
	}
}

void EntityList::ReadEntityList() 
{
	std::vector<desiredPlayerValues> playerListTemp;
	std::vector<desiredOtherValues> otherListTemp;

	const auto& entityList = memory.Read<uintptr_t>(client + Offsets::dwEntityList);
	if (!entityList) return;

	auto gameEntSys = memory.Read<uintptr_t>(client + Offsets::dwGameEntitySystem);
	if (!gameEntSys) return;

	int highIx = memory.Read<int>(gameEntSys + Offsets::dwGameEntitySystem_highestEntityIndex);
	if (!highIx) return;

	for (int i = highIx; i >= 1; i--)
	{
		if (i <= 64) {
			uintptr_t listEntry1 = memory.Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
			if (!listEntry1) continue;

			uintptr_t playerController = memory.Read<uintptr_t>(listEntry1 + 120 * (i & 0x1FF));
			if (!playerController) continue;

			if (playerController == memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController)) {
				Globals::LocalPlayerIndex = i;
				continue;
			}

			uintptr_t playerPawnHandle = memory.Read<uintptr_t>(playerController + Offsets::client::m_hPawn);
			if (!playerPawnHandle) continue;

			uintptr_t listEntry2 = memory.Read<uintptr_t>(entityList + 0x8 * ((playerPawnHandle & 0x7FFF) >> 9) + 16);
			if (!listEntry2) continue;

			uintptr_t playerPawn = memory.Read<uintptr_t>(listEntry2 + 120 * (playerPawnHandle & 0x1FF));
			if (!playerPawn) continue;

			std::string playerName = memory.ReadStr<32>(memory.Read<uintptr_t>(playerController + Offsets::client::m_sSanitizedPlayerName));
			if (playerName.empty()) continue;

			desiredPlayerValues player;
			player.playerController = playerController;
			player.playerPawn = playerPawn;
			player.playerName = playerName;
			player.playerIndex = i;
			playerListTemp.emplace_back(std::move(player));
		}
	}

	playerList.swap(playerListTemp);
}