#include "pch.h"

#include "Camera.hpp"

namespace Voortman3D {

	void Camera::updateViewMatrix()
	{
		glm::mat4 rotM = glm::mat4(1.0f);
		glm::mat4 transM;

		rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 translation = position;
		if (flipY) {
			translation.y *= -1.0f;
		}
		transM = glm::translate(glm::mat4(1.0f), translation);

		if (type == CameraType::firstperson)
		{
			matrices.view = rotM * transM;
		}
		else
		{
			matrices.view = transM * rotM;
		}

		viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

		updated = true;
	}

	void Camera::setPerspective(float fov, float aspect, float znear, float zfar)
	{
		this->fov = fov;
		this->znear = znear;
		this->zfar = zfar;
		matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		if (flipY) {
			matrices.perspective[1][1] *= -1.0f;
		}
	}

	void Camera::update(float deltaTime)
	{
		updated = false;
		updateViewMatrix();
	}


	bool Camera::updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
	{
		bool retVal = false;

		if (type == CameraType::firstperson)
		{
			// Use the common console thumbstick layout		
			// Left = view, right = move

			const float deadZone = 0.0015f;
			const float range = 1.0f - deadZone;

			glm::vec3 camFront;
			camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
			camFront.y = sin(glm::radians(rotation.x));
			camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
			camFront = glm::normalize(camFront);

			float moveSpeed = deltaTime * movementSpeed * 2.0f;
			float rotSpeed = deltaTime * rotationSpeed * 50.0f;

			// Move
			if (fabsf(axisLeft.y) > deadZone)
			{
				float pos = (fabsf(axisLeft.y) - deadZone) / range;
				position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
				retVal = true;
			}
			if (fabsf(axisLeft.x) > deadZone)
			{
				float pos = (fabsf(axisLeft.x) - deadZone) / range;
				position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
				retVal = true;
			}

			// Rotate
			if (fabsf(axisRight.x) > deadZone)
			{
				float pos = (fabsf(axisRight.x) - deadZone) / range;
				rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
				retVal = true;
			}
			if (fabsf(axisRight.y) > deadZone)
			{
				float pos = (fabsf(axisRight.y) - deadZone) / range;
				rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
				retVal = true;
			}
		}
		else
		{
			// todo: move code from example base class for look-at
		}

		if (retVal)
		{
			updateViewMatrix();
		}

		return retVal;
	}

}