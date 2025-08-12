
#include "CameraMC.hpp"
#include "CORE/GameObject.hpp"

void CameraMC::update(float dt) {


	// Only process mouse input when mouse is locked
	if (Input::isMouseLocked()) {
		glm::vec2 mouseDelta = Input::getMouseDelta();
		float xoffset = mouseDelta.x * m_sensitivity;
		float yoffset = mouseDelta.y * m_sensitivity;

		yaw += xoffset;
		pitch += yoffset; // Note: Y is already inverted in our Input system

		// Clamp the pitch value to prevent flipping
		if (pitch > 89.0f) pitch  = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;


		setYawPitch(yaw, pitch);
		//TransformableComponent::setRotation(rot);

	}

	/*
	// Process keyboard input for movement (only when mouse is locked)
	if (Input::isMouseLocked()) {
		auto pos = getWorldPosition();
		//glm::vec3 direction = glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
		//glm::vec3 right = glm::normalize(m_right);

		if (Input::isKeyPressed(GLFW_KEY_W)) {
			pos += m_forward * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_S)) {
			pos -= m_forward * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_A)) {
			pos -= m_right * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_D)) {
			pos += m_right * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_SPACE)) {
			pos.y += m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
			pos.y -= m_speed * dt;
		}

		//TransformableComponent::setPosition(pos);
 		//this->getGameObject()->setPosition(pos);
		getWorldPosition

		if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			m_speed = 32.0f;
		}
		else {
			m_speed = 10.0f;
		}
	}
		*/

	// Always update the view matrix
	Camera::update(dt);
}
