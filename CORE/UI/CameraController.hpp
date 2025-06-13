#pragma once

#include "../Cameras/Camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace UI {
	void UICameraController(std::shared_ptr<Camera> cam) {
		//an Imgui that allow me to control the camera parameters
		ImGui::Begin("Camera");
		glm::vec3 pos = cam->getPosition();
		glm::vec3 rot = cam->getRotation();
		float fov = cam->getFOV();
		float aspectRatio = cam->getAspectRatio();
		float nearPlane = cam->getNearPlane();
		float farPlane = cam->getFarPlane();

		ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f);
		ImGui::SliderFloat("Aspect Ratio", &aspectRatio, 0.1f, 10.0f);
		ImGui::SliderFloat("Near Plane", &nearPlane, 0.1f, 100.0f);
		ImGui::SliderFloat("Far Plane", &farPlane, 100.0f, 10000.0f);
		cam->setFOV(fov);
		cam->setAspectRatio(aspectRatio);
		cam->setNearPlane(nearPlane);
		cam->setFarPlane(farPlane);


		ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100.0f, 100.0f);
		ImGui::SliderFloat3("Rotation", glm::value_ptr(rot), -180.0f, 180.0f);
		cam->setPosition(pos);
		cam->setRotation(rot);
		ImGui::End();
	}
}