#include <CAD_modeler/window.hpp>

#include <glad/glad.h>


unsigned int Window::instances_cnt = 0;


Window::Window(int width, int height, const std::string & name):
    window(CreateGFLWwindow(width, height, name)), controller(width, height)
{
    glfwSwapInterval(1);

    // Set window pointer to this class for all callback
    ///glfwSetWindowUserPointer(window, this);
}


Window::~Window()
{
    glfwDestroyWindow(window);

    if (--instances_cnt == 0) {
        DeinitializeGLFW();
    }
}


GLFWwindow * Window::CreateGFLWwindow(int width, int height, const std::string &name)
{
    if (instances_cnt == 0) {
        InitializeGLFW();
    }

    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw WindowCreationError("Cannot create window");
    }

    glfwMakeContextCurrent(window);
    InitializeGlad();

    instances_cnt++;

    return window;
}


void Window::InitializeGLFW()
{
    if (glfwInit() != GLFW_TRUE) {
        throw WindowCreationError("Cannot initialize window module");
    }

    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


void Window::DeinitializeGLFW()
{
    glfwTerminate();
}

#include <iostream>
void Window::InitializeGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw WindowCreationError("Cannot initialize Glad");
    }
}


void Window::RunMessageLoop()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        controller.Render();

        glfwSwapBuffers(window);
    }
}


void Window::SizeChangedCallback(GLFWwindow * glfwWindow, int width, int height)
{
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    window->controller.SizeChanged(width, height);
}


void Window::MouseMoveCallback(GLFWwindow * window, double xpos, double ypos)
{
}


void Window::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
}
