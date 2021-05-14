#include "Rendering/Resource/NESurface.h"
#include "Rendering/Manager/NERenderManager.h"

NESurface::NESurface()
{
}

NESurface::NESurface(GLuint srcTexId, GLenum textureMode, int w, int h){
    fb = 0;
    depth_rb = 0;

    GLint prevFrameBuffer;
    GL->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);

    GL->glGenFramebuffers(1, &fb);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, fb);
    //Attach 2D texture to this FBO
    GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureMode, srcTexId, 0);
    //-------------------------
    GL->glGenRenderbuffers(1, &depth_rb);
    GL->glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
    GL->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    //-------------------------
    //Attach depth buffer to FBO
    GL->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
    //-------------------------
    //Does the GPU support current FBO configuration?
    GLenum status;
    status = GL->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        //TGL::Logger::Print("GLSurface create success.");
        break;
    default:
        //LOG_E("NESurface create failed.");
        break;
    }
    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
}

NESurface::~NESurface(){
    GL->glDeleteFramebuffers(1, &fb);
    GL->glDeleteRenderbuffers(1, &depth_rb);
    fb = -1;
    depth_rb = -1;
}

NESurface *NESurface::createGLSurface(GLuint srcTexId, GLenum textureMode, int w, int h){
    return new NESurface(srcTexId, textureMode, w, h);
}
