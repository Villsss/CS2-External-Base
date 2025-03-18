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
	}

	namespace buttons 
	{
		inline std::ptrdiff_t jump = cs2_dumper::buttons::jump;
	}
}