file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/CAD_modeler/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(modeler_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(modeler_lib PUBLIC ../include)
target_link_libraries(modeler_lib PRIVATE glad glm)
target_compile_features(modeler_lib PRIVATE cxx_std_20)

# For IDEs
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST}
)

# Copy shaders to binary directory
file(COPY ../shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
