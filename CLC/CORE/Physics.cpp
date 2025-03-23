#include "Physics.hpp"


namespace Physics {

	namespace Internal {
		PxDefaultAllocator gAllocator;
		PxDefaultErrorCallback gErrorCallback;

		PxFoundation* gFoundation = nullptr;
		PxPhysics* gPhysics = nullptr;
		PxPvd* gPvd = nullptr;
	}


	void init() {

		Internal::gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Internal::gAllocator, Internal::gErrorCallback);

		Internal::gPvd = PxCreatePvd(*Internal::gFoundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
		Internal::gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		Internal::gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *Internal::gFoundation, PxTolerancesScale(), true, Internal::gPvd);
		if (!Internal::gPhysics)
		{
			printf("Error creating PhysX3 device\n");
		}

	}

	void update() {
	


	}

	void shutdown()
	{
		Internal::gPhysics->release();
		Internal::gPvd->release();
		Internal::gFoundation->release();
	}
	PxScene* createScene()
	{
		PxSceneDesc sceneDesc(Internal::gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(4);
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		PxScene* scene = Internal::gPhysics->createScene(sceneDesc);
		return scene;
	}

	PxPhysics* getPhysics()
	{
		return Internal::gPhysics;
	}

	bool raycast(PxScene *scene, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, PxRaycastHit& hitInfo) {
		PxRaycastBuffer hit;
		PxVec3 pxOrigin(origin.x, origin.y, origin.z);
		PxVec3 pxDirection(direction.x, direction.y, direction.z);

		bool status = scene->raycast(pxOrigin, pxDirection, maxDistance, hit);
		if (status && hit.hasBlock) {
			hitInfo = hit.block;
		}
		return status;
	}

}
