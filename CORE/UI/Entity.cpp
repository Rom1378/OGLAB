#include "Entity.hpp"
#include "CORE/Scene.hpp"
#include "imgui.h"
#include "CORE/EntityBrowser.hpp"

#include <string>


#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Renderable.hpp"
#include "CORE/Components/Camera.hpp"
#include "CORE/Components/Light.hpp"
#include "CORE/Components/Physics.hpp"


namespace UI {
	Entity selectedEntity = 1;
	void DrawEntityTree(Scene* scene) {

		if (!scene) return;

		ImGui::Begin("Entity Browser");

		// Split view: entity list on left, component inspector on right
		ImGui::BeginChild("EntityList", ImVec2(200, 0), true);

		const auto& entities = scene->getEntities();
		for (const Entity& entity : entities) {
			std::string label = "Entity " + std::to_string(entity.getId());

			if (ImGui::Selectable(label.c_str(),
				selectedEntity.getId() == entity.getId())) {
				selectedEntity = entity;
			}
		}

		ImGui::EndChild();
		ImGui::SameLine();

		// Component inspector
		ImGui::BeginChild("ComponentInspector");

		if (selectedEntity != INVALID_ENTITY_ID) {
			ImGui::Text("Entity ID: %u", selectedEntity.getId());
			ImGui::Separator();

			int componentCount = 0;

			// Display all registered components for this entity
			const auto& registeredComponents = EntityBrowser::getRegisteredComponents();
			for (auto& componentInfo : registeredComponents) {
				if (componentInfo.hasFunc(selectedEntity)) {
					componentCount++;

					if (ImGui::CollapsingHeader(componentInfo.name.c_str(),
						ImGuiTreeNodeFlags_DefaultOpen)) {
						ImGui::PushID(componentInfo.name.c_str());
						ImGui::Indent();

						componentInfo.displayFunc(selectedEntity);

						ImGui::Spacing();
						if (ImGui::Button("Remove Component")) {
							componentInfo.removeFunc(selectedEntity);
						}

						ImGui::Unindent();
						ImGui::PopID();
					}
				}
			}

			if (componentCount == 0) {
				ImGui::TextDisabled("No components attached");
			}
			else {
				ImGui::Separator();
				ImGui::Text("Total components: %d", componentCount);
			}

		}
		else {
			ImGui::TextDisabled("Select an entity to inspect");
		}

		ImGui::EndChild();

		ImGui::End();
	}



