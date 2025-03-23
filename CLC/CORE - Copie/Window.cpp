#include "Window.hpp"

#include <iostream>
#include <cassert>

namespace Window
{
    namespace Internal
    {
        inline GLFWwindow *m_window = nullptr;
        inline WindowProps m_props;
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    }

    void init(const WindowProps &props)
    {
        Internal::m_props = props;

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        Internal::m_window = glfwCreateWindow(
            props.width,
            props.height,
            props.title.c_str(),
            nullptr,
            nullptr
        );

        if (!Internal::m_window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(Internal::m_window);
        glfwSetFramebufferSizeCallback(Internal::m_window, Internal::framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD\n";
            return;
        }

        setVSync(props.vsync);
    }

    void update()
    {
        glfwPollEvents();
    }

    void shutdown()
    {
        if (Internal::m_window)
        {
            glfwDestroyWindow(Internal::m_window);
            Internal::m_window = nullptr;
        }
        glfwTerminate();
    }

    bool isOpen()
    {
        return Internal::m_window && !glfwWindowShouldClose(Internal::m_window);
    }

    GLFWwindow* getNativeWindow()
    {
        return Internal::m_window;
    }

    uint32_t getWidth()
    {
        return Internal::m_props.width;
    }

    uint32_t getHeight()
    {
        return Internal::m_props.height;
    }

    bool isVSync()
    {
        return Internal::m_props.vsync;
    }

    void setVSync(bool enabled)
    {
        Internal::m_props.vsync = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    namespace Internal
    {
        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            m_props.width = width;
            m_props.height = height;
            glViewport(0, 0, width, height);
        }
    }
}