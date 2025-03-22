#include "../../include.h"

void BombInfo::GetBombInfo() 
{
	uintptr_t client = memory.clientDLL;
	for (;;)
	{
		Sleep(1);

		view_matrix_t viewMatrix = memory.Read<view_matrix_t>(client + Offsets::dwViewMatrix);

		uintptr_t tempbomb;
		bool tempplanted;
		int tempsite;
		bool tempisDefusing;
		float tempdefuseTime;
		bool tempisTicking;
		Vec3 tempsOrigin;
		bool tempDefused;
		bool tempExploded;

		tempbomb = memory.Read<uintptr_t>(client + Offsets::dwPlantedC4);
		tempplanted = memory.Read<bool>(client + Offsets::dwPlantedC4 - 0x8);
		tempbomb = memory.Read<uintptr_t>(tempbomb);
		tempsite = memory.Read<int>(tempbomb + Offsets::client::m_nBombSite);
		tempisDefusing = memory.Read<bool>(tempbomb + Offsets::client::m_bBeingDefused);
		tempdefuseTime = memory.Read<float>(tempbomb + Offsets::client::m_flDefuseLength);
		tempisTicking = memory.Read<bool>(tempbomb + Offsets::client::m_bBombTicking);
		uintptr_t node = memory.Read<uintptr_t>(tempbomb + Offsets::client::m_pGameSceneNode);
		Vec3 origin = memory.Read<Vec3>(node + Offsets::client::m_vecAbsOrigin);
		tempDefused = memory.Read<bool>(tempbomb + Offsets::client::m_bBombDefused);
		tempExploded = memory.Read<bool>(tempbomb + Offsets::client::m_bHasExploded);
		tempsOrigin = origin.worldToScreen(viewMatrix); if (tempsOrigin.z < 0.1f) continue;

		if (origin.IsZero()) continue;

		{
			std::unique_lock<std::shared_mutex> lock(Draw::BombMutex);
			bomb = tempbomb;
			planted = tempplanted;
			site = tempsite;
			isDefusing = tempisDefusing;
			defuseTime = tempdefuseTime;
			isTicking = tempisTicking;
			sOrigin = tempsOrigin;
			defused = tempDefused;
			exploded = tempExploded;
		}
	}
}

