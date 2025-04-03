#include "../CORE/Scene.hpp"
#include "../CORE/Window.hpp"
#include "../CORE/Input.hpp"
#include "../CORE/Shader.hpp"
#include <iostream>
#include <PxPhysicsAPI.h>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../CORE/Engine.hpp"
#include "../CORE/Prefabs/SomePrefabs.hpp"
#include "../CORE/UI/CameraController.hpp"
#include "../CORE/RenderComponents/Cursor.hpp"
#include "../CORE/UI/SceneObjectEditor.hpp"

int main() {

	Engine::init();

	// Create scene
	Scene scene;
	scene.init();
	//add cube prefab
	auto prefCube = PrefabManager::instantiate("DynamicCubePrefab", scene);

	/*
	{
		// Load texture
		auto txtr = TextureManager::loadTexture("res/textures/CAT.png", "CAT.png");

		auto cubeObj = PrefabManager::instantiate("DynamicCubePrefab", scene, glm::vec3(100.0f, 100.0f, 100.0f));
		cubeObj->getComponent<RenderComponent>()->addTexture(txtr);
		cubeObj->addComponent<CubePhysics>(PhysicsComponent::Type::DYNAMIC);



		//sphere
		auto sphereObj = std::make_shared<GameObject>("Sphere");
		sphereObj->setPosition(glm::vec3(0.0f, 10.0f, 0.0f)); // Positionner le cube au centre
		auto sphereRenderer = sphereObj->addComponent<SphereRenderer>();
		auto spherePhysics = sphereObj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);

		spherePhysics->setMass(10);
		scene.addGameObject(sphereObj);

		spherePhysics->applyForce(glm::vec3(0.0f, 0.0f, -100.0f));



		//textures
		std::vector<std::string> faces = { "res/textures/CubeMaps/skybox/right.jpg",
											"res/textures/CubeMaps/skybox/left.jpg",
											"res/textures/CubeMaps/skybox/top.jpg",
											"res/textures/CubeMaps/skybox/bottom.jpg",
											"res/textures/CubeMaps/skybox/front.jpg",
											"res/textures/CubeMaps/skybox/back.jpg" };
		//cubemap
		auto cb = std::make_shared<CubeMap>();
		cb->init();
		cb->setHDRTexture("res/textures/CubeMaps/small_harbour_sunset_4k.hdr");
		scene.setCubemap(cb);

		//sun

	}*/
	auto world = std::make_shared<GameObject>("World");
	world->setScale(glm::vec3(100.0f, 10.0f, 100.0f));
	world->setPosition(glm::vec3(0.0f, -150.0f, 0.0f));
	auto wren = world->addComponent<CubeRenderer>();
	world->addComponent<CubePhysics>(CubePhysics::Type::STATIC);
	scene.addGameObject(world);
	auto doogtxtr = TextureManager::loadTexture("res/textures/doog.PNG", "doog.PNG");
	world->getComponent<RenderComponent>()->addTexture(doogtxtr);

	auto camera = std::make_shared<CameraMC>(45, 1280.0f / 720.0f, 0.1f, 100000.0f);
	camera->setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	camera->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	scene.setCamera(camera);


	// Set up directional light (sun)
	auto sunLight = std::make_shared<Light>(
		LightType::DIRECTIONAL,
		glm::vec3(-2.0f, 4.0f, -1.0f),  // Position high up
		glm::vec3(-0.5f, -1.0f, -0.3f),  // Direction - angled for better shadows
		glm::vec3(1.0f, 0.95f, 0.8f),    // Warm sunlight color
		1.0f                              // Full intensity
	);
	LightManager::addLight(sunLight);


	//LightSpherePrefab
	//auto sphereObj = PrefabManager::instantiate("LightSpherePrefab", scene);
	//scene.addGameObject(sphereObj);

	//gameobject cursor 
	auto cursor = std::make_shared<GameObject>("Cursor");
	cursor->addComponent<UICursorComponent>();
	scene.addGameObject(cursor);


	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	
	//polygon mode


	// Create the framebuffer
	Window::CreateFramebuffer(1280, 720); // Initialize with proper size
	while (Window::isOpen()) {
		// Handle input
		if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
			Input::setMouseLocked(false);
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
			Input::setMouseLocked(true);
		}

		// Cube spawning logic
		{
			static float timer = 0.0f;
			timer += Engine::get_dt();
			if (timer > 221) {
				timer = 0.0f;
				PrefabManager::instantiate("DynamicCubePrefab", scene, glm::vec3(0.0f, 40.0f, 0.0f));
			}
		}


		Window::drawImGuiInterface();
		// Update camera controller
		UI::UICameraController(scene.getCamera());

		// Handle raycast selection
		{
			auto cameraPos = scene.getCamera()->getPosition();
			auto cameraDir = scene.getCamera()->getRotation();
			PxRaycastHit hitInfo;
			if (Physics::raycast(scene.getPhysicsScene()->getScene(), cameraPos, glm::normalize(cameraDir), 1000.0f, hitInfo)) {
				GameObject* hitObject = static_cast<GameObject*>(hitInfo.actor->userData);
				UI::handleRaycastSelection(hitObject);
				if (Input::isKeyPressed(GLFW_KEY_P)) {
					PrefabManager::instantiate("CubePrefab", scene,
						glm::vec3(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z));
				}
			}
		}


		Engine::renderUI(&scene);
		Engine::update(&scene);

		//LightManager::compute_shadow_mapping(&scene);
		Engine::render(&scene);
	}

	Window::shutdown();
	return 0;
}


