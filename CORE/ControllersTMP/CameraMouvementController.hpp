/*#pragma once

#include "CORE/Controllers/Controller.hpp"
#include "CORE/CameraComponents/Camera.hpp"


class CameraMouvementController : public Controller {

public:
	CameraMouvementController(std::shared_ptr<GameObject> go = nullptr) : Controller(), m_targetObj(go) {}
	virtual ~CameraMouvementController() override {};

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


	std::shared_ptr<GameObject> get_target_object() const { return m_targetObj; }
	void set_target_object(std::shared_ptr<GameObject> go) { m_targetObj = go; }

private:
	std::shared_ptr<GameObject> m_targetObj;

};*/