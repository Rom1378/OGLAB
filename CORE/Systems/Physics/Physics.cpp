#include "Physics.hpp"
#include "CORE/Scene.hpp"

#include "CORE/Debug.hpp"
#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Physics.hpp"
#include "CORE/Systems/Physics/PhysicsScene.hpp"

#include <imgui.h>

#include <queue>


using namespace physx;

namespace PhysicsSystem {
	namespace Internal {
		PxDefaultAllocator gAllocator;
		PxDefaultErrorCallback gErrorCallback;

		PxFoundation* gFoundation = nullptr;
		PxPhysics* gPhysics = nullptr;
		PxPvd* gPvd = nullptr;


		PxScene* pxScene = nullptr;
		PxControllerManager* controllerManager = nullptr;


	}

	PxMaterial* defaultMaterial{ nullptr };
	std::queue<PhysicsBodyComponent*> creationQueue{};
	std::queue<PhysicsBodyComponent*> destructionQueue{};

	PxVec3 gravity{ 0.0f, 0, 0.0f };
	//PxVec3 gravity{ 0.0f, -9.81f, 0.0f };


	struct visualisationParametersStates {
		bool eCOLLISION_SHAPES = 0;
		bool eSCALE = 0;
		bool  eACTOR_AXES = 0;
		bool eCULL_BOX = 0;

	} visParamStates;


	static void attachCollider(physx::PxRigidActor* actor, const ColliderComponent& col, physx::PxMaterial* defaultMat);
	static void destroyBody(PhysicsBodyComponent* body);
	static physx::PxRigidActor* createActor(PhysicsBodyComponent* body, const TransformComponent& transform);



