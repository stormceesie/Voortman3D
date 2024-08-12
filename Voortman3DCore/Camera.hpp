// Class derived from: 

/*
* Basic camera class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// This class is based of Sacha Willems - Florent Kegler

#include "pch.hpp"

namespace Voortman3D {
	class Camera
	{
	private:
		float fov;
		float znear, zfar;

		void updateViewMatrix() noexcept;
	public:
		glm::vec3 rotation = glm::vec3();
		glm::vec3 position = glm::vec3();
		glm::vec4 viewPos = glm::vec4();

		float rotationSpeed = 1.0f;
		float movementSpeed = 1.0f;

		bool updated = false;
		bool flipY = false;

		struct
		{
			glm::mat4 perspective;
			glm::mat4 view;
		} matrices;

		struct
		{
			bool left = false;
			bool right = false;
			bool up = false;
			bool down = false;
		} keys;

		_NODISCARD inline const bool moving() const noexcept { return keys.left || keys.right || keys.up || keys.down; }


		_NODISCARD inline const float getNearClip() const noexcept { return znear; }

		_NODISCARD inline const float getFarClip() const noexcept { return zfar; }

		void setPerspective(const float fov, const float aspect, const float znear, const float zfar) noexcept;

		inline void updateAspectRatio(const float aspect) noexcept
		{
			matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
			if (flipY) _LIKELY {
				matrices.perspective[1][1] *= -1.0f;
			}
		}

		void setPosition(glm::vec3 position) noexcept
		{
			this->position = position;
			updateViewMatrix();
		}

		void setRotation(glm::vec3 rotation) noexcept
		{
			this->rotation = rotation;
			updateViewMatrix();
		}

		void rotate(glm::vec3 delta) noexcept
		{
			this->rotation += delta;
			updateViewMatrix();
		}

		void setTranslation(glm::vec3 translation) noexcept
		{
			this->position = translation;
			updateViewMatrix();
		};

		void translate(glm::vec3 delta) noexcept
		{
			this->position += delta;
			updateViewMatrix();
		}

		inline void setRotationSpeed(float rotationSpeed) noexcept
		{
			this->rotationSpeed = rotationSpeed;
		}

		inline void setMovementSpeed(float movementSpeed) noexcept
		{
			this->movementSpeed = movementSpeed;
		}

		void update(float deltaTime) noexcept;
	};
}