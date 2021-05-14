#pragma once

class NESurface
{
public:
    NESurface();
    NESurface(GLuint srcTexId, GLenum textureMode, int w, int h);
    static NESurface *createGLSurface(GLuint srcTexId, GLenum textureMode, int w, int h);
    ~NESurface();

private:
    GLuint fb;
    GLuint depth_rb;
};
