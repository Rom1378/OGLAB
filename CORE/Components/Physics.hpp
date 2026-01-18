#pragma once

#include <glm/glm.hpp>
#include <physx/PxActor.h>
#include <physx/PxMaterial.h>
#include "CORE/Systems/Physics/Physics.hpp"

struct CharacterControllerStateComponent {
	glm::vec3 velocity{ 0 };
	float yaw{ 0 };
	float pitch{ 0 };
	bool isSprinting{ 0 };
	bool isOnTheGround{ 0 };
};


struct PhysicsBodyComponent {
	enum class PhysicsRole { Dynamic, Kinematic, CharacterController };

	PhysicsBodyComponent(PhysicsRole role) : role{ role } { PhysicsSystem::requestCreation(this); }
	~PhysicsBodyComponent() { PhysicsSystem::requestDestruction(this); }
	PhysicsRole role;
	physx::PxMaterial* material{ nullptr };
	float mass{ 1.0f }; // kg
	union {
		physx::PxRigidDynamic* rigidDynamic;
		physx::PxRigidActor* rigidActor;
		physx::PxController* controller;
	};
};
