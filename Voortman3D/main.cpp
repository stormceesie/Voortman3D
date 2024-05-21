#include "main.hpp"

namespace Voortman3D {
	Voortman3D::Voortman3D(HINSTANCE hInstance) : Voortman3DCore(hInstance) {
		this->title = L"Voortman3D";
		this->name = L"Voortman3D";
		this->icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	}
}