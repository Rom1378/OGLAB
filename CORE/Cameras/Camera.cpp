#include "Camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane) :
	fov(fov), aspectRatio(aspectRatio), maxLightDistance(1000000.0f),
	nearPlane(nearPlane), farPlane(farPlane), Component(), Transform() {
	updateProjection();
}


void Camera::update(float dt) {
	if (Window::getViewPortChanged())
	{
		setAspectRatio(Window::getFrameBufferWidth() / Window::getFrameBufferHeight());
	}
	updateView();
}

// Camera control methods
void Camera::setPosition(const glm::vec3& pos) {
	m_position = pos;
	updateView();
}

void Camera::setRotation(const glm::vec3& rot) {
	m_rotation = rot;
	updateView();
}

void Camera::lookAt(const glm::vec3& target) {
	view = glm::lookAt(m_position, target, up);
	m_forward = glm::normalize(target - m_position); // Update forward vector
	m_right = glm::normalize(glm::cross(m_forward, up)); // Update right vector
	m_up = glm::normalize(glm::cross(m_right, m_forward)); // Update up vector

	view = glm::lookAt(m_position, target, up);
}

void Camera::setRotation(float pitch, float yaw, float roll) {
	m_rotation = glm::vec3(pitch, yaw, roll);
	updateView();
}


void Camera::updateView() {
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


void   Camera::setAspectRatio(float ratio) {
	if (aspectRatio != ratio) {
		aspectRatio = ratio;
		updateProjection();
	}
}


void  Camera::setFOV(float f) {
	if (fov != f) {
		fov = f;
		updateProjection();
	}
}


void Camera::setNearPlane(float near) {
	if (nearPlane != near) {
		nearPlane = near;
		updateProjection();
	}
}


void Camera::setFarPlane(float far) {
	if (farPlane != far) {
		farPlane = far;
		updateProjection();
	}
}


void Camera::onImGuiRender() {
	glm::vec3 pos = getPosition();
	glm::vec3 rot = getRotation();
	float fov = getFOV();
	float aspectRatio = getAspectRatio();
	float nearPlane = getNearPlane();
	float farPlane = getFarPlane();

	ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f);
	ImGui::SliderFloat("Aspect Ratio", &aspectRatio, 0.1f, 10.0f);
	ImGui::SliderFloat("Near Plane", &nearPlane, 0.1f, 100.0f);
	ImGui::SliderFloat("Far Plane", &farPlane, 100.0f, 10000.0f);
	setFOV(fov);
	setAspectRatio(aspectRatio);
	setNearPlane(nearPlane);
	setFarPlane(farPlane);


	ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100.0f, 100.0f);
	ImGui::SliderFloat3("Rotation", glm::value_ptr(rot), -180.0f, 180.0f);
	setPosition(pos);
	setRotation(rot);
}
