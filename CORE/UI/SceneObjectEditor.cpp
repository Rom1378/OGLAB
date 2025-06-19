#include "SceneObjectEditor.hpp"


namespace UI {
	GameObject* g_SelectedObject = nullptr;
	bool g_SelectedFromHierarchy = false;
	bool g_JustSelectedFromHierarchy = false;

	void renderImGuiSceneHierarchy(Scene* scene) {
		if (!scene) return;

		auto& gameObjects = scene->getGameObjects();
		auto& shadowCaster = scene->getShadowCasters();
		auto& uIComponents = scene->getUIComponents();


		// Left
		static int selectedObjectIndex = 0;
		static std::shared_ptr<GameObject> selectedObj = nullptr;

		auto ObjectSelector = [&](std::vector<std::shared_ptr<GameObject>> const& objs)
			{
				ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_ResizeX);
				for (int i = 0; i < objs.size(); i++)
				{
					char label[128];
					snprintf(label, 128, objs[i]->getName());
					if (ImGui::Selectable(label, selectedObjectIndex == i)) {
						selectedObjectIndex = i;
						selectedObj = objs[i];
					}
				}
				ImGui::EndChild();
			};

		ImGui::BeginGroup();
		ImGui::Columns(2, nullptr, true);

		ImGui::BeginChild("Objects", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		//ImGui::Text("MyObject: %d", selected);
		ImGui::Separator();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Games Objects"))
			{
				ObjectSelector(gameObjects);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shadow casters"))
			{
				ObjectSelector(shadowCaster);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Ui Components")) {
				ObjectSelector(uIComponents);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		ImGui::NextColumn();

		if (selectedObj) {
			ImGui::BeginChild("Object editor", ImVec2(0, 0)); // Leave room for 1 line below us
			ImGui::Text("selected object: %s", selectedObj->getName());
			//renderImGuiObjectEditor();
			selectedObj->onImGuiRender();


			ImGui::EndChild();

		}

		ImGui::EndGroup();



		/*

		if (ImGui::Begin("Scene Hierarchy")) {
			// Search filter
			static char searchFilter[256] = "";
			ImGui::InputText("Search", searchFilter, IM_ARRAYSIZE(searchFilter));

			// List all objects
			ImGui::BeginChild("Object List", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);

			for (auto& obj : scene->getGameObjects()) {
				if (!obj) continue;

				// Filter by name
				if (searchFilter[0] != '\0' &&
					std::string(obj->getName()).find(searchFilter) == std::string::npos) {
					continue;
				}

				// Display selectable object
				bool isSelected = (g_SelectedObject == obj.get());
				if (ImGui::Selectable(obj->getName(), isSelected)) {
					g_SelectedObject = obj.get();
					g_SelectedFromHierarchy = true;
					g_JustSelectedFromHierarchy = true;
				}

				if (isSelected && g_SelectedFromHierarchy && ImGui::GetScrollMaxY() > 0) {
					ImGui::SetScrollHereY(0.5f);
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
		*/
	}

	void renderImGuiObjectEditor() {
		if (!g_SelectedObject) return;

		if (ImGui::Begin("Object Editor")) {
			ImGui::Text("Selected: %s", g_SelectedObject->getName());
			if (!g_SelectedFromHierarchy) {
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "(Raycast Selected)");
			}
			ImGui::Separator();
			// Position editor
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

				glm::vec3 position = g_SelectedObject->getPosition();
				if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
					g_SelectedObject->setPosition(position, true);
				}

				// Rotation editor with tabs for Euler/Quaternion
				if (ImGui::BeginTabBar("RotationTabs")) {
					// Euler angles tab
					if (ImGui::BeginTabItem("Euler Angles")) {
						glm::vec3 rotation = g_SelectedObject->getRotation();
						if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f)) {
							g_SelectedObject->setRotation(rotation, true);
						}
						ImGui::EndTabItem();
					}

					// Quaternion tab
					if (ImGui::BeginTabItem("Quaternion")) {
						glm::quat rotQuat = g_SelectedObject->getRotationQuaternion();
						float quatValues[4] = { rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w };
						if (ImGui::DragFloat4("Rotation", quatValues, 0.01f)) {
							glm::quat newQuat(quatValues[3], quatValues[0], quatValues[1], quatValues[2]);
							newQuat = glm::normalize(newQuat);
							g_SelectedObject->setRotationQuaternion(newQuat, true);
						}

						if (ImGui::Button("Normalize")) {
							glm::quat normalized = glm::normalize(g_SelectedObject->getRotationQuaternion());
							g_SelectedObject->setRotationQuaternion(normalized, true);
						}
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}

				// Scale editor
				glm::vec3 scale = g_SelectedObject->getScale();
				if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
					g_SelectedObject->setScale(scale);

				}
			}
		}
		ImGui::End();
	}

	void setRaycastSelectedObject(GameObject* obj) {
		g_SelectedFromHierarchy = obj;
		// Clear hierarchy selection when selecting via raycast
		if (obj) {
			g_SelectedFromHierarchy = false;
		}
	}

	void handleRaycastSelection(GameObject* hitObject) {
		// Only set from raycast if we didn't just select from hierarchy
		// and if the mouse isn't over the ImGui window
		if (!g_JustSelectedFromHierarchy && !ImGui::GetIO().WantCaptureMouse) {
			if (hitObject && Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
				g_SelectedObject = hitObject;
				g_SelectedFromHierarchy = false;
			}
		}
		g_JustSelectedFromHierarchy = false;
	}
}