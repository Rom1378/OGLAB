// PrefabManager.cpp
#include "PrefabManager.hpp"
#include <iostream>

namespace PrefabManager {

    std::unordered_map<std::string, PrefabDefinition> prefabs;



    void registerPrefab(const PrefabDefinition& prefabDef) {
        prefabs[prefabDef.name] = prefabDef;
        std::cout << "Registered prefab: " << prefabDef.name << std::endl;
    }


    bool hasPrefab(const std::string& prefabName) {
        return prefabs.find(prefabName) != prefabs.end();
    }

    std::shared_ptr<GameObject> instantiate(
        const std::string& prefabName,
        Scene& scene,
        const glm::vec3& position
    ) {
        if (!hasPrefab(prefabName)) {
            std::cerr << "Error: Prefab '" << prefabName << "' not found!" << std::endl;
            return nullptr;
        }

        const auto& prefabDef = prefabs[prefabName];

        // Create a new GameObject with the prefab's name
        // Convert std::string to const char* using c_str()
        auto gameObject = std::make_shared<GameObject>(prefabName.c_str());

        // Rest of the function remains the same
        gameObject->setPosition(position.x != 0.0f || position.y != 0.0f || position.z != 0.0f
            ? position : prefabDef.defaultPosition);
        gameObject->setRotation(prefabDef.defaultRotation);
        gameObject->setScale(prefabDef.defaultScale);

        // Run the setup function to add and configure components
        prefabDef.setup(gameObject);

        // Add to scene
        scene.addGameObject(gameObject);

        return gameObject;
    }
    }