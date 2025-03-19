#include "../../include.h"

void PlayerEsp::RunPlayerEsp() 
{
	Sleep(400);
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		uintptr_t client = memory.clientDLL;
		std::vector<DrawObject_t> vecDrawDataTemp;
		std::vector<std::thread> playerThreads;

		const auto& localPlayerPawn = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerPawn);
		if (!localPlayerPawn) return;

		const auto& localPlayerController = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController);
		if (!localPlayerController) return;

		view_matrix_t viewMatrix = memory.Read<view_matrix_t>(client + Offsets::dwViewMatrix);

		std::vector<desiredPlayerValues> playerListTemp = List.playerList;
		for (auto& player : playerListTemp)
		{
			if (player.playerIndex == Globals::LocalPlayerIndex) continue;
			if (!memory.Read<bool>(player.playerController + Offsets::client::m_bPawnIsAlive)) continue;

			if (memory.Read<int>(localPlayerController + Offsets::client::m_iTeamNum) == memory.Read<int>(player.playerController + Offsets::client::m_iTeamNum) && !Config::espTeamCheck)
				continue;

			playerThreads.push_back(std::thread(EachPlayer, &vecDrawDataTemp, player.playerPawn, player.playerController, player.playerIndex, player.playerName, viewMatrix));
		}

		for (auto& t : playerThreads) {
			t.join();
		}

		{
			std::unique_lock<std::shared_mutex> lock(Draw::PlayerEspMutex);
			Draw::playerEspData.swap(vecDrawDataTemp);
		}
	}
}

ImVec4 PlayerEsp::CalcBox(float hPosX, float hPosY, float sPosX, float sPosY)
{
	Vec2 Size, Pos;
	Size.y = (sPosY - hPosY) * 1.26f;
	Size.x = Size.y * 0.57f;

	Pos = ImVec2(hPosX - Size.x / 2, hPosY - Size.y * 0.13f);

	return ImVec4{ Pos.x,Pos.y,Size.x,Size.y };
}

