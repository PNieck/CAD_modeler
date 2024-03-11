file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/CAD_modeler/*.hpp")

add_library(modeler_lib shader.cpp ${HEADER_LIST})
target_include_directories(modeler_lib PUBLIC ../include)
target_link_libraries(modeler_lib PRIVATE glad)

# For IDEs
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST}
)
