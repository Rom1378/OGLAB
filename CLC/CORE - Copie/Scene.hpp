#pragma once
#include <memory>
#include <vector>
#include "GameObject.hpp"

class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void shutdown() {}
    
    void update(float dt) {
        for (auto& gameObject : m_gameObjects) {
            gameObject->update(dt);
        }
    }
    
    void render() {
        for (auto& gameObject : m_gameObjects) {
            gameObject->render();
        }
    }

    std::shared_ptr<GameObject> createGameObject() {
        auto gameObject = std::make_shared<GameObject>();
        m_gameObjects.push_back(gameObject);
        return gameObject;
    }

    void destroyGameObject(std::shared_ptr<GameObject> gameObject) {
        auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
        if (it != m_gameObjects.end()) {
            m_gameObjects.erase(it);
        }
    }

protected:
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
};