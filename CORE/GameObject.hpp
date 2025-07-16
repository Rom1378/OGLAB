#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Debug.hpp"
#include "Transform.hpp"
#include "RenderComponents/RenderComponent.hpp"
#include "Lights/LightManager.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <typeinfo>
#include "Component.hpp"

class PhysicsComponent;

class GameObject : public Transform
{
public:
	GameObject() : Transform() {}
	GameObject(const char* name) : Transform(), m_name(name) {}
	template<typename T>
	std::shared_ptr<T> addComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		std::shared_ptr<T> component = std::make_shared<T>();
		component->setGameObject(this);
		m_components.push_back(component);
		component->init();
		return component;
	}

	//addComponent with arguments of the T constructor
	template<typename T, typename... Args>
	std::shared_ptr<T> addComponent(Args&&... args)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		std::shared_ptr<T> component = std::make_shared<T>(std::forward<Args>(args)...);
		component->setGameObject(this);
		m_components.push_back(component);
		component->init();
		//if physx component put gameobject ptr as userdata
		if (auto physicsComponent = std::dynamic_pointer_cast<PhysicsComponent>(component))
		{
			physicsComponent->setUserData(this);
		}

		return component;
	}


	template<typename T>
	std::shared_ptr<T> getComponent()
	{
		for (auto& component : m_components)
		{
			std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(component);
			if (casted)
			{
				return casted;
			}
		}
		return nullptr;
	}

	// Update all components
	void update(float dt);

	// Render if has RenderComponent
	void render(const glm::mat4& view, const glm::mat4& projection);

	void render(std::shared_ptr<Camera> cam);

	// Set position and update physics component if exists
	void setPosition(const glm::vec3& position, bool update_physx = true);

	//set rotation and update physics component if exists
	void setRotation(const glm::vec3& rotation, bool update_physx = true);

	// Add this to your GameObject class
	void setRotationQuaternion(const glm::quat& rotation, bool update_physx = true);
	// Set scale and update physics component if exists
	void setScale(const glm::vec3& scale);

	//set shader for all render components
	void setShader(std::shared_ptr<ShaderProgram> shader)
	{
		for (auto& component : m_components)
		{
			if (auto renderComponent = std::dynamic_pointer_cast<RenderComponent>(component))
			{
				renderComponent->setShader(shader);
			}
		}
	}

	//render raw geometry for all render components
	void renderRawGeometry(const glm::mat4& lightSpaceMatrix)
	{
		for (auto& component : m_components)
		{
			if (auto renderComponent = std::dynamic_pointer_cast<RenderComponent>(component))
			{
				renderComponent->renderRawGeometry(lightSpaceMatrix);
			}
		}
	}
	//render with materials for all render components
	void renderWithMaterials(const std::shared_ptr<Camera>& cam)
	{
		for (auto& component : m_components)
		{
			if (auto renderComponent = std::dynamic_pointer_cast<RenderComponent>(component))
			{
				renderComponent->renderWithMaterials(cam);
			}
		}
	}




	void onImGuiRender() {

		//transform
		glm::vec3 position = getPosition();
		glm::vec3 rotation = getRotation(); // Euler degrees
		glm::vec3 scale = getScale();

		if (ImGui::TreeNode("Game Object Transform")) {
			if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
				setPosition(position);
			}

			if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f)) {
				setRotation(rotation);  // Will update quaternion
			}

			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
				setScale(scale);
			}

			ImGui::TreePop();
		}

		
		ImGui::Columns(2, nullptr, true);

		ImGui::BeginChild("GameObject Components");
		for (int i = 0; i < m_components.size(); i++)
		{
			char label[128];
			//snprintf(label, 128, getCompom_components[i].get()));
			snprintf(label, 128, "Component: %s", typeid(*m_components[i].get()).name()+6);
			if (ImGui::Selectable(label, selectedComponentIndex == i)) {
				selectedComponentIndex = i;
				selectedComp = m_components[i];
			}
		}
		ImGui::EndChild();

		ImGui::NextColumn();

		if (selectedComp)
		{
				//local component transform
			if (auto tfComponent = std::dynamic_pointer_cast<TransformableComponent>(selectedComp))
			{
				glm::vec3 pos=tfComponent->getWorldPosition();
				glm::vec3 rot=tfComponent->getWorldDirection();

				if (ImGui::TreeNode("Component Transform")) {
					if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
						tfComponent->setPosition(pos);
					}


					if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
						tfComponent->setRotation(rot);
					}

					ImGui::TreePop();
				}

			}


			//component specifications
			selectedComp->onImGuiRender();
		}
	}

	const char* getName() const { return m_name.c_str(); }


	int selectedComponentIndex=0;
	std::shared_ptr<Component> selectedComp = nullptr;

private:
	std::vector<std::shared_ptr<Component>> m_components;
	//gameobject name
	std::string m_name;

}; // class GameObject
