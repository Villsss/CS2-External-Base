#pragma once

#include "buttons.h"
#include "client_dll.h"
#include "offsets.h"

using namespace cs2_dumper;

namespace Offsets
{
	inline std::ptrdiff_t dwEntityList = offsets::client_dll::dwEntityList;
	inline std::ptrdiff_t dwLocalPlayerController = offsets::client_dll::dwLocalPlayerController;
	inline std::ptrdiff_t dwLocalPlayerPawn = offsets::client_dll::dwLocalPlayerPawn;
	inline std::ptrdiff_t dwViewMatrix = offsets::client_dll::dwViewMatrix;
	inline std::ptrdiff_t dwGameEntitySystem = offsets::client_dll::dwGameEntitySystem;
	inline std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = offsets::client_dll::dwGameEntitySystem_highestEntityIndex;
	inline std::ptrdiff_t dwSensitivity = offsets::client_dll::dwSensitivity;
	inline std::ptrdiff_t dwSensitivity_sensitivity = offsets::client_dll::dwSensitivity_sensitivity;
	inline std::ptrdiff_t dwPlantedC4 = offsets::client_dll::dwPlantedC4;

	inline std::ptrdiff_t dwBuildNumber = offsets::engine2_dll::dwBuildNumber;

	namespace client 
	{
		inline std::ptrdiff_t m_hPawn = schemas::client_dll::CBasePlayerController::m_hPawn;
		inline std::ptrdiff_t m_vOldOrigin = schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin;
		inline std::ptrdiff_t m_vecViewOffset = schemas::client_dll::C_BaseModelEntity::m_vecViewOffset;
		inline std::ptrdiff_t m_modelState = schemas::client_dll::CSkeletonInstance::m_modelState;
		inline std::ptrdiff_t m_pGameSceneNode = schemas::client_dll::C_BaseEntity::m_pGameSceneNode;
		inline std::ptrdiff_t m_bPawnIsAlive = schemas::client_dll::CCSPlayerController::m_bPawnIsAlive;
		inline std::ptrdiff_t m_sSanitizedPlayerName = schemas::client_dll::CCSPlayerController::m_sSanitizedPlayerName;
		inline std::ptrdiff_t m_iHealth = schemas::client_dll::C_BaseEntity::m_iHealth;
		inline std::ptrdiff_t v_angle = schemas::client_dll::C_BasePlayerPawn::v_angle;
		inline std::ptrdiff_t m_iShotsFired = schemas::client_dll::C_CSPlayerPawn::m_iShotsFired;
		inline std::ptrdiff_t m_aimPunchCache = schemas::client_dll::C_CSPlayerPawn::m_aimPunchCache;
		inline std::ptrdiff_t m_aimPunchAngle = schemas::client_dll::C_CSPlayerPawn::m_aimPunchAngle;
		inline std::ptrdiff_t m_pClippingWeapon = schemas::client_dll::C_CSPlayerPawnBase::m_pClippingWeapon;
		inline std::ptrdiff_t m_nSubclassID = schemas::client_dll::C_BaseEntity::m_nSubclassID + 0x8;
		inline std::ptrdiff_t m_bIsFullAuto = schemas::client_dll::CCSWeaponBaseVData::m_bIsFullAuto;
		inline std::ptrdiff_t m_iTeamNum = schemas::client_dll::C_BaseEntity::m_iTeamNum;
		inline std::ptrdiff_t m_vecAbsOrigin = schemas::client_dll::CGameSceneNode::m_vecAbsOrigin;
		inline std::ptrdiff_t m_hOwnerEntity = schemas::client_dll::C_BaseEntity::m_hOwnerEntity;
		inline std::ptrdiff_t m_AttributeManager = schemas::client_dll::C_EconEntity::m_AttributeManager;
		inline std::ptrdiff_t m_Item = schemas::client_dll::C_AttributeContainer::m_Item;
		inline std::ptrdiff_t m_iItemDefinitionIndex = schemas::client_dll::C_EconItemView::m_iItemDefinitionIndex;
		inline std::ptrdiff_t m_pWeaponServices = schemas::client_dll::C_BasePlayerPawn::m_pWeaponServices;
		inline std::ptrdiff_t m_hMyWeapons = schemas::client_dll::CPlayer_WeaponServices::m_hMyWeapons;
		inline std::ptrdiff_t m_WeaponType = schemas::client_dll::CCSWeaponBaseVData::m_WeaponType;
		inline std::ptrdiff_t m_szName = schemas::client_dll::CCSWeaponBaseVData::m_szName;
		inline std::ptrdiff_t m_iIDEntIndex = schemas::client_dll::C_CSPlayerPawnBase::m_iIDEntIndex;
		inline std::ptrdiff_t m_pObserverServices = schemas::client_dll::C_BasePlayerPawn::m_pObserverServices;
		inline std::ptrdiff_t m_hObserverTarget = schemas::client_dll::CPlayer_ObserverServices::m_hObserverTarget;
		inline std::ptrdiff_t m_nBombSite = schemas::client_dll::C_PlantedC4::m_nBombSite;
		inline std::ptrdiff_t m_bBeingDefused = schemas::client_dll::C_PlantedC4::m_bBeingDefused;
		inline std::ptrdiff_t m_flDefuseLength = schemas::client_dll::C_PlantedC4::m_flDefuseLength;
		inline std::ptrdiff_t m_bBombTicking = schemas::client_dll::C_PlantedC4::m_bBombTicking;
		inline std::ptrdiff_t m_bHasExploded = schemas::client_dll::C_PlantedC4::m_bHasExploded;
		inline std::ptrdiff_t m_bBombDefused = schemas::client_dll::C_PlantedC4::m_bBombDefused;
	}

	namespace buttons 
	{
		inline std::ptrdiff_t jump = cs2_dumper::buttons::jump;
	}
}