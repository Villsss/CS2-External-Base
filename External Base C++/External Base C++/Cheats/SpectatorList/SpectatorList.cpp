#include "../../include.h"

float sizeX;
float sizeY;

void SpectatorList::GetSpectators() {
	for (;;) {
		Sleep(10);

		std::vector<std::string> spectatorsTemp;

		const auto& entityList = memory.Read<uintptr_t>(memory.clientDLL + Offsets::dwEntityList);
		if (!entityList) continue;

		const auto& localPlayerController = memory.Read<uintptr_t>(memory.clientDLL + Offsets::dwLocalPlayerController);
		if (!localPlayerController) continue;

		uintptr_t localPlayerPawnHandle = memory.Read<uintptr_t>(localPlayerController + Offsets::client::m_hPawn);
		uintptr_t list_entry2 = memory.Read<uintptr_t>(entityList + 0x8 * ((localPlayerPawnHandle & 0x7FFF) >> 9) + 16);
		if (!list_entry2) continue;

		const uintptr_t localPlayerPawn = memory.Read<uintptr_t>(list_entry2 + 120 * (localPlayerPawnHandle & 0x1FF));

		std::vector<desiredPlayerValues> playerListTemp1 = List.playerList;
		for (auto& list1 : playerListTemp1) {
			uintptr_t obs = memory.Read<uintptr_t>(list1.playerPawn + Offsets::client::m_pObserverServices);
			uint64_t oTarget = memory.Read<uint64_t>(obs + Offsets::client::m_hObserverTarget);
			uintptr_t handle = 0;

			if (obs) {
				uintptr_t listEntryThird = memory.Read<uintptr_t>(entityList + 0x8 * ((oTarget & 0x7FFF) >> 9) + 16);
				handle = listEntryThird == 0 ? 0 : memory.Read<uintptr_t>(listEntryThird + 120 * (oTarget & 0x1FF));
			}

			if (Config::onlyLocalSpectators) {
				if (handle == localPlayerPawn) {
					std::string spectatorEntry = list1.playerName;
					spectatorsTemp.push_back(spectatorEntry);
				}
			}
			else {
				std::vector<desiredPlayerValues> playerListTemp2 = List.playerList;
				for (auto& list2 : playerListTemp2) {
					if (handle == list2.playerPawn) {
						std::string spectatorEntry = list1.playerName + " -> " + list2.playerName;
						spectatorsTemp.push_back(spectatorEntry);

						float entryWidth = ImGui::CalcTextSize(spectatorEntry.c_str()).x;
						if (entryWidth > sizeX)
							sizeX = entryWidth;
					}
				}
			}
		}

		if(spectatorsTemp.empty())
			spectatorsTemp.push_back("No Current Spectators");

		{
			std::unique_lock<std::shared_mutex> lock(Draw::SpectatorMutex);
			spectators.swap(spectatorsTemp);
		}
	}
}

void SpectatorList::DrawSpectatorList() {
	if (!Config::spectatorListToggle) {
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowPos({ 0.f, 200.f }, ImGuiCond_FirstUseEver);

	ImGuiStyle& style2 = ImGui::GetStyle();
	style2.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.f);
	style2.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style2.WindowRounding = 5.f;

	std::vector<std::string> spectatorsTemp;
	{
		std::unique_lock<std::shared_mutex> lock(Draw::SpectatorMutex);
		spectatorsTemp = spectators;
	}
	float textHeight = 0.0f;
	if (!spectatorsTemp.empty()) {
		ImGui::PushFont(Globals::ESPFont);
		for (const auto& name : spectatorsTemp) {
			textHeight += ImGui::CalcTextSize(name.c_str()).y * 1.45f; 
		}
		ImGui::PopFont();
	}

	float windowHeight = max(30.f + textHeight, 35.f); 

	float x = 170;
	if (sizeX > 170)
		x = sizeX;

	ImGui::SetNextWindowSize({ x, windowHeight }, ImGuiCond_Always);

	if (ImGui::Begin("Spectators", nullptr, flags)) {
		if (!spectatorsTemp.empty()) {
			ImGui::PushFont(Globals::ESPFont);
			for (const auto& name : spectatorsTemp) {
				ImGui::SetCursorPosX(10);
				ImGui::TextColored(ImColor(255, 255, 255), name.c_str());
			}
			ImGui::PopFont();
		}
	}
	ImGui::End();
}