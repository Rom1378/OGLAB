#pragma once
#include "ShaderProgram.hpp"
#include "Light.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include "Scene.hpp"
#include <memory>

class Camera;


namespace LightManager {



    extern std::vector<std::shared_ptr<Light>> s_lights;

    
    void init();
	void compute_shadow_mapping(Scene* scene);
    void bindShadowMap(std::shared_ptr<ShaderProgram> shader);


    unsigned int getDepthMap();
    void renderQuad();



    


    std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights);
    void addLight(const std::shared_ptr<Light> light);
    void clearLights();
    const std::vector<std::shared_ptr<Light>>& getLights();


}