#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>
#include "CORE/Entity.hpp"

class IComponentStorage {
public:
	virtual ~IComponentStorage() = default;
	virtual void remove(Entity entityId) = 0;
	virtual bool has(Entity entityId) const = 0;
};


namespace ComponentManager {
	//stores all the array of different components.
	inline std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> componentStorages;



	template<typename T>
	class ComponentStorage : public IComponentStorage {
	public:
		ComponentStorage() : components{}, entities{}, sparse{} {}

		template<typename... Args>
		T* add(Entity t, Args&&... args) {
			if (has(t))
				return &components[sparse[t]];

			//
			components.emplace_back(std::forward<Args>(args)...);
			entities.push_back(t);


			// Resize sparse array if needed
			if (t >= sparse.size()) {
				sparse.resize(t.getId() + 1, INVALID_INDEX);
			}
			sparse[t] = components.size() - 1;

			return &components.back();

		}

		void  remove(Entity t) override {

			if (!has(t)) {
				return;
			}

			size_t indexToRemove = sparse[t];
			size_t lastIndex = components.size() - 1;

			if (indexToRemove != lastIndex) {
				components[indexToRemove] = std::move(components[lastIndex]);
				entities[indexToRemove] = entities[lastIndex];
				sparse[entities[indexToRemove]] = indexToRemove;
			}

			components.pop_back();
			entities.pop_back();
			sparse[t] = INVALID_INDEX;

		}
		T* get(Entity t) {
			if (!has(t)) {
				return nullptr;
			}
			return &components[sparse[t.getId()]];
		}
	
		bool has(Entity t) const override {
			if (t >= sparse.size()) {
				return false;
			}
			size_t index = sparse[t];
			return index != INVALID_INDEX &&
				index < entities.size() &&
				entities[index] == t;
		}



		std::vector<std::pair<Entity, T*>> getComponentsWithEntities() {
			std::vector<std::pair<Entity, T*>> result;
			result.reserve(components.size());

			for (size_t i = 0; i < components.size(); ++i) {
				result.emplace_back(entities[i], &components[i]);
			}

			return result;
		}


		//view
		struct ComponentView {
			std::vector<T>* components;  
			std::vector<Entity>* entities; 

			struct Iterator {
				size_t index;
				std::vector<T>* components;  // Changed to pointer
				std::vector<Entity>* entities; // Changed to pointer

				auto operator*() {
					return std::pair<Entity, T*>{(*entities)[index], & (*components)[index]};
				}

				Iterator& operator++() { ++index; return *this; }
				bool operator!=(const Iterator& other) const { return index != other.index; }
			};

			Iterator begin() {
				return { 0, components, entities };
			}

			Iterator end() {
				size_t size = (components && entities) ? components->size() : 0;
				return { size, components, entities };
			}

			size_t size() const {
				return (components && entities) ? components->size() : 0;
			}

			bool empty() const {
				return !components || !entities || components->empty();
			}
		};

		ComponentView getComponentView() {
			return { &components, &entities };  // Return pointers
		}


	private:
		std::vector<T> components;
		std::vector<Entity> entities; //all entity that have a component T
		std::vector<size_t> sparse;		// make the link between Entity and component;

		static constexpr size_t INVALID_INDEX = SIZE_MAX;

	};

	template<typename T>
	ComponentStorage<T>* getStorage() {
		auto typeIdx = std::type_index(typeid(T));
		auto it = componentStorages.find(typeIdx);

		if (it == componentStorages.end()) {
			// Create storage if it doesn't exist
			auto storage = std::make_unique<ComponentStorage<T>>();
			auto* ptr = storage.get();
			componentStorages[typeIdx] = std::move(storage);
			return ptr;
		}

		return static_cast<ComponentStorage<T>*>(it->second.get());
	}

	template<typename T, typename... Args>
	T* addComponent(Entity t, Args&&... args) {
		auto* storage = getStorage<T>();
		return storage->add(t, std::forward<Args>(args)...);
	}

	template<typename T>
	void removeComponent(Entity t) {
		auto* storage = getStorage<T>();
		storage->remove(t);
	}

	template<typename T>
	T* getComponent(Entity t) {
		auto* storage = getStorage<T>();
		return storage->get(t);
	}

	template<typename T>
	bool hasComponent(Entity t) {
		auto* storage = getStorage<T>();
		return storage->has(t);
	}

	inline void removeAllComponents(Entity t) {
		for (auto& [type, storage] : componentStorages) {
			storage->remove(t);
		}
	}

	inline void clear() {
		componentStorages.clear();
	}

}
