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

#include "CORE/Physics.hpp"

class PhysicsComponent;

class GameObject : public Transform
{
public:
	GameObject() : Transform() {}
	GameObject(const char* name) : Transform(), m_name(name) {}
	GameObject(const std::string& name) : Transform(), m_name(name) {}
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
	const std::shared_ptr<T>& getComponent() const
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

	//return all components
	inline std::vector<std::shared_ptr<Component>>const& getAllComponents() const { return this->m_components; }

	// Update all components
	void update(float dt);
	virtual void onImGuiRender() {};
	virtual void onUpdate(float dt) {}

	void ImGuiRender();

	// Render if has RenderComponent
	void render(const glm::mat4& view, const glm::mat4& projection) const;

	void render(std::shared_ptr<CameraComponent> cam) const;

	// Set position and update physics component if exists
	void setPosition(const glm::vec3& position, bool update_physx = true);
	void setPosition(float x, float y, float z, bool update_physx = true); 
	void setPosition(const PxVec3d& p, bool update_physx = true);
	void setPosition(const PxVec3& p, bool update_physx = true); 

	
	void move(const glm::vec3& offset, bool update_physx = true);

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
	void renderWithMaterials(const std::shared_ptr<CameraComponent>& cam)
	{
		for (auto& component : m_components)
		{
			if (auto renderComponent = std::dynamic_pointer_cast<RenderComponent>(component))
			{
				renderComponent->renderWithMaterials(cam);
			}
		}
	}

	const char* getName() const { return m_name.c_str(); }

private:
	std::vector<std::shared_ptr<Component>> m_components;
	//gameobject name
	std::string m_name;

}; // class GameObject
