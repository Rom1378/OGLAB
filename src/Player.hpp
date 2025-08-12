#pragma once
#include "CORE/GameObject.hpp"
#include "CORE/Window/Input.hpp"

class Player : public GameObject {

public:
	Player(const char* name) : GameObject(name) {}


	void onUpdate(float dt) override {
		// Process keyboard input for movement (only when mouse is locked)
		if (Input::isMouseLocked()) {
			auto pos = getPosition();


			//get attached camera
			if (auto cam = getComponent<Camera>()) {

				glm::vec3 forward = cam->getForward();
				glm::vec3 right = cam->getRight();


				//glm::vec3 direction = glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
				//glm::vec3 right = glm::normalize(m_right);

				if (Input::isKeyPressed(GLFW_KEY_W)) {
					pos += forward * m_actualSpeed * dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_S)) {
					pos -= forward * m_actualSpeed * dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_A)) {
					pos -= right * m_actualSpeed * dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_D)) {
					pos += right * m_actualSpeed * dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_SPACE)) {
					pos.y += m_actualSpeed * dt;

				}
				if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
					pos.y -= m_actualSpeed * dt;
				}

				//TransformableComponent::setPosition(pos);
				//this->getGameObject()->setPosition(pos);
				setPosition(pos);

				if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) 
					m_actualSpeed = m_sprintSpeed;
				else 
					m_actualSpeed = m_walkSpeed;
			}
		}
	}

private:

	const float m_walkSpeed = 7.0f;
	const float m_sprintSpeed = 15.0f;

	float m_actualSpeed = m_walkSpeed;


	bool m_isSprinting = 0;
};