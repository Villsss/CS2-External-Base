#include "../../include.h"

void Triggerbot::RunTriggerbot() 
{
	int lastCrossHariEntity = -1;
	bool firstShotOnTarget = true;

	for (;;)
	{
		uintptr_t client = memory.clientDLL;
		Sleep(1);

		if (GetAsyncKeyState(VK_LBUTTON))
			continue;

		if (!Config::triggerToggle) continue;

		auto entityList = memory.Read<uintptr_t>(client + Offsets::dwEntityList);
		if (!entityList) continue;

		const auto& localPlayerPawn = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerPawn);
		if (!localPlayerPawn) continue;

		uintptr_t activeWeaponHandle = memory.Read<uintptr_t>(localPlayerPawn + Offsets::client::m_pClippingWeapon);
		int activeWeaponID = memory.Read<int>(activeWeaponHandle + Offsets::client::m_AttributeManager + Offsets::client::m_Item + Offsets::client::m_iItemDefinitionIndex);

		int afterShotDelay = GetTriggerDelay(activeWeaponID);

		const auto& localPlayerController = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController);
		if (!localPlayerController) continue;

		int crossHairEntity = memory.Read<int>(localPlayerPawn + Offsets::client::m_iIDEntIndex);
		int localHealth = memory.Read<int>(localPlayerPawn + Offsets::client::m_iHealth);
		int localTeamID = memory.Read<int>(localPlayerController + Offsets::client::m_iTeamNum);

		uintptr_t crossHairEntEntry = memory.Read<uintptr_t>(entityList + 0x8 * (crossHairEntity >> 9) + 0x10);
		uintptr_t playerPawn = memory.Read<uintptr_t>(crossHairEntEntry + 0x78 * (crossHairEntity & 0x1FF));

		uintptr_t crossHairController = playerPawn;

		int crosshairPawnHealth = memory.Read<int>(playerPawn + Offsets::client::m_iHealth);
		int crossHairTeamId = memory.Read<int>(crossHairController + Offsets::client::m_iTeamNum);

		bool isValidEntity = (crossHairEntity != -1 && crosshairPawnHealth > 0 && crosshairPawnHealth <= 100 && crossHairTeamId != localTeamID);
		bool isDeathMatchEntity = (crossHairEntity != -1 && crosshairPawnHealth > 0 && crosshairPawnHealth <= 100 && Config::triggerOnTeam);

		int delay = 20;

		if (activeWeaponID == ItemDefinitionIndex::weapon_revolver || activeWeaponID == 262208)
			delay = 400;

		if (isValidEntity || isDeathMatchEntity) {
			Sleep(4);
			if (crossHairEntity != lastCrossHariEntity) {
				firstShotOnTarget = true;  
				lastCrossHariEntity = crossHairEntity;  
			}

			if (firstShotOnTarget) {
				Sleep(Config::triggerDelay);
				firstShotOnTarget = false;
			}

			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(delay);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

			if (firstShotOnTarget == false && Config::useTriggerAfterShotDelay)
			{
				Sleep(afterShotDelay);
			}
		}
		else {
			lastCrossHariEntity = -1;
			firstShotOnTarget = true;
		}
	}
}

int Triggerbot::GetTriggerDelay(int w_Id)
{
	if (w_Id == ItemDefinitionIndex::weapon_revolver || w_Id == 262208)
		return 500;

	if (w_Id == ItemDefinitionIndex::weapon_deagle)
		return 450;

	if (w_Id == ItemDefinitionIndex::weapon_usp_silencer || w_Id == ItemDefinitionIndex::weapon_p250)
		return 200;

	if (w_Id == ItemDefinitionIndex::weapon_ak47 || w_Id == ItemDefinitionIndex::weapon_sg556)
		return 195;

	if (w_Id == ItemDefinitionIndex::weapon_fiveseven || w_Id == ItemDefinitionIndex::weapon_hkp2000)
		return 190;

	if (w_Id == ItemDefinitionIndex::weapon_elite)
		return 180;

	if (w_Id == ItemDefinitionIndex::weapon_aug || w_Id == ItemDefinitionIndex::weapon_famas || w_Id == 28 || w_Id == 14 || w_Id == ItemDefinitionIndex::weapon_cz75a || w_Id == ItemDefinitionIndex::weapon_tec9)
		return 170;

	if (w_Id == ItemDefinitionIndex::weapon_glock)
		return 140;

	if (w_Id == ItemDefinitionIndex::weapon_m4a1_silencer || w_Id == ItemDefinitionIndex::weapon_m4a1)
		return 130;

	if (w_Id == 23)
		return 105;

	if (w_Id == ItemDefinitionIndex::weapon_ump)
		return 80;

	if (w_Id == ItemDefinitionIndex::weapon_mac10 || w_Id == ItemDefinitionIndex::weapon_mp7 || w_Id == ItemDefinitionIndex::weapon_mp9 || w_Id == ItemDefinitionIndex::weapon_bizon || w_Id == ItemDefinitionIndex::weapon_p90)
		return 70;

	return 120;
}