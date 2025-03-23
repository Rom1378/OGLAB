#include "../CORE/Scene.hpp"
#include "../CORE/Window.hpp"
#include "../CORE/Input.hpp"
#include "../CORE/Shader.hpp"
#include "../CORE/Camera.hpp"
#include <iostream>
#include <PxPhysicsAPI.h>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std::chrono;

#include "../CORE/PhysicsComponent.hpp"
#include "../CORE/RenderComponent.hpp"
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



class UICursorComponent : public RenderComponent {
public:
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
};

void registerPrefabs() {
    // Register a Cube prefab
    PrefabManager::registerPrefab(
        PrefabDefinition("CubePrefab", [](std::shared_ptr<GameObject> obj) {
            // Add required components
            auto cubeRenderer = obj->addComponent<CubeRenderer>();
            auto cubePhysics = obj->addComponent<CubePhysics>(PhysicsComponent::Type::DYNAMIC);
            cubePhysics->setMass(10);
            })
        .setDefaultPosition(glm::vec3(0.0f, 20.0f, 0.0f))
                );

    // Register a Sphere prefab
    PrefabManager::registerPrefab(
        PrefabDefinition("SpherePrefab", [](std::shared_ptr<GameObject> obj) {
            auto sphereRenderer = obj->addComponent<SphereRenderer>();
            auto spherePhysics = obj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);
            spherePhysics->setMass(10);
            })
        .setDefaultPosition(glm::vec3(0.0f, 10.0f, 0.0f))


                );

    // Register a Ground/World prefab
    PrefabManager::registerPrefab(
        PrefabDefinition("WorldPrefab", [](std::shared_ptr<GameObject> obj) {
            obj->setScale(glm::vec3(100.0f, 1.0f, 100.0f));
            obj->addComponent<CubeRenderer>();
            obj->addComponent<CubePhysics>();
            })
        .setDefaultPosition(glm::vec3(0.0f, -10.0f, 0.0f))
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
    {

        auto cubeObj = std::make_shared<GameObject>("Cube");
        cubeObj->setPosition(glm::vec3(0.0f, 10.0f, -30.0f)); // Positionner le cube au centre
        auto cubeRenderer = cubeObj->addComponent<CubeRenderer>();
        auto cubePhysics = cubeObj->addComponent<CubePhysics>(PhysicsComponent::Type::DYNAMIC);
        cubePhysics->setMass(10);
        scene.addGameObject(cubeObj);

        //add cube prefab
		auto prefCube = PrefabManager::instantiate("CubePrefab", scene);

        //sphere
        auto sphereObj = std::make_shared<GameObject>("Sphere");
        sphereObj->setPosition(glm::vec3(0.0f, 10.0f, 0.0f)); // Positionner le cube au centre
        auto sphereRenderer = sphereObj->addComponent<SphereRenderer>();
        auto spherePhysics = sphereObj->addComponent<SpherePhysics>(PhysicsComponent::Type::DYNAMIC);

        spherePhysics->setMass(10);
        scene.addGameObject(sphereObj);

        spherePhysics->applyForce(glm::vec3(0.0f, 0.0f, -100.0f));
        auto world = std::make_shared<GameObject>("World");
        world->setScale(glm::vec3(100.0f, 1.0f, 100.0f));
        world->setPosition(glm::vec3(0.0f, -50.0f, 0.0f));
        world->addComponent<CubeRenderer>();
        world->addComponent<CubePhysics>();
        scene.addGameObject(world);

        auto camera = std::make_shared<CameraMC>(45, 1280.0f / 720.0f, 0.1f, 100000.0f);
        camera->setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
        camera->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
        scene.setCamera(camera);


    }

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
        scene.update(deltaTime);
        scene.render();


        ///////

        {
            //instanciate a cube every seconds at 0 40 0
			static float timer = 0.0f;
			timer += deltaTime;
			if (timer > 1.0f)
			{
				timer = 0.0f;
				PrefabManager::instantiate("CubePrefab", scene, glm::vec3(0.0f, 40.0f, 0.0f));

            }


        }


        ///////

        Window::drawImGuiInterface();

        //draw data of the object touching the ray
        ImGui::Begin("Raycast");
        PxRaycastHit hitInfo;
		auto cameraPos = scene.getCamera()->getPosition();
        auto cameraDir = scene.getCamera()->getForward();
		//raycast(pxscene*, origin, direction, maxDistance, hitInfo, filterData, queryFlags, maxHits, hitCall, hitBlock)
		if (Physics::raycast(scene.getPhysicsScene()->getScene(), cameraPos, cameraDir, 1000.0f, hitInfo)) {
			ImGui::Text("Hit object at: %f, %f, %f", hitInfo.position.x, hitInfo.position.y, hitInfo.position.z);

            //if click p, create one cube at this position
			if (Input::isKeyJustPressed(GLFW_KEY_P))
            {
				PrefabManager::instantiate("CubePrefab", scene, glm::vec3(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z));
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
			if (seletctedGM)
			{
				ImGui::Text("Selected object: %s", seletctedGM->getName());
                //move and rotate with slider
                    glm::vec3 pos = seletctedGM->getPosition();
                    glm::vec3 rot = seletctedGM->getRotation();
                    ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100.0f, 100.0f);
                    ImGui::SliderFloat3("Rotation", glm::value_ptr(rot), -1.0f, 1.0f);
                    seletctedGM->setPosition(pos);
                    seletctedGM->setRotation(rot);


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