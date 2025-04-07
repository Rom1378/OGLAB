#include "GameObject.hpp"
#include "PhysicsComponents/PhysicsComponent.hpp"


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
void GameObject::setPosition(const glm::vec3& position, bool update_physx){

	Transform::setPosition(position);
	if (update_physx) {
		if (auto physicsComponent = getComponent<PhysicsComponent>()) {
			physicsComponent->setPosition(position);
		}
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

// Add this to your GameObject class
void GameObject::setRotationQuaternion(const glm::quat& rotation, bool update_physx) {
	Transform::setRotationQuaternion(rotation);

	if (update_physx) {
		// Update physics component if it exists
		auto physicsComponent = getComponent<PhysicsComponent>();
		if (physicsComponent) {
			physicsComponent->updatePhysX();
		}
	}
}

// Set scale and update physics component if exists
void GameObject::setScale(const glm::vec3& scale) {

	Transform::setScale(scale);

	if (auto physicsComponent = getComponent<PhysicsComponent>()) {
		physicsComponent->setScale(scale);
	}
}