void PlayerEsp::EachPlayer(std::vector<DrawObject_t>* vecDrawData, uintptr_t pawn, uintptr_t controller, int index, std::string playerName, view_matrix_t matrix)
{
	float top = 0;
	float bottom = 0;

	std::lock_guard<std::mutex> Lock(playerMutex);

	const auto& localPlayerPawn = memory.Read<uintptr_t>(memory.clientDLL + Offsets::dwLocalPlayerPawn);
	if (!localPlayerPawn) return;

	const auto& entityList = memory.Read<uintptr_t>(memory.clientDLL + Offsets::dwEntityList);
	if (!entityList) return;

	uintptr_t node = memory.Read<uintptr_t>(pawn + Offsets::client::m_pGameSceneNode);
	uintptr_t bonearray = memory.Read<uintptr_t>(node + Offsets::client::m_modelState + 0x80);

	Vec3 localPos = memory.Read<Vec3>(localPlayerPawn + Offsets::client::m_vOldOrigin);
	Vec3 pawnPos = memory.Read<Vec3>(pawn + Offsets::client::m_vOldOrigin);
	Vec3 headPos = memory.Read<Vec3>(bonearray + 6 * 32);

	Vec3 sPawnPos = pawnPos.worldToScreen(matrix); if (sPawnPos.z < 0.1f) return;
	Vec3 sHeadPos = headPos.worldToScreen(matrix); if (sHeadPos.z < 0.1f) return;

	ImVec4 Box = CalcBox(sHeadPos.x, sHeadPos.y, sPawnPos.x, sPawnPos.y);

	if (Config::boxToggle)
		Draw::AddRect(vecDrawData, { Box.x, Box.y }, { Box.x + Box.z,Box.y + Box.w }, ImColor(255, 0, 0), DRAW_RECT_OUTLINE | DRAW_RECT_BORDER | DRAW_RECT_ALIGNED, ImColor(0,0,0,0));

	ImVec2 nameSize = Globals::ESPFont->CalcTextSizeA(10.5f, FLT_MAX, 0.0f, playerName.c_str());
	if (Config::nameToggle) {
		top += nameSize.y;
		Draw::AddText(vecDrawData, Globals::ESPFont, 10.5f, { Box.x + (Box.z / 2) - (nameSize.x / 2), Box.y - top - 1 }, playerName, ImColor(255, 255, 255), DRAW_TEXT_OUTLINE);
	}
	
	if (Config::healthBarToggle) {
		int pawnHealth = memory.Read<int>(pawn + Offsets::client::m_iHealth);
		float healthBarYOffset = ((int)(Box.w * pawnHealth * 0.01f));
		float red = (255.f - (pawnHealth * 2.55f)) - 100.f;
		float green = (pawnHealth * 2.55f) / 100.f;
		Draw::AddRect(vecDrawData, { Box.x - 5.f, Box.y - 0.5f }, { Box.x - 4.5f + 3, Box.y + Box.w + 0.5f }, ImColor(0.f, 0.f, 0.f, 0.6f), DRAW_RECT_FILLED);
		Draw::AddRect(vecDrawData, { Box.x - 4.5f, Box.y - healthBarYOffset + Box.w }, { Box.x - 4.5f + 2, Box.y - healthBarYOffset + Box.w + healthBarYOffset }, ImColor(red, green, 0.f, 1.f), DRAW_RECT_FILLED);
	
		std::string health = std::to_string(pawnHealth);
	
		if (pawnHealth < 100) {
			ImVec2 size = ImGui::CalcTextSize(health.c_str());
			Draw::AddText(vecDrawData, Globals::ESPFont, 9, { Box.x - (size.x / 2) - 8, Box.y - healthBarYOffset + Box.w - 0.7f }, health, ImColor(red, green, 0.f, 1.f), DRAW_TEXT_OUTLINE);
		}
	}

	if (Config::boneEspToggle) {
		for (int i = 0; i < sizeof(boneConnections) / sizeof(boneConnections[0]); i++) {
			int bone1 = boneConnections[i].bone1;
			int bone2 = boneConnections[i].bone2;

			Vec3 bonePos1 = memory.Read<Vec3>(bonearray + bone1 * 32);
			Vec3 bonePos2 = memory.Read<Vec3>(bonearray + bone2 * 32);

			Vec3 sBone1 = bonePos1.worldToScreen(matrix); if (sBone1.z < 0.1f) continue;
			Vec3 sBone2 = bonePos2.worldToScreen(matrix); if (sBone2.z < 0.1f) continue;

			Draw::AddLine(vecDrawData, { sBone1.x, sBone1.y }, { sBone2.x, sBone2.y }, ImColor(255, 255, 255, 160));
		}
	}

	uintptr_t activeWeaponHandle = memory.Read<uintptr_t>(pawn + Offsets::client::m_pClippingWeapon);
	int activeWeaponID = memory.Read<int>(activeWeaponHandle + Offsets::client::m_AttributeManager + Offsets::client::m_Item + Offsets::client::m_iItemDefinitionIndex);
	std::string weaponName = GetWeaponName(activeWeaponID);
	weaponName = GunIcon(weaponName);
	ImVec2 wTextSize = Globals::WEAPONFont->CalcTextSizeA(9.f, FLT_MAX, 0.0f, weaponName.c_str());

	if(Config::activeWeaponEsp) {
		bottom += wTextSize.y + 1;
		Draw::AddText(vecDrawData, Globals::WEAPONFont, 9.f, { Box.x + (Box.z / 2) - (wTextSize.x / 2), Box.y + Box.w + 1}, weaponName.c_str(), ImColor(255, 255, 255), DRAW_TEXT_OUTLINE);
	}

	if(Config::utilEsp) {
		std::vector<std::string> grenadeNames;
		myWeapon m_hMyWeapons = memory.Read<myWeapon>(memory.Read<uintptr_t>(pawn + Offsets::client::m_pWeaponServices) + Offsets::client::m_hMyWeapons);

		for (int i = 0; i < m_hMyWeapons.size; ++i)
		{
			int weaponHandle = memory.Read<int>(m_hMyWeapons.handle + 0x4 * i);
			if (weaponHandle == -1) continue;

			weaponHandle &= 0x7FFF;
			auto list = memory.Read<uintptr_t>(entityList + 8 * (weaponHandle >> 9) + 16);
			auto controller = list + 120 * (weaponHandle & 0x1FF);
			auto weapon = memory.Read<uintptr_t>(controller);
			if (!weapon) continue;

			auto subclassID = memory.Read<std::uint64_t>(weapon + Offsets::client::m_nSubclassID);
			auto nameAddress = memory.Read<std::uint64_t>(subclassID + Offsets::client::m_szName);

			if (!nameAddress) continue;

			char buf[MAX_PATH] = {};
			memory.ReadRaw(nameAddress, buf, MAX_PATH);
			std::string weaponName(buf);

			if (weaponName.size() > 7)
				weaponName.erase(0, 7);

			if (memory.Read<e_weapon_type>(subclassID + Offsets::client::m_WeaponType) == e_weapon_type::grenade) {
				grenadeNames.push_back(weaponName);
			}			
		}

		float spacing = 2.0f;
		float iconSize = 6.0f;
		float totalWidth = (grenadeNames.size() * iconSize) + ((grenadeNames.size()) * spacing);
		float startX = Box.x + (Box.z / 2) - (totalWidth / 2);

		for (const auto& grenadeName : grenadeNames) {
			std::string name = GunIcon(grenadeName);

			Draw::AddText(
				vecDrawData,
				Globals::WEAPONFont,
				10,
				{ startX, Box.y + Box.w + bottom },
				name.c_str(),
				ImColor(255, 255, 255),
				DRAW_TEXT_OUTLINE
			);

			startX += iconSize + spacing;
		}
	}

	if(Config::distanceEsp) {
		int distance = pawnPos.DistanceTo(localPos) / 100;
		std::string str = std::to_string(distance) + "m";
		ImVec2 dTextSize = Globals::ESPFont->CalcTextSizeA(10.f, FLT_MAX, 0.0f, str.c_str());

		if (!top)
			top = 1;

		top += dTextSize.y;
		Draw::AddText(vecDrawData, Globals::ESPFont, 10.f, { Box.x + (Box.z / 2) - (dTextSize.x / 2), Box.y - top - 1 }, str.c_str(), ImColor(255, 255, 255), DRAW_TEXT_OUTLINE);
	}
}
