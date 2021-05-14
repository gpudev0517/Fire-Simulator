#pragma once

class NEIndexBuffer
{
public:
    //! Lock down the buffer and start loading. Returns pointer to client side data area.
    void* beginLoadData();
    //! Unlock the buffer, we're done loading. Returns true if all went well.
    uint endLoadData();
    //! Directly map a predefined data to the buffer
    uint setData(GLsizeiptr bytes, const void* data = 0, uint usage = GL_DYNAMIC_DRAW);

    friend class NEGLResourceManager;
    friend class NEGLRenderer;
    friend class NEGLManager;

private:
    NEIndexBuffer();
    ~NEIndexBuffer();
    //! Create the resources for the index buffer
    uint create(uint numIndices, void* data = 0, uint usage = GL_DYNAMIC_DRAW);
    //! Destroy the resources for the index buffer
    void destroy();
    //! Make this the active index buffer
    uint makeActive();
    //! Unimplemented to prevent any accidental copy operation
    NEIndexBuffer(const NEIndexBuffer& other);
    NEIndexBuffer& operator=(const NEIndexBuffer& other);

    uint m_NumIndices;
    uint m_BufferID;
};
