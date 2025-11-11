#include <../../../../include/CAD_modeler/model/millingPathsDesigner/depthBuffer.hpp>

#include <stdexcept>


DepthBuffer::DepthBuffer(const int width, const int height)
{
    glGenFramebuffers(1, &fbo);
    Use();

    // Generate depth buffer
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

    // Attach it to render buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Error while creating framebuffer");

    UseDefault();
}


DepthBuffer::~DepthBuffer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &depthRenderbuffer);
}