	static void flushCreationDeletion(Scene* scene) {
		// --- CREATE ---
		auto physicsBodycomps = scene->getComponentView<PhysicsBodyComponent>();

		for (auto [entity, physicsComp] : physicsBodycomps) {
			TransformComponent* transform = scene->getComponent<TransformComponent>(entity);

			if (physicsComp->state != PhysicsBodyComponent::State::Uninitialized)
				continue;

			if (physicsComp->role == PhysicsBodyComponent::PhysicsRole::CharacterController) {

				PxCapsuleControllerDesc desc;
				desc.height = 1.8f;
				desc.radius = 0.4f;
				desc.stepOffset = 0.4f;
				desc.position = PxExtendedVec3(transform->pos.x, transform->pos.y, transform->pos.z);
				desc.material = defaultMaterial;

				physicsComp->controller = Internal::controllerManager->createController(desc);

			}
			else {
				physicsComp->actor = createActor(physicsComp, *transform);
				if (!physicsComp->actor) {
					LOG_WARN("Could not create actor");
					continue;
				}
				auto colliderComponents = scene->getComponent<ColliderComponent>(entity);
				if (!colliderComponents) {
					LOG_WARN("Entity with PhysicsBodyComponent and non CharacterController must have a ColliderComponent");
					continue;
				}
				else {

					attachCollider(physicsComp->actor, *colliderComponents, defaultMaterial);

				}

				Internal::pxScene->addActor(*physicsComp->actor);
			}

			physicsComp->state = PhysicsBodyComponent::State::Alive;
		}

		// --- DESTROY ---
		for (auto [entity, physicsComp] : physicsBodycomps) {
			if (physicsComp->state == PhysicsBodyComponent::State::PendingDestroy) {
				destroyBody(physicsComp);
				physicsComp->state = PhysicsBodyComponent::State::Uninitialized;
			}
		}
	}
	void update(Scene* scene, float dt) {
		flushCreationDeletion(scene);

		//TODO USE VIEW
		// 1 update physx from game transforms
		for (Entity e : scene->getEntities()) {
			PhysicsBodyComponent* physicsBody = scene->getComponent<PhysicsBodyComponent>(e);
			TransformComponent* transform = scene->getComponent<TransformComponent>(e);

			if (!physicsBody || !transform)
				continue;

			if (physicsBody->controlledByPhysics)
				continue;
			//physicsBody->releaseControl();


			switch (physicsBody->role) {
			case PhysicsBodyComponent::PhysicsRole::CharacterController: {
				if (physicsBody->controller) {
					physicsBody->controller->setPosition(physx::PxExtendedVec3(
						transform->pos.x, transform->pos.y, transform->pos.z
					));
				}
				break;


				/*

				auto* state = scene->getComponent<CharacterControllerStateComponent>(e);
				if (!state) {
					LOG_WARN("CharacaterController has no CharacterControllerStateComponent");
					break;
				}
				PxController* controller = physicsBody->controller;
				if (!controller) {
					LOG_WARN("CharacaterController has no PxController");
					break;
				}

				PxControllerFilters filters;

				PxVec3 displacement{ state->velocity.x, state->velocity.y, state->velocity.z };
				displacement += gravity * physicsBody->mass * dt;

				controller->move(displacement, 0.001f, dt, filters);
				//state->isOnTheGround = filters.
				state->velocity = { 0,0,0 };
				//controller dont rotate, the rotation on the transform will be done in the Transformsync systems

				break;*/


			}
			case PhysicsBodyComponent::PhysicsRole::Dynamic: {
				// forces already applied elsewhere
				if (auto* dynamic = physicsBody->actor->is<physx::PxRigidDynamic>()) {
					dynamic->setGlobalPose(transform->toPx());
					// Clear velocities to prevent momentum issues after manual positioning
					dynamic->setLinearVelocity(physx::PxVec3(0));
					dynamic->setAngularVelocity(physx::PxVec3(0));
				}
				break;
			}

			case PhysicsBodyComponent::PhysicsRole::Kinematic: {
				if (auto* dynamic = physicsBody->actor->is<physx::PxRigidDynamic>()) {
					dynamic->setKinematicTarget(transform->toPx());
				}
				break;
			}
			case PhysicsBodyComponent::PhysicsRole::Static: {
				if (auto* staticActor = physicsBody->actor->is<physx::PxRigidStatic>()) {
					staticActor->setGlobalPose(transform->toPx());
				}
				break;
			}
			}


		}


		// 2 apply gameplay forces/velocities
		for (Entity e : scene->getEntities()) {
			PhysicsBodyComponent* physicsBody = scene->getComponent<PhysicsBodyComponent>(e);
			if (!physicsBody)
				continue;

			switch (physicsBody->role) {
			case PhysicsBodyComponent::PhysicsRole::CharacterController: {
				auto* state = scene->getComponent<CharacterControllerStateComponent>(e);
				if (!state) {
					LOG_WARN("CharacterController has no CharacterControllerStateComponent");
					break;
				}

				physx::PxController* controller = physicsBody->controller;
				if (!controller) {
					LOG_WARN("CharacterController has no PxController");
					break;
				}

				physx::PxControllerFilters filters;
				physx::PxVec3 displacement{ state->velocity.x, state->velocity.y, state->velocity.z };
				displacement += gravity * physicsBody->mass * dt;
				controller->move(displacement, 0.001f, dt, filters);
				state->velocity = { 0, 0, 0 };
				break;
			}
			case PhysicsBodyComponent::PhysicsRole::Dynamic: {
				// Apply forces here
				break;
			}
			case PhysicsBodyComponent::PhysicsRole::Kinematic: {
				// Kinematic movement handled in phase 1 or here depending on your needs
				break;
			}
			}
		}

		//step simulation
		Internal::pxScene->simulate(dt);
		Internal::pxScene->fetchResults(true);



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


	void init() {

		Internal::gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Internal::gAllocator, Internal::gErrorCallback);


		//Init debug
		Internal::gPvd = PxCreatePvd(*Internal::gFoundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
		Internal::gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);


		Internal::gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *Internal::gFoundation, PxTolerancesScale(), true, Internal::gPvd);
		if (!Internal::gPhysics)
		{
			LOG_ERROR("Error creating PhysX3 device");
		}

