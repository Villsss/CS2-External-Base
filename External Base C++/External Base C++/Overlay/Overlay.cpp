#include "../include.h"

void overlay::registerWNDClass(HINSTANCE hInstance, WNDPROC Wndproc, LPCWSTR windowname) {
	this->hInstance = hInstance;

	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = Wndproc;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = windowname;

	if (RegisterClassExW(&windowClass)) {
		LOG_INFO("Registered WND Class");
		return;
	}

	LOG_ERROR("Could Not Register WND Class");
};

void overlay::createWindow() {
	HWND window = nullptr;

	int winx = GetSystemMetrics(SM_CXSCREEN);
	int winy = GetSystemMetrics(SM_CYSCREEN);

	window = CreateWindowExW(WS_EX_TOPMOST, windowClass.lpszClassName, windowClass.lpszClassName, WS_POPUP, 0, 0, winx, winy, 0, 0, windowClass.hInstance, 0);
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	if (window) {
		LOG_INFO("Window -> ", window);
	}
	else {
		LOG_ERROR("Could Not Create Window");
	}

	this->window = window;
}

void overlay::makeFrameIntoClientArea() {

	RECT clientArea{};
	GetClientRect(window, &clientArea);

	RECT windowArea{};
	GetWindowRect(window, &windowArea);

	POINT diff{};
	ClientToScreen(window, &diff);

	const MARGINS margins{
		windowArea.left + (diff.x - windowArea.left),
		windowArea.top + (diff.y - windowArea.top),
		clientArea.right,
		clientArea.bottom
	};

	DwmExtendFrameIntoClientArea(window, &margins);

	LOG_INFO("Extended Frame Into Client Area");
}


void overlay::makeDeviceAndSwapChain() {
	DEVMODE devMode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

	LOG_INFO("Found Framerate -> ", devMode.dmDisplayFrequency);

	swapChain.BufferDesc.RefreshRate.Numerator = devMode.dmDisplayFrequency;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.SampleDesc.Count = 1;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 2;
	swapChain.OutputWindow = window;
	swapChain.Windowed = TRUE;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChain, &loadedSwapChain, &device, &loadedLevel, &deviceContext) == S_OK) {
		LOG_INFO("Created Device And Swap Chain");
	}
	else {
		LOG_ERROR("Could Not Create Device And Swap Chain");
	}

	loadedSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (backBuffer) {
		LOG_INFO("backBuffer -> ", backBuffer);
		device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
		backBuffer->Release();
	}
	else {
		LOG_ERROR("Could Not Get backBuffer");
	}
};

void overlay::initWindow(int nShowCmd) {
	ShowWindow(window, nShowCmd);
	UpdateWindow(window);

	LOG_INFO("nShowCmd -> ", nShowCmd);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	ImFont* arialFont = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyhbd.ttc", 14.f);
	Globals::ESPFont = arialFont;

	LOG_INFO("Globals::ESPFont -> ", Globals::ESPFont);

	std::filesystem::path fsPath = Globals::GetWorkingPath() / "Util";
	if (!std::filesystem::is_directory(fsPath))
	{
		LOG_WARNING("Directory not found");
		std::filesystem::remove(fsPath);
		if (!std::filesystem::create_directories(fsPath))
		{
			LOG_WARNING("Could Not Create Directories");
		}
		else 
		{
			LOG_INFO("Created Working Path -> ", Globals::GetWorkingPath().string() + "\\");
		}
	}

	LOG_INFO("Working Path -> ", Globals::GetWorkingPath().string() + "\\");

	std::string fullPath = fsPath.string() + "\\weaponIcons.ttf";
	Globals::ExecuteCmdCommand("curl -L -o \"" + fullPath + "\" \"https://raw.githubusercontent.com/Villsss/CS2-External-Base/main/weaponIcons.ttf\"");

	ImFont* weapon = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), 24.f);
	Globals::WEAPONFont = weapon;

	if (!Globals::WEAPONFont) {
		LOG_ERROR("Could Not Get Globals::WEAPONFont");
	}
	else {
		LOG_INFO("Globals::WEAPONFont -> ", Globals::WEAPONFont);
	}
	
	ImGui::StyleColorsDark();

	if (ImGui_ImplWin32_Init(window)) {
		LOG_INFO("Called ImGui_ImplWin32_Init");
	}
	else {
		LOG_ERROR("Could Not Call ImGui_ImplWin32_Init");
	}

	if (ImGui_ImplDX11_Init(device, deviceContext)) {
		LOG_INFO("Called ImGui_ImplDX11_Init");
	}
	else {
		LOG_ERROR("Could Not Call ImGui_ImplDX11_Init");
	}
}

void overlay::renderLoop()
{
	bool state = true;
	bool check = false;

	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	SetPriorityClass(window, HIGH_PRIORITY_CLASS);

	memory.GetHandle("cs2.exe");
	memory.clientDLL = memory.GetModuleAddress("client.dll");
	memory.engineDLL = memory.GetModuleAddress("engine2.dll");

	uintptr_t dwBuildNumber = memory.Read<uintptr_t>(memory.engineDLL + Offsets::dwBuildNumber);

	LOG_INFO("dwBuildNumber -> ", dwBuildNumber);

	if (dwBuildNumber == (uintptr_t)14072) {
		LOG_INFO("dwBuildNumber = Current Cheat Version");
	}
	else {
		LOG_WARNING("dwBuildNumber != Current CheatVersion And It Might Not Work");
	}

	// entity list
	std::thread([]() {
		LOG_INFO("Started Entity Loop");
		List.StartEntityLoop();
		}).detach();

	// cheat threads
	std::thread([]() {
		LOG_INFO("Started PlayerEsp Loop");
		PlayerEsp::RunPlayerEsp();
		}).detach();

	std::thread([]() {
		LOG_INFO("Started Aimbot Loop");
		Aimbot::RunAimbot();
		}).detach();

	std::thread([]() {
		LOG_INFO("Started ItemEsp Loop");
		ItemEsp::RunItemEsp();
		}).detach();

	std::thread([]() {
		LOG_INFO("Started Trigger Loop");
		Triggerbot::RunTriggerbot();
		}).detach();

	std::thread([]() {
		LOG_INFO("Started Spectator Loop");
		SpectatorList::GetSpectators();
		}).detach();

	std::thread([]() {
		LOG_INFO("Started BombInfo Loop");
		BombInfo::GetBombInfo();
		}).detach();

	while (state) {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			menutoggle = !menutoggle;
		}

		MSG msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				state = false;
			}

			if (!state) {
				break;
			}
		}

		if (GetAsyncKeyState(VK_END)) {
			this->destroyWindow();
			LOG_INFO("Exiting Cheat");
			exit(0);
		}

		std::this_thread::sleep_for(std::chrono::nanoseconds(1));

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Run::Cheats();

		if (menutoggle) {
			renderMenu();
			SetWindowLong(this->window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}
		else {
			SetWindowLong(this->window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		}

		ImGui::EndFrame();
		ImGui::Render();

		float color[4]{ 0.f,0.f ,0.f ,0.f };
		deviceContext->OMSetRenderTargets(1, &renderTargetView, 0);
		deviceContext->ClearRenderTargetView(renderTargetView, color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		loadedSwapChain->Present(0, 0);

		if (!check) {
			check = !check;
		}
	}
}

void overlay::destroyWindow() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (loadedSwapChain) {
		loadedSwapChain->Release();
	}
	if (deviceContext) {
		deviceContext->Release();
	}
	if (device) {
		device->Release();
	}
	if (renderTargetView) {
		renderTargetView->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
}
