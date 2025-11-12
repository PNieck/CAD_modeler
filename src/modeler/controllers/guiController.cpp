#include <CAD_modeler/controllers/guiController.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <stdexcept>


GuiController::GuiController(GLFWwindow* window)
{
    // FIXME: function, which generates glsl version string
    const auto glsl_version = "#version 410";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
}


GuiController::~GuiController()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void GuiController::PreFrameRender() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void GuiController::PostFrameRender() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


bool GuiController::WantCaptureMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}


bool GuiController::WantCaptureKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

