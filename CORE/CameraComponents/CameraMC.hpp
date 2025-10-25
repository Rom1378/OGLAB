#pragma once

#include "Camera.hpp"

class CameraMCComponent : public CameraComponent {

public:
	CameraMCComponent(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f)
		: CameraComponent(fov, aspectRatio, nearPlane, farPlane) {
		//m_attachedToGameObject = false;
	}

	void update(float dt) override;

	const char* getName() override { return "CameraMC"; }

private:
	//float m_speed = 5.0f;
	float m_sensitivity = 0.1f;
};
