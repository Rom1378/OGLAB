#include "Scene.hpp"
#include "Engine.hpp"
#include <algorithm>

#include "CORE/Entity.hpp"
#include "CORE/EntityManager.hpp"

#include "CORE/Systemes/Renderer/Renderer.hpp"
#include "CORE/Camera.hpp"

#include "CORE/Components/Camera.hpp"
#include "CORE/Components/Transform.hpp"


void Scene::init() {

	//systems inits
	Renderer::init();

	cameraEntity= createEntity("Camera entity");
	addComponent<CameraComponent>(cameraEntity);
	addComponent<TransformComponent>(cameraEntity);

	activeCamera = cameraEntity;

	system("cd");
	skybox = Skybox::createSkybox({
		"res/textures/CubeMaps/skybox/right.jpg",
		"res/textures/CubeMaps/skybox/left.jpg",
		"res/textures/CubeMaps/skybox/top.jpg",
		"res/textures/CubeMaps/skybox/bottom.jpg",
		"res/textures/CubeMaps/skybox/front.jpg",
		"res/textures/CubeMaps/skybox/back.jpg"

		});

}

void Scene::update(float dt) {



	//InputSystem(world);
	//MouseLookSystem(world);
	//PlayerMovementSystem(world, dt);
	//PhysicsStep(world, dt);
	//TransformSyncSystem(world);
	//RenderSystem(world);

	Camera cam = getComponent<CameraComponent>(activeCamera)->cam;
	TransformComponent* transform = getComponent<TransformComponent>(activeCamera);

	Renderer::setViewProjection(transform->getViewMatrix(), cam.getProjection());
	Renderer::update(dt);

	onUpdate();
}

Entity Scene::createEntity() {
	entities.push_back(EntityManager::Create());
	return entities.back();
}

Entity Scene::createEntity(const char* name) {
	LOG_WARN("NO name for now");
	return createEntity();
}

bool Scene::DestroyEntity(Entity e) {
	auto it = std::find(entities.begin(), entities.end(), e);
	if (it == entities.end()) {
		return false;
	}
	return EntityManager::Destroy(e);
}
