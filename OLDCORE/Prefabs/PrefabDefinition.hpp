#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include "../GameObject.hpp"

class PrefabDefinition {
public:
    std::string name;
    glm::vec3 defaultPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 defaultRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 defaultScale = glm::vec3(1.0f, 1.0f, 1.0f);

    // Setup function to add components and configure the object
    std::function<void(std::shared_ptr<GameObject>)> setup;

    PrefabDefinition() = default;
    PrefabDefinition(const std::string& prefabName,
        const std::function<void(std::shared_ptr<GameObject>)>& setupFunc)
        : name(prefabName), setup(setupFunc) {}

    PrefabDefinition& setDefaultPosition(const glm::vec3& pos) {
        defaultPosition = pos;
        return *this;
    }

    PrefabDefinition& setDefaultRotation(const glm::vec3& rot) {
        defaultRotation = rot;
        return *this;
    }

    PrefabDefinition& setDefaultScale(const glm::vec3& scale) {
        defaultScale = scale;
        return *this;
    }
};