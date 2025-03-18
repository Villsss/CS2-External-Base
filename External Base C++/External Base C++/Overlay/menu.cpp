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
                ImGui::Text("Aimbot settings go here.");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Players"))
            {
                ImGui::Checkbox("Box Esp toggle", &Config::boxToggle);
                ImGui::Checkbox("Name Esp toggle", &Config::nameToggle);
                ImGui::Checkbox("Health Bar toggle", &Config::healthBarToggle);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc"))
            {
                ImGui::Text("Misc settings go here.");

                ImGui::EndTabItem();
            }

        }ImGui::EndTabBar();

    }ImGui::End();
    
}