	void initEntityTreeBrowser() {
		EntityBrowser::registerComponent<TransformComponent>("Transform",
			[](TransformComponent& t, Entity e) {

				// Check if entity has PhysicsBodyComponent and is controlled by physics
				bool needsControl = false;
				if (auto* physicsBody = ComponentManager::getComponent<PhysicsBodyComponent>(e.getId())) {
					if (physicsBody->controlledByPhysics) {
						needsControl = true;
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.6f);
					}
				}

				ImGui::DragFloat3("Position", &t.pos.x, 0.1f);
				if (needsControl && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("This entity has a PhysicsBodyComponent controlled by physics.\nYou need to call takeControl() to modify transform manually.");
				}

				// Display rotation as Euler angles for easier editing
				glm::vec3 euler = t.getEulerAngles();
				if (ImGui::DragFloat3("Rotation (Euler)", &euler.x, 1.0f)) {
					t.setEulerAngles(euler);
				}
				if (needsControl && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("This entity has a PhysicsBodyComponent controlled by physics.\nYou need to call takeControl() to modify transform manually.");
				}

				// Option: Show quaternion values (read-only)
				if (ImGui::TreeNode("Quaternion (Raw)")) {
					ImGui::Text("x: %.3f, y: %.3f, z: %.3f, w: %.3f",
						t.rot.x, t.rot.y, t.rot.z, t.rot.w);
					ImGui::TreePop();
				}

				ImGui::DragFloat3("Scale", &t.scale.x, 0.01f, 0.001f, 100.0f);
				if (needsControl && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("This entity has a PhysicsBodyComponent controlled by physics.\nYou need to call takeControl() to modify transform manually.");
				}

				if (needsControl) {
					ImGui::PopStyleVar();
				}

				// Display direction vectors
				if (ImGui::TreeNode("Direction Vectors")) {
					glm::vec3 fwd = t.forward();
					glm::vec3 up = t.up();
					glm::vec3 left = t.left();
					ImGui::Text("Forward: (%.2f, %.2f, %.2f)", fwd.x, fwd.y, fwd.z);
					ImGui::Text("Up: (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
					ImGui::Text("Left: (%.2f, %.2f, %.2f)", left.x, left.y, left.z);
					ImGui::TreePop();
				}
			});
		// Register LightComponent
		EntityBrowser::registerComponent<LightComponent>("Light",
			[](LightComponent& light, Entity e) {
				// Light type selector
				const char* types[] = { "Point Light", "Directional Light", "Spot Light" };
				int currentType = static_cast<int>(light.type);
				if (ImGui::Combo("Light Type", &currentType, types, 3)) {
					light.type = static_cast<LightComponent::Type>(currentType);

					// Update variant based on type
					switch (light.type) {
					case LightComponent::Type::POINT:
						light.data = PointLightData{};
						break;
					case LightComponent::Type::DIR:
						light.data = DirLightData{};
						break;
					case LightComponent::Type::SPOT:
						light.data = SpotLightData{};
						break;
					}
				}

				ImGui::Separator();

				// Common properties
				ImGui::ColorEdit3("Ambient", &light.ambient.x);
				ImGui::ColorEdit3("Diffuse", &light.diffuse.x);
				ImGui::ColorEdit3("Specular", &light.specular.x);

				ImGui::Separator();

				// Type-specific properties
				if (auto* pointData = light.getPointData()) {
					ImGui::Text("Point Light Properties");
					ImGui::DragFloat("Constant", &pointData->constant, 0.01f, 0.0f, 10.0f);
					ImGui::DragFloat("Linear", &pointData->linear, 0.001f, 0.0f, 1.0f);
					ImGui::DragFloat("Quadratic", &pointData->quadratic, 0.0001f, 0.0f, 1.0f);
				}
				else if (auto* dirData = light.getDirData()) {
					ImGui::Text("Directional Light Properties");
					ImGui::TextDisabled("(No additional properties)");
				}
				else if (auto* spotData = light.getSpotData()) {
					ImGui::Text("Spot Light Properties");
					ImGui::DragFloat("Cut Off", &spotData->cutOff, 0.1f, 0.0f, 90.0f);
					ImGui::DragFloat("Outer Cut Off", &spotData->outerCutOff, 0.1f, 0.0f, 90.0f);
					ImGui::DragFloat("Constant", &spotData->constant, 0.01f, 0.0f, 10.0f);
					ImGui::DragFloat("Linear", &spotData->linear, 0.001f, 0.0f, 1.0f);
					ImGui::DragFloat("Quadratic", &spotData->quadratic, 0.0001f, 0.0f, 1.0f);
				}
			});

		// Register RenderableComponent
		EntityBrowser::registerComponent<RenderableComponent>("Renderable",
			[](RenderableComponent& r, Entity e) {
				ImGui::Text("Renderable Handle: %u", r.rhandle);
				if (r.rhandle == MeshRenderer::INVALID_HANDLE) {
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "INVALID HANDLE");
				}
				// Add buttons to change mesh if needed
				// if (ImGui::Button("Change Mesh...")) { /* open mesh selector */ }
			});

