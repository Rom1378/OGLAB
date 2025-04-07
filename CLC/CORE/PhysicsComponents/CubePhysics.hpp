#pragma once

#include "PhysicsComponent.hpp"



class CubePhysics : public PhysicsComponent {
public:
	CubePhysics(Type t = Type::STATIC) : PhysicsComponent(t) {	}

	void init();

	void applyScale(const glm::vec3& scale);
};


