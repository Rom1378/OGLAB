#include "Camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


CameraComponent::CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane) :
	fov(fov), aspectRatio(aspectRatio), maxLightDistance(1000000.0f),
	nearPlane(nearPlane), farPlane(farPlane), TransformableComponent() {
	updateProjection();
}


void CameraComponent::update(float dt) {
	if (Window::getViewPortChanged())
	{
		setAspectRatio(Window::getFrameBufferWidth() / Window::getFrameBufferHeight());
	}

	//if  attached to gameObject freecam. need to check where we should enable it.
	//if ();

	updateView();
}

// Camera control methods
void CameraComponent::setPosition(const glm::vec3& pos) {
	TransformableComponent::setPosition(pos);
	updateView();
}

void CameraComponent::setRotation(const glm::vec3& rot) {
	TransformableComponent::setRotation(rot);
	updateView();
}

void CameraComponent::lookAt(const glm::vec3& target) {
	glm::vec3 pos = getWorldPosition();
	view = glm::lookAt(pos , target, up);
	m_forward = glm::normalize(target - pos);// Update forward vector
	m_right = glm::normalize(glm::cross(m_forward, up)); // Update right vector
	m_up = glm::normalize(glm::cross(m_right, m_forward)); // Update up vector

	view = glm::lookAt(pos, target, up);
}

void CameraComponent::setRotation(float pitch, float yaw, float roll) {
	this->setRotation(pitch, yaw, roll);
	//m_rotation = glm::vec3(pitch, yaw, roll);
	updateView();
}


void CameraComponent::updateView() {
	// Calculate the camera's orientation vectors based on its rotation
	//glm::mat4 rotationMatrix = glm::mat4(1.0f);
	/*
	auto rot = glm::quat(glm::radians(getLocalRotation()));
	auto pos = this->getWorldPosition();


	glm::quat lookRot =
		glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0)) *
		glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));

	glm::quat finalRot = rot * lookRot;


	//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
	//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
	//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

	// Apply the rotation to the camera's orientation vectors
	//m_forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
	//m_right = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
	//m_up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

	m_forward = finalRot * glm::vec3(0, 0, -1);
	m_right = finalRot * glm::vec3(1, 0, 0);
	m_up = finalRot * glm::vec3(0, 1, 0);


	// Construct the view matrix
	view = glm::lookAt(pos, pos + m_forward, m_up);



	*/
	/*

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	m_forward = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_up = glm::normalize(glm::cross(m_right, m_forward));

	glm::vec3 pos = getWorldPosition();
	view = glm::lookAt(pos, pos + m_forward, m_up);


	*/

	// Parent rotation as quaternion
	glm::quat parentRot = glm::quat(glm::radians(getLocalRotation())); // from TransformableComponent

	// Camera local rotation from yaw/pitch
	glm::quat yawRot = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
	glm::quat pitchRot = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));

	glm::quat localRot = yawRot * pitchRot; // yaw first, then pitch

	// Combine with parent's rotation
	glm::quat finalRot = parentRot * localRot;

	// Derive basis vectors
	m_forward = glm::normalize(finalRot * glm::vec3(0, 0, -1));
	m_right = glm::normalize(finalRot * glm::vec3(1, 0, 0));
	m_up = glm::normalize(finalRot * glm::vec3(0, 1, 0));

	glm::vec3 pos = getWorldPosition();
	view = glm::lookAt(pos, pos + m_forward, m_up);

}


void   CameraComponent::setAspectRatio(float ratio) {
	if (aspectRatio != ratio) {
		aspectRatio = ratio;
		updateProjection();
	}
}


void  CameraComponent::setFOV(float f) {
	if (fov != f) {
		fov = f;
		updateProjection();
	}
}


void CameraComponent::setNearPlane(float near) {
	if (nearPlane != near) {
		nearPlane = near;
		updateProjection();
	}
}


void CameraComponent::setFarPlane(float far) {
	if (farPlane != far) {
		farPlane = far;
		updateProjection();
	}
}


void CameraComponent::onImGuiRender() {
	glm::vec3 pos = getLocalPosition();
	glm::vec3 rot = getLocalRotation();
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


	ImGui::SliderFloat3("Local Component Position", glm::value_ptr(pos), -100.0f, 100.0f);
	ImGui::SliderFloat3("Local Component Rotation", glm::value_ptr(rot), -180.0f, 180.0f);

	ImGui::SliderFloat3("Forward Vector", glm::value_ptr(m_forward), -180.0f, 180.0f);

	ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f);
	ImGui::SliderFloat("Pitch", &pitch, -180.0f, 180.0f);

	setPosition(pos);
	setRotation(rot);
	setYawPitch(yaw, pitch);

}

void CameraComponent::setYawPitch(float newYaw, float newPitch) {

	yaw = newYaw;
	pitch = newPitch;

	// Clamp pitch to avoid flipping
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
}
