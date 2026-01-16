#pragma once
#include <memory>
#include <vector>
#include "CORE/ComponentManager.hpp"
#include "CORE/Systemes/Renderer/Skybox.hpp"


class Entity;
class Camera;

class Scene {
public:
	Scene() = default;

	virtual ~Scene() = default;
	virtual void init();
	virtual void shutdown() {}
	virtual void onUpdate() {}
	virtual void onRender() {}
	virtual void onImGuiRender() {}

	void update(float dt);


	Entity createEntity();
	Entity createEntity(const char* name);
	bool DestroyEntity(Entity e);

	template<typename T, typename... Args>
	inline T* addComponent(Entity e, Args&&... args) {
		return ComponentManager::addComponent<T>(e.getId(), std::forward<Args>(args)...);
	}
	template<typename T>
	inline void removeComponent(Entity e) {
		ComponentManager::removeComponent<T>(e.getId());
	}
	template<typename T>
	inline T* getComponent(Entity e) {
		return ComponentManager::getComponent<T>(e.getId());
	}
	template<typename T>
	inline bool hasComponent(Entity e) {
		return ComponentManager::hasComponent<T>(e.getId());
	}


	Entity getActiveCamera() const { return activeCamera; }
	bool hasSkybox() const { return skybox.isValid(); }
	const Skybox::skyboxHandle& getSkyboxHandle() const { return skybox; }
private:

	std::vector<Entity> entities;

	Skybox::skyboxHandle skybox = Skybox::INVALID_HANDLE;

	//scene camera
	Entity cameraEntity;

	Entity activeCamera;


};