		Internal::pxScene = createScene();
		Internal::controllerManager = PxCreateControllerManager(*Internal::pxScene);


		defaultMaterial = Internal::gPhysics->createMaterial(0.5, 0.5, 0.5);

	}

	void shutdown()
	{
		defaultMaterial->release();
		Internal::controllerManager->release();
		Internal::pxScene->release();
		Internal::gPhysics->release();
		Internal::gPvd->release();
		Internal::gFoundation->release();
	}




	const PxPhysics* getPhysics()
	{
		return Internal::gPhysics;
	}
	const physx::PxScene* getScene() {
		return Internal::pxScene;
	}


	bool raycast(PxScene* scene, const glm::vec3& origin, const glm::vec3& direction, float maxDistance, PxRaycastHit& hitInfo) {
		PxRaycastBuffer hit;
		PxVec3 pxOrigin(origin.x, origin.y, origin.z);
		PxVec3 pxDirection(direction.x, direction.y, direction.z);

		bool status = scene->raycast(pxOrigin, pxDirection, maxDistance, hit);
		if (status && hit.hasBlock) {
			hitInfo = hit.block;
		}
		return status;
	}

	void enable_debug_visualization(PxScene* scene, bool enable) {
		scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f * enable);
		scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f * enable);
	}

	void imgui_debug_menu(PxScene* scene) {

		visParamStates.eSCALE = scene->getVisualizationParameter(PxVisualizationParameter::eSCALE);
		visParamStates.eACTOR_AXES = scene->getVisualizationParameter(PxVisualizationParameter::eACTOR_AXES);
		visParamStates.eCOLLISION_SHAPES = scene->getVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES);
		visParamStates.eCULL_BOX = scene->getVisualizationParameter(PxVisualizationParameter::eCULL_BOX);

		if (ImGui::Checkbox("eCOLLISION_SHAPES", &visParamStates.eCOLLISION_SHAPES))
			scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f * visParamStates.eCOLLISION_SHAPES);
		if (ImGui::Checkbox("eACTOR_AXES", &visParamStates.eACTOR_AXES))
			scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f * visParamStates.eACTOR_AXES);
		if (ImGui::Checkbox("eSCALE", &visParamStates.eSCALE))
			scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f * visParamStates.eSCALE);
		if (ImGui::Checkbox("eCULL_BOX", &visParamStates.eCULL_BOX))
			scene->setVisualizationParameter(PxVisualizationParameter::eCULL_BOX, 1.0f * visParamStates.eCULL_BOX);

	}

	PxControllerManager& getControllerManger() {
		return *Internal::controllerManager;
	}


	/*

		static void createBody(PhysicsBodyComponent* phys) {
			if (!phys) return;

			PxMaterial* material = phys->material;
			if (!material) {
				material = defaultMaterial;
			}
			switch (phys->role) {
			case PhysicsBodyComponent::PhysicsRole::Dynamic: {
				PxRigidDynamic* dynamic = PxCreateDynamic(
					*Internal::gPhysics,
					PxTransform(PxVec3(0, 5, 0)), // TODO: use entity transform
					PxBoxGeometry(0.5f, 0.5f, 0.5f), // Example box
					*material,
					phys->mass // mass TODO use component mass
				);
				Internal::pxScene->addActor(*dynamic);
				phys->rigidDynamic = dynamic;
				break;
			}

			case PhysicsBodyComponent::PhysicsRole::Kinematic: {
				PxRigidDynamic* kinematic = PxCreateDynamic(
					*Internal::gPhysics,
					PxTransform(PxVec3(0, 5, 0)),
					PxBoxGeometry(0.5f, 0.5f, 0.5f),
					*material,
					0.0f
				);
				kinematic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
				Internal::pxScene->addActor(*kinematic);
				phys->rigidDynamic = kinematic;
				break;
			}

			case PhysicsBodyComponent::PhysicsRole::CharacterController: {

			}
			}
		}

		


		/*
		void requestCreation(PhysicsBodyComponent* c) {
			creationQueue.push(c);
		}
		void requestDestruction(PhysicsBodyComponent* c) {
			destructionQueue.push(c);
		}

		void flushQueues(Scene* scene) {
			while (!creationQueue.empty()) {
				PhysicsBodyComponent* comp = creationQueue.front();
				creationQueue.pop();

				if (comp->role == PhysicsBodyComponent::PhysicsRole::CharacterController)
				{
					PxCapsuleControllerDesc desc;
					desc.height = 1.8f;
					desc.radius = 0.4f;
					desc.stepOffset = 0.4f;
					desc.position = PxExtendedVec3(0, 5, 0); // TODO: entity transform
					desc.material = material;

					phys->controller = Internal::controllerManager->createController(desc);
					break;
				}
				else {
					createActor();
				createBody(comp); // TODO pass a shape or add the shape data in the component
				}

			}

			while (!destructionQueue.empty()) {
				PhysicsBodyComponent* comp = destructionQueue.front();
				destructionQueue.pop();
				destroyBody(comp);
			}
		}

	*/



	static void destroyBody(PhysicsBodyComponent* body) {
		if (!body)
			return;

		// --- Character Controller ---
		if (body->controller) {
			body->controller->release();
			body->controller = nullptr;
		}

		// --- Rigid Actor (dynamic / kinematic) ---
		if (body->actor) {
			body->actor->release(); // releases attached shapes automatically
			body->actor = nullptr;
		}
	}

