#pragma once

#include "Camera.hpp"

class CameraMC : public Camera {

public:
	CameraMC(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f)
		: Camera(fov, aspectRatio, nearPlane, farPlane) {}

	void update(float dt) override;

private:
	float m_speed = 5.0f;
	float m_sensitivity = 0.1f;
};
