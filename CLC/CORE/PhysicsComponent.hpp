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

using namespace physx;

class PhysicsComponent : public Component {
protected:

	PxRigidActor* body = nullptr;
	PxMaterial* material = nullptr;

	float mass = 1.0f;
	bool isDynamic = false;

    std::vector<PxShape*> shapes;

public:
	enum class Type {
		STATIC,
		DYNAMIC
	};
	PhysicsComponent(Type t = Type::STATIC);

	//update static or dynamic
	//void setType(Type t);
	void applyForce(const glm::vec3& force);
	void applyTorque(const glm::vec3& torque);
	void setMass(float m);
	float getMass();
	void setAngularVelocity(const glm::vec3& velocity);
	void setLinearVelocity(const glm::vec3& velocity);
	void updateTransform();

    // Apply a scale to the physics body
    virtual void applyScale(const glm::vec3& scale) {}
    // Release all shapes and recreate them with the new scale
    void releaseAllShapes();

	// Override update to call updateTransform
	void update(float dt) override {
		updateTransform();
	}
	// Set the position and rotation of the physics body from the GameObject
	void setRotation(const glm::vec3& rotation) {
		if (body) {
			PxQuat quat = PxQuat(glm::radians(rotation.x), PxVec3(-1, 0, 0)) *
				PxQuat(glm::radians(rotation.y), PxVec3(0, 1, 0)) *
				PxQuat(glm::radians(rotation.z), PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, quat));
		}
	}

	void setPosition(const glm::vec3& position) {
		if (body) {
			body->setGlobalPose(PxTransform(position.x, position.y, position.z));
		}
	}

	void setScale(const glm::vec3& scale) {
		applyScale(scale);
	}
	
    // Helper method to get the current scale (approximate)
    glm::vec3 getScale() {
        if (!body || body->getNbShapes() == 0) return glm::vec3(1.0f);

        PxShape* shape;
        body->getShapes(&shape, 1);
        PxGeometryHolder geomHolder = shape->getGeometry();

        switch (geomHolder.getType()) {
        case PxGeometryType::eBOX: {
            PxBoxGeometry box = geomHolder.box();
            return glm::vec3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
        }
                                 // Add cases for other geometry types as needed
        default:
            return glm::vec3(1.0f);
        }
    }

	~PhysicsComponent() {
		if (body) {
			body->release();
		}
		if (material) {
			material->release();
		}
	}

	//get actor
	PxRigidActor* getActor() {
		return body;
	}

	void setUserData(void* data) {
		body->userData = data;
	}
};

class CubePhysics : public PhysicsComponent {
public:
	CubePhysics(Type t = Type::STATIC) : PhysicsComponent(t) {	}

	void init();

    void applyScale(const glm::vec3& scale);
};



