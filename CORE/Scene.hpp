#pragma once
#include <memory>
#include <vector>
#include "GameObject.hpp"
#include "Cameras/Camera.hpp"
#include "RenderComponents/CubeMap.hpp"
#include "PhysicsScene.hpp"


class Scene {
public:
	Scene() : m_camera(nullptr), m_cubemap(nullptr) {
		m_physicsScene = std::make_shared<PhysicsScene>();
	}
	virtual ~Scene() = default;
	virtual void init() {
		m_physicsScene->init();
	}
	virtual void shutdown() {}
	void update(float dt);
	void render();


	void renderShadowCasters(const glm::mat4& lightMatrix);
	void renderMainPass();

	std::shared_ptr<GameObject> createGameObject();
	void destroyGameObject(std::shared_ptr<GameObject> gameObject);

	void addGameObject(std::shared_ptr<GameObject> gameObject);
	inline std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return m_gameObjects; }

	inline void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
	inline std::shared_ptr<Camera> getCamera() { return m_camera; }
	inline void setCubemap(std::shared_ptr<CubeMap> cubemap) { m_cubemap = cubemap; }
	inline std::shared_ptr<CubeMap> getCubemap() { return m_cubemap; }
	inline std::shared_ptr<PhysicsScene> getPhysicsScene() { return m_physicsScene; }


protected:
	std::vector<std::shared_ptr<GameObject>> m_gameObjects;

	std::vector<std::shared_ptr<GameObject>> shadowCasters;

	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<CubeMap> m_cubemap;
	std::shared_ptr<PhysicsScene> m_physicsScene;

};