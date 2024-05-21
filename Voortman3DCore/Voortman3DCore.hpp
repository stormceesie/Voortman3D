#pragma once
#include "pch.h"

namespace Voortman3D {
	class Voortman3DCore {
	public:
		static std::vector<const char*> args;
		std::wstring title = L"Title Not Set";
		std::wstring name = L"Name Not Set";

		uint32_t width = 1280;
		uint32_t height = 720;

		Voortman3DCore();
		~Voortman3DCore();

		void initVulkan();
		void setupWindow(HINSTANCE hInstance, WNDPROC WndProc);
		void prepare();
		void renderLoop();
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		HINSTANCE hInstance;
		HWND hWnd;
	};
}

#define VOORTMAN_3D_MAIN()																		\
Voortman3D::Voortman3D* voortman3D;																\
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {					\
	if (voortman3D != NULL) {																	\
		voortman3D->handleMessages(hWnd, uMsg, wParam, lParam);									\
	}																							\
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));											\
}																								\
																								\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {								\
for (int32_t i = 0; i < __argc; i++) { Voortman3D::Voortman3D::args.push_back(__argv[i]); };	\
	voortman3D = new Voortman3D::Voortman3D();													\
	voortman3D->initVulkan();																	\
	voortman3D->setupWindow(hInstance, WndProc);												\
	voortman3D->prepare();																		\
	voortman3D->renderLoop();																	\
	delete(voortman3D);																			\
	return 0;																					\
}																								