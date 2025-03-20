#include "../include.h"

void overlay::registerWNDClass(HINSTANCE hInstance, WNDPROC Wndproc, LPCWSTR windowname) {
	this->hInstance = hInstance;

	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = Wndproc;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = windowname;

	RegisterClassExW(&windowClass);
};

inline std::vector<char> ReadBinaryFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate); // Open file in binary mode, move to end

	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return {};
	}

	std::streamsize fileSize = file.tellg(); // Get file size
	file.seekg(0, std::ios::beg); // Move back to start

	std::vector<char> buffer(fileSize);
	if (!file.read(buffer.data(), fileSize)) {
		std::cerr << "Failed to read file: " << filename << std::endl;
		return {};
	}

	return buffer;
}

inline void SaveBinaryToTextFile(const std::vector<char>& data, const std::string& outputFile) {
	std::ofstream outFile(outputFile, std::ios::binary); // Open in binary mode
	if (!outFile) {
		std::cerr << "Failed to open output file: " << outputFile << std::endl;
		return;
	}

	outFile.write(data.data(), data.size()); // Write binary data
	std::cout << "Successfully saved " << data.size() << " bytes to " << outputFile << std::endl;
}
std::filesystem::path GetWorkingPath()
{
	std::filesystem::path fsWorkingPath;
	// get path to user documents
	if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
	{
		fsWorkingPath = std::filesystem::path(pszPathToDocuments);  // Assign path
		fsWorkingPath /= "External Base C++";  // Use /= for proper path joining
		CoTaskMemFree(pszPathToDocuments);
	}

	return fsWorkingPath;
}

void overlay::createWindow() {
	HWND window = nullptr;

	int winx = GetSystemMetrics(SM_CXSCREEN);
	int winy = GetSystemMetrics(SM_CYSCREEN);

	window = CreateWindowExW(WS_EX_TOPMOST, windowClass.lpszClassName, windowClass.lpszClassName, WS_POPUP, 0, 0, winx, winy, 0, 0, windowClass.hInstance, 0);
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	this->window = window;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
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
}


void overlay::makeDeviceAndSwapChain() {

	swapChain.BufferDesc.RefreshRate.Numerator = 240;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.SampleDesc.Count = 1;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 2;
	swapChain.OutputWindow = window;
	swapChain.Windowed = TRUE;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChain, &loadedSwapChain, &device, &loadedLevel, &deviceContext);

	loadedSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (backBuffer) {
		device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
		backBuffer->Release();
	}
};

inline std::wstring StringToWString(const std::string& str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}

void overlay::initWindow(int nShowCmd) {
	ShowWindow(window, nShowCmd);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	ImFont* arialFont = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyhbd.ttc", 14.f);
	Globals::ESPFont = arialFont;

	std::filesystem::path fsPath = GetWorkingPath() / "Util";
	if (!std::filesystem::is_directory(fsPath))
	{
		std::filesystem::remove(fsPath);
		if (!std::filesystem::create_directories(fsPath))
		{
			exit(1);
		}
	}

	std::string fullPath = fsPath.string() + "\\weaponIcons.ttf";
	Globals::ExecuteCmdCommand("curl -L -o \"" + fullPath + "\" \"https://raw.githubusercontent.com/Villsss/CS2-External-Base/main/weaponIcons.ttf\"");

	ImFont* weapon = io.Fonts->AddFontFromFileTTF(fullPath.c_str(), 24.f);
	Globals::WEAPONFont = weapon;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, deviceContext);
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

	// entity list
	std::thread([]() {
		List.StartEntityLoop();
		}).detach();

	// cheat threads
	std::thread([]() {
		PlayerEsp::RunPlayerEsp();
		}).detach();

	std::thread([]() {
		Aimbot::RunAimbot();
		}).detach();

	std::thread([]() {
		ItemEsp::RunItemEsp();
		}).detach();

	std::thread([]() {
		Triggerbot::RunTriggerbot();
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