		// Register PhysicsBodyComponent
		EntityBrowser::registerComponent<PhysicsBodyComponent>("Physics Body",
			[](PhysicsBodyComponent& body, Entity e) {
				// Physics role selector
				const char* roles[] = { "Dynamic", "Kinematic", "Static", "Character Controller" };
				int currentRole = static_cast<int>(body.role);
				if (ImGui::Combo("Physics Role", &currentRole, roles, 4)) {
					body.role = static_cast<PhysicsBodyComponent::PhysicsRole>(currentRole);
				}

				// State display
				const char* states[] = { "Uninitialized", "Alive", "Pending Destroy" };
				ImGui::Text("State: %s", states[static_cast<int>(body.state)]);

				const char* controlledBy[] = { "Game", "Physics simulation" };
				ImGui::Text("Controlled by: %s", controlledBy[static_cast<int>(body.controlledByPhysics)]);
				if (ImGui::Button("Take control"))
					body.takeControl();
				if (ImGui::Button("Release control"))
					body.releaseControl();


				// Mass (only for dynamic bodies)
				if (body.role == PhysicsBodyComponent::PhysicsRole::Dynamic) {
					ImGui::DragFloat("Mass", &body.mass, 0.1f, 0.01f, 10000.0f);
				}

				// Display actor/controller status
				ImGui::Separator();
				ImGui::Text("Actor: %s", body.actor ? "Valid" : "NULL");
				ImGui::Text("Controller: %s", body.controller ? "Valid" : "NULL");
			});

		// Register ColliderComponent
		EntityBrowser::registerComponent<ColliderComponent>("Collider",
			[](ColliderComponent& collider, Entity e) {
				// Collider type selector
				const char* types[] = { "Box", "Sphere", "Capsule", "Mesh" };
				int currentType = static_cast<int>(collider.type);
				if (ImGui::Combo("Collider Type", &currentType, types, 4)) {
					collider.type = static_cast<ColliderComponent::Type>(currentType);
				}

				ImGui::Separator();

				// Type-specific properties
				switch (collider.type) {
				case ColliderComponent::Type::Box:
					ImGui::DragFloat3("Half Extents", &collider.halfExtents.x, 0.01f, 0.001f, 100.0f);
					break;
				case ColliderComponent::Type::Sphere:
					ImGui::DragFloat("Radius", &collider.radius, 0.01f, 0.001f, 100.0f);
					break;
				case ColliderComponent::Type::Capsule:
					ImGui::DragFloat("Radius", &collider.radius, 0.01f, 0.001f, 100.0f);
					ImGui::DragFloat("Height", &collider.height, 0.01f, 0.001f, 100.0f);
					break;
				case ColliderComponent::Type::Mesh:
					ImGui::TextDisabled("Mesh collider properties");
					break;
				}

				ImGui::Separator();
				ImGui::DragFloat3("Local Position", &collider.localPosition.x, 0.01f);

				// Display quaternion as Euler for easier editing
				glm::vec3 euler = collider.getLocalEulerAngles();
				if (ImGui::DragFloat3("Local Rotation", &euler.x, 1.0f)) {
					collider.setLocalEulerAngles(euler);
				}

				ImGui::Separator();
				ImGui::Text("Material: %s", collider.material ? "Valid" : "NULL");
			});

		// Register CharacterControllerStateComponent
		EntityBrowser::registerComponent<CharacterControllerStateComponent>("Character Controller State",
			[](CharacterControllerStateComponent& state, Entity e) {
				ImGui::Text("Velocity: (%.2f, %.2f, %.2f)",
					state.velocity.x, state.velocity.y, state.velocity.z);

				float speed = glm::length(state.velocity);
				ImGui::Text("Speed: %.2f", speed);

				ImGui::Separator();

				ImGui::DragFloat("Yaw", &state.yaw, 0.1f);
				ImGui::DragFloat("Pitch", &state.pitch, 0.1f);

				ImGui::Separator();

				ImGui::Checkbox("Is Sprinting", &state.isSprinting);
				ImGui::Checkbox("Is On The Ground", &state.isOnTheGround);

				// Display as degrees too
				ImGui::Text("Yaw (degrees): %.1f", glm::degrees(state.yaw));
				ImGui::Text("Pitch (degrees): %.1f", glm::degrees(state.pitch));
			});
	}


}