void BombInfo::DrawBombInfo()
{
    uintptr_t tempbomb;
    bool tempplanted;
    int tempsite;
    bool tempisDefusing;
    float tempdefuseTime;
    bool tempisTicking;
    Vec3 tempsOrigin;
    bool tempDefused;
    bool tempExploded;
    static float bombTimer = 40.0f;
    static float defuseTimer = 0;

    {
        std::unique_lock<std::shared_mutex> lock(Draw::BombMutex);
        tempbomb = bomb;
        tempplanted = planted;
        tempsite = site;
        tempisDefusing = isDefusing;
        tempdefuseTime = defuseTime;
        tempisTicking = isTicking;
        tempsOrigin = sOrigin;
        tempDefused = defused;
        tempExploded = exploded;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    if (Config::bombEsp && tempplanted) {
        static const ImVec2 offsets[] = { {-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f} };
        for (const auto& offset : offsets)
            drawList->AddText(Globals::WEAPONFont, 10, { tempsOrigin.x + offset.x, tempsOrigin.y + offset.y }, ImColor(0, 0, 0), "o");

        drawList->AddText(Globals::WEAPONFont, 10, { tempsOrigin.x, tempsOrigin.y }, ImColor(255, 0, 0), "o");

        char timerText[16];
        sprintf(timerText, "%.1f", bombTimer);
        std::string text = std::string(timerText) + "s";

        float percentage = bombTimer / 40.0f;
        int red = static_cast<int>((1.0f - percentage) * 255);
        int green = static_cast<int>(percentage * 255);
        ImColor timerColor(red, green, 0);

        if (!(tempExploded || tempDefused)) {
            ImVec2 textSize = Globals::ESPFont->CalcTextSizeA(10.f, FLT_MAX, 0.0f, text.c_str());
            for (const auto& offset : offsets)
                drawList->AddText(Globals::ESPFont, 10, { tempsOrigin.x - (textSize.x / 4) + offset.x, tempsOrigin.y + textSize.y + 1 + offset.y }, ImColor(0, 0, 0), text.c_str());

            drawList->AddText(Globals::ESPFont, 10, { tempsOrigin.x - (textSize.x / 4), tempsOrigin.y + textSize.y + 1 }, timerColor, text.c_str());
        }
    }

    if (tempplanted && tempisTicking) {
        bombTimer -= ImGui::GetIO().DeltaTime;
        bombTimer = max(0.0f, bombTimer);
    }
    if (tempisDefusing) {
        if (defuseTimer <= 0.0f) {
            defuseTimer = tempdefuseTime;
        }
        defuseTimer -= ImGui::GetIO().DeltaTime;
        defuseTimer = max(0.0f, defuseTimer);
    }
    else {
        defuseTimer = 0;
    }

    if (Config::showBombUi && (Config::showBombUiWhenPlantedOnly ? tempplanted : true)) {
        ImGui::SetNextWindowSize({ 200, 77 }, ImGuiCond_Always);
        ImGui::Begin("Bomb Timer", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImVec2 windowPos = ImGui::GetCursorScreenPos();
        ImVec2 center = { windowPos.x + 30, windowPos.y + 30 };
        float radius = 30.0f;
        float bombPercentage = bombTimer / 40.0f;
        float defusePercentage = defuseTimer / tempdefuseTime;
        int segments = 100;
        float angleStep = (2 * IM_PI) / segments;
        ImDrawList* wdrawList = ImGui::GetWindowDrawList();

        ImColor progressColor(180, 0, 255);
        ImColor backgroundColor(50, 0, 80);
        ImColor defuseColor(0, 255, 0);

        for (int i = 0; i < segments; i++) {
            float angleStart = i * angleStep - IM_PI / 2;
            float angleEnd = (i + 1) * angleStep - IM_PI / 2;
            ImVec2 p1 = { center.x + cos(angleStart) * radius, center.y + sin(angleStart) * radius };
            ImVec2 p2 = { center.x + cos(angleEnd) * radius, center.y + sin(angleEnd) * radius };
            wdrawList->AddLine(p1, p2, backgroundColor, 4.0f);
        }

        for (int i = 0; i < segments * bombPercentage; i++) {
            float angleStart = i * angleStep - IM_PI / 2;
            float angleEnd = (i + 1) * angleStep - IM_PI / 2;
            ImVec2 p1 = { center.x + cos(angleStart) * radius, center.y + sin(angleStart) * radius };
            ImVec2 p2 = { center.x + cos(angleEnd) * radius, center.y + sin(angleEnd) * radius };
            wdrawList->AddLine(p1, p2, progressColor, 4.0f);
        }

        if (tempisDefusing && tempplanted) {
            for (int i = 0; i < segments * defusePercentage; i++) {
                float angleStart = i * angleStep - IM_PI / 2;
                float angleEnd = (i + 1) * angleStep - IM_PI / 2;
                ImVec2 p1 = { center.x + cos(angleStart) * radius * 0.85f, center.y + sin(angleStart) * radius * 0.85f };
                ImVec2 p2 = { center.x + cos(angleEnd) * radius * 0.85f, center.y + sin(angleEnd) * radius * 0.85f };
                wdrawList->AddLine(p1, p2, defuseColor, 3.0f);
            }
        }

        wdrawList->AddText(Globals::WEAPONFont, 30, { center.x - 14, center.y - 15.5f }, ImColor(255, 255, 255), "o");

        ImGui::PushFont(Globals::ESPFont);
        if (tempplanted && tempisTicking) {
            static std::vector<std::string> sites = { "A", "B", "C" };

            ImGui::SetCursorPos({ 85, 7 });
            ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Bomb On: %s", sites[tempsite].c_str());

            ImGui::SetCursorPos({ 85, 22.5f });
            ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Bomb: %.1fs", bombTimer);

            ImGui::SetCursorPos({ 85, 37.5f });
            if (tempisDefusing) {
                ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Defuse: %.1fs", defuseTimer);
            }
            else {
                ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Not Defusing");
            }

            ImGui::SetCursorPos({ 85, 53 });
            if (tempisDefusing) {
                if (tempdefuseTime < bombTimer) {
                    ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Has Time");
                }
                else {
                    ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "No Time");
                }
            }
        }
        else if (tempDefused && tempplanted) {
            bombTimer = 40;
            defuseTimer = 0;
            ImGui::SetCursorPos({ 85, 12 });
            ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Bomb Defused");
        }
        else if (tempExploded && tempplanted) {
            bombTimer = 40;
            defuseTimer = 0;
            ImGui::SetCursorPos({ 85, 12 });
            ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Bomb Exploded");
        }
        else {
            bombTimer = 40;
            defuseTimer = 0;
            ImGui::SetCursorPos({ 85, 12 });
            ImGui::TextColored(ImVec4(0.7f, 0.0f, 1.0f, 1.0f), "Bomb Not Planted");
        }
        ImGui::PopFont();

        ImGui::End();
    }
}