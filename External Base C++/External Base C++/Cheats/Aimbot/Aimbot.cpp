#include "../../include.h"

#define M_PI       3.14159265358979323846 

void Aimbot::RunRcs(uintptr_t localPawn, bool m_bIsFullAuto)
{
	static Vec3 oldPunch{ 0,0,0 };
	static Vec3 currentPunch{ 0,0,0 };

	if (memory.Read<int>(localPawn + Offsets::client::m_iShotsFired) && m_bIsFullAuto)
	{
		Vec3 viewAngles = memory.Read<Vec3>(localPawn + Offsets::client::v_angle);
		Vec3 aimPunchAngle = memory.Read<Vec3>(localPawn + Offsets::client::m_aimPunchAngle);

		Vec3 deltaPunch;
		deltaPunch.x = (aimPunchAngle.x * 2.0f) - oldPunch.x;
		deltaPunch.y = (aimPunchAngle.y * 2.0f) - oldPunch.y;

		float smoothingFactor = 0.6f;

		currentPunch.x += smoothingFactor * (deltaPunch.x - currentPunch.x);
		currentPunch.y += smoothingFactor * (deltaPunch.y - currentPunch.y);
		currentPunch.z += smoothingFactor * (deltaPunch.z - currentPunch.z);

		currentPunch.x /= 2.f;
		currentPunch.y /= 2.f;

		currentPunch.x *= Config::rcsY + 1;
		currentPunch.y *= Config::rcsX + 1;

		float mouse_x = currentPunch.y / (Globals::m_fSensitivity * 0.022f);
		float mouse_y = currentPunch.x / (Globals::m_fSensitivity * 0.022f);

		Vec3 mouseAngles = { mouse_x, -mouse_y, 0 };

		if (mouseAngles.x > 150)
			return;
		if (mouseAngles.y > 150)
			return;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			mouse_event(MOUSEEVENTF_MOVE, (DWORD)mouseAngles.x, (DWORD)mouseAngles.y, 0, 0);
		}

		oldPunch = aimPunchAngle * 2.0f;
	}
	else
	{
		oldPunch = Vec3{ 0,0,0 };
		currentPunch = Vec3{ 0,0,0 };
	}
}

