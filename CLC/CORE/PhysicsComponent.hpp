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

    void setScale(const glm::vec3& scale) {
        if (!body) return;

        // Get current shape(s)
        PxU32 nbShapes = body->getNbShapes();
        PxShape** shapes = new PxShape * [nbShapes];
        body->getShapes(shapes, nbShapes);

        // Scale each shape
        for (PxU32 i = 0; i < nbShapes; i++) {
            PxShape* shape = shapes[i];
            PxGeometryHolder geomHolder = shape->getGeometry();

            // Handle different geometry types
            switch (geomHolder.getType()) {
            case PxGeometryType::eBOX: {
                PxBoxGeometry box = geomHolder.box();
                box.halfExtents = PxVec3(
                    box.halfExtents.x * scale.x,
                    box.halfExtents.y * scale.y,
                    box.halfExtents.z * scale.z
                );
                shape->setGeometry(box);
                break;
            }
            case PxGeometryType::eSPHERE: {
                PxSphereGeometry sphere = geomHolder.sphere();
                // For sphere, we'll scale by largest component (uniform scaling)
                float maxScale = glm::max(glm::max(scale.x, scale.y), scale.z);
                sphere.radius *= maxScale;
                shape->setGeometry(sphere);
                break;
            }
            case PxGeometryType::eCAPSULE: {
                PxCapsuleGeometry capsule = geomHolder.capsule();
                // For capsule, scale radius by average of x/z, height by y
                float radiusScale = (scale.x + scale.z) * 0.5f;
                capsule.radius *= radiusScale;
                capsule.halfHeight *= scale.y;
                shape->setGeometry(capsule);
                break;
            }
                                         // Add cases for other geometry types as needed
            default:
                std::cerr << "Warning: Unsupported geometry type for scaling" << std::endl;
                break;
            }
        }

        delete[] shapes;

        // For dynamic bodies, we need to wake them up and update mass/inertia
        if (isDynamic) {
            PxRigidDynamic* dynamic = static_cast<PxRigidDynamic*>(body);
            dynamic->wakeUp();

            // Update mass properties if mass was previously set
            if (mass > 0.0f) {
                PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);
            }
        }
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
};



