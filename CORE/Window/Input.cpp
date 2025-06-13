#include "Input.hpp"
#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

namespace Input
{
    namespace Internal
    {
        inline glm::vec2 mousePosition{ 0.0f };
        inline glm::vec2 prevMousePosition{ 0.0f };
        inline glm::vec2 mouseDelta{ 0.0f };
        inline bool mouseLocked = false;
        inline bool firstMouse = true;
        inline float scrollDelta = 0.0f;
        inline bool keysPressed[GLFW_KEY_LAST] = { false };
        inline bool keysJustPressed[GLFW_KEY_LAST] = { false };
        inline bool keysJustReleased[GLFW_KEY_LAST] = { false };
        inline float lastTime = 0.0f;
    }

    void init()
    {
        GLFWwindow* window = Window::getNativeWindow();
        glfwSetCursorPosCallback(window, Internal::mouse_callback);
        glfwSetScrollCallback(window, Internal::scroll_callback);
        glfwSetKeyCallback(window, Internal::key_callback);
        glfwSetMouseButtonCallback(window, Internal::mouse_button_callback);
        glfwSetWindowFocusCallback(window, Internal::window_focus_callback);
       
    }


    void update() {

        // Reset per-frame values
        Internal::scrollDelta = 0.0f;

        Internal::mouseDelta = glm::vec2(0.0f);
        // Reset just pressed/released states
        for (int i = 0; i < GLFW_KEY_LAST; i++) {
            Internal::keysJustPressed[i] = false;
            Internal::keysJustReleased[i] = false;
        }

        // Check if ImGui captures the inputs
        ImGuiIO& io = ImGui::GetIO();
        bool imguiWantsMouse = io.WantCaptureMouse;
        bool imguiWantsKeyboard = io.WantCaptureKeyboard;

        // Reset per-frame values


        if (Internal::mouseLocked) {
            // If mouse is locked, center it in the window
            int width, height;
            width = Window::getFrameBufferWidth();
            height = Window::getFrameBufferHeight();

            //glfwGetWindowSize(Window::getNativeWindow(), &width, &height);

            float centerX = width / 2.0f;
            float centerY = height / 2.0f;

            // Calculate the delta before resetting position
            if (!Internal::firstMouse) {
                Internal::mouseDelta.x = -Internal::mousePosition.x + centerX;
                Internal::mouseDelta.y = centerY - Internal::mousePosition.y; // Inverted Y axis
            }
            else {
                Internal::firstMouse = false;
                Internal::mouseDelta = glm::vec2(0.0f);
            }

            // Reset cursor to center
            glfwSetCursorPos(Window::getNativeWindow(), centerX, centerY);
            Internal::mousePosition = glm::vec2(centerX, centerY);
        }
        else {
            // If not locked, delta is calculated in mouse_callback, but reset it here after one frame
            Internal::prevMousePosition = Internal::mousePosition;
        }
        glfwPollEvents();

    }

    const glm::vec2& getMousePosition() { return Internal::mousePosition; }
    const glm::vec2& getMouseDelta() { return Internal::mouseDelta; }
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

    bool isKeyJustReleased(int keycode)
    {
        return Internal::keysJustReleased[keycode];
    }

    void setMouseLocked(bool locked)
    {
        if (Internal::mouseLocked == locked)
            return;

        Internal::mouseLocked = locked;
        Internal::firstMouse = true;

        glfwSetInputMode(
            Window::getNativeWindow(),
            GLFW_CURSOR,
            locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

     
        std::cout << "Mouse lock state changed to: " << (locked ? "Locked" : "Unlocked") << std::endl;
    }

    bool isMouseLocked() { return Internal::mouseLocked; }

    namespace Internal
    {
        void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            // Forward to ImGui
            ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

            // Update mouse position
            mousePosition = glm::vec2(xpos, ypos);

            // If mouse is not locked, calculate delta normally
            if (!mouseLocked) {
                if (firstMouse) {
                    prevMousePosition = mousePosition;
                    firstMouse = false;
                    mouseDelta = glm::vec2(0.0f);
                }
                else {
                    mouseDelta.x = mousePosition.x - prevMousePosition.x;
                    mouseDelta.y = prevMousePosition.y - mousePosition.y; // Inverted Y for camera
                    prevMousePosition = mousePosition;
                }
            }
        }

        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
        {
            ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
            scrollDelta = static_cast<float>(yoffset);
        }

        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

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
                    keysJustReleased[key] = true;
                }
            }
        }

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
            // Forward mouse button events to ImGui
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        }

        void window_focus_callback(GLFWwindow* window, int focused)
        {
            if (!focused) {
                // Window lost focus, release the mouse
                if (mouseLocked) {
                    // Temporarily release the mouse without changing the internal state
                    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            else {
                // Window gained focus, re-lock the mouse if it was locked before
                if (mouseLocked) {
                    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    firstMouse = true; // Reset to avoid jumps
                }
            }
        }
    }
}