#pragma once

namespace Config 
{
	inline bool boxToggle = true;
	inline bool nameToggle = true;
	inline bool healthBarToggle = true;
	inline bool boneEspToggle = true;
	inline bool espTeamCheck = true;
	inline bool distanceEsp = true;

	inline bool aimToggle = true;
	inline bool aimWithKeyToggle = true;
	inline float smoothing = 5.f;
	inline int aimKey = VK_LBUTTON;

	inline bool rcsToggle = true;
	inline float rcsX, rcsY;

	inline bool fovtoggle = true;
	inline bool movingFov = true;
	inline float fov = 50;
	inline bool aimTeamCheck = true;

	inline bool nadeEsp = true;
	inline bool droppedItemEsp = true;
	inline float maxitemDistance = true;

	inline bool activeWeaponEsp = true;
	inline bool utilEsp = true;

	inline bool triggerToggle = false;
	inline bool triggerOnTeam = true;
	inline int triggerDelay = 0; 
	inline bool useTriggerAfterShotDelay = true;

	inline bool spectatorListToggle = true;
	inline bool onlyLocalSpectators = false;

	inline bool showBombUi = true;
	inline bool showBombUiWhenPlantedOnly = false;
	inline bool bombEsp = true;
}