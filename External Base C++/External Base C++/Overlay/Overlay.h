#pragma once

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class overlay {
public:

	HINSTANCE hInstance;
	WNDCLASSEXW windowClass;
	HWND window;

	DXGI_SWAP_CHAIN_DESC swapChain{};
	D3D_FEATURE_LEVEL featureLevels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};
	ID3D11Device* device{ 0 };
	ID3D11DeviceContext* deviceContext{ 0 };
	IDXGISwapChain* loadedSwapChain{ 0 };
	ID3D11RenderTargetView* renderTargetView{ 0 };
	D3D_FEATURE_LEVEL loadedLevel{};
	ID3D11Texture2D* backBuffer{ 0 };

	bool menutoggle;

	void registerWNDClass(HINSTANCE hInstance, WNDPROC Wndproc, LPCWSTR windowname);

	void createWindow();

	void makeFrameIntoClientArea();

	void makeDeviceAndSwapChain();

	void initWindow(int nShowCmd);

	void renderLoop();

	void __fastcall renderMenu();

	void destroyWindow();
};
