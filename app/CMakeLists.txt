add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE modeler_lib glfw glad imgui ecs algebra glm MGSerializer fileBrowser)
