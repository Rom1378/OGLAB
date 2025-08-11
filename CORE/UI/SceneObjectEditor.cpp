#include "SceneObjectEditor.hpp"
#include <typeinfo>

namespace UI {
	GameObject* g_SelectedObject = nullptr;
	bool g_SelectedFromHierarchy = false;
	bool g_JustSelectedFromHierarchy = false;


	void drawGameObjectTree(const std::vector<std::shared_ptr<GameObject>>& gameObjects) {

		static std::shared_ptr<GameObject> selected_gameobject = nullptr;
		static std::shared_ptr<Component> selected_component = nullptr;

		//ImGui::Begin("Scene Hierarchy");
		ImGui::BeginChild("Scene hi");
		ImGui::Columns(2);

		for (auto& obj : gameObjects) {
			// GameObject node
			bool nodeOpen = ImGui::TreeNode(obj->getName());
			if (ImGui::IsItemClicked()) {
				selected_gameobject = obj;
				selected_component = nullptr;
			}

			if (nodeOpen) {
				for (auto& comp : obj->getAllComponents()) {
					// Component as a leaf node
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx(comp->getName(), flags | ImGuiTreeNodeFlags_Selected * (comp == selected_component));

					if (ImGui::IsItemClicked()) {
						selected_gameobject = obj;
						selected_component = comp;
					}
				}
				ImGui::TreePop();
			}
		}

		//ImGui::End();
		ImGui::NextColumn();
		// Right panel: settings

		//ImGui::Begin("Inspector");
		if (selected_gameobject) {
			ImGui::Text("GameObject: %s", selected_gameobject->getName());
			// Draw game object settings here
		}
		if (selected_component) {
			ImGui::Text("Component: %s", selected_component->getName());
			// Draw component settings here
		}

		if (selected_gameobject) {
			selected_gameobject->onImGuiRender();
			// Draw game object settings here
		}
		if (selected_component) {
			selected_component->onImGuiRender();
			// Draw component settings here
		}
		//ImGui::End();
		ImGui::EndChild();

	}


	void renderImGuiSceneHierarchy(Scene* scene) {
		if (!scene) return;



		std::vector<std::shared_ptr<GameObject>>const* componentsArray[] = 
			{ &scene->getGameObjects(), &scene->getShadowCasters() ,&scene->getUIComponents()};


		// Left
		static int selectedObjectIndex = 0;
		static std::shared_ptr<GameObject> selectedObj = nullptr;
	


		//auto& selectedGameObjTypes=gameObjects;

		auto ObjectSelector = [&](std::vector<std::shared_ptr<GameObject>> const& objs)
			{
				if (ImGui::BeginChild("left pane", ImVec2(0, 0))) {
					for (int i = 0; i < objs.size(); i++)
					{
						char label[128];
						snprintf(label, 128, objs[i]->getName());
						if (ImGui::Selectable(label, selectedObjectIndex == i)) {
							selectedObjectIndex = i;
							selectedObj = objs[i];
							//GameObject::selectedComp = nullptr;
							//GameObject::selectedComponentIndex = 0;
						}
					}
					ImGui::EndChild();
				}
			};

		//ImGui::BeginGroup();
		static int selected = 0;
		{
			//ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_ResizeX);
			ImGui::RadioButton("GameObject", &selected, 0);
			ImGui::RadioButton("ShadowCaster", &selected, 1);
			ImGui::RadioButton("UIComponents", &selected, 2);
			//ImGui::EndChild();
		}

		ImGui::Text("selected objects list index %d", selected);

		drawGameObjectTree(*componentsArray[selected]);


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
				LOG("raycast selection called!");
			}
		}
		g_JustSelectedFromHierarchy = false;
	}
}