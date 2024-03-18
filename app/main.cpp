#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <CAD_modeler/shader.hpp>
#include <CAD_modeler/objects/torus.hpp>
#include <CAD_modeler/camera.hpp>

#define MIN_VAL 0.001f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT);
Torus torus(0.2f, 0.5f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool shift_clicked;
bool left_mouse_button_clicked = false;
bool middle_mouse_button_clicked = false;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CAD", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Imgui
    const char* glsl_version = "#version 330";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    glEnable(GL_PRIMITIVE_RESTART);
    //glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

    Shader shader("../../shaders/vertexShader.vert", "../../shaders/fragmentShader.frag");

    auto vertices = torus.generate_vertices(4, 2);
    auto indices = torus.generate_edges(4, 2);

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * vertices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glPointSize(5.0f);

    int small_circs = 4;
    int old_small_circs = 0;
    int big_circs = 3;
    int old_big_circs = 0;
    float R = 0.5f;
    float old_R = 0.0f;
    float r = 0.2f;
    float old_r = 0.0f;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // input
        // -----
        processInput(window);

        // gui
        // -----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::InputInt("Small circles", &small_circs);
        ImGui::InputInt("Big circles", &big_circs);
        ImGui::SliderFloat("R", &R, MIN_VAL, 10.0f);
        ImGui::SliderFloat("r", &r, MIN_VAL, R - MIN_VAL);
        ImGui::End();

        if (old_small_circs != small_circs ||
            old_big_circs != big_circs ||
            old_R != R ||
            old_r != r)
        {
            glBindVertexArray(VAO);

            if (small_circs < 3) {
                small_circs = 3;
            }

            if (big_circs < 3) {
                big_circs = 3;
            }

            if (r < 0) {
                r = MIN_VAL;
            }

            if (R < 0) {
                R = MIN_VAL;
            }

            if (r > R) {
                r = R - MIN_VAL;
            }

            torus.R = R;
            torus.r = r;

            vertices = torus.generate_vertices(small_circs, big_circs);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

            indices = torus.generate_edges(small_circs, big_circs);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

            old_small_circs = small_circs;
            old_big_circs = big_circs;
            old_R = R;
            old_r = r;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4x4 MVP = camera.GetPerspectiveMatrix() * camera.GetViewMatrix() * torus.model_matrix();
        shader.setMatrix4("MVP", MVP);

        // draw our first triangle
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_LINE_LOOP, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
 
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    camera.aspect_ratio = (float)width / (float)height;
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(static_cast<float>(xposIn), static_cast<float>(yposIn));

    if (io.WantCaptureMouse)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && left_mouse_button_clicked) {
        camera.target.x += xoffset * 0.1;
        camera.target.y += yoffset * 0.1;
    }
    else if (middle_mouse_button_clicked) {
        torus.MoveX(xoffset * 0.01);
        torus.MoveY(-yoffset * 0.01);
    }
    else if (left_mouse_button_clicked) {
        torus.rotate_x(yoffset * 0.02);
        torus.rotate_y(xoffset * 0.02);
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            left_mouse_button_clicked = true;
        }
        else if (action == GLFW_RELEASE) {
            left_mouse_button_clicked = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            middle_mouse_button_clicked = true;
        }
        else if (action == GLFW_RELEASE) {
            middle_mouse_button_clicked = false;
        }
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float new_scale = torus.ScaleGet() + yoffset*0.2;

    if (new_scale > 0)
        torus.ScaleSet(new_scale);
}
