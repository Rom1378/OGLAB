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


void PhysicsComponent::updateTransform() {
	if (body) {
		PxTransform transform = body->getGlobalPose();

		getGameObject()->setPosition(glm::vec3(transform.p.x, transform.p.y, transform.p.z));

		// Convertir correctement PxQuat en angles d'Euler (en radians)
		glm::quat rotationQuat(transform.q.w, -transform.q.x, transform.q.y, transform.q.z);
		glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);

		// Ne mettez à jour la rotation que si elle n'a pas été modifiée par l'utilisateur
			getGameObject()->setRotation(glm::degrees(eulerAngles), false);  // Convertir en degrés
	}
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


