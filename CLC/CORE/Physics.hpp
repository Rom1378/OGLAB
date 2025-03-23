#pragma once

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

using namespace physx;

namespace Physics
{
	void init();
	void update();
	void shutdown();

	PxScene* createScene();

	PxPhysics* getPhysics();
	bool raycast(PxScene*scene, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, PxRaycastHit& hitInfo);
}

