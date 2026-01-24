#include "Scene.hpp"
#include "Engine.hpp"
#include <algorithm>

#include "CORE/Entity.hpp"
#include "CORE/EntityManager.hpp"

#include "CORE/Systems/Renderer/MeshRenderer.hpp"
#include "CORE/Systems/Renderer/Renderer.hpp"

#include "CORE/Systems/Input/Mouvement.hpp"

#include "CORE/Systems/Physics/Physics.hpp"
#include "CORE/Systems/TransformSync/Sync.hpp"

#include "CORE/Camera.hpp"

#include "CORE/Components/Camera.hpp"
#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Physics.hpp"
#include "CORE/Components/Renderable.hpp"

void Scene::init() {



	cameraEntity= createEntity("Dev camera");

	addComponent<CameraComponent>(cameraEntity);
	addComponent<TransformComponent>(cameraEntity);
	
	// stores dx dt dz yaw pitch
	addComponent<CharacterControllerStateComponent>(cameraEntity);
	
	// simulate dx dt dz yaw pitch
	addComponent<PhysicsBodyComponent>(cameraEntity, PhysicsBodyComponent::PhysicsRole::CharacterController);
	
	activeCamera = cameraEntity;
	activeEntity = cameraEntity; //will 

	skybox = Skybox::createSkybox({
		"res/textures/CubeMaps/skybox/right.jpg",
		"res/textures/CubeMaps/skybox/left.jpg",
		"res/textures/CubeMaps/skybox/top.jpg",
		"res/textures/CubeMaps/skybox/bottom.jpg",
		"res/textures/CubeMaps/skybox/front.jpg",
		"res/textures/CubeMaps/skybox/back.jpg"
		});

	Entity cube = createEntity("Cube");
	auto rdcomp = addComponent<RenderableComponent>(cube, MeshRenderer::createCube());

	Texture& cat = *TextureManager::loadTexture("res/textures/CAT.png", "cat");
	Texture& container = *TextureManager::loadTexture("res/textures/container/container2.png", "container");
	container.type = Texture::Type::DIFFUSE;
	cat.type = Texture::Type::DIFFUSE;
	Texture& specular = *TextureManager::loadTexture("res/textures/container/container2_specular.png", "container_specular");
	specular.type = Texture::Type::SPECULAR;

	MeshRenderer::setCubeTextures(rdcomp->rhandle, { container, specular});


	TransformComponent comp{  };
	comp.scale = { 1.0,3.0f,1.0f };
	comp.pos = { 0,-4,0 };
	addComponent<TransformComponent>(cube, comp);

}

void Scene::update(float dt) {


// 1 input system
// 2 simulate physx
// 3 sync
// 4 update 3d

	if (Input::isMouseLocked())
		InputSystem::update(this, dt);
	PhysicsSystem::update(this, dt);
	TransformSyncSystem::update(this);
//	PhysicsStep(world, dt);
	//PlayerMovementSystem(world, dt);
	//RenderSystem(world);

	Camera cam = getComponent<CameraComponent>(activeCamera)->cam;
	TransformComponent* transform = getComponent<TransformComponent>(activeCamera);

	//MouseLok
	//MouseLookSystem::update();

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
