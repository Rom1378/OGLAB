#include "Engine.hpp"

namespace Engine {
    bool init() {
        // Initialize window with default properties
        Window::init();
        
        // Initialize input system
        Input::init();
        
        Internal::running = true;
        return true;
    }

    void shutdown() {
        Window::shutdown();
        Internal::running = false;
    }

    void update() {
        // Update window events
        glfwPollEvents();
        
        // Update input state
        Input::update();
        
        // Check if window should close
        if (!Window::isOpen()) {
            Internal::running = false;
        }
    }

    void render() {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Your rendering code here
        
        // Swap buffers
        glfwSwapBuffers(Window::getNativeWindow());
    }

    bool isRunning() {
        return Internal::running && Window::isOpen();
    }
}