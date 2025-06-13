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

	glm::vec3 scale = gm->getScale();


	// Position and rotation
	PxVec3 position(gm->getPosition().x, gm->getPosition().y, gm->getPosition().z);

	// Convert glm::vec3 rotation (Euler angles) to PxQuat
	PxQuat quat = PxQuat(glm::radians(gm->getRotation().x), PxVec3(1, 0, 0)) *
		PxQuat(glm::radians(gm->getRotation().y), PxVec3(0, 1, 0)) *
		PxQuat(glm::radians(gm->getRotation().z), PxVec3(0, 0, 1));

	PxTransform transform(position, quat);

	float radius = 1.0f * glm::max(glm::max(scale.x, scale.y), scale.z);

	if (body) {
		releaseAllShapes();

		// Create a new shape
		PxShape* shape = Physics::getPhysics()->createShape(
			PxSphereGeometry(radius),
			*material,
			true // Exclusive - allows modification later
		);

		body->attachShape(*shape);
		shapes.push_back(shape); // Track for cleanup

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

void SpherePhysics::applyScale(const glm::vec3& scale) {
	if (!body || shapes.empty()) return;

	// For sphere, use largest scale component
	float radius = 1.0f * glm::max(glm::max(scale.x, scale.y), scale.z);

	// Create new shape with updated geometry
	releaseAllShapes();

	PxShape* shape = Physics::getPhysics()->createShape(
		PxSphereGeometry(radius),
		*material,
		true
	);
	body->attachShape(*shape);
	shapes.push_back(shape);

	// Update mass if dynamic
	if (isDynamic) {
		PxRigidDynamic* dynamic = body->is<PxRigidDynamic>();
		PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);
		dynamic->wakeUp();
	}
}