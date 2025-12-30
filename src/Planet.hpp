#pragma once
#include "CORE/GameObject.hpp"
#include "CORE/Window/Input.hpp"
#include "CORE/RenderComponents/SphereRenderer.hpp"
#include "CORE/PhysicsComponents/SpherePhysics.hpp"

class Planet : public GameObject {

public:
	Planet(const char* name, float radius, float mass) : GameObject(name) {
		m_sphereRenderer = addComponent<SphereRenderer>(radius);

		m_spherePhysics = addComponent<SpherePhysics>(SpherePhysics::Type::STATIC);
		m_spherePhysics->setScale(glm::vec3(radius, radius, radius));
	}

	void onUpdate(float dt) override {
		
	}

	void onImGuiRender() override {

	}
private:
	float m_mass{ 80000000.0f };

	std::shared_ptr<SphereRenderer> m_sphereRenderer;
	std::shared_ptr<SpherePhysics> m_spherePhysics;


};