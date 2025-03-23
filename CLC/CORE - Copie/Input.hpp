#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Input
{

    namespace Internal {
        inline glm::vec2 mousePosition{0.0f};
        inline glm::vec2 mouseDelta{0.0f};
        inline bool mouseLocked = false;
        inline bool firstMouse = true;
        
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
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
}