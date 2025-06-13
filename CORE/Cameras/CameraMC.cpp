
#include "CameraMC.hpp"

void CameraMC::update(float dt) {

	// Only process mouse input when mouse is locked
	if (Input::isMouseLocked()) {
		glm::vec2 mouseDelta = Input::getMouseDelta();
		float xoffset = mouseDelta.x * m_sensitivity;
		float yoffset = mouseDelta.y * m_sensitivity;

		m_rotation.y += xoffset;
		m_rotation.x += yoffset; // Note: Y is already inverted in our Input system

		// Clamp the pitch value to prevent flipping
		if (m_rotation.x > 89.0f) m_rotation.x = 89.0f;
		if (m_rotation.x < -89.0f) m_rotation.x = -89.0f;
	}

	// Process keyboard input for movement (only when mouse is locked)
	if (Input::isMouseLocked()) {
		glm::vec3 direction = glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
		glm::vec3 right = glm::normalize(m_right);

		if (Input::isKeyPressed(GLFW_KEY_W)) {
			m_position += direction * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_S)) {
			m_position -= direction * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_A)) {
			m_position -= right * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_D)) {
			m_position += right * m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_SPACE)) {
			m_position.y += m_speed * dt;
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
			m_position.y -= m_speed * dt;
		}

		if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			m_speed = 32.0f;
		}
		else {
			m_speed = 10.0f;
		}
	}

	// Always update the view matrix
	Camera::update(dt);
}
