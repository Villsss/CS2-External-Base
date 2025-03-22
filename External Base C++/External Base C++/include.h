#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable : 6001 )
#pragma warning (disable : 4244 )

// apis
#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <format>
#include <random>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <vector>
#include <d3d11.h>
#include <dwmapi.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>
#include <tchar.h>
#include <memory>
#include <string>
#include <winternl.h>
#include <numbers>
#include <math.h>
#include <algorithm>
#include <xstring>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <any>
#include <array>
#include <format>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <cmath>
#include <limits>
#include <map>
#include <filesystem>
#include <shlobj_core.h>
#include <KnownFolders.h>
#include <Psapi.h>
#include <DirectXMath.h>
#include <shlobj.h>

// imgui
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_impl_dx11.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_internal.h"

// logger
#include "Logger/Logger.h"

// memory
#include "Memory/Memory.h"

// Vector
#include "Vector/Vector.h"

// Overlay
#include "Overlay/Overlay.h"

// Config
#include "Config/Config.hpp"

// HotKeys
#include "Config/HotKey.hpp"

// wndProc
inline LRESULT Wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return 0;
	}
	switch (msg) {
	case WM_SYSCOMMAND: {
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Console
inline void allocateConsole() {
	if (AllocConsole()) {
		// Redirect standard input, output, and error streams to the console
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		freopen_s(&fp, "CONIN$", "r", stdin);

		LOG_INFO("Console allocated successfully!");
	}
	else {
		LOG_ERROR("Failed to allocate console.");
	}
}

// bone data
#include "Cheats/Bone.hpp"

// weapon data
#include "Cheats/Weapon.hpp"

// Offsets
#include "Offsets/UsedOffsets.hpp"

// cheat entry
#include "Cheats/Entry.h"

// entity list 
#include "EntityList/EntityList.h"

// draw
#include "Draw/Draw.h"

// cheats
#include "Cheats/PlayerEsp/PlayerEsp.h"
#include "Cheats/Aimbot/Aimbot.h"
#include "Cheats/ItemEsp/ItemEsp.h"
#include "Cheats/Triggerbot/Triggerbot.h"
#include "Cheats/SpectatorList/SpectatorList.h"
#include "Cheats/BombInfo/BombInfo.h"

// globals 
namespace Globals
{
	inline ImFont* ESPFont;
	inline ImFont* WEAPONFont;

	inline int LocalPlayerIndex;

	inline float m_fSensitivity;

	inline void ExecuteCmdCommand(const std::string& command) {
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;

		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		std::string cmd = "cmd.exe /c " + command;

		BOOL result = CreateProcess(
			NULL,          
			&cmd[0],           
			NULL,            
			NULL,             
			FALSE,           
			CREATE_NO_WINDOW,    
			NULL,             
			NULL,               
			&si,                 
			&pi                 
		);

		if (result) {
			WaitForSingleObject(pi.hProcess, INFINITE);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}

	inline std::filesystem::path GetWorkingPath()
	{
		std::filesystem::path fsWorkingPath;
		if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
		{
			fsWorkingPath = std::filesystem::path(pszPathToDocuments);
			fsWorkingPath /= "External Base C++";
			CoTaskMemFree(pszPathToDocuments);
		}

		return fsWorkingPath;
	}
}
