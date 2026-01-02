#include "Scene.hpp"
#include "Engine.hpp"
#include <algorithm>

#include "CORE/Entity.hpp"
#include "CORE/EntityManager.hpp"

void Scene::init() {



}

void Scene::update(float dt) {

	//InputSystem(world);
	//MouseLookSystem(world);
	//PlayerMovementSystem(world, dt);
	//PhysicsStep(world, dt);
	//TransformSyncSystem(world);
	//RenderSystem(world);

	onUpdate();
}

Entity Scene::createEntity() {
	entities.push_back(EntityManager::Create());
	return entities.back();
}

bool Scene::DestroyEntity(Entity e) {
	auto it = std::find(entities.begin(), entities.end(), e);
	if (it == entities.end()) {
		return false;
	}
	return EntityManager::Destroy(e);
}
