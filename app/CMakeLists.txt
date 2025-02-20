add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE modeler_lib)
enable_compiler_warnings(${PROJECT_NAME})
