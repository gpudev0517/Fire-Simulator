#pragma once

class IGLSurface;
class NEGLRendererPrivate
{
public:
    NEGLRendererPrivate(NEGLRenderer* s):S(s)
    {
    }

    ~NEGLRendererPrivate()
    {
    }


private:
    NEGLRenderer* S;
};
