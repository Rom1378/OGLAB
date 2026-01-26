#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4
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


struct ColliderComponent {
	enum class Type {
		Box,
		Sphere,
		Capsule,
		Mesh // later
	};

	ColliderComponent(Type t) : type{ t } {}
	Type type;

	glm::vec3 halfExtents{ 0.5f }; // For Box
	float radius{ 0.5f };          // For Sphere / Capsule
	float height{ 1.8f };          // For Capsule

	glm::vec3 localPosition{ 0.0f };
	glm::quat localRotation{ 1, 0, 0, 0 };

	physx::PxMaterial* material{ nullptr };

	glm::vec3 getLocalEulerAngles() const {
		return glm::degrees(glm::eulerAngles(localRotation));
	}

	void setLocalEulerAngles(const glm::vec3& euler) {
		localRotation = glm::quat(glm::radians(euler));
	}
};


struct PhysicsBodyComponent {
	enum class PhysicsRole { Dynamic, Kinematic, Static, CharacterController };
	enum class State { Uninitialized, Alive, PendingDestroy	};

	PhysicsRole role;
	State state{ State::Uninitialized };

	bool controlledByPhysics{ true }; // false = game controls, true = physics controls
	void takeControl() { controlledByPhysics = false; }  // Game takes control
	void releaseControl() { controlledByPhysics = true; }  // Physics takes control

	float mass{ 1.0f };

	physx::PxRigidActor* actor{ nullptr };
	physx::PxController* controller{ nullptr };

	PhysicsBodyComponent(PhysicsRole r) : role(r), state{ State::Uninitialized } {

		//PhysicsSystem::requestCreation(this);
	}
	~PhysicsBodyComponent() {
		state = State::PendingDestroy;
		//PhysicsSystem::requestDestruction(this);
	}
};


/*
struct PhysicsBodyComponent {
	enum class PhysicsRole { Dynamic, Kinematic, CharacterController };
	enum class Shape{ Cube, Spheres, Capsules, CharacterController, Boxes, Planes
	};


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
};*/
