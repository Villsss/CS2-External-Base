#include "../../include.h"

#define M_PI       3.14159265358979323846 

void RadarEsp::GetRadarPoints()
{
	uintptr_t client = memory.clientDLL;
	for (;;)
	{
		std::vector<std::pair<Vec3, float>> tempenemies;
		float tempLocalYaw;
		Vec3 tempLocalPlayerPos;
		Sleep(1);

		const auto& localPlayerPawn = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayerPawn);
		if (!localPlayerPawn) return;

		tempLocalPlayerPos = memory.Read<Vec3>(localPlayerPawn + Offsets::client::m_vOldOrigin);
		tempLocalYaw = memory.Read<Vec3>(localPlayerPawn + Offsets::client::v_angle).y;

		std::vector<desiredPlayerValues> playerListTemp = List.playerList;
		for (auto& player : playerListTemp)
		{
			if (player.playerIndex == Globals::LocalPlayerIndex) continue;
			if (!memory.Read<bool>(player.playerController + Offsets::client::m_bPawnIsAlive)) continue;

			tempenemies.emplace_back(memory.Read<Vec3>(player.playerPawn + Offsets::client::m_vOldOrigin), memory.Read<Vec3>(player.playerPawn + Offsets::client::m_angEyeAngles).y);
		}

		{
			std::unique_lock<std::shared_mutex> lock(Draw::RadarMutex);
			localPlayerYaw = tempLocalYaw;
			localPlayerPos = tempLocalPlayerPos;
			enemies.swap(tempenemies);
		}
	}
}

void RadarEsp::DrawRadar() {
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;
	float alphaValueBg = 0.1f;
	float scale = 1.f;
	//scale = Config::Get<float>(g_Variables.m_flRadarSize);

	float renderRange = 150 * 1.3f;
	float radarZoom = 6250 / (1.f / 0.4f);

	float radarSize = renderRange * scale;
	float radarCenterX = renderRange * scale;
	float radarCenterY = renderRange * scale;
	const float mapSize = radarZoom;
	const float scaleFactor = radarSize / (mapSize * 0.5f);

	ImGui::SetNextWindowBgAlpha(alphaValueBg);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::Begin("External Radar Overlay", 0, windowFlags);
	ImGui::SetWindowSize({ radarSize * 2, radarSize * 2 });
	ImGui::SetWindowPos({ 0, 0 });

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddCircleFilled(ImVec2(radarCenterX, radarCenterY), radarSize * 0.82f, IM_COL32(50, 50, 50, 150));

	float tempLocalYaw;
	Vec3 tempLocalPlayerPos;
	std::vector<std::pair<Vec3, float>> tempenemies;
	{
		std::unique_lock<std::shared_mutex> lock(Draw::RadarMutex);
		tempenemies = enemies;
		tempLocalYaw = localPlayerYaw;
		tempLocalPlayerPos = localPlayerPos;
	}

	float localRad = (-tempLocalYaw + 90.0f) * (M_PI / 180.0f);

	for (const auto& [enemyPos, enemyYaw] : tempenemies) {
		float deltaX = (enemyPos.x - tempLocalPlayerPos.x) / (mapSize * 0.5f);
		float deltaY = (enemyPos.y - tempLocalPlayerPos.y) / (mapSize * 0.5f);

		float rotX = cos(localRad) * deltaX - sin(localRad) * deltaY;
		float rotY = sin(localRad) * deltaX + cos(localRad) * deltaY;

		float screenX = radarCenterX + (rotX * radarSize);
		float screenY = radarCenterY - (rotY * radarSize);

		float dist = sqrt(rotX * rotX + rotY * rotY);
		if (dist > 0.82f) {
			screenX = radarCenterX + (rotX / dist) * radarSize * 0.82f;
			screenY = radarCenterY - (rotY / dist) * radarSize * 0.82f;
		}

		drawList->AddCircleFilled(ImVec2(screenX, screenY), 6.5f, IM_COL32(255, 0, 0, 255));

		ImVec2 TrianglePoint, TrianglePoint_1, TrianglePoint_2;

		float Angle = (tempLocalYaw - enemyYaw) - 90;

		TrianglePoint.x = screenX + (7 + 7 / 3) * cos(-Angle * M_PI / 180);
		TrianglePoint.y = screenY - (7 + 7 / 3) * sin(-Angle * M_PI / 180);

		TrianglePoint_1.x = screenX + 7 * cos(-(Angle - 30) * M_PI / 180);
		TrianglePoint_1.y = screenY - 7 * sin(-(Angle - 30) * M_PI / 180);

		TrianglePoint_2.x = screenX + 7 * cos(-(Angle + 30) * M_PI / 180);
		TrianglePoint_2.y = screenY - 7 * sin(-(Angle + 30) * M_PI / 180);

		drawList->PathLineTo(TrianglePoint);
		drawList->PathLineTo(TrianglePoint_1);
		drawList->PathLineTo(TrianglePoint_2);
		drawList->PathFillConvex(ImColor(220, 220, 220, 240));
	}

	ImGui::End();
	ImGui::PopStyleVar(2);
}
