
#include "CORE/GameObject.hpp"
#include "CORE/PhysicsComponents/PhysicsComponent.hpp"

namespace PhysicsSystem{
	
	void SyncGameObjectsFromPhysx(const std::vector<std::shared_ptr<GameObject>>& gos) {
		for (auto& go : gos) {
			const auto& pc= go->getComponent<PhysicsComponent>();

			if (pc) {
				pc->updateTransform();
			}
		}
	}

}