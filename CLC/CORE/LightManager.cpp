#include "LightManager.hpp"
#include "Camera.hpp"
#include <glm/glm.hpp>
#include <iostream>

namespace LightManager {
    // Use a static vector to store lights
    static std::vector<std::shared_ptr<Light>> s_lights;

    std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights) {
        std::vector<std::shared_ptr<Light>> relevantLights;
        for (const auto& light : s_lights) {
            // Add logging to verify lights are present
            std::cout << "Number of lights: " << s_lights.size() << std::endl;

            float distance = glm::distance( light->position, cam->getPosition());
            if (distance < cam->getMaxLightDistance()) {
                relevantLights.push_back(light);
            }
            if (relevantLights.size() >= maxLights) break;
        }
        return relevantLights;
    }

    void addLight(const std::shared_ptr<Light> light) {
        // Add logging to verify light addition
        std::cout << "Adding light. Current size before: " << s_lights.size() << std::endl;
        s_lights.push_back(light);
        std::cout << "Adding light. Current size after: " << s_lights.size() << std::endl;
    }

    void clearLights() {
        s_lights.clear();
    }

    const std::vector<std::shared_ptr<Light>>& getLights() {
        return s_lights;
    }
}