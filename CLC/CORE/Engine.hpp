#pragma once
#include "Window.hpp"
#include "Input.hpp"

class Scene;
namespace Engine{

    void init();
    void update(Scene* scene = nullptr);
    void shutdown();
    void renderUI(Scene* scene = nullptr);
    void render(Scene* scene = nullptr);
    bool isRunning();

    float get_dt();
}