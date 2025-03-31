#pragma once
#include "../LightManager.hpp"
#include "PrefabManager.hpp"
#include "../GameObject.hpp"
#include "../SphereRenderer.hpp"
#include "../SpherePhysics.hpp"
#include "../CubeRenderer.hpp"


inline void registerPrefabs() {
	// Register a Cube prefab
	PrefabManager::registerPrefab(
		PrefabDefinition("DynamicCubePrefab", [](std::shared_ptr<GameObject> obj) {
			// Add required components
			auto cubeRenderer = obj->addComponent<CubeRenderer>();
			auto cubePhysics = obj->addComponent<CubePhysics>(PhysicsComponent::Type::DYNAMIC);
			cubePhysics->setMass(1);
			})
		.setDefaultPosition(glm::vec3(0.0f, 20.0f, 0.0f))
	);

	// Register a Sphere prefab
	PrefabManager::registerPrefab(
		PrefabDefinition("SpherePrefab", [](std::shared_ptr<GameObject> obj) {
			auto sphereRenderer = obj->addComponent<SphereRenderer>();
			auto spherePhysics = obj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);
			spherePhysics->setMass(7);
			})
		.setDefaultPosition(glm::vec3(0.0f, 10.0f, 0.0f))


	);

	// Register a Ground/World prefab
	PrefabManager::registerPrefab(
		PrefabDefinition("WorldPrefab", [](std::shared_ptr<GameObject> obj) {
			obj->setScale(glm::vec3(100.0f, 1.0f, 100.0f));
			obj->addComponent<CubeRenderer>();
			obj->addComponent<CubePhysics>(PhysicsComponent::Type::STATIC);
			})
		.setDefaultPosition(glm::vec3(0.0f, -10.0f, 0.0f))
	);

	//LightSphere
	PrefabManager::registerPrefab(
		PrefabDefinition("LightSpherePrefab", [](std::shared_ptr<GameObject> obj) {
			auto sphereRenderer = obj->addComponent<SphereRenderer>();
			sphereRenderer->setColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0));
			auto spherePhysics = obj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);
			auto light = obj->addComponent<Light>(LightType::POINT, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), 1.0f);
			LightManager::addLight(light);
			spherePhysics->setMass(3);
			//add movement
			})
		.setDefaultPosition(glm::vec3(0.0f, 10.0f, 0.0f))
	);
}