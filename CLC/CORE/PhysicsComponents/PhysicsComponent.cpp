#include "PhysicsComponent.hpp"
#include "SpherePhysics.hpp"

PhysicsComponent::PhysicsComponent(Type t) {
	material = Physics::getPhysics()->createMaterial(0.5f, 0.5f, 0.2f); // Friction & restitution

	if (body)
		body->release();
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

void PhysicsComponent::updatePhysX() {
	if (body) {
		// Get the current transform from the game object
		glm::vec3 position = getGameObject()->getPosition();
		glm::quat rotation = getGameObject()->getRotationQuaternion();

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
	if (body) {
		PxTransform pxTransform = body->getGlobalPose();

		// Update position
		glm::vec3 position(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);

		// Convert PhysX quaternion to glm quaternion and normalize
		PxQuat pxQuat = pxTransform.q;
		pxQuat.normalize();
		glm::quat rotation(pxQuat.w, pxQuat.x, pxQuat.y, pxQuat.z);

		// Update GameObject with quaternion directly
		getGameObject()->setPosition(position, false);
		getGameObject()->setRotationQuaternion(rotation, false);
	}
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


void PhysicsComponent::setPosition(const glm::vec3& position) {
	if (body) {
		body->setGlobalPose(PxTransform(position.x, position.y, position.z));
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
	if (body) {
		// Convert to quaternion (world-space)
		glm::quat worldRot = glm::quat(glm::radians(eulerDegrees));

		// Get current PhysX rotation
		PxQuat currentPxRot = body->getGlobalPose().q;
		glm::quat currentRot(currentPxRot.w, currentPxRot.x, currentPxRot.y, currentPxRot.z);

		// Convert to local-space rotation
		glm::quat localRot = glm::inverse(currentRot) * worldRot;

		// Apply to PhysX (as delta rotation)
		PxQuat pxLocalRot(localRot.x, localRot.y, localRot.z, localRot.w);
		pxLocalRot.normalize();

		body->setGlobalPose(PxTransform(
			body->getGlobalPose().p,
			pxLocalRot * body->getGlobalPose().q
		));
	}
}

PhysicsComponent::~PhysicsComponent() {
	if (body) {
		body->release();
	}
	if (material) {
		material->release();
	}
}