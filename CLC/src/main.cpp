#include "../CORE/LightManager.hpp"

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
#include "../CORE/Prefabs/SomePrefabs.hpp"

#include "../CORE/UI/CameraController.hpp"

using namespace std::chrono;

high_resolution_clock::time_point lastTime = high_resolution_clock::now();

float getDeltaTime() {
	high_resolution_clock::time_point currentTime = high_resolution_clock::now();
	duration<float> deltaTime = duration_cast<duration<float>>(currentTime - lastTime);
	lastTime = currentTime;
	return deltaTime.count(); // Returns time in seconds
}

void renderImGuiObjectEditor(GameObject* selectedObject) {
	if (!selectedObject) return;

	// Create a window for the object editor
	if (ImGui::Begin("Object Editor")) {
		ImGui::Text("Selected: %s", selectedObject->getName());

		// Position editor
		glm::vec3 position = selectedObject->getPosition();
		if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
			selectedObject->setPosition(position, true); // true to update physics
		}

		// Option 1: Edit using Euler angles (more intuitive)
		glm::vec3 rotation = selectedObject->getRotation();
		if (ImGui::DragFloat3("Rotation (Euler)", glm::value_ptr(rotation), 1.0f)) {
			selectedObject->setRotation(rotation, true); // true to update physics
		}

		// Option 2: Edit using quaternion (more precise, but less intuitive)
		glm::quat rotQuat = selectedObject->getRotationQuaternion();
		float quatValues[4] = { rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w };
		if (ImGui::DragFloat4("Rotation (Quaternion)", quatValues, 0.01f)) {
			// Normalize the quaternion to prevent distortion
			glm::quat newQuat(quatValues[3], quatValues[0], quatValues[1], quatValues[2]);
			newQuat = glm::normalize(newQuat);
			selectedObject->setRotationQuaternion(newQuat, true);
		}

		// Add a button to normalize the quaternion explicitly
		if (ImGui::Button("Normalize Quaternion")) {
			glm::quat normalized = glm::normalize(selectedObject->getRotationQuaternion());
			selectedObject->setRotationQuaternion(normalized, true);
		}

		// Scale editor
		glm::vec3 scale = selectedObject->getScale();
		if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
			selectedObject->setScale(scale);
		}
	}
	ImGui::End();
}



class UICursorComponent : public RenderComponent {
public:

	UICursorComponent() : RenderComponent() {
		init();
	}
	UICursorComponent(const UICursorComponent& other) : RenderComponent(other) {
		init();
	}
	void init() override {
		vertices = { -10.0f,  0.0f,  // Horizontal line
	 10.0f,  0.0f,
	 0.0f, -10.0f,  // Vertical line
	 0.0f,  10.0f };
		indices = {};


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		setShader("cursor");
	}

	void draw(const std::shared_ptr<Camera> cam) override {
		draw();
	}
	void draw() override {

		auto shader = ShaderManager::getShader("cursor");
		if (!shader) return;

		shader->use();

		glDisable(GL_DEPTH_TEST);  // UI elements should render on top

		// Compute orthographic projection
		glm::mat4 projection = glm::ortho(0.0f, (float)Window::getFrameBufferWidth(), 0.0f, (float)Window::getFrameBufferHeight());
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(Window::getFrameBufferWidth() / 2.0f, Window::getFrameBufferHeight() / 2.0f, 0.0f));

		// Use shader program

		// Send matrices to shader
		GLuint projLoc = glGetUniformLocation(shader->getProgram(), "uProjection");
		GLuint modelLoc = glGetUniformLocation(shader->getProgram(), "uModel");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Draw crosshair
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 4);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
	}
	void draw(const glm::mat4& view, const glm::mat4& projection) override {
		draw();
	}
};




