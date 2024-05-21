// Voortman3DCore.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "Voortman3DCore.hpp"

namespace Voortman3D {
	std::vector<const char*> Voortman3DCore::args;

	Voortman3DCore::Voortman3DCore(HINSTANCE hInstance) : hInstance{hInstance} {

		// Setup console if we are in debug mode
#ifdef _DEBUG
		setupConsole(L"Debug Console");
#endif
		setupDPIAwareness();
	}

	void Voortman3DCore::setupConsole(const std::wstring& title) {
		AllocConsole();
		AttachConsole(GetCurrentProcessId());

		FILE* stream;
		freopen_s(&stream, "CONIN$", "r", stdin);
		freopen_s(&stream, "CONOUT$", "w+", stdout);
		freopen_s(&stream, "CONOUT$", "w+", stderr);
		// Enable flags so we can color the output
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(consoleHandle, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(consoleHandle, dwMode);
		SetConsoleTitle(title.c_str());
	}

	void Voortman3DCore::setupDPIAwareness() {
		typedef HRESULT* (__stdcall* SetProcessDpiAwarenessFunc)(PROCESS_DPI_AWARENESS);

		HMODULE shCore = LoadLibraryA("Shcore.dll");
		if (shCore)
		{
			SetProcessDpiAwarenessFunc setProcessDpiAwareness =
				(SetProcessDpiAwarenessFunc)GetProcAddress(shCore, "SetProcessDpiAwareness");

			if (setProcessDpiAwareness != nullptr)
			{
				setProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			}

			FreeLibrary(shCore);
		}
	}

	Voortman3DCore::~Voortman3DCore() {
	}

	void Voortman3DCore::initVulkan() {
#ifdef _DEBUG
		STARTCOUNTER("Initializing Vulkan")
#endif

		// Initialize Vulkan

#ifdef _DEBUG
		STOPCOUNTER()
#endif
	}

	void Voortman3DCore::setupWindow(WNDPROC WndProc) {
		// If window doesn't exist create a new unique ptr
		if (!window)
			window = std::make_unique<Window>(hInstance, WndProc, icon, name, title, height, width);
	}

	void Voortman3DCore::prepare() {
#ifdef _DEBUG
		STARTCOUNTER("Final Preperations")
#endif

		// Prepare...

#ifdef _DEBUG
		STOPCOUNTER()
#endif
	}

	void Voortman3DCore::renderLoop() {
		MSG msg;
		bool quitMessageReceived = false;
		while (!quitMessageReceived) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					quitMessageReceived = true;
					break;
				}
			}
		}
	}

	void Voortman3DCore::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_CLOSE:
			DestroyWindow(window->window());
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			ValidateRect(window->window(), NULL);
			break;
		}
	}
}
