#
# Algebra library
#
file(GLOB_RECURSE ALGEBRA_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/algebra/*.hpp")
file(GLOB_RECURSE ALGEBRA_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/algebra/*.cpp")

add_library(algebra ${ALGEBRA_LIB_SRCS} ${ALGEBRA_LIB_HEADERS})
target_include_directories(algebra PUBLIC ../include)
enable_compiler_warings(algebra)

#
# ECS library
#
file(GLOB_RECURSE ECS_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/ecs/*.hpp")
file(GLOB_RECURSE ECS_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/ecs/*.cpp")

add_library(ecs ${ECS_LIB_SRCS} ${ECS_LIB_HEADERS})
target_include_directories(ecs PUBLIC ../include)
enable_compiler_warings(ecs)


#
# Graph library
#
file(GLOB_RECURSE GRAPH_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/graph/*.hpp")
file(GLOB_RECURSE GRAPH_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/graph/*.cpp")

add_library(graph ${GRAPH_LIB_SRCS} ${GRAPH_LIB_HEADERS})
target_include_directories(graph PUBLIC ../include)
enable_compiler_warings(graph)


#
# Main library
#
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/CAD_modeler/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/modeler/*.cpp")

add_library(modeler_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(modeler_lib PUBLIC ../include)
target_link_libraries(modeler_lib PUBLIC glfw glad imgui ecs algebra graph fileBrowser nlohmann_json::nlohmann_json dlib::dlib)
enable_compiler_warings(modeler_lib)

# Copy shaders
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
)
add_dependencies(modeler_lib copy_shaders)


# For IDEs
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST} ${ECS_LIB_HEADERS}
)
