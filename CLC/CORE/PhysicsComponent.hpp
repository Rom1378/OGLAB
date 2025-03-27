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

	// scale
	// Improved PhysicsComponent scaling method
	void setScale(const glm::vec3& newScale) {
		if (!body) return;

        // Remove the existing shape
        PxShape* existingShape;
        PxU32 shapeCount = body->getNbShapes();
        if (shapeCount == 0) return;

        body->getShapes(&existingShape, 1);

        // Get the existing geometry
        PxGeometry* existingGeometry = nullptr;
        PxGeometryType::Enum geometryType = existingShape->getGeometry().getType();

        // Create a new shape based on the geometry type
        PxShape* newShape = nullptr;
        switch (geometryType) {
        case PxGeometryType::eBOX: {
            PxBoxGeometry boxGeom;
            existingShape->getGeometry(boxGeom);
            PxVec3 halfExtents(
                std::abs(newScale.x) / 2.0f,
                std::abs(newScale.y) / 2.0f,
                std::abs(newScale.z) / 2.0f
            );
            newShape = PxGetPhysics().createShape(
                PxBoxGeometry(halfExtents),
                *material,
                true
            );
            break;
        }
        case PxGeometryType::eSPHERE: {
            PxSphereGeometry sphereGeom;
            existingShape->getGeometry(sphereGeom);
            float averageScale = (newScale.x + newScale.y + newScale.z) / 3.0f;
            newShape = PxGetPhysics().createShape(
                PxSphereGeometry(sphereGeom.radius * averageScale),
                *material,
                true
            );
            break;
        }
        case PxGeometryType::eCAPSULE: {
            PxCapsuleGeometry capsuleGeom;
            auto cg=existingShape->getGeometry();
            existingShape->getGeometry(capsuleGeom);
            float averageScale = (newScale.x + newScale.y + newScale.z) / 3.0f;
            newShape = PxGetPhysics().createShape(
                PxCapsuleGeometry(
                    capsuleGeom.radius * averageScale,
                    capsuleGeom.halfHeight * averageScale
                ),
                *material,
                true
            );
            break;
        }
        default:
            // Unsupported geometry type
            return;
        }

        // Detach old shape and attach new shape
        body->detachShape(*existingShape);
        body->attachShape(*newShape);

        // Adjust mass for dynamic bodies
        if (isDynamic) {
            float originalMass = mass;
            float volumeScale = calculateVolumeScale(existingShape, newShape);
            mass *= volumeScale;

        }

        // Clean up
        existingShape->release();
        newShape->release();
    }

    // Helper method to calculate volume scale
    float calculateVolumeScale(PxShape* oldShape, PxShape* newShape) {
        PxGeometryType::Enum geometryType = oldShape->getGeometry().getType();

        switch (geometryType) {
        case PxGeometryType::eBOX: {
            PxBoxGeometry oldBoxGeom, newBoxGeom;
            oldShape->getGeometry(oldBoxGeom);
            newShape->getGeometry(newBoxGeom);
            return (newBoxGeom.halfExtents.x * newBoxGeom.halfExtents.y * newBoxGeom.halfExtents.z) /
                (oldBoxGeom.halfExtents.x * oldBoxGeom.halfExtents.y * oldBoxGeom.halfExtents.z);
        }
        case PxGeometryType::eSPHERE: {
            PxSphereGeometry oldSphereGeom, newSphereGeom;
            oldShape->getGeometry(oldSphereGeom);
            newShape->getGeometry(newSphereGeom);
            float oldVolume = (4.0f / 3.0f) * PxPi * PxPow(oldSphereGeom.radius, 3);
            float newVolume = (4.0f / 3.0f) * PxPi * PxPow(newSphereGeom.radius, 3);
            return newVolume / oldVolume;
        }
        case PxGeometryType::eCAPSULE: {
            PxCapsuleGeometry oldCapsuleGeom, newCapsuleGeom;
            oldShape->getGeometry(oldCapsuleGeom);
            newShape->getGeometry(newCapsuleGeom);
            float oldVolume = PxPi * PxPow(oldCapsuleGeom.radius, 2) *
                (4.0f / 3.0f * oldCapsuleGeom.radius + 2.0f * oldCapsuleGeom.halfHeight);
            float newVolume = PxPi * PxPow(newCapsuleGeom.radius, 2) *
                (4.0f / 3.0f * newCapsuleGeom.radius + 2.0f * newCapsuleGeom.halfHeight);
            return newVolume / oldVolume;
        }
        default:
            return 1.0f;
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
};



