/*#pragma once


#include "CORE/Controllers/SingleTargetController.hpp"
#include "CORE/CameraComponents/Camera.hpp"\


class CameraRotationController : public SingleTargetController {

public:
	CameraRotationController(const std::shared_ptr<GameObject>& go = nullptr) : SingleTargetController(go) {}
	virtual ~CameraRotationController() override {};


	void update(float dt) override {

		if (!m_targetObj || !Input::isMouseLocked())
			return;

		const auto& camComp = m_targetObj->getComponent<CameraComponent>();
		if (!camComp) {
			LOG_WARN("PlayerController: m_targetObj has no CameraComponent");
			return;
		}


		glm::vec2 mouseDelta = Input::getMouseDelta();
		float sensitivity = camComp->getSensitivity();
		float yaw = camComp->getYaw();
		float pitch = camComp->getPitch();

		if (mouseDelta.x or mouseDelta.y) {
			float xoffset = mouseDelta.x * sensitivity;
			float yoffset = mouseDelta.y * sensitivity;

			yaw += xoffset;
			pitch += yoffset; // Note: Y is already inverted in our Input system

			// Clamp the pitch value to prevent flipping
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;


			camComp->setYawPitch(yaw, pitch);
		}

	}



}; */
