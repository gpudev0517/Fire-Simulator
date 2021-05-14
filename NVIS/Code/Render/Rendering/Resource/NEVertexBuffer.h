#pragma once

#include "Rendering/Resource/NEVertexFormats.h"

class NECEXP_NERENDERING NEVertexBuffer
{
public:
    //! Lock down the buffer and start loading. Returns pointer to client side data area.
    void* beginLoadData();
    //! Unlock the buffer, we're done loading. Returns true if all went well.
    uint endLoadData();
    //! Directly map a predefined data to the buffer
    uint setData(GLsizeiptr bytes, const void* data = 0,  uint usage = GL_DYNAMIC_DRAW);
    void setStride(uint stride);

    friend class NEGLResourceManager;
    friend class NEGLRenderer;
    friend class NEGLManager;

    uint getBufferID()
    {
        return m_BufferID;
    }

    uint getVertexCount();

private:
    NEVertexBuffer();
    ~NEVertexBuffer();

    //! Create the resources for the vertex buffer
    uint create(NEVertexFormat format, uint numVertices, void* data = 0, uint usage = GL_DYNAMIC_DRAW);
    uint create(NEVertexFormat format);
    //! Destroy the resources for the vertex buffer
    void destroy();

    //! Make this the active vertex buffer
    uint makeActive();

    //	uint generate();
    //! Unimplemented to prevent any accidental copy operation
    NEVertexBuffer(const NEVertexBuffer& other);
    NEVertexBuffer& operator=(const NEVertexBuffer& other);

    NEVertexFormat m_VertexFormat;
    uint m_NumVertices;
    uint m_BufferID;
    uint m_Stride;
};
