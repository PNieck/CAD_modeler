# Check if subrepo was downloaded
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/imgui/imgui.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

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

    # Text input wrappers for standard library (STL) std::string
    imgui/misc/cpp/imgui_stdlib.h
    imgui/misc/cpp/imgui_stdlib.cpp
)

target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC imgui/)
