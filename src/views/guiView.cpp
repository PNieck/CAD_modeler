#include <CAD_modeler/views/guiView.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


GuiView::GuiView(GLFWwindow * window)
{
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
}


void GuiView::RenderGui(GuiController& controller, const Model& model) const
{
    //const Torus& torus = model.ConstTorusGet();

    int small_circs;
    int big_circs;

    float old_r = 1.0f, old_R = 1.0f, r = 1.0f, R = 1.0f;

    // old_r = r = torus.SmallRadiusGet();
    // old_R = R = torus.BigRadiusGet();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Torus");
    // ImGui::InputInt("Small circles", &small_circs);
    // ImGui::InputInt("Big circles", &big_circs);
    ImGui::SliderFloat("R", &R, 0.0f, 10.0f);
    ImGui::SliderFloat("r", &r, 0.0f, 10.0f);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (old_r != r) {
        controller.TorusSmallRadiusChanged(r);
    }

    if (old_R != R) {
        controller.TorusBigRadiusChanged(R);
    }
}


GuiView::~GuiView()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
