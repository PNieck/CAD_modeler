#include <openglHelpers/debug.hpp>

#include <iostream>


GLenum glCheckError_(const char *file, const int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
#       ifdef GL_INVALID_ENUM
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
#       endif

#       ifdef GL_INVALID_VALUE
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
#       endif

#       ifdef GL_INVALID_OPERATION
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
#       endif

#       ifdef GL_STACK_OVERFLOW
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
#       endif

#       ifdef GL_STACK_UNDERFLOW
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
#       endif

#       ifdef GL_OUT_OF_MEMORY
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
#       endif

#       ifdef GL_INVALID_FRAMEBUFFER_OPERATION
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
#       endif

            default: throw std::runtime_error("Unknown OpenGL error code");
        }

        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}