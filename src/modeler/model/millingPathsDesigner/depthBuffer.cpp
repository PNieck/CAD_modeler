#include <../../../../include/CAD_modeler/model/millingPathsDesigner/depthBuffer.hpp>

#include <stdexcept>


DepthBuffer::DepthBuffer(const int width, const int height)
{
    glGenFramebuffers(1, &fbo);
    Use();

    // Generate depth buffer
    glGenRenderbuffers(1, &depthBufferTexture);

    glGenTextures(1, &depthBufferTexture);
    glBindTexture(GL_TEXTURE_2D, depthBufferTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT32,   // internal format
        width,
        height,
        0,
        GL_DEPTH_COMPONENT,     // format
        GL_FLOAT,               // type
        nullptr
    );

    // Attach it to render buffer object
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthBufferTexture,
        0
    );

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Error while creating framebuffer");

    UseDefault();
}


DepthBuffer::~DepthBuffer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &depthBufferTexture);
}