void Aimbot::RunAimbot()
{
	for (;;)
	{
		std::vector<DrawObject_t> vecDrawDataTemp;

		Sleep(1);

		HWND activeWindow = GetForegroundWindow();
		HWND targetWindow = FindWindow(NULL, "Counter-Strike 2");
		if (activeWindow != targetWindow) continue;

		uintptr_t client = memory.clientDLL;

		const auto& localPlayerPawn = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerPawn);
		if (!localPlayerPawn) continue;

		const auto& localPlayerController = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerController);
		if (!localPlayerController) continue;

		const auto& entityList = memory.Read<uintptr_t>(client + Offsets::dwEntityList);
		if (!entityList) continue;

		view_matrix_t viewMatrix = memory.Read<view_matrix_t>(client + Offsets::dwViewMatrix);

		if (!Config::aimToggle) continue;

		Target ClosestTarget = GetBestTarget(viewMatrix);

		uintptr_t closestEntity = ClosestTarget.entity;

		Vec3 DesiredAimPos;

		uintptr_t node = memory.Read<uintptr_t>(closestEntity + Offsets::client::m_pGameSceneNode);
		uintptr_t bonearray = memory.Read<uintptr_t>(node + Offsets::client::m_modelState + 0x80);

		DesiredAimPos = memory.Read<Vec3>(bonearray + 6 * 32);
		if (DesiredAimPos.IsZero()) continue;
		Vec3 localOrigin = memory.Read<Vec3>(localPlayerPawn + Offsets::client::m_vOldOrigin);
		C_UTL_VECTOR aimPunchC = memory.Read<C_UTL_VECTOR>(localPlayerPawn + Offsets::client::m_aimPunchCache);
		Vec3 aimPunchAngle = memory.Read<Vec3>((aimPunchC.Data + (aimPunchC.Count - 1) * sizeof(Vec3)));
		Vec3 LocalAngles = memory.Read<Vec3>(localPlayerPawn + Offsets::client::v_angle);
		Vec3 LocalEyePos = localOrigin + memory.Read<Vec3>(localPlayerPawn + Offsets::client::m_vecViewOffset);
		Vec3 FovAngle = CalculateAngle(LocalEyePos, DesiredAimPos, LocalAngles);
		Vec3 punch = memory.Read<Vec3>(localPlayerPawn + Offsets::client::m_aimPunchAngle);

		Vec3 rcsFov = LocalAngles;
		if (memory.Read<int>(localPlayerPawn + Offsets::client::m_iShotsFired)) {
			rcsFov.x = rcsFov.x + punch.x * 2.f;
			rcsFov.y = rcsFov.y + punch.y * 2.f;
		}

		bool m_bIsFullAuto = memory.Read<bool>(memory.Read<uintptr_t>(memory.Read<uintptr_t>(localPlayerPawn + Offsets::client::m_pClippingWeapon) + Offsets::client::m_nSubclassID) + Offsets::client::m_bIsFullAuto);
		RunRcs(localPlayerPawn, m_bIsFullAuto);

		Vec3 EndPos;
		float LineLength = cos(rcsFov.x * M_PI / 180) * 20000;
		EndPos.x = localOrigin.x + cos(rcsFov.y * M_PI / 180) * LineLength;
		EndPos.y = localOrigin.y + sin(rcsFov.y * M_PI / 180) * LineLength;
		EndPos.z = localOrigin.z - sin(rcsFov.x * M_PI / 180) * 20000;

		Vec3 temp = EndPos.worldToScreen(viewMatrix);
		ImVec2 criclePos = { temp.x, temp.y };

		if (Config::fovtoggle) {
			if (memory.Read<int>(localPlayerPawn + Offsets::client::m_iShotsFired) && Config::movingFov && m_bIsFullAuto)
				Draw::AddCircle(&vecDrawDataTemp, criclePos, Config::fov, ImColor(0, 0, 0, 255), 0);
			else
				Draw::AddCircle(&vecDrawDataTemp, { ScreenSizeX / 2, ScreenSizeY / 2 }, Config::fov, ImColor(0, 0, 0, 255), 0);
		}

		{
			std::unique_lock<std::shared_mutex> lock(Draw::AimbotMutex);
			Draw::aimbotData.swap(vecDrawDataTemp);
		}

		if (memory.Read<int>(localPlayerPawn + Offsets::client::m_iShotsFired) && m_bIsFullAuto)
		{
			Vec3 Angle = CalculateAngle(LocalEyePos, DesiredAimPos, LocalAngles);
			Angle.x = Angle.x - (aimPunchAngle.x * 2.f);
			Angle.y = Angle.y - (aimPunchAngle.y * 2.f);
			Vec3 newAngle = calculateBestAngle(Angle, Config::fov / 10);
			newAngle.x = (newAngle.x / (0.022f * Globals::m_fSensitivity)) / Config::smoothing / 2;
			newAngle.y = (newAngle.y / (0.022f * Globals::m_fSensitivity)) / Config::smoothing / 2;

			if (Config::aimWithKeyToggle) {
				if (GetAsyncKeyState(Config::aimKey)) {
					MoveUsingAngle(newAngle);
				}
			}
			else {
				MoveUsingAngle(newAngle);
			}
			continue;
		}

		if (calculateBestAngle(FovAngle, Config::fov / 10).IsZero()) continue;

		Vec3 aimPos = DesiredAimPos.worldToScreen(viewMatrix); if (aimPos.z < 0.1f) continue;
		if (Config::aimWithKeyToggle) {
			if (GetAsyncKeyState(Config::aimKey)) {
				MoveMouse(aimPos);
			}
		}
		else {
			MoveMouse(aimPos);
		}
	}
}

void Aimbot::MoveUsingAngle(Vec3 Pos)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;

	const float deadZoneThreshold = 0.05f;
	float magnitude = sqrt(Pos.x * Pos.x + Pos.y * Pos.y);

	if (magnitude < deadZoneThreshold) {
		return;
	}

	float moveX = -Pos.y;
	float moveY = Pos.x;

	if (fabs(moveX) < 1.f) moveX = moveX < 0 ? -1.f : 1.f;
	if (fabs(moveY) < 1.f) moveY = moveY < 0 ? -1.f : 1.f;

	input.mi.dx = static_cast<float>(moveX);
	input.mi.dy = static_cast<float>(moveY);
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	Sleep(2);
	SendInput(1, &input, sizeof(INPUT));
}

