cmake_minimum_required(VERSION 3.27)

project(CAD_modeler VERSION 0.1
                    DESCRIPTION "Simple CAD modeling software"
                    LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Using -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(app)
