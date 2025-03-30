#pragma once
#include <memory>
#include <vector>
#include <string>
#include <PxPhysicsAPI.h>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "../CORE/Component.hpp"
#include "../CORE/GameObject.hpp"
#include "Physics.hpp"
#include "PxActor.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxMaterial.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include <iomanip>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace physx;

class PhysicsComponent : public Component {
protected:

	PxRigidActor* body = nullptr;
	PxMaterial* material = nullptr;

	float mass = 1.0f;
	bool isDynamic = false;

    std::vector<PxShape*> shapes;

public:
	enum class UpdateMode {
		PHYSICS,    // Let physics control the body
		MANUAL      // We control the body manually
	};

	enum class Type {
		STATIC,
		DYNAMIC
	};
	PhysicsComponent(Type t = Type::DYNAMIC);
	void applyForce(const glm::vec3& force);
	void applyTorque(const glm::vec3& torque);
	void setMass(float m);
	float getMass();
	void setAngularVelocity(const glm::vec3& velocity);
	void setLinearVelocity(const glm::vec3& velocity);
	void updateTransform();

	void updatePhysX();

    // Apply a scale to the physics body
    virtual void applyScale(const glm::vec3& scale) {}
    // Release all shapes and recreate them with the new scale
    void releaseAllShapes();

	// Override update to call updateTransform
	void update(float dt) override {
		updateTransform();
	}
	// Set the position and rotation of the physics body from the GameObject

	void setRotation(const glm::vec3& rotation);
	void setPosition(const glm::vec3& position);
	inline	void setScale(const glm::vec3& scale) {	applyScale(scale); }
	
	glm::vec3 getScale();

	inline PxRigidActor* getActor() { return body; }
	inline void setUserData(void* data) { body->userData = data; }
	~PhysicsComponent() {
		if (body) {
			body->release();
		}
		if (material) {
			material->release();
		}
	}
};

class CubePhysics : public PhysicsComponent {
public:
	CubePhysics(Type t = Type::STATIC) : PhysicsComponent(t) {	}

	void init();

    void applyScale(const glm::vec3& scale);
};


