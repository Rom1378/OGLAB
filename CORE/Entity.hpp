#pragma once
#include <cstdint>


//#include "CORE/ComponentManager.hpp"

constexpr uint32_t INVALID_ENTITY_ID = UINT32_MAX;


class Entity {
public:
	Entity() : entity_id{ INVALID_ENTITY_ID } {}
	Entity(const Entity& e) : entity_id{ e.getId() } {}
	Entity(const uint32_t& e) : entity_id{ e } {}

	Entity& operator=(const Entity& e) = default;

	/*
	template<typename T, typename... Args>
	T* addComponent(Args&&... args) { 
		return ComponentManager::addComponent<T>(entity_id, std::forward<Args>(args)...); 
	}

	template<typename T>
	void removeComponent() { ComponentManager::removeComponent<T>(entity_id); }

	template<typename T>
	T* getComponent() { return ComponentManager::getComponent<T>(entity_id); }

	template<typename T>
	const T* getComponent() const { return ComponentManager::getComponent<T>(entity_id); }

	template<typename T>
	bool hasComponent() const { return ComponentManager::hasComponent<T>(entity_id); }

*/
	inline uint32_t getId() const { return entity_id; }
	inline bool isValid() const { return entity_id != INVALID_ID; }


	operator uint32_t() const { return entity_id; }
	bool operator==(const Entity& other) const { return entity_id == other.entity_id; }
	bool operator!=(const Entity& other) const { return entity_id != other.entity_id; }
	bool operator!=(const uint32_t& other) const { return entity_id != other; }

	//uint32_t operator+

	static constexpr uint32_t INVALID_ID = UINT32_MAX;
private:
	uint32_t entity_id;
};
