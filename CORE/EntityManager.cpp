#include "CORE/Entity.hpp"
#include "CORE/EntityManager.hpp"

#include <algorithm>
#include <ranges>
#include <unordered_set>
#include <list>


namespace EntityManager {

	std::list<uint32_t> freeIds{};
	std::unordered_set<uint32_t> activeEntities{};
	uint32_t nextId = { 0 };

	std::unordered_set<uint32_t>& getActiveEntities() { return activeEntities; }
	
	Entity Create() {
		uint32_t id;

		if (!freeIds.empty()) {
			id = freeIds.back();
			freeIds.pop_back();
		}
		else {
			id = nextId++;
		}

		activeEntities.insert(id);
	
		return Entity(id);
	}

	bool Destroy(Entity e) {

		uint32_t id = e.getId();

		auto it = activeEntities.find(id);
		if (it == activeEntities.end()) {
			return false;
		}

		// Remove all components
		ComponentManager::removeAllComponents(id);

		// Remove from active entities
		activeEntities.erase(it);
		freeIds.push_back(id);

		return true;

	}

	uint32_t getNumEntitiesAlive() {
		return static_cast<uint32_t>(activeEntities.size());
	}

	bool isValid(Entity id) {
		return activeEntities.find(id) != activeEntities.end();
	}
	void reset() {
		ComponentManager::clear();
		activeEntities.clear();
		freeIds.clear();
		nextId = 0;
	}

	

}