static physx::PxRigidActor* createActor(PhysicsBodyComponent* body, const TransformComponent& transform) {
			PxTransform pxT(
				PxVec3(transform.pos.x, transform.pos.y, transform.pos.z),
				PxQuat(transform.rot.x, transform.rot.y, transform.rot.z, transform.rot.w)
			);

			switch (body->role) {
			case PhysicsBodyComponent::PhysicsRole::Dynamic: {
				auto* a = Internal::gPhysics->createRigidDynamic(pxT);
				a->setMass(body->mass);
				return a;
			}
			case PhysicsBodyComponent::PhysicsRole::Kinematic: {
				auto* a = Internal::gPhysics->createRigidDynamic(pxT);
				a->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
				return a;
			}
			case PhysicsBodyComponent::PhysicsRole::Static: {
				auto* a = Internal::gPhysics->createRigidStatic(pxT);
				return a;
			}
			default:
				return nullptr;
			}
		}


	static void attachCollider(physx::PxRigidActor* actor, const ColliderComponent& col, physx::PxMaterial* defaultMat) {
		PxMaterial* mat = col.material ? col.material : defaultMat;

		PxShape* shape = nullptr;

		switch (col.type) {
		case ColliderComponent::Type::Box:
			shape = Internal::gPhysics->createShape(
				PxBoxGeometry(col.halfExtents.x, col.halfExtents.y, col.halfExtents.z),
				*mat
			);
			break;

		case ColliderComponent::Type::Sphere:
			shape = Internal::gPhysics->createShape(
				PxSphereGeometry(col.radius),
				*mat
			);
			break;

		case ColliderComponent::Type::Capsule:
			shape = Internal::gPhysics->createShape(
				PxCapsuleGeometry(col.radius, col.height * 0.5f),
				*mat
			);
			break;
		}

		PxTransform localT(
			PxVec3(col.localPosition.x, col.localPosition.y, col.localPosition.z),
			PxQuat(col.localRotation.x, col.localRotation.y, col.localRotation.z, col.localRotation.w)
		);

		shape->setLocalPose(localT);
		actor->attachShape(*shape);
		shape->release(); // actor keeps a ref
	}



}