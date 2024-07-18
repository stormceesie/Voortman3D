#include "main.hpp"

namespace Voortman3D {
	Voortman3D::Voortman3D(HINSTANCE hInstance) : Voortman3DCore(hInstance) {
		this->title = L"Voortman3D";
		this->name = L"Voortman3D";
		this->icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

		camera.type = Camera::CameraType::lookat;
		camera.flipY = true;
		camera.setPosition(glm::vec3(0.0f, -0.1f, -1.0f));
		camera.setRotation(glm::vec3(0.0f, 45.0f, 0.0f));
		camera.setPerspective(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 256.0f);
	}
}