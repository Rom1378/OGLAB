#include "Input.hpp"
#include "Window.hpp"

namespace Input
{

    namespace Internal
    {
        inline float scrollDelta = 0.0f;
        inline bool keysPressed[GLFW_KEY_LAST] = {false};
        inline bool keysJustPressed[GLFW_KEY_LAST] = {false};
    }
    void init()
    {
        GLFWwindow *window = Window::getNativeWindow();
        glfwSetCursorPosCallback(window, Internal::mouse_callback);
        glfwSetScrollCallback(window, Internal::scroll_callback);
        glfwSetKeyCallback(window, Internal::key_callback);
        setMouseLocked(true);
    }

    void update()
    {
        // Reset per-frame values
        Internal::mouseDelta = glm::vec2(0.0f);
        Internal::scrollDelta = 0.0f;

        // Reset just pressed states
        for (int i = 0; i < GLFW_KEY_LAST; i++)
        {
            Internal::keysJustPressed[i] = false;
        }
    }

    const glm::vec2 &getMousePosition() { return Internal::mousePosition; }
    const glm::vec2 &getMouseDelta() { return Internal::mouseDelta; }
    float getScrollDelta() { return Internal::scrollDelta; }

    bool isMouseButtonPressed(int button)
    {
        return glfwGetMouseButton(Window::getNativeWindow(), button) == GLFW_PRESS;
    }

    bool isKeyPressed(int keycode)
    {
        return Internal::keysPressed[keycode];
    }

    bool isKeyJustPressed(int keycode)
    {
        return Internal::keysJustPressed[keycode];
    }

    void setMouseLocked(bool locked)
    {
        Internal::mouseLocked = locked;
        glfwSetInputMode(
            Window::getNativeWindow(),
            GLFW_CURSOR,
            locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    bool isMouseLocked() { return Internal::mouseLocked; }

    namespace Internal
    {
        void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
        {
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (firstMouse)
            {
                mousePosition = glm::vec2(xpos, ypos);
                firstMouse = false;
                return;
            }

            mouseDelta.x = xpos - mousePosition.x;
            mouseDelta.y = mousePosition.y - ypos; // Reversed for Y-coordinate
            mousePosition = glm::vec2(xpos, ypos);
        }

        // glfw: whenever the mouse scroll wheel scrolls, this callback is called
        // ----------------------------------------------------------------------
        void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
        {
            scrollDelta = static_cast<float>(yoffset);
        }

        void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            if (key >= 0 && key < GLFW_KEY_LAST)
            {
                if (action == GLFW_PRESS)
                {
                    keysPressed[key] = true;
                    keysJustPressed[key] = true;
                }
                else if (action == GLFW_RELEASE)
                {
                    keysPressed[key] = false;
                }
            }
        }
    }
}