void Aimbot::MoveMouse(Vec3 Pos)
{
    float TargetX = 0;
    float TargetY = 0;

    static float ScreenCenterX = ScreenSizeX / 2;
    static float ScreenCenterY = ScreenSizeY / 2;

    if (Pos.x != 0)
    {
        if (Pos.x > ScreenCenterX)
        {
            TargetX = -(ScreenCenterX - Pos.x);
            TargetX /= 1;
            if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
        }

        if (Pos.x < ScreenCenterX)
        {
            TargetX = Pos.x - ScreenCenterX;
            TargetX /= 1;
            if (TargetX + ScreenCenterX < 0) TargetX = 0;
        }
    }
    if (Pos.y != 0)
    {
        if (Pos.y > ScreenCenterY)
        {
            TargetY = -(ScreenCenterY - Pos.y);
            TargetY /= 1;
            if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
        }

        if (Pos.y < ScreenCenterY)
        {
            TargetY = Pos.y - ScreenCenterY;
			TargetY /= 1;
            if (TargetY + ScreenCenterY < 0) TargetY = 0;
        }
    }

	const float deadZoneThreshold = 0.7f;
	float magnitude = sqrt(TargetX * TargetX + TargetY * TargetY);

	if (magnitude < deadZoneThreshold) {
		return;
	}

    TargetX /= Config::smoothing;
	TargetY /= Config::smoothing;
    if (abs(TargetX) < 1)
    {
        if (TargetX > 0)
        {
            TargetX = 1;
        }
        if (TargetX < 0)
        {
            TargetX = -1;
        }
    }
    if (abs(TargetY) < 1)
    {
        if (TargetY > 0)
        {
            TargetY = 1;
        }
        if (TargetY < 0)
        {
            TargetY = -1;
        }
    }
    mouse_event(0x0001, (UINT)TargetX, (UINT)TargetY, NULL, NULL);
	Sleep(2);
    return;
}

Aimbot::Target Aimbot::GetBestTarget(view_matrix_t matrix)
{
	std::vector<desiredPlayerValues> playerListTemp = List.playerList;
	std::vector<Target> targets;

	const auto& localPlayerController = memory.Read<uintptr_t>(memory.clientDLL + Offsets::dwLocalPlayerController);
	if (!localPlayerController) return Target(0, 0);

	for (auto& player : playerListTemp)
	{
		if (player.playerIndex == Globals::LocalPlayerIndex) continue;
		if (!memory.Read<bool>(player.playerController + Offsets::client::m_bPawnIsAlive)) continue;

		if (memory.Read<int>(localPlayerController + Offsets::client::m_iTeamNum) == memory.Read<int>(player.playerController + Offsets::client::m_iTeamNum) && !Config::aimTeamCheck)
			continue;

		uintptr_t node = memory.Read<uintptr_t>(player.playerPawn + Offsets::client::m_pGameSceneNode);
		uintptr_t bonearray = memory.Read<uintptr_t>(node + Offsets::client::m_modelState + 0x80);

		Vec3 desiredBonePos = memory.Read<Vec3>(bonearray + 6 * 32);
		Vec3 desiredBonePosScreen = desiredBonePos.worldToScreen(matrix); if (desiredBonePosScreen.z < 0.1f) continue;

		Vec2 ScreenMid = { (float)ScreenSizeX / 2, (float)ScreenSizeY / 2 };
		Vec2 Pos = { desiredBonePosScreen.x, desiredBonePosScreen.y };
		float distToCrossHair = Pos.DistanceTo(ScreenMid);
		targets.push_back({ player.playerPawn, distToCrossHair });
	}

	std::stable_sort(targets.begin(), targets.end(), [](const Target& a, const Target& b) {
		return a.fovDistance < b.fovDistance;
		});

	if (!targets.empty() && targets[0].fovDistance < 200) 
		return targets[0];

	return Target(0, 0);
}

Vec3 Aimbot::CalcAngle(Vec3 localPosition, Vec3 enemyPosition, Vec3 viewAngles) {
	return (enemyPosition - localPosition).ToAngle() - viewAngles;
}

Vec3 Aimbot::CalcBestAngle(Vec3 angle, float configFov) {
	Vec3 newAngle;

	float calcFov = std::sqrt(angle.x * angle.x + angle.y * angle.y);
	float fov = configFov;

	if (calcFov > fov)
		return { 0,0,0 };

	if (calcFov < fov) {
		calcFov = fov;
		newAngle = angle;
	}
	return newAngle;
}