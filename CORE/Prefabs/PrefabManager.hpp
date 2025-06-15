// PrefabManager.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "../Scene.hpp"
#include "PrefabDefinition.hpp"

namespace PrefabManager {
    void registerPrefab(const PrefabDefinition& prefabDef);

    bool hasPrefab(const std::string& prefabName);

    std::shared_ptr<GameObject> instantiate(
        const std::string& prefabName,
        const glm::vec3& position = glm::vec3(0.0f)
    );
};