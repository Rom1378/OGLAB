#include "devScene.hpp"

void DevScene::init() {
	//auto prefCube = PrefabManager::instantiate("DynamicCubePrefab");
	//addGameObject(prefCube);

	//prefCube->setPosition(glm::vec3(0.0f, 10.0f, 0.0f));

	//remove gravaity
	//scene.getPhysicsScene()->getScene()->setGravity(PxVec3(0, 0, 0));
	//scene.getPhysicsScene()->getScene()->setGravity(PxVec3(0, 0, 0));

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

		//sun

	}*/

	//cubemap

	/*
	auto cb = std::make_shared<CubeMap>();
	cb->init();
	cb->setHDRTexture("res/textures/CubeMaps/small_harbour_sunset_4k.hdr");
	setCubemap(cb);

	auto world = std::make_shared<GameObject>("World");
	world->setScale(glm::vec3(220.0f, 10.0f, 220.0f));
	world->setPosition(glm::vec3(0.0f, -0.0f, 0.0f));
	auto wren = world->addComponent<CubeRenderer>();
	world->addComponent<CubePhysics>(CubePhysics::Type::STATIC);
	addGameObject(world);
	auto doogtxtr = TextureManager::loadTexture("res/textures/doog.PNG", "doog.PNG");
	auto cattxtr = TextureManager::loadTexture("res/textures/CAT.png", "CAT.png");
	world->getComponent<RenderComponent>()->addTexture(doogtxtr);

	auto sphere = PrefabManager::instantiate("SpherePrefab", glm::vec3(0.0f, 10.0f, 0.0f));
	//add velocity
	sphere->getComponent<PhysicsComponent>()->setLinearVelocity(glm::vec3(10.0f, 30.0f, 0.0f));
	*/

	auto camera = std::make_shared<CameraMC>(45, 1280.0f / 720.0f, 0.1f, 100000.0f);
	camera->setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	camera->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
	setCamera(camera);


	// Set up directional light (sun)
	sunLight = std::make_shared<Light>(
		LightType::DIRECTIONAL,
		glm::vec3(-2.0f, 200.0f, -1.0f),  // Position high up
		glm::vec3(-0.5f, -1.0f, -0.3f),  // Direction - angled for better shadows
		glm::vec3(1.0f, 0.95f, 0.8f),    // Warm sunlight color
		1.0f                              // Full intensity
	);
	LightManager::addLight(sunLight, "SunLight");


	//LightSpherePrefab
	//auto sphereObj = PrefabManager::instantiate("LightSpherePrefab", scene);
	//scene.addGameObject(sphereObj);

	//gameobject cursor 
	auto cursor = std::make_shared<GameObject>("Cursor");
	cursor->addComponent<UICursorComponent>();
	addGameObject(cursor);
}

void DevScene::onUpdate() {
	// Cube spawning logic
	static float timer = 0.0f;
	timer += Engine::get_dt();
	if (timer > 111) {
		timer = 0.0f;
		auto c = PrefabManager::instantiate("DynamicCubePrefab", glm::vec3(0.0f, 140.0f, 0.0f));
		c->getComponent<RenderComponent>()->addTexture(TextureManager::getTexture("CAT.png"));
	}
	if (Input::isKeyJustPressed(GLFW_KEY_F1)) {
		currentMode = EDIT;
		LOG("Switched to EDIT mode");
	}
	else if (Input::isKeyJustPressed(GLFW_KEY_F2)) {
		currentMode = PLAY;
		LOG("Switched to PLAY mode");
	}
	// Handle raycast selection
		if (!m_camera) {
			LOG_WARN("m_camera is null");
		}
		else {

			auto cameraPos = m_camera->getPosition();
			auto cameraDir = m_camera->getRotation();
			if (Physics::raycast(getPhysicsScene()->getScene(), cameraPos, glm::normalize(cameraDir), 1000.0f, hitInfo)) {
				hitRaycastObject = static_cast<GameObject*>(hitInfo.actor->userData);


				if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
					if (currentMode == EDIT) {
						// If in EDIT mode, add a cube at the hit position
					auto cube=	PrefabManager::instantiate("DynamicCubePrefab", glm::vec3(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z));
					cube->getComponent<RenderComponent>()->addTexture(TextureManager::getTexture("CAT.png"));
					addGameObject(cube);
					}
					else if (currentMode == PLAY) {
						// If in PLAY mode, select the nearest object under the cursor
						if (hitRaycastObject) {
							selectedHitRaycastObject = hitRaycastObject;
						}
					}
				}
			}
		}


}

void DevScene::onImGuiRender() {
	// Edit camera controller
	UI::UICameraController(getCamera());

	UI::handleRaycastSelection(selectedHitRaycastObject);
	

	//change light pos
	ImGui::Begin("Light");
	glm::vec3 lightPos = sunLight->getPosition();
	if (ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.1f)) {
		sunLight->setPosition(lightPos);
	}
	glm::vec3 lightDir = sunLight->getDirection();
	if (ImGui::DragFloat3("Light Direction", glm::value_ptr(lightDir), 0.1f)) {
		sunLight->setDirection(lightDir);
	}
	glm::vec3 lightColor = sunLight->getColor();
	if (ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor))) {
		sunLight->setColor(lightColor);
	}
	float lightIntensity = sunLight->getIntensity();
	if (ImGui::DragFloat("Light Intensity", &lightIntensity, 0.01f, 0.0f, 10.0f)) {
		sunLight->setIntensity(lightIntensity);
	}
	ImGui::End();

	//farplan and ortho size edit
	ImGui::Begin("LGITH");
	float farPlane = LightManager::getShadowMapper()->getFarPlane();
	if (ImGui::DragFloat("Far Plane", &farPlane, 0.1f, 0.0f, 10000.0f)) {
		LightManager::getShadowMapper()->setFarPlane(farPlane);
	}
	float orthoSize = LightManager::getShadowMapper()->getOrthoSize();
	if (ImGui::DragFloat("Ortho Size", &orthoSize, 0.1f, 0.0f, 10000.0f)) {
		LightManager::getShadowMapper()->setOrthoSize(orthoSize);
	}
	ImGui::End();
}