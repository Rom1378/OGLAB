#pragma once

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

class Scene;
class PhysicScene;
class PhysicsBodyComponent;

namespace PhysicsSystem {
	void init();
	void update(Scene* scene, float dt);

	void shutdown();

	//physx::PxScene* createScene();
	const physx::PxPhysics* getPhysics();
	const physx::PxScene* getScene();

	void flushQueues(Scene* scene);

	void requestCreation(PhysicsBodyComponent* c);
	void requestDestruction(PhysicsBodyComponent* c);


	bool raycast(physx::PxScene* scene, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, physx::PxRaycastHit& hitInfo);
	void enable_debug_visualization(physx::PxScene* scene, bool enable);
	void imgui_debug_menu(physx::PxScene* scene);
	physx::PxControllerManager& getControllerManger();


}