#include "SpherePhysics.hpp"



SpherePhysics::SpherePhysics(Type t) : PhysicsComponent(t) {}

void SpherePhysics::createDynamic(const PxTransform& transform, const PxSphereGeometry& geometry, PxMaterial* material, float mass) {
	body = PxCreateDynamic(*Physics::getPhysics(), transform, geometry, *material, mass);
	isDynamic = true;
}

void SpherePhysics::init() {
	if (!material) {
		std::cerr << "Material creation failed!" << std::endl;
		return;
	}

	GameObject* gm = getGameObject();

	// Position and rotation
	PxVec3 position(gm->getPosition().x, gm->getPosition().y, gm->getPosition().z);

	// Convert glm::vec3 rotation (Euler angles) to PxQuat
	PxQuat quat = PxQuat(glm::radians(gm->getRotation().x), PxVec3(1, 0, 0)) *
		PxQuat(glm::radians(gm->getRotation().y), PxVec3(0, 1, 0)) *
		PxQuat(glm::radians(gm->getRotation().z), PxVec3(0, 0, 1));

	PxTransform transform(position, quat);

	if (body) {
		body->attachShape(*Physics::getPhysics()->createShape(PxSphereGeometry(1.0f), *material));
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