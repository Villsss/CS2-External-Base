#include "../include.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	allocateConsole();

	overlay overlayClass;
	overlayClass.registerWNDClass(hInstance, Wndproc, L"External Base WND");
	overlayClass.createWindow();

	overlayClass.makeFrameIntoClientArea();
	overlayClass.makeDeviceAndSwapChain();
	overlayClass.initWindow(nShowCmd);

	overlayClass.renderLoop();
}