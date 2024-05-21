// Voortman3DCore.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "Voortman3DCore.hpp"

namespace Voortman3D {
	std::vector<const char*> Voortman3DCore::args;

	Voortman3DCore::Voortman3DCore() {

	}

	Voortman3DCore::~Voortman3DCore() {

	}

	void Voortman3DCore::initVulkan() {

	}

	void Voortman3DCore::setupWindow(HINSTANCE hInstance, WNDPROC WndProc) {
		WNDCLASSEX wndClass;

		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = name.c_str();
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&wndClass))
		{
			std::cout << "Could not register window class!\n";
			fflush(stdout);
			exit(1);
		}

		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		RECT windowRect;
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = (long)width;
		windowRect.bottom = (long)height;

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		hWnd = CreateWindowEx(0,
			name.c_str(),
			title.c_str(),
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL, 
			hInstance,
			NULL);

		if (!hWnd) {
			printf("Could not create a window!\n");
			fflush(stdout);
			return;
		}

		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
	}

	void Voortman3DCore::prepare() {

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

	}
}
