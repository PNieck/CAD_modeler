cmake_minimum_required(VERSION 3.27)

project(CAD_modeler VERSION 0.1
                    DESCRIPTION "Simple CAD modeling software"
                    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

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
include(cmake/submodule.cmake)
include(cmake/compilerWarnings.cmake)

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
