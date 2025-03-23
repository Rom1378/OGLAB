#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Component.hpp"
#include "Transform.hpp"


class GameObject
{
public:
	
	GameObject() : transform(std::make_shared<Transform>()) {}
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
	void update(float dt) {
		for (auto& component : m_components) {
			component->update(dt);
		}
	}

	// Render if has RenderComponent
	void render() {
		if (auto renderComponent = getComponent<RenderComponent>()) {
			renderComponent->draw();
		}
	}

	std::shared_ptr<Transform> getTransform() { return transform; }

	
private:
	std::vector<std::shared_ptr<Component>> m_components;
	std::shared_ptr<Transform> transform;

}; // class GameObject