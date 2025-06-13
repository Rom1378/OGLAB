#include "CubePhysics.hpp"

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

void CubePhysics::applyScale(const glm::vec3& scale) {
	if (!body) return;

	// Release existing shapes
	releaseAllShapes();

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
