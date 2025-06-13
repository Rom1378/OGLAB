#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Input
{

    namespace Internal {
        
        
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void window_focus_callback(GLFWwindow* window, int focused);
    }

    void init();
    void update();

    // Mouse input
    const glm::vec2& getMousePosition();
    const glm::vec2& getMouseDelta();
    bool isMouseButtonPressed(int button);
    float getScrollDelta();
    
    // Keyboard input
    bool isKeyPressed(int keycode);
    bool isKeyJustPressed(int keycode);
    
    // Mouse lock handling
    void setMouseLocked(bool locked);
    bool isMouseLocked();

    // Window focus handling
}