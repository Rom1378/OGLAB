#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Component.hpp"
#include "Input.hpp"
#include <iostream>


class Camera : public Transform, public Component {
public:
	Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f)
		: fov(fov), aspectRatio(aspectRatio),
		nearPlane(nearPlane), farPlane(farPlane) {
		updateProjection();
	}


	void update(float dt) override {
		updateView();
	}

	// Camera control methods
	void setPosition(const glm::vec3& pos) {
		m_position = pos;
		updateView();
	}

	void setRotation(const glm::vec3& rot) {
		m_rotation = rot;
		updateView();
	}

	void lookAt(const glm::vec3& target) {
		view = glm::lookAt(m_position, target, up);
		m_forward = glm::normalize(target - m_position); // Update forward vector
		m_right = glm::normalize(glm::cross(m_forward, up)); // Update right vector
		m_up = glm::normalize(glm::cross(m_right, m_forward)); // Update up vector

		view = glm::lookAt(m_position, target, up);
	}

	void setRotation(float pitch, float yaw, float roll) {
		m_rotation = glm::vec3(pitch, yaw, roll);
		updateView();
	}

	// Camera properties
	const glm::mat4& getViewMatrix() const { return view; }
	const glm::mat4& getProjectionMatrix() const { return projection; }

	//gets
	const glm::vec3& getForward() const { return m_forward; }
	const glm::vec3& getRight() const { return m_right; }
	const glm::vec3& getUp() const { return m_up; }


	void updateView() {
			// Calculate the camera's orientation vectors based on its rotation
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

			// Apply the rotation to the camera's orientation vectors
			m_forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
			m_right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
			m_up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

			// Construct the view matrix
			view = glm::lookAt(m_position, m_position + m_forward, m_up);

	}
protected:
	glm::vec3 m_forward{ 0.0f, 0.0f, -1.0f }; // Forward direction
	glm::vec3 m_right{ 1.0f, 0.0f, 0.0f };   // Right direction
	glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };      // Up direction
private:
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };



	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;




	void updateProjection() {
		projection = glm::perspective(glm::radians(fov), aspectRatio,
			nearPlane, farPlane);
	}
}; // class Camera



class CameraMC : public Camera {

public:
	CameraMC(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f)
		: Camera(fov, aspectRatio, nearPlane, farPlane) {}


	void update(float dt) override {
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
				m_speed = 10.0f;
			}
			else {
				m_speed = 5.0f;
			}
		}

		// Always update the view matrix
		updateView();
	}
private:

	float m_speed = 5.0f;
	float m_sensitivity = 0.1f;

}; // class CameraMC
