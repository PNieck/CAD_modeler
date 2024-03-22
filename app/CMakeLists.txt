add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm imgui modeler_lib)


add_executable(TestMain test.cpp)
target_link_libraries(TestMain PRIVATE modeler_lib glfw glad glm)
