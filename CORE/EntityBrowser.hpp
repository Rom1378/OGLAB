#pragma once
#include <functional>
#include <vector>
#include <string>

#include "CORE/ComponentManager.hpp"

class Scene;
class Entity;

namespace EntityBrowser {
    using ComponentDisplayFunc = std::function<void(Entity)>;


    struct ComponentTypeInfo {
        std::string name;
        ComponentDisplayFunc displayFunc;
        std::function<bool(Entity)> hasFunc;
        std::function<void(Entity)> removeFunc;
    };

    extern std::vector<ComponentTypeInfo> registeredComponents;

    template<typename T>
    void registerComponent(const std::string& name,
        std::function<void(T&, Entity)> displayFunc) {
        ComponentTypeInfo info;
        info.name = name;

        info.hasFunc = [](Entity e) {
            return ComponentManager::hasComponent<T>(e);
            };

        info.displayFunc = [name, displayFunc](Entity e) {
            T* component = ComponentManager::getComponent<T>(e);
            if (component) {
                displayFunc(*component, e);
            }
            };

        info.removeFunc = [](Entity e) {
            ComponentManager::removeComponent<T>(e);
            };

        registeredComponents.push_back(info);
    }

    const std::vector<ComponentTypeInfo>& getRegisteredComponents();

}