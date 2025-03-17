#pragma once
#define WIN32_LEAN_AND_MEAN
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

// imgui
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_impl_dx11.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_internal.h"

// memory
#include "Memory/Memory.h"

// Vector
#include "Vector/Vector.h"

// Overlay
#include "Overlay/Overlay.h"

// Config
#include "Config/Config.hpp"

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

		std::cout << "Console allocated successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to allocate console." << std::endl;
	}
}

// Offsets
#include "Offsets/UsedOffsets.hpp"

// cheat entry
#include "Cheats/Entry.h"
