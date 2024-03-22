#include <CAD_modeler/window.hpp>

#include <glad/glad.h>


unsigned int Window::instances_cnt = 0;


Window::Window(int width, int height, const std::string & name):
    window(CreateGFLWwindow(width, height, name)), controller(width, height)
{
    // Set window pointer to this class for all callback
    glfwSetWindowUserPointer(window, this);

    // Enable "vsync"
    glfwSwapInterval(1);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, SizeChangedCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
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


void Window::MouseMoveCallback(GLFWwindow * glfwWindow, double xpos, double ypos)
{
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);

    window->controller.MouseMoved((int)xpos, (int)ypos);
}


void Window::MouseButtonCallback(GLFWwindow * glfwWindow, int button, int action, int mods)
{
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    
    MouseButton mouseButton;

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        mouseButton = MouseButton::Left;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        mouseButton = MouseButton::Left;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        mouseButton = MouseButton::Left;
        break;
    
    default:
        return;
    }

    switch (action)
    {
    case GLFW_PRESS:
        window->controller.MouseClicked(mouseButton);
        break;

    case GLFW_RELEASE:
        window->controller.MouseReleased(mouseButton);
        break;
    
    default:
        break;
    }    

}


void Window::ScrollCallback(GLFWwindow * glfwWindow, double xoffset, double yoffset)
{
}
