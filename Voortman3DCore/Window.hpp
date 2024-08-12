#pragma once
#include "pch.hpp"

namespace Voortman3D {
	class Window {
	public:
		Window(HINSTANCE hInstance,
			WNDPROC WndProc,
			HICON Icon,
			const std::wstring& name,
			const std::wstring& title,
			uint32_t height,
			uint32_t width);

		const inline HWND window() noexcept { return hWnd; }

	private:
		HWND hWnd;
	};
}