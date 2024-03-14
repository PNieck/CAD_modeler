file(GLOB IMGUI_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/extern/imgui/*.h")
file(GLOB IMGUI_SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/extern/imgui/*.cpp")

add_library(
    imgui
    STATIC
    ${IMGUI_HEADER_LIST}
    ${IMGUI_SOURCE_LIST}
)

target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC /)
