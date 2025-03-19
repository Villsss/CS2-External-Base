#pragma once

namespace Aimbot
{
	struct Target {
		uintptr_t entity;
		float fovDistance;
	};

	Target GetBestTarget(view_matrix_t matrix);
	void RunAimbot();
	Vec3 CalcAngle(Vec3 localPosition, Vec3 enemyPosition, Vec3 viewAngles);
	Vec3 CalcBestAngle(Vec3 angle, float configFov);
	void MoveMouse(Vec3 newAngle);
	void MoveUsingAngle(Vec3 newAngle);
	void RunRcs(uintptr_t localPawn, bool m_bIsFullAuto);
}