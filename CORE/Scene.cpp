#include "Scene.hpp"
#include "PhysicsScene.hpp"
#include "PhysicsComponents/PhysicsComponent.hpp"
#include "renderComponents/Cursor.hpp"
#include "CameraComponents/CameraMC.hpp"
#include "Engine.hpp"


Scene::Scene() : m_current_camera(nullptr), m_current_camera_component{nullptr}, m_cubemap(nullptr),
m_devCamera{ nullptr }, m_gameCamera{ nullptr }, currentMode{EDIT}
{
    m_physicsScene = std::make_shared<PhysicsScene>();
    m_physicsScene->init();
    
    m_devCamera = std::make_shared<GameObject>("Dev Camera");
    m_current_camera_component=m_devCamera->addComponent<CameraMCComponent>();

}
void Scene::update(float dt) { 
    if(m_current_camera)
        m_current_camera->update(dt);

    // EDIT MODE
    if (Input::isKeyJustPressed(GLFW_KEY_F1)) {
        currentMode = EDIT;
        m_current_camera = m_devCamera;
        m_current_camera_component = m_devCamera->getComponent<CameraComponent>();
        LOG("Switched to EDIT mode");
    }
    else if (Input::isKeyJustPressed(GLFW_KEY_F2)) {
        Engine::reset_dt();
        currentMode = PLAY;
        if (m_gameCamera) {
            m_current_camera = m_gameCamera;
            m_current_camera_component = m_current_camera->getComponent<CameraComponent>();
        }

        LOG("Switched to PLAY mode");
    }
  
    switch (currentMode) {
    case PLAY:
        m_physicsScene->update(dt);
        for (auto& gameObject : m_gameObjects) {
            gameObject->update(dt);
        }

        onUpdate();
        break;
    case EDIT:

        break;
    }
   

}


void Scene::renderShadowCasters(const glm::mat4& lightMatrix) {
    std::shared_ptr<ShaderProgram> shadowShader = ShaderManager::getShader("simpleDepthShader");
    shadowShader->use();
    shadowShader->setMat4("lightSpaceMatrix", lightMatrix);

    for (auto obj : m_shadowCasters) {
        obj->renderRawGeometry(lightMatrix); // No material binding!
    }
}

void Scene::renderMainPass() {
    //cubemaps
    if (m_cubemap)
		m_cubemap->draw(m_current_camera_component->getViewMatrix(), m_current_camera_component->getProjectionMatrix());
    
    // Normal rendering with materials
    for (auto obj : m_gameObjects) {
        obj->renderWithMaterials(m_current_camera_component);
    }

}
void Scene::renderUIPass() {
	// Render UI components
	for (auto& uiComponent : m_UIcomponents) {
        uiComponent->renderWithMaterials(m_current_camera_component);
		//uiComponent->render(m_camera);
	}

}

std::shared_ptr<GameObject> Scene::createGameObject() {
    auto gameObject = std::make_shared<GameObject>();
    m_gameObjects.push_back(gameObject);
    return gameObject;
}

void Scene::destroyGameObject(std::shared_ptr<GameObject> gameObject) {
    auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
    if (it != m_gameObjects.end()) {
        m_gameObjects.erase(it);
    }
}

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject) {
    if (!gameObject)
        ENGINE_ASSERT(gameObject, "Cannot add null GameObject to Scene");


    m_gameObjects.push_back(gameObject);
    if (auto physicsComponent = gameObject->getComponent<PhysicsComponent>()) {
        m_physicsScene->addActor(physicsComponent->getActor());
    }
    if (auto physicsComponent = gameObject->getComponent<RenderComponent>()) {
		if (physicsComponent->getIsShadowCaster()) {
			m_shadowCasters.push_back(gameObject);
		}
	}
	if (auto uiComponent = gameObject->getComponent<UIComponent>()) {
		m_UIcomponents.push_back(gameObject);
	}

    if (auto camera = gameObject->getComponent<CameraComponent>()) {
        m_cameras.push_back(camera);
        //m_current_camera = m_cameras[0];
    }
}


void Scene::renderPhysxDebugPass() {
	if (m_physicsScene) {
        m_physicsScene->drawDebugVisualization();
        m_physicsScene->renderDebugVisualization(m_current_camera_component);
	}
	else {
		LOG_WARN("Physics scene is null, cannot render debug pass");
	}

}





