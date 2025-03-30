#include "PhysicsComponent.hpp"
#include "SpherePhysics.hpp"

PhysicsComponent::PhysicsComponent(Type t) {
	// Here you can initialize material or perform other initialization
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

/*
void PhysicsComponent::updateTransform() {
	if (body) {
		PxTransform transform = body->getGlobalPose();

		getGameObject()->setPosition(glm::vec3(transform.p.x, transform.p.y, transform.p.z), false);

		// Convertir correctement PxQuat en angles d'Euler (en radians)
		glm::quat rotationQuat(transform.q.w, -transform.q.x, transform.q.y, transform.q.z);
		glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);

		// Ne mettez à jour la rotation que si elle n'a pas été modifiée par l'utilisateur
		getGameObject()->setRotation(glm::degrees(eulerAngles), false);  // Convertir en degrés
	}
}*/
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


void PhysicsComponent::debugDrawRotationAxes() {
	if (!body) return;

	PxTransform transform = body->getGlobalPose();
	PxVec3 position = transform.p;

	// Draw axes (using your rendering system)
	float axisLength = 1.0f;

	// X axis (red)
	//drawDebugLine(position, position + transform.q.rotate(PxVec3(axisLength, 0, 0)), RED);
	// Y axis (green)
	//drawDebugLine(position, position + transform.q.rotate(PxVec3(0, axisLength, 0)), GREEN);
	// Z axis (blue)
	//drawDebugLine(position, position + transform.q.rotate(PxVec3(0, 0, axisLength)), BLUE);
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

void CubePhysics::init() {
	if (!material) {
		std::cerr << "Material creation failed!" << std::endl;
		return;
	}

	GameObject* gm = getGameObject();

	gm->getPosition();
	// Position and rotation
	PxVec3 position(gm->getPosition().x, gm->getPosition().y, gm->getPosition().z);

	// Convert glm::vec3 rotation (Euler angles) to PxQuat
	PxQuat quat = PxQuat(glm::radians(gm->getRotation().x), PxVec3(1, 0, 0)) *
		PxQuat(glm::radians(gm->getRotation().y), PxVec3(0, 1, 0)) *
		PxQuat(glm::radians(gm->getRotation().z), PxVec3(0, 0, 1));

	PxTransform transform(position, quat);

	glm::vec3 scale_factor = gm->getScale();

	if (body) {
		body->attachShape(*Physics::getPhysics()->createShape(PxBoxGeometry(0.5f * scale_factor.x, 0.5f * scale_factor.y, 0.5f * scale_factor.z), *material));
		body->setGlobalPose(transform);

		if (isDynamic) {
			PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
			PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);
		}
	}
	else {
		std::cerr << "Failed to create rigid body!" << std::endl;
	}
}
/*
void CubePhysics::init() {
	if (!material) {
		std::cerr << "Material creation failed!" << std::endl;
		return;
	}

	GameObject* gm = getGameObject();
	glm::vec3 scale = gm->getScale();

	// Create shape with proper collision geometry
	PxShape* shape = Physics::getPhysics()->createShape(
		PxBoxGeometry(0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z),
		*material,
		true
	);

	// CRITICAL: Set proper local pose
	shape->setLocalPose(PxTransform(PxIdentity));

	body->attachShape(*shape);
	shapes.push_back(shape);

	// Set initial transform - must match GameObject's transform
	PxTransform transform(
		PxVec3(gm->getPosition().x, gm->getPosition().y, gm->getPosition().z),
		glmEulerToPxQuat(gm->getRotation())
	);
	body->setGlobalPose(transform);

	if (isDynamic) {
		PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
		if (dynamic) {
			// Correct way to unlock all rotations
			dynamic->setRigidDynamicLockFlags(PxRigidDynamicLockFlags());

			// Set reasonable damping
			dynamic->setAngularDamping(0.5f);

			// Update mass properties
			PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);

			// Wake up the actor
			dynamic->wakeUp();
		}
	}

}*/


void CubePhysics::applyScale(const glm::vec3& scale) {
	if (!body) return;

	// Release existing shapes
	releaseAllShapes();

	// Create new shape with proper scaled dimensions
	// Note: The 0.5f factors are because PhysX uses half-extents
	PxShape* shape = Physics::getPhysics()->createShape(
		PxBoxGeometry(0.5f * scale.x, 0.5f * scale.y, 0.5f * scale.z),
		*material,
		true
	);

	// Adjust local pose if needed (center of mass, etc.)
	shape->setLocalPose(PxTransform(PxIdentity));

	body->attachShape(*shape);
	shapes.push_back(shape);

	// Update mass properties if dynamic

	if (isDynamic) {
		PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
		PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);
		dynamic->wakeUp();
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

/*
void PhysicsComponent::setRotation(const glm::vec3& eulerAngles) {
	if (body) {
		// Euler to Quaternion conversion
		glm::vec3 radAngles = glm::radians(
			glm::all(glm::isfinite(eulerAngles)) ? eulerAngles : glm::vec3(0)
		);

		// GLM's built-in conversion (uses XYZ order)
		glm::quat q = glm::quat(radAngles);

		// Convert to PhysX format (xyzw -> xyzw)
		PxQuat pxQuat(q.x, q.y, q.z, q.w);
		pxQuat.normalize();

		body->setGlobalPose(PxTransform(body->getGlobalPose().p, pxQuat));
	}
}

/*
void PhysicsComponent::setRotation(const glm::vec3& rotation) {
	if (!body) return;

	// Ensure correct quaternion multiplication order (yaw, pitch, roll)
	PxQuat quat = PxQuat(glm::radians(rotation.z), PxVec3(0, 0, 1)) *  // Roll
		PxQuat(glm::radians(rotation.x), PxVec3(1, 0, 0)) *  // Pitch
		PxQuat(glm::radians(rotation.y), PxVec3(0, 1, 0));   // Yaw

	body->setGlobalPose(PxTransform(body->getGlobalPose().p, quat));
	return;

	PxTransform newPose = body->getGlobalPose();
	newPose.q = quat;
	body->setGlobalPose(newPose);

	// Ensure inertia tensor updates if dynamic
	PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
	if (dynamic) {
		dynamic->setAngularVelocity(PxVec3(0, 0, 0));
		PxRigidBodyExt::updateMassAndInertia(*dynamic, dynamic->getMass());
	}
}*/
