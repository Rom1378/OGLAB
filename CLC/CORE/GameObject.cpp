#include "GameObject.hpp"
#include "PhysicsComponent.hpp"


// Update all components
void GameObject::update(float dt) {
	for (auto& component : m_components) {
		component->update(dt);
	}
}

// Render if has RenderComponent
void GameObject::render(const glm::mat4& view, const glm::mat4& projection) {
	if (auto renderComponent = getComponent<RenderComponent>()) {
		renderComponent->draw(view, projection);
	}
}

void GameObject::render(std::shared_ptr<Camera> cam) {
	if (auto renderComponent = getComponent<RenderComponent>()) {
		renderComponent->draw(cam);
	}
}

// Set position and update physics component if exists
void GameObject::setPosition(const glm::vec3& position) {

	Transform::setPosition(position);

	if (auto physicsComponent = getComponent<PhysicsComponent>()) {
		physicsComponent->setPosition(position);
	}
}

// Set rotation and update physics component if exists
void GameObject::setRotation(const glm::vec3& rotation, bool update_physx) {

	Transform::setRotation(rotation);

	if (update_physx) {
		if (auto physicsComponent = getComponent<PhysicsComponent>())
			physicsComponent->setRotation(rotation);
	}
}

