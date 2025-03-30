#pragma once
#include "../GameObject.hpp"
#include "../Scene.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>

namespace UI {

	void renderImGuiSceneHierarchy(Scene* scene, GameObject** selectedObject) {
		if (!scene) return;

		if (ImGui::Begin("Scene Hierarchy")) {
			// Search filter
			static char searchFilter[256] = "";
			ImGui::InputText("Search", searchFilter, IM_ARRAYSIZE(searchFilter));

			// List all objects
			ImGui::BeginChild("Object List", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);

			bool anySelected = false;
			for (auto& obj : scene->getGameObjects()) {
				if (!obj) continue;

				// Filter by name
				if (searchFilter[0] != '\0' &&
					std::string(obj->getName()).find(searchFilter) == std::string::npos) {
					continue;
				}

				// Display selectable object
				bool isSelected = (*selectedObject == obj.get());
				if (ImGui::Selectable(obj->getName(), isSelected)) {
					*selectedObject = obj.get();
				}

				if (isSelected) {
					anySelected = true;
					// Scroll to show selected item
					if (ImGui::GetScrollMaxY() > 0) {
						ImGui::SetScrollHereY(0.5f);
					}
				}
			}

			// Clear selection if no object is selected anymore
			if (!anySelected && *selectedObject) {
				*selectedObject = nullptr;
			}

			ImGui::EndChild();

			// Add/Remove buttons
			if (ImGui::Button("Create Object")) {
				scene->createGameObject();
			}

			ImGui::SameLine();

			/*
			if (ImGui::Button("Delete Selected") && *selectedObject) {
				scene->destroyGameObject(*selectedObject->shared_from_this());
				*selectedObject = nullptr;
			}
			*/
		}
		ImGui::End();
	}

	void renderImGuiObjectEditor(GameObject* selectedObject) {
		if (!selectedObject) return;

		// Create a window for the object editor
		if (ImGui::Begin("Object Editor")) {
			ImGui::Text("Selected: %s", selectedObject->getName());
			ImGui::Separator();

			// Position editor
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3 position = selectedObject->getPosition();
				if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
					selectedObject->setPosition(position, true);
				}

				// Rotation editor with tabs for Euler/Quaternion
				if (ImGui::BeginTabBar("RotationTabs")) {
					// Euler angles tab
					if (ImGui::BeginTabItem("Euler Angles")) {
						glm::vec3 rotation = selectedObject->getRotation();
						if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f)) {
							selectedObject->setRotation(rotation, true);
						}
						ImGui::EndTabItem();
					}

					// Quaternion tab
					if (ImGui::BeginTabItem("Quaternion")) {
						glm::quat rotQuat = selectedObject->getRotationQuaternion();
						float quatValues[4] = { rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w };
						if (ImGui::DragFloat4("Rotation", quatValues, 0.01f)) {
							glm::quat newQuat(quatValues[3], quatValues[0], quatValues[1], quatValues[2]);
							newQuat = glm::normalize(newQuat);
							selectedObject->setRotationQuaternion(newQuat, true);
						}

						if (ImGui::Button("Normalize")) {
							glm::quat normalized = glm::normalize(selectedObject->getRotationQuaternion());
							selectedObject->setRotationQuaternion(normalized, true);
						}
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}

				// Scale editor
				glm::vec3 scale = selectedObject->getScale();
				if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
					selectedObject->setScale(scale);
				}
			}

			// Add more editable properties here as needed
			if (ImGui::CollapsingHeader("Other Properties")) {
				// Example: Name editing
				static char nameBuffer[256];
				strncpy(nameBuffer, selectedObject->getName(), 256);
				if (ImGui::InputText("Name", nameBuffer, 256)) {
					selectedObject->setName(nameBuffer);
				}

				// Add more properties here...
			}
		}
		ImGui::End();
	}
}