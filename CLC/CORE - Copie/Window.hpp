#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Window
{
    struct WindowProps
    {
        std::string title = "Engine";
        uint32_t width = 1280;
        uint32_t height = 720;
        bool vsync = true;
    };

    void init(const WindowProps& props = WindowProps{});
    void update();
    void shutdown();

    bool isOpen();
    GLFWwindow* getNativeWindow();
    uint32_t getWidth();
    uint32_t getHeight();
    bool isVSync();
    void setVSync(bool enabled);
}