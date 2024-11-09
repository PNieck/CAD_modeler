add_library(
    imguiLoadingSpinner
    STATIC
    imguiLoadingSpinner/imguiLoadingSpinner.cpp
)

target_include_directories(imguiLoadingSpinner INTERFACE imguiLoadingSpinner/)
target_link_libraries(imguiLoadingSpinner imgui)
