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

using namespace std::chrono;

#include "../CORE/CubeRenderer.hpp"
#include "../CORE/SphereRenderer.hpp"
#include "../CORE/SpherePhysics.hpp"
#include "../CORE/Prefabs/PrefabManager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

high_resolution_clock::time_point lastTime = high_resolution_clock::now();

float getDeltaTime() {
    high_resolution_clock::time_point currentTime = high_resolution_clock::now();
    duration<float> deltaTime = duration_cast<duration<float>>(currentTime - lastTime);
    lastTime = currentTime;
    return deltaTime.count(); // Returns time in seconds
}

void UICameraController(std::shared_ptr<Camera> cam) {
	//an Imgui that allow me to control the camera parameters
	ImGui::Begin("Camera");
	glm::vec3 pos = cam->getPosition();
	glm::vec3 rot = cam->getRotation();
    float fov = cam->getFOV();
	float aspectRatio = cam->getAspectRatio();
	float nearPlane = cam->getNearPlane();
	float farPlane = cam->getFarPlane();

	ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f);
	ImGui::SliderFloat("Aspect Ratio", &aspectRatio, 0.1f, 10.0f);
	ImGui::SliderFloat("Near Plane", &nearPlane, 0.1f, 100.0f);
	ImGui::SliderFloat("Far Plane", &farPlane, 100.0f, 10000.0f);
	cam->setFOV(fov);
	cam->setAspectRatio(aspectRatio);
	cam->setNearPlane(nearPlane);
	cam->setFarPlane(farPlane);


	ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100.0f, 100.0f);
	ImGui::SliderFloat3("Rotation", glm::value_ptr(rot), -180.0f, 180.0f);
	cam->setPosition(pos);
	cam->setRotation(rot);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

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
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(Window::getFrameBufferWidth()/ 2.0f, Window::getFrameBufferHeight() / 2.0f, 0.0f));

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

void registerPrefabs() {
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
            sphereRenderer->setColor(glm::vec4(1.0f, 0.0f, 1.0f,1.0));
			auto spherePhysics = obj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);
            auto light = obj->addComponent<Light>(LightType::POINT, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), 1.0f);
            LightManager::addLight(light);
			spherePhysics->setMass(3);
            //add movement
			})
		.setDefaultPosition(glm::vec3(0.0f, 10.0f, 0.0f))
				);
}


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
        auto wren= world->addComponent<CubeRenderer>();
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

            //A ui that allow me to control a gameobject, moving it turning it. adding forces...
		}


        /*
        if (seletctedGM)
        {
            ImGui::Text("Selected object: %s", seletctedGM->getName());
            //move and rotate with slider
            glm::vec3 pos = seletctedGM->getPosition();
            glm::vec3 rot = seletctedGM->getRotation();
            glm::vec3 scale = seletctedGM->getScale();
            glm::vec3 col = seletctedGM->getComponent<RenderComponent>()->getColor();

            ImGui::ColorEdit3("Color", glm::value_ptr(col));
            ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100.0f, 100.0f);
            ImGui::SliderFloat3("Rotation", glm::value_ptr(rot), -90.0f, 90.0f);
            ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 0.1f, 10.0f);
            seletctedGM->setPosition(pos);
            seletctedGM->setRotation(rot);
            seletctedGM->setScale(scale);
            seletctedGM->getComponent<RenderComponent>()->setColor(col);


        }*/


        if (seletctedGM)
        {
            ImGui::Text("Selected object: %s", seletctedGM->getName());
            //move and rotate with slider
            glm::vec3 pos = seletctedGM->getPosition();
            glm::vec3 rot = seletctedGM->getRotation();
            glm::vec3 scale = seletctedGM->getScale();
            glm::vec3 col = seletctedGM->getComponent<RenderComponent>()->getColor();

            ImGui::ColorEdit3("Color", glm::value_ptr(col));
            if (ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100.0f, 100.0f))
                seletctedGM->setPosition(pos);
            if (ImGui::SliderFloat3("Rotation", glm::value_ptr(rot), -180.0f, 180.0f)){
                if (auto physics = seletctedGM->getComponent<PhysicsComponent>()) {
                    // Make kinematic, rotate, then optionally make dynamic again
                    physics->setRotation(rot);

                    // Optional: Add small delay before making dynamic again
                    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
        }

                    if (ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 0.1f, 10.0f))
                        seletctedGM->setScale(scale);

					seletctedGM->getComponent<RenderComponent>()->setColor(col);

                    if (ImGui::Button("Test 90° Y Rotation")) {
                        if (seletctedGM) {
                            glm::vec3 currentRot = seletctedGM->getRotation();
                            glm::vec3 newRot = currentRot + glm::vec3(0, 90, 0);

                            // Get physics component
                            if (auto physics = seletctedGM->getComponent<PhysicsComponent>()) {
                                // Make kinematic, rotate, then optionally make dynamic again
                                physics->setRotation(newRot);

                                // Optional: Add small delay before making dynamic again
                                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            }

                            // Update game object transform
                            seletctedGM->setRotation(newRot, false); // Don't trigger physics update again
                        }
                    }

			}
		ImGui::End();

       
       
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        Window::update();

       }

    Window::shutdown();
    return 0;
}