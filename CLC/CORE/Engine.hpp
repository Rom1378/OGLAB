#pragma once
#include "Window.hpp"
#include "Input.hpp"

class Scene;
//define class ShadowMapper
#include "LightManager.hpp"


namespace Engine{

    void init();
    void update(Scene* scene = nullptr);
    void shutdown();

    void renderFrame(Scene* scene, LightManager::ShadowMapper* shadowMapper);


    void renderUI(Scene* scene = nullptr);
    void render(Scene* scene = nullptr);
    bool isRunning();

    float get_dt();
}