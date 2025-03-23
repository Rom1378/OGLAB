#pragma once
#include "Window.hpp"
#include "Input.hpp"


namespace Engine{
    namespace Internal {
        inline bool running=false;
    }


    bool init();
    void shutdown();
    void update();
    void render();
    bool isRunning();

}