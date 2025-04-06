#include "Scene.hpp"
#include "PhysicsComponent.hpp"

void Scene::update(float dt) { 

    if(m_camera)
        m_camera->update(dt);


    m_physicsScene->update(dt);

    for (auto& gameObject : m_gameObjects) {
        gameObject->update(dt);
    }

}


void Scene::renderShadowCasters(const glm::mat4& lightMatrix) {
    std::shared_ptr<ShaderProgram> shadowShader = ShaderManager::getShader("simpleDepthShader");
    shadowShader->use();
    shadowShader->setMat4("lightSpaceMatrix", lightMatrix);

    for (auto obj : shadowCasters) {
        obj->renderRawGeometry(lightMatrix); // No material binding!
    }
}

void Scene::renderMainPass() {
    //cubemaps
    if (m_cubemap)
		m_cubemap->draw(m_camera->getViewMatrix(), m_camera->getProjectionMatrix());
    
    // Normal rendering with materials
    for (auto obj : m_gameObjects) {
        obj->renderWithMaterials(m_camera);
    }


}

void Scene::render() {

    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = m_camera->getProjectionMatrix();

    //drawcubemap
    if (m_cubemap)
        m_cubemap->draw(view,projection);
    for (auto& gameObject : m_gameObjects) {
        //gameObject->render(view, projection);
        gameObject->render(m_camera);
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
    m_gameObjects.push_back(gameObject);
    if (auto physicsComponent = gameObject->getComponent<PhysicsComponent>()) {
        m_physicsScene->addActor(physicsComponent->getActor());
    }
    if (auto physicsComponent = gameObject->getComponent<RenderComponent>()) {
		if (physicsComponent->getIsShadowCaster()) {
			shadowCasters.push_back(gameObject);
		}
	}
}
