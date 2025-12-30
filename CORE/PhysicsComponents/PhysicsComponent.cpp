#include "PhysicsComponent.hpp"
#include "SpherePhysics.hpp"
#include <glm/gtx/matrix_decompose.hpp>

PhysicsComponent::PhysicsComponent(Type t) : body(nullptr), material(nullptr) {
	material = Physics::getPhysics()->createMaterial(0.5f, 0.5f, 0.2f); // Friction & restitution

	if (body) {

		body->release();
		body = nullptr;
	}
		
	if (t == Type::DYNAMIC) {
		body = Physics::getPhysics()->createRigidDynamic(PxTransform(PxIdentity));
	}
	else {
		body = Physics::getPhysics()->createRigidStatic(PxTransform(PxIdentity));
	}
	isDynamic = (t == Type::DYNAMIC);
}

void PhysicsComponent::applyForce(const glm::vec3& force) {

	if (isDynamic && body) {
		body->is<PxRigidDynamic>()->addForce(PxVec3(force.x, force.y, force.z));
	}
}

void PhysicsComponent::applyTorque(const glm::vec3& torque) {
	if (isDynamic && body) {
		body->is<PxRigidDynamic>()->addTorque(PxVec3(torque.x, torque.y, torque.z));
	}
}

void PhysicsComponent::setMass(float m) {
	mass = m;
	if (isDynamic) {
		PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
		if (dynamic) {
			PxRigidBodyExt::updateMassAndInertia(*dynamic, m);
		}
	}
}

float PhysicsComponent::getMass() {
	return mass;
}

//angular velocity
void PhysicsComponent::setAngularVelocity(const glm::vec3& velocity) {
	PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
	if (dynamic) {
		dynamic->setAngularVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	}
}

//linear velocity
void PhysicsComponent::setLinearVelocity(const glm::vec3& velocity) {
	PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
	if (dynamic) {
		dynamic->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	}
}

void PhysicsComponent::updatePhysX() { //need to ADD LOCAL transform
	if (body) {
		// Get the current transform from the game object
		glm::vec3 position = getWorldPosition();
		glm::quat rotation = getLocalRotation();

		// Convert to PhysX format
		PxTransform transform(
			PxVec3(position.x, position.y, position.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);

		// Update PhysX body
		body->setGlobalPose(transform);
	}
}

void PhysicsComponent::updateTransform() {
	if (body and this->m_attachedToGameObject) {
		PxTransform pxTransform = body->getGlobalPose();
		// Update position
		glm::vec3 physxPosition(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);

		// Convert PhysX quaternion to glm quaternion and normalize
		PxQuat pxQuat = pxTransform.q;
		pxQuat.normalize();
		glm::quat physxRotation(pxQuat.w, pxQuat.x, pxQuat.y, pxQuat.z);

		//transform matrix from physx data
		glm::mat4 physxMatrix = glm::translate(glm::mat4(1.0f), physxPosition) *
			glm::mat4_cast(physxRotation);

		glm::mat4 worldMatrix = physxMatrix * getLocalMatrix();

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(worldMatrix, scale, rotation, position, skew, perspective);

		// Update GameObject with quaternion directly
		
		getGameObject().setPosition(position, false);
		getGameObject().setRotationQuaternion(rotation, false);
	}
}

void PhysicsComponent::updateRotation() {
	this->setRotationQuaternion(getWorldDirection());//TODO not exactly working
}

void PhysicsComponent::updatePosition() {
	setPosition(getWorldPosition());
}

void PhysicsComponent::releaseAllShapes() {
	if (!body) return;

	// Get all shapes attached to the body
	PxU32 nbShapes = body->getNbShapes();
	PxShape** shapes = new PxShape * [nbShapes];
	body->getShapes(shapes, nbShapes);

	// Detach and release each shape
	for (PxU32 i = 0; i < nbShapes; i++) {
		body->detachShape(*shapes[i]);
		shapes[i]->release();
	}

	delete[] shapes;

	// Clear our tracked shapes
	this->shapes.clear();
}

void PhysicsComponent::setPosition(const glm::vec3& worldPosition) {
	if (body) {
	
		glm::mat4 inverseLocal = glm::inverse(getLocalMatrix());
		glm::vec3 physxPosition = inverseLocal * glm::vec4(worldPosition, 1.0f);

		PxTransform transform = body->getGlobalPose();
		transform.p = PxVec3(physxPosition.x, physxPosition.y, physxPosition.z);
		if (body->is<PxRigidDynamic>()) {
			PxRigidDynamic* dynamic = static_cast<PxRigidDynamic*>(body);

			if (dynamic->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC) {
				// Kinematic body - use kinematic target
				dynamic->setKinematicTarget(transform);

			}
			else {
				// Dynamic body - set pose and wake up
				dynamic->setGlobalPose(transform);
				dynamic->wakeUp();
				dynamic->setLinearVelocity(PxVec3(0, 0, 0));
				dynamic->setAngularVelocity(PxVec3(0, 0, 0));
			}
		}
		else {
			// Static body
			body->setGlobalPose(transform);
		}
		
		updateTransform();
		

		//body->setGlobalPose(transform);
		//body->setKi

	}
}

glm::vec3 PhysicsComponent::getScale() {
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

void PhysicsComponent::setRotation(const glm::vec3& eulerDegrees) {
	LOG_WARN("should use quaternion instead of euler");
	if (body) {
		// Convert world rotation to PhysX rotation considering local transform
		glm::quat worldRotation = glm::quat(glm::radians(eulerDegrees));

		// Account for component's local rotation
		glm::quat localRotation = glm::quat(glm::radians(getLocalRotation()));
		glm::quat physxRotation = worldRotation * glm::inverse(localRotation);

		PxTransform transform = body->getGlobalPose();
		transform.q = PxQuat(physxRotation.x, physxRotation.y, physxRotation.z, physxRotation.w);
		transform.q.normalize();

		rotatePhysxBody(transform);
	}
}

void PhysicsComponent::setRotationQuaternion(const glm::quat& worldRotation) {
	if (body) {
		// Account for component's local rotation
		glm::quat localRotation = glm::quat(glm::radians(getLocalRotation()));
		glm::quat physxRotation = worldRotation * glm::inverse(localRotation);

		PxTransform transform = body->getGlobalPose();
		transform.q = PxQuat(physxRotation.x, physxRotation.y, physxRotation.z, physxRotation.w);
		transform.q.normalize();

		rotatePhysxBody(transform);
	}
}

PhysicsComponent::~PhysicsComponent() {
	if (body )
		if (body->isReleasable())
			body->release();
	if (material) {
		material->release();
	}
}

void PhysicsComponent::LockRotationX(bool v) {
	PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
	if (dynamic) {		dynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, v);	}
}

void PhysicsComponent::LockRotationY(bool v) {
	PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
	if (dynamic) { dynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, v); }
}

void PhysicsComponent::LockRotationZ(bool v) {
	PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
	if (dynamic) { dynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, v); }
}


void PhysicsComponent::rotatePhysxBody(const PxTransform& transform) {
	if (body->is<PxRigidDynamic>()) {
		PxRigidDynamic* dynamic = static_cast<PxRigidDynamic*>(body);

		if (dynamic->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC) {
			dynamic->setKinematicTarget(transform);
		}
		else {
			dynamic->setGlobalPose(transform);
			dynamic->wakeUp();
			dynamic->setLinearVelocity(PxVec3(0, 0, 0));
			dynamic->setAngularVelocity(PxVec3(0, 0, 0));
		}
	}
	else {
		body->setGlobalPose(transform);
	}
}

