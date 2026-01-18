/*#include "Physics.hpp"
#include "imgui.h"


namespace Physics {

	namespace Internal {
		PxDefaultAllocator gAllocator;
		PxDefaultErrorCallback gErrorCallback;

		PxFoundation* gFoundation = nullptr;
		PxPhysics* gPhysics = nullptr;
		PxPvd* gPvd = nullptr;


		PxControllerManager* controllerManager = nullptr;
	}

	struct visualisationParametersStates {
		bool eCOLLISION_SHAPES = 0;
		bool eSCALE = 0;
		bool  eACTOR_AXES = 0;
		bool eCULL_BOX = 0;

	} visParamStates;
	


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

	void setControllerManagerScene(PxScene* scene) {
		Internal::controllerManager = PxCreateControllerManager(*scene);
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

	void enable_debug_visualization(PxScene* scene,bool enable) {
		scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f * enable);
		scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f * enable);
	}

	void imgui_debug_menu(PxScene* scene) {

		visParamStates.eSCALE= scene->getVisualizationParameter(PxVisualizationParameter::eSCALE);
		visParamStates.eACTOR_AXES =scene->getVisualizationParameter(PxVisualizationParameter::eACTOR_AXES);
		visParamStates.eCOLLISION_SHAPES =scene->getVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES	);
		visParamStates.eCULL_BOX = scene->getVisualizationParameter(PxVisualizationParameter::eCULL_BOX);

		if (ImGui::Checkbox("eCOLLISION_SHAPES", & visParamStates.eCOLLISION_SHAPES))
			scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f * visParamStates.eCOLLISION_SHAPES );
		if (ImGui::Checkbox("eACTOR_AXES", &visParamStates.eACTOR_AXES))
			scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f * visParamStates.eACTOR_AXES);
		if (ImGui::Checkbox("eSCALE",  & visParamStates.eSCALE))
			scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f * visParamStates.eSCALE);
		if (ImGui::Checkbox("eCULL_BOX", &visParamStates.eCULL_BOX))
			scene->setVisualizationParameter(PxVisualizationParameter::eCULL_BOX, 1.0f * visParamStates.eCULL_BOX);

	}


	 PxControllerManager& getControllerManger() {
		return *Internal::controllerManager;
	}
}*/
