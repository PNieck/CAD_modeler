FetchContent_Declare(dlib
    GIT_REPOSITORY https://github.com/davisking/dlib.git
    GIT_TAG        v19.24
)
FetchContent_MakeAvailable(dlib)
target_compile_definitions(dlib PRIVATE HAVE_BOOLEAN)