int main() {
	// register prefabs
	registerPrefabs();

	// Initialize window
	Window::WindowProps props;
	props.title = "CLC";
	props.width = 1280;
	props.height = 720;
	Window::init(props);
	std::cout << Window::isVSync() << std::endl;


	// Initialize input
	Input::init();

	// Load shaders
	ShaderManager::loadConfigs("shaders.json");

	// Initialize PhysX
	Physics::init();

	GameObject* seletctedGM = nullptr;

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
	std::vector<std::string> faces = {  "res/textures/CubeMaps/skybox/right.jpg",
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

}
*/
	auto world = std::make_shared<GameObject>("World");
	world->setScale(glm::vec3(1000.0f, 10.0f, 1000.0f));
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


	auto sunLight = std::make_shared<Light>(
		LightType::DIRECTIONAL,  // Light type is DIRECTIONAL for sun
		glm::vec3(0.0f, 0.0f, 0.0f),  // Position (less relevant for directional lights)
		glm::vec3(0.0f, -1.0f, 0.0f),  // Direction - pointing downward, simulating sun from above
		glm::vec3(1.0f, 0.95f, 0.8f),  // Warm sunlight color (slightly warm white)
		1.0f  // Intensity
	);
	LightManager::addLight(sunLight);


	//LightSpherePrefab
	//auto sphereObj = PrefabManager::instantiate("LightSpherePrefab", scene);



	//gameobject cursor 
	auto cursor = std::make_shared<GameObject>("Cursor");
	cursor->addComponent<UICursorComponent>();
	scene.addGameObject(cursor);



	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	// Create the framebuffer
	Window::CreateFramebuffer(1, 1);

	while (Window::isOpen()) {
		float deltaTime = getDeltaTime();

		if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
			Input::setMouseLocked(false); // Unlock mouse when Escape is pressed
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
			Input::setMouseLocked(true);
		}

		Window::clear();
		Input::update();
		// Update physics with dt = 0.001f
		scene.update(0.01f);
		scene.render();

		///////
		{
			//instanciate a cube every seconds at 0 40 0
			static float timer = 0.0f;
			timer += deltaTime;
			if (timer > 0.6f)
			{
				timer = 0.0f;
				PrefabManager::instantiate("DynamicCubePrefab", scene, glm::vec3(0.0f, 40.0f, 0.0f));
			}
		}
		///////

		Window::drawImGuiInterface();
		if (Window::getViewPortChanged())
		{
			scene.getCamera()->setAspectRatio(Window::getFrameBufferWidth() / Window::getFrameBufferHeight());
		}
		UICameraController(scene.getCamera());


		//draw data of the object touching the ray
		ImGui::Begin("Raycast");
		PxRaycastHit hitInfo;
		auto cameraPos = scene.getCamera()->getPosition();
		auto cameraDir = scene.getCamera()->getForward();
		//raycast(pxscene*, origin, direction, maxDistance, hitInfo, filterData, queryFlags, maxHits, hitCall, hitBlock)
		if (Physics::raycast(scene.getPhysicsScene()->getScene(), cameraPos, cameraDir, 1000.0f, hitInfo)) {
			ImGui::Text("Hit object at: %f, %f, %f", hitInfo.position.x, hitInfo.position.y, hitInfo.position.z);

			//if click p, create one cube at this position
			if (Input::isKeyPressed(GLFW_KEY_P))
			{
				PrefabManager::instantiate("CubePrefab", scene, glm::vec3(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z));
				std::cout << "cube created at: " << hitInfo.position.x << ", " << hitInfo.position.y << ", " << hitInfo.position.z << std::endl;
			}

			//check if user data is not null and if not null detect wich class it is from
			GameObject* obj = static_cast<GameObject*>(hitInfo.actor->userData);
			if (obj)// now show data of the object like obj,getName
			{
				ImGui::Text("Object name: %s", obj->getName());
				//if click, selectec the gm
				if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				{
					seletctedGM = obj;
				}
			}
			else
			{
				ImGui::Text("Object name: No name");
			}
		}

		renderImGuiObjectEditor(seletctedGM);
		ImGui::End();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		Window::update();

	}

	Window::shutdown();
	return 0;
}
