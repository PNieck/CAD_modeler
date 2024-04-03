cmake_minimum_required(VERSION 3.27)

project(CAD_modeler VERSION 0.1
                    DESCRIPTION "Simple CAD modeling software"
                    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Using -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Add tests
    include(CTest)
    enable_testing()
endif()

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glfw glm)

# Copy shaders to binary directory
file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# External dependencies
add_subdirectory(extern)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(app)

# Adding tests directory
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
