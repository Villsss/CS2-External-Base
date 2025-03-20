#include "../include.h"

void SetPurpleStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.05f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.1f, 0.4f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.1f, 0.6f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.5f, 0.2f, 0.8f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.7f, 0.3f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.6f, 0.2f, 0.9f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.1f, 0.3f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.2f, 0.6f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.5f, 0.3f, 0.7f, 1.0f);
}

void __fastcall overlay::renderMenu()
{
    SetPurpleStyle();

    ImGui::Begin("External Base C++");
    {
        ImGui::BeginTabBar("Feature");
        {
            if (ImGui::BeginTabItem("Aimbot"))
            {
                ImGui::Checkbox("Aim Toggle", &Config::aimToggle);
                ImGui::Checkbox("Aim With Key", &Config::aimWithKeyToggle);
                ImGui::SliderFloat("Aim Smoothing", &Config::smoothing, 1, 50);
                HotKey::HotkeySelector("Aim Key", Config::aimKey);

                ImGui::Text("Rcs Stuff");
                ImGui::Checkbox("Rcs Toggle", &Config::rcsToggle);
                ImGui::SliderFloat("Rcs Scale X", &Config::rcsX, 0, 1);
                ImGui::SliderFloat("Rcs Scale Y", &Config::rcsY, 0, 1);

                ImGui::Checkbox("Fov Toggle", &Config::fovtoggle);
                ImGui::Checkbox("Fov Follow Recoil", &Config::movingFov);
                ImGui::SliderFloat("Aim Fov", &Config::fov, 0, 100);
                ImGui::Checkbox("Aim At Team", &Config::aimTeamCheck);

                ImGui::Checkbox("Trigger Toggle", &Config::triggerToggle);
                ImGui::Checkbox("Use After Shot Delay", &Config::useTriggerAfterShotDelay);
                ImGui::Checkbox("Trigger On Team", &Config::triggerOnTeam);
                ImGui::SliderInt("Trigger Delay", &Config::triggerDelay, 0, 300);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Players"))
            {
                ImGui::Checkbox("Box Esp toggle", &Config::boxToggle);
                ImGui::Checkbox("Name Esp toggle", &Config::nameToggle);
                ImGui::Checkbox("Health Bar toggle", &Config::healthBarToggle);
                ImGui::Checkbox("Show Team", &Config::espTeamCheck);
                ImGui::Checkbox("Active Weapon Esp", &Config::activeWeaponEsp);
                ImGui::Checkbox("Util Esp", &Config::utilEsp);
                ImGui::Checkbox("Distance Esp", &Config::distanceEsp);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc"))
            {
                ImGui::Checkbox("Grenade Esp toggle", &Config::nadeEsp);
                ImGui::Checkbox("Item Esp toggle", &Config::droppedItemEsp);
                ImGui::SliderFloat("Max Item Distance", &Config::maxitemDistance, 0, 100);

                ImGui::EndTabItem();
            }

        }ImGui::EndTabBar();

    }ImGui::End();
    
}