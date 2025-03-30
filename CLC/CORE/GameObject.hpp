#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Component.hpp"
#include "Transform.hpp"
#include "RenderComponent.hpp"

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
	void setPosition(const glm::vec3& position, bool update_physx=true);

	//set rotation and update physics component if exists
	void setRotation(const glm::vec3& rotation, bool update_physx = true);

	// Add this to your GameObject class
	void setRotationQuaternion(const glm::quat& rotation, bool update_physx = true);
	// Set scale and update physics component if exists
	void setScale(const glm::vec3& scale);


	const char* getName() const { return m_name.c_str(); }
	
private:
	std::vector<std::shared_ptr<Component>> m_components;
	//gameobject name
	std::string m_name;

}; // class GameObject
