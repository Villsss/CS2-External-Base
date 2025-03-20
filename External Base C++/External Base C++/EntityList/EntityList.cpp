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
		if (i < 64) {
			uintptr_t listEntry1 = memory.Read<uintptr_t>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
			if (!listEntry1) continue;

			uintptr_t playerController = memory.Read<uintptr_t>(listEntry1 + 120 * (i & 0x1FF));
			if (!playerController) continue;

			if (playerController == memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController)) {
				Globals::LocalPlayerIndex = i;
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
        else {
            uintptr_t listEntry = memory.Read<uintptr_t>(entityList + 0x8 * ((i & 0x7FFF) >> 9) + 0x10);
            if (!listEntry) continue;

            uintptr_t grenadeHandle = memory.Read<uintptr_t>(listEntry + 0x78 * (i & 0x1FF));
            if (!grenadeHandle) continue;

            uintptr_t entity = memory.Read<uintptr_t>(grenadeHandle + 0x10);
            if (!entity) continue;

            uintptr_t designerNameAddy = memory.Read<uintptr_t>(entity + 0x20);
            if (!designerNameAddy) continue;

            int owner = memory.Read<int>(grenadeHandle + Offsets::client::m_hOwnerEntity);

            char designerNameBuffer[MAX_PATH]{};
            memory.ReadRaw(designerNameAddy, designerNameBuffer, MAX_PATH);
            std::string name = std::string(designerNameBuffer);

            uintptr_t gameSceneNode = memory.Read<uintptr_t>(grenadeHandle + Offsets::client::m_pGameSceneNode);
            Vec3 origin = memory.Read<Vec3>(gameSceneNode + Offsets::client::m_vecAbsOrigin);

            if (owner != -1)
            {
                if (strstr(name.c_str(), "_projectile")) {
                    size_t pos = name.find("_projectile");
                    if (pos != std::string::npos) {
                        name.erase(pos, std::string("_projectile").length());
                    }
                }
                else continue;

                desiredOtherValues other;
                other.handle = grenadeHandle;
                other.otherName = name;
                other.ownerID = owner;

                otherListTemp.emplace_back(std::move(other));
            }
            else
            {
                if (name.find("weapon_") != std::string::npos) {
                    name.erase(name.find("weapon_"), std::string("weapon_").length());
                }
                else if (name.find("_projectile") != std::string::npos) {
                    name.erase(name.find("_projectile"), std::string("_projectile").length());
                }
                else if (name.find("baseanimgraph") != std::string::npos) {
                    name = "kit";
                }
                else
                    continue;

                if (name == "hkp2000")
                    name = "p2000";

                desiredOtherValues other;
                other.handle = grenadeHandle;
                other.otherName = name;
                other.ownerID = owner;

                otherListTemp.emplace_back(std::move(other));
            }
        }
	}

	Globals::m_fSensitivity = memory.Read<float>(memory.Read<uintptr_t>(client + Offsets::dwSensitivity) + Offsets::dwSensitivity_sensitivity);

	playerList = std::move(playerListTemp);
    otherList = std::move(otherListTemp);
}