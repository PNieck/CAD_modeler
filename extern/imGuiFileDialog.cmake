FetchContent_Declare(
  fileDialog
  GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
  GIT_TAG v0.6.7
)

FetchContent_MakeAvailable(fileDialog)

target_link_libraries(ImGuiFileDialog PRIVATE imgui)
