#include <CAD_modeler/window.hpp>

#include <glad/glad.h>


unsigned int Window::instances_cnt = 0;


Window::Window(const int width, const int height, const std::string & name):
    window(CreateGFLWwindow(width, height, name)), controller(window, width, height)
{
    // Set window pointer to this class for all callback
    glfwSetWindowUserPointer(window, this);

    // Enable "vsync"
    glfwSwapInterval(1);

    InitializeKeysMap();
}


Window::~Window()
{
    glfwDestroyWindow(window);

    if (--instances_cnt == 0) {
        DeinitializeGLFW();
    }
}


GLFWwindow * Window::CreateGFLWwindow(const int width, const int height, const std::string &name)
{
    if (instances_cnt == 0) {
        InitializeGLFW();
    }

    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        throw WindowCreationError("Cannot create window");
    }

    glfwMakeContextCurrent(window);
    InitializeGlad();

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, SizeChangedCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyboardButtonCallback);

    instances_cnt++;

    return window;
}


void Window::InitializeGLFW()
{
    if (glfwInit() != GLFW_TRUE) {
        throw WindowCreationError("Cannot initialize window module");
    }

    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


void Window::DeinitializeGLFW()
{
    glfwTerminate();
}


void Window::InitializeGlad()
{
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwTerminate();
        throw WindowCreationError("Cannot initialize Glad");
    }
}


void Window::InitializeKeysMap()
{
    keysMap[GLFW_KEY_A] = KeyboardKey::a;
    keysMap[GLFW_KEY_C] = KeyboardKey::c;

    keysMap[GLFW_KEY_LEFT_SHIFT] = KeyboardKey::left_shift;
    keysMap[GLFW_KEY_RIGHT_SHIFT] = KeyboardKey::right_shift;

    keysMap[GLFW_KEY_LEFT_CONTROL] = KeyboardKey::left_control;
    keysMap[GLFW_KEY_RIGHT_CONTROL] = KeyboardKey::right_control;
}


void Window::RunMessageLoop()
{
    double time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        const double newTime = glfwGetTime();
        const double dt = newTime - time;
        time = newTime;

        controller.Update(dt);

        controller.Render();

        glfwSwapBuffers(window);
    }
}


void Window::SizeChangedCallback(GLFWwindow * glfwWindow, const int width, const int height)
{
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    window->controller.SizeChanged(width, height);
}


void Window::MouseMoveCallback(GLFWwindow * glfwWindow, const double xpos, const double ypos)
{
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);

    window->controller.MouseMoved(static_cast<int>(xpos), static_cast<int>(ypos));
}


void Window::MouseButtonCallback(GLFWwindow * glfwWindow, const int button, const int action, int mods)
{
    (void)mods;

    const auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    
    MouseButton mouseButton;

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        mouseButton = Left;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        mouseButton = Middle;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        mouseButton = Right;
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


void Window::ScrollCallback(GLFWwindow * glfwWindow, const double xoffset, const double yoffset)
{
    const auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->controller.ScrollMoved(static_cast<int>(yoffset));

    (void)xoffset;
}


void Window::KeyboardButtonCallback(GLFWwindow *glfwWindow, const int key, const int scancode, const int action, const int mods)
{
    const auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    const auto it = window->keysMap.find(key);
    if (it == window->keysMap.end())
        return;

    const KeyboardKey keyboardKey = it->second;

    if (action == GLFW_PRESS)
        window->controller.KeyboardKeyPressed(keyboardKey);
    else if (action == GLFW_RELEASE)
        window->controller.KeyboardKeyReleased(keyboardKey);

    (void)scancode;
    (void)mods;
}
