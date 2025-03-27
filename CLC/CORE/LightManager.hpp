#pragma once
#include "Light.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include "Camera.hpp"


namespace LightManager {

   std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights);
   void addLight(const std::shared_ptr<Light> light);
   void clearLights();
   const std::vector<std::shared_ptr<Light>>& getLights();

    //void addLight(const std::shared_ptr<Light> light);
    //void clearLights();
    //const std::vector<std::shared_ptr<Light>>& getLights();
}