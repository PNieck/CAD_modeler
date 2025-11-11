#pragma once

#include <glad/glad.h>


class DepthBuffer {
public:
    DepthBuffer(int width, int height);
    DepthBuffer(DepthBuffer&) = delete;
    DepthBuffer(const DepthBuffer&) = delete;

    ~DepthBuffer();

    void Use() const
        { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

    static void UseDefault()
        { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    DepthBuffer& operator=(DepthBuffer&) = delete;
private:
    unsigned int fbo = 0;
    //unsigned int colorTexture = 0;
    unsigned int depthRenderbuffer = 0;
};
