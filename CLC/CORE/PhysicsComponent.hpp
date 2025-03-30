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
	void setUpdateMode(UpdateMode mode) {
		if (isDynamic && body) {
			PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
			if (dynamic) {
				dynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, mode == UpdateMode::MANUAL);
				if (mode == UpdateMode::MANUAL) {
					dynamic->setLinearVelocity(PxVec3(0, 0, 0));
					dynamic->setAngularVelocity(PxVec3(0, 0, 0));
				}
			}
		}
	}
		//update static or dynamic
	//void setType(Type t);
	void applyForce(const glm::vec3& force);
	void applyTorque(const glm::vec3& torque);
	void setMass(float m);
	float getMass();
	void setAngularVelocity(const glm::vec3& velocity);
	void setLinearVelocity(const glm::vec3& velocity);
	void updateTransform();

	void updatePhysX();
	void debugDrawRotationAxes();

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

	/*
	void setRotation(const glm::vec3& eulerAngles) {
		if (body) {
			glm::vec3 radAngles = glm::radians(eulerAngles);
			glm::quat q = glm::quat(radAngles);
			PxQuat pxQuat(q.x, q.y, q.z, q.w);
			pxQuat.normalize();

			if (isDynamic) {
				PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
				if (dynamic) {
					dynamic->setAngularVelocity(PxVec3(0, 0, 0));
					bool wasKinematic = dynamic->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC;
					if (!wasKinematic) {
						dynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
					}
					dynamic->setGlobalPose(PxTransform(dynamic->getGlobalPose().p, pxQuat));
					if (!wasKinematic) {
						dynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
					}
				}
			}
			else {
				body->setGlobalPose(PxTransform(body->getGlobalPose().p, pxQuat));
			}
		}
	}*/

		// Add this to your PhysicsComponent initialization
	void alignCoordinateSystems() {
		// PhysX typically uses Z-up, while OpenGL uses Y-up
		// We may need to transform between them
		static bool systemsAligned = false;
		if (!systemsAligned) {
			// Example: Rotate -90° around X to convert Y-up to Z-up
			// PxQuat adjustQuat(PxPi/2, PxVec3(1,0,0));
			// body->setGlobalPose(PxTransform(body->getGlobalPose().p, adjustQuat));
			systemsAligned = true;
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
	// In PhysicsComponent class
	static PxQuat glmEulerToPxQuat(const glm::vec3& eulerDegrees) {
		glm::quat q = glm::quat(glm::radians(eulerDegrees));
		return PxQuat(q.x, q.y, q.z, q.w);
	}

	static glm::vec3 pxQuatToGlmEuler(const PxQuat& q) {
		glm::quat glmQuat(q.w, q.x, q.y, q.z);
		return glm::degrees(glm::eulerAngles(glmQuat));
	}

	static PxQuat glmToPxQuat(const glm::quat& q) {
		return PxQuat(q.x, q.y, q.z, q.w);
	}

	static glm::quat pxToGlmQuat(const PxQuat& q) {
		return glm::quat(q.w, q.x, q.y, q.z);
	}
};

class CubePhysics : public PhysicsComponent {
public:
	CubePhysics(Type t = Type::STATIC) : PhysicsComponent(t) {	}

	void init();

    void applyScale(const glm::vec3& scale);
};



