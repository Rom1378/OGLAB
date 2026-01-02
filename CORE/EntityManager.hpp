#pragma once
#include <cstdint>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4

#include <vector>
#include <unordered_map>
#include "CORE/Debug.hpp"
#include "CORE/ComponentManager.hpp"

#include <unordered_set>

class Entity;

namespace EntityManager {
	typedef uint32_t uint32_t;


	Entity Create();
	bool Destroy(Entity e);
	uint32_t getNumEntitiesAlive();
	bool isValid(Entity id);
	void reset();
	std::unordered_set<uint32_t>& getActiveEntities();


	template<typename T>
	std::vector<Entity> getEntitiesWithComponent() {
		std::vector<Entity> result;
		auto* storage = ComponentManager::getStorage<T>();

		std::unordered_set<uint32_t>& activeEtt = getActiveEntities();
		for (uint32_t entityId : storage->getEntities()) {
			if (activeEtt.find(entityId) != activeEtt.end()) {
				result.push_back(Entity(entityId));
			}
		}

		return result;
	}

	constexpr uint32_t INVALID_ID = UINT32_MAX;

};


