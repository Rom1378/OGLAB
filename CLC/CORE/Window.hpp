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
    void clear();
    void shutdown();

    bool isOpen();
    GLFWwindow* getNativeWindow();
    uint32_t getWidth();
    uint32_t getHeight();
    bool isVSync();
    void setVSync(bool enabled);

    GLuint getTexture();
    void CreateFramebuffer(int width, int height);


    void bind_framebuffer();
    void unbind_framebuffer();
    void rescale_framebuffer(float width, float height);
    GLuint getFramebufferTexture();
	WindowProps const* getWindowProps();
	float getFrameBufferWidth();
	float getFrameBufferHeight();
    bool getViewPortChanged();
    
    void drawImGuiInterface();

    float getAvgFPS();


}