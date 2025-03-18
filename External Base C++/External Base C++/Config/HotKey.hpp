#pragma once

namespace HotKey
{
	inline static const char* activeHotkeyLabel = nullptr;

	inline std::string GetKeyNameExtended(int virtualKey) {
		switch (virtualKey) {
		case VK_LBUTTON: return "Mouse Left";
		case VK_RBUTTON: return "Mouse Right";
		case VK_MBUTTON: return "Mouse Middle";
		case VK_XBUTTON1: return "Mouse X1";
		case VK_XBUTTON2: return "Mouse X2";
		default: {
			char name[128];
			if (GetKeyNameTextA(MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC) << 16, name, sizeof(name)) > 0) {
				if (std::string(name) == "ESC")
					return "None";
				return std::string(name);
			}
			return "None";
		}
		}
	}

	inline bool IsKeyPressedExtended(int virtualKey) {
		return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
	}

	inline void HotkeySelector(const char* label, int& hotkey) {
		bool isWaiting = (activeHotkeyLabel == label);

		if (ImGui::Button(label)) {
			if (isWaiting) {
				activeHotkeyLabel = nullptr;
			}
			else {
				activeHotkeyLabel = label;
			}
		}

		ImGui::SameLine();
		ImGui::Text("Hotkey: %s", (hotkey != -1) ? GetKeyNameExtended(hotkey).c_str() : "None");

		if (isWaiting) {
			ImGui::Text("Press any key or mouse button...");
			for (int i = 0; i < 256; ++i) { 
				if (IsKeyPressedExtended(i)) {
					hotkey = i; 
					activeHotkeyLabel = nullptr;
					return;
				}
			}

			if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
				activeHotkeyLabel = nullptr;
			}
		}
	}
}