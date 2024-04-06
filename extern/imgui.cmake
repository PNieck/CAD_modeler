# Basic ImGui files
file(GLOB IMGUI_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/extern/imgui/*.h")
file(GLOB IMGUI_SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/extern/imgui/*.cpp")

add_library(
    imgui
    STATIC
    ${IMGUI_HEADER_LIST}
    ${IMGUI_SOURCE_LIST}

    # Files for opengl3 and glfw integration
    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3_loader.h
)

target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC imgui/)
