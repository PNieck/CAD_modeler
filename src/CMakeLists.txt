#
# Algebra library
#
file(GLOB_RECURSE ALGEBRA_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/algebra/*.hpp")
file(GLOB_RECURSE ALGEBRA_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/algebra/*.cpp")

add_library(algebra ${ALGEBRA_LIB_SRCS} ${ALGEBRA_LIB_HEADERS})
target_include_directories(algebra PUBLIC ../include)
enable_compiler_warnings(algebra)

#
# ECS library
#
file(GLOB_RECURSE ECS_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/ecs/*.hpp")
file(GLOB_RECURSE ECS_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/ecs/*.cpp")

add_library(ecs ${ECS_LIB_SRCS} ${ECS_LIB_HEADERS})
target_include_directories(ecs PUBLIC ../include)
enable_compiler_warnings(ecs)


#
# Graph library
#
file(GLOB_RECURSE GRAPH_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/graph/*.hpp")
file(GLOB_RECURSE GRAPH_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/graph/*.cpp")

add_library(graph ${GRAPH_LIB_SRCS} ${GRAPH_LIB_HEADERS})
target_include_directories(graph PUBLIC ../include)
enable_compiler_warnings(graph)


#
# OpenGl Helpers library
#
file(GLOB_RECURSE OPENGL_HELPERS_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/openglHelpers/*.hpp")
file(GLOB_RECURSE OPENGL_HELPERS_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/openglHelpers/*.cpp")

add_library(openGlHelpers ${OPENGL_HELPERS_LIB_SRCS} ${OPENGL_HELPERS_LIB_HEADERS})
target_include_directories(openGlHelpers PUBLIC ../include)
target_link_libraries(openGlHelpers PUBLIC glad)
enable_compiler_warnings(openGlHelpers)


#
# Optimization library
#
file(GLOB_RECURSE OPTIMIZATION_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/optimization/*.hpp")
file(GLOB_RECURSE OPTIMIZATION_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/optimization/*.cpp")

add_library(optimization ${OPTIMIZATION_LIB_SRCS} ${OPTIMIZATION_LIB_HEADERS})
target_include_directories(optimization PUBLIC ../include)
enable_compiler_warnings(optimization)


#
# Root finding library
#
file(GLOB_RECURSE ROOT_FINDING_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/rootFinding/*.hpp")
file(GLOB_RECURSE ROOT_FINDING_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/rootFinding/*.cpp")

add_library(rootFinding ${ROOT_FINDING_LIB_SRCS} ${ROOT_FINDING_LIB_HEADERS})
target_include_directories(rootFinding PUBLIC ../include)
target_link_libraries(rootFinding PUBLIC algebra)
enable_compiler_warnings(rootFinding)


#
# Main library
#
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/CAD_modeler/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/modeler/*.cpp")

add_library(modeler_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(modeler_lib PUBLIC ../include)
target_link_libraries(
    modeler_lib
    PUBLIC
    glfw
    glad
    imgui
    ecs
    algebra
    graph
    ImGuiFileDialog
    nlohmann_json::nlohmann_json
    dlib::dlib
    imguiLoadingSpinner
    openGlHelpers
    optimization
    rootFinding
)
enable_compiler_warnings(modeler_lib)

# Copy shaders
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
)
add_dependencies(modeler_lib copy_shaders)

# Copy models
add_custom_target(copy_models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
)
add_dependencies(modeler_lib copy_models)


# For IDEs
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST} ${ECS_LIB_HEADERS}
)
