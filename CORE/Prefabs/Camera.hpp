#include "PrefabManager.hpp"



PrefabManager::registerPrefab(
	PrefabDefinition("CameraPrefab", [](std::shared_ptr<GameObject> obj) {
		auto sphereRenderer = obj->addComponent<SphereRenderer>();
		sphereRenderer->setColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0));
		auto spherePhysics = obj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);
		auto light = obj->addComponent<Light>(LightType::POINT, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), 1.0f);
		//LightManager::addLight(light);
		spherePhysics->setMass(3);
		//add movement
		})
	.setDefaultPosition(glm::vec3(0.0f, 10.0f, 0.0f))
);