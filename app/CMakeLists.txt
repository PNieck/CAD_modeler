add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE modeler_lib glfw glad imgui ecs algebra glm fileBrowser nlohmann_json::nlohmann_json)
enable_compiler_warings(${PROJECT_NAME})
