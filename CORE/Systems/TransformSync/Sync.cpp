#include "Sync.hpp"
#include "CORE/Scene.hpp"

#include "CORE/Components/Physics.hpp"
#include "CORE/Components/Transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <PxPhysicsAPI.h>

using namespace physx;

namespace TransformSyncSystem {

	void update(Scene* scene) {
		auto bodies = scene->getComponentView<PhysicsBodyComponent>();

		for (auto [entity, phys] : bodies) {

			// Only sync active physics objects
			if (phys->state != PhysicsBodyComponent::State::Alive)
				continue;

			TransformComponent* transform = scene->getComponent<TransformComponent>(entity);
			if (!transform)
				continue;


			if (!phys->controlledByPhysics)
				continue;
			// --- Character Controller ---

			switch (phys->role) {
			case PhysicsBodyComponent::PhysicsRole::CharacterController: {
				PxExtendedVec3 p = phys->controller->getPosition();
				transform->pos = { p.x,p.y,p.z };

				// Rotation is gameplay-driven, not physics-driven
				if (auto* cc = scene->getComponent<CharacterControllerStateComponent>(entity)) {
					glm::vec3 rpy{
						cc->pitch,
						cc->yaw,
						0.0f
					};

					transform->rot = glm::quat(glm::radians(rpy));
				}

				continue;

				break;
			}
			case PhysicsBodyComponent::PhysicsRole::Dynamic:
			case PhysicsBodyComponent::PhysicsRole::Kinematic:
			case PhysicsBodyComponent::PhysicsRole::Static: {
				if (phys->actor) {
					physx::PxTransform pxTrans = phys->actor->getGlobalPose();
					transform->fromPx(pxTrans);
				}
				break;
			}


														  /*
			if (phys->role == PhysicsBodyComponent::PhysicsRole::CharacterController) {

				if (!phys->controller)
					continue;

				PxExtendedVec3 p = phys->controller->getPosition();
				transform->pos = { p.x,p.y,p.z };

				// Rotation is gameplay-driven, not physics-driven
				if (auto* cc = scene->getComponent<CharacterControllerStateComponent>(entity)) {
					glm::vec3 rpy{
						cc->pitch,
						cc->yaw,
						0.0f
					};

					transform->rot = glm::quat(glm::radians(rpy));
				}

				continue;
			}

			// --- Rigid Bodies ---
			if (!phys->actor)
				continue;

			// Only dynamic / kinematic actors reach here
			PxTransform pxT = phys->actor->getGlobalPose();
			transform->fromPx(pxT);
			*/
			}

		}
	}
}
