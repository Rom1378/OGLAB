#include "Sync.hpp"
#include "CORE/Scene.hpp"

#include "CORE/Components/Physics.hpp"
#include "CORE/Components/Transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4

#include <PxPhysicsAPI.h>

using namespace physx;

namespace TransformSyncSystem {

	void update(Scene* scene) {
        for (Entity e : scene->getEntities()) {

            auto* phys = scene->getComponent<PhysicsBodyComponent>(e);
            auto* transform = scene->getComponent<TransformComponent>(e);
            auto* charController = scene->getComponent<CharacterControllerStateComponent>(e);



            if (!phys || !transform) continue;

            if (phys->role == PhysicsBodyComponent::PhysicsRole::CharacterController) {
                PxExtendedVec3 p = phys->controller->getPosition();
                transform->pos = { (float)p.x, (float)p.y, (float)p.z };

                glm::vec3 rpy{ charController->pitch, charController->yaw, 0,  };
                rpy = glm::radians(rpy);
                transform->rot = glm::quat(rpy);
                //charController->yaw = 0;
                //charController->pitch = 0;

                LOG(transform->pos.x, transform->pos.y);
            }

            if (phys->role == PhysicsBodyComponent::PhysicsRole::Dynamic) {
                PxTransform t = phys->rigidDynamic->getGlobalPose();
                transform->fromPx(t);
            }
        }
	}


}