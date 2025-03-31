#pragma once
#include "Light.hpp"
#include <vector>
#include <memory>
#include <iostream>

class Camera;

namespace LightManager {
    extern std::vector<std::shared_ptr<Light>> s_lights;
    std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights);


    void addLight(const std::shared_ptr<Light> light);
    void clearLights();
    const std::vector<std::shared_ptr<Light>>& getLights();


}