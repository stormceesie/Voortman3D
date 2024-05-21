#pragma once
#include "pch.h"
#include "Window.hpp"

namespace Voortman3D {
	class Voortman3DCore {
	public:
		static std::vector<const char*> args;

		std::wstring title = L"Title Not Set";
		std::wstring name = L"Name Not Set";

		uint32_t width = 1280;
		uint32_t height = 720;

		/// <summary>
		/// Constructor of the Voortman3DCore app
		/// </summary>
		/// <param name="hInstance">hInstance goed through the constructor of the class so that childs of this class can use it already while initializing</param>
		Voortman3DCore(HINSTANCE hInstance);
		
		/// <summary>
		/// Virtual deconstructor so that childs can tailor the constructor to their exact use
		/// </summary>
		virtual ~Voortman3DCore();

		/// <summary>
		/// Function to initialize Vulkan.
		/// </summary>
		void initVulkan();

		/// <summary>
		/// Function that will setup the WIN32 window
		/// </summary>
		/// <param name="WndProc">WNDPROC function that will be called each frame</param>
		void setupWindow(WNDPROC WndProc);

		/// <summary>
		/// Function that does some preperation like loading 3D models etc
		/// </summary>
		void prepare();

		/// <summary>
		/// Renderloop function. This function will endure the entire duration of the program.
		/// </summary>
		void renderLoop();

		/// <summary>
		/// Function that will handle the WIN32 messages
		/// </summary>
		/// <param name="hWnd">HWND pointer for use</param>
		/// <param name="uMsg">Message ID</param>
		/// <param name="wParam">Parameters</param>
		/// <param name="lParam">Parameters</param>
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HICON icon;

	private:
		HINSTANCE hInstance;

		std::unique_ptr<Window> window;

		void setupDPIAwareness();
		void setupConsole(const std::wstring& title);
	};
}

#define VOORTMAN_3D_MAIN()																		  \
Voortman3D::Voortman3D* voortman3D;																  \
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {					  \
	if (voortman3D != NULL) {																	  \
		voortman3D->handleMessages(hWnd, uMsg, wParam, lParam);									  \
	}																							  \
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));											  \
}																								  \
																								  \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {								  \
	for (int32_t i = 0; i < __argc; i++) { Voortman3D::Voortman3D::args.push_back(__argv[i]); };  \
	voortman3D = new Voortman3D::Voortman3D(hInstance);											  \
	voortman3D->initVulkan();																	  \
	voortman3D->setupWindow(WndProc);															  \
	voortman3D->prepare();																		  \
	voortman3D->renderLoop();																	  \
	delete(voortman3D);																			  \
	return 0;																					  \
}																								