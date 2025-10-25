#pragma once
#include <memory>
#include <vector>
#include "RenderComponents/CubeMap.hpp"
#include "RenderComponents/Cursor.hpp"

class PhysicsScene;
class CameraComponent;

class Scene {
public:
	Scene();


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
	void renderPhysxDebugPass();

	std::shared_ptr<GameObject> createGameObject();
	void destroyGameObject(std::shared_ptr<GameObject> gameObject);

	void addGameObject(std::shared_ptr<GameObject> gameObject);
	inline std::vector<std::shared_ptr<GameObject>> const& getGameObjects() const { return m_gameObjects; }
	inline std::vector<std::shared_ptr<GameObject>> const& getShadowCasters() const { return m_shadowCasters; }
	inline std::vector<std::shared_ptr<GameObject>> const& getUIComponents() const { return m_UIcomponents; }

	//inline void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
	inline void setCubemap(std::shared_ptr<CubeMap> cubemap) { m_cubemap = cubemap; }
	inline std::shared_ptr<CubeMap> getCubemap() const { return m_cubemap; }
	inline std::shared_ptr<PhysicsScene> getPhysicsScene() const { return m_physicsScene; }


	inline const std::shared_ptr<CameraComponent>& getCurentCameraComponent() const { return m_current_camera_component; }
	inline const std::shared_ptr<GameObject>& getCurrentCamera() const { return m_current_camera; }
	inline const std::shared_ptr<GameObject>& getGameCameraHost() const { return m_gameCamera; }
	inline const std::shared_ptr<GameObject>& getDevCamera() const { return m_devCamera; }

	//inline void setGameCamera(const std::shared_ptr<CameraComponent>& cam) { m_gameCamera = cam; }
	inline void setGameCameraHost(const std::shared_ptr<GameObject>& go) { 
		if (go->getComponent<CameraComponent>() == nullptr)
			throw std::exception("GameCameraHost must have a camera component");
		else
			m_gameCamera = go;
	}
	inline void setDevCamera(const std::shared_ptr<GameObject>& cam) { m_devCamera = cam; }


protected:

	enum mode {
		EDIT,
		PLAY
	} currentMode;



	std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	std::vector<std::shared_ptr<GameObject>> m_shadowCasters; //this is not even used.... TODO
	std::vector<std::shared_ptr<GameObject>> m_UIcomponents;
	std::vector<std::shared_ptr<CameraComponent>> m_cameras;

	std::shared_ptr<GameObject> m_current_camera; // currently used camera gameobject
	std::shared_ptr<CameraComponent> m_current_camera_component;// camera component of the currently used camera gameobject

	std::shared_ptr<CubeMap> m_cubemap;
	std::shared_ptr<PhysicsScene> m_physicsScene;
	
	std::shared_ptr<GameObject> m_devCamera; // dev camera
	std::shared_ptr<GameObject> m_gameCamera;// camera from the game




};