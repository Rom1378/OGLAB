#pragma once
#include <memory>
#include "Physics.hpp"

class PhysicsScene {
public:
	void init() { m_scene = Physics::createScene(); }

	void update(float dt) {
		m_scene->simulate(dt);
		m_scene->fetchResults(true);

	}

	void shutdown() { m_scene->release(); }

	void addActor(PxRigidActor* actor) { m_scene->addActor(*actor); }

	void setGravity(float gravityx, float gravityy, float gravityz) {
		m_gravity = glm::vec3(gravityx, gravityy, gravityz);
		m_scene->setGravity(PxVec3(gravityx, gravityy, gravityz));

	}

	glm::vec3 getGravity() { return m_gravity; }

	PxScene* getScene() { return m_scene; }
private:
	PxScene* m_scene;
	glm::vec3 m_gravity;

};