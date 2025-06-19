#pragma once
#include <memory>
#include <vector>
#include "Cameras/Camera.hpp"
#include "RenderComponents/CubeMap.hpp"
#include "PhysicsScene.hpp"
#include "RenderComponents/Cursor.hpp"


class Scene {
public:
	Scene() : m_camera(nullptr), m_cubemap(nullptr) {
		m_physicsScene = std::make_shared<PhysicsScene>();
		m_physicsScene->init();
	}


	virtual ~Scene() = default;
	virtual void init() {}
	virtual void shutdown() {}
	virtual void onUpdate() {}
	virtual void onRender() {}
	virtual void onImGuiRender() {}



	void update(float dt);


	void renderShadowCasters(const glm::mat4& lightMatrix);
	void renderMainPass();
	void renderUIPass();

	std::shared_ptr<GameObject> createGameObject();
	void destroyGameObject(std::shared_ptr<GameObject> gameObject);

	void addGameObject(std::shared_ptr<GameObject> gameObject);
	inline std::vector<std::shared_ptr<GameObject>> const& getGameObjects() { return m_gameObjects; }
	inline std::vector<std::shared_ptr<GameObject>> const& getShadowCasters() { return m_shadowCasters; }
	inline std::vector<std::shared_ptr<GameObject>> const& getUIComponents() { return m_UIcomponents; }

	inline void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
	inline std::shared_ptr<Camera> getCamera() { return m_camera; }
	inline void setCubemap(std::shared_ptr<CubeMap> cubemap) { m_cubemap = cubemap; }
	inline std::shared_ptr<CubeMap> getCubemap() { return m_cubemap; }
	inline std::shared_ptr<PhysicsScene> getPhysicsScene() { return m_physicsScene; }



protected:
	std::vector<std::shared_ptr<GameObject>> m_gameObjects;

	std::vector<std::shared_ptr<GameObject>> m_shadowCasters; //this is not even used.... TODO

	std::vector<std::shared_ptr<GameObject>> m_UIcomponents;

	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<CubeMap> m_cubemap;
	std::shared_ptr<PhysicsScene> m_physicsScene;




};