#include "Rendering/Resource/NEIndexBuffer.h"
#include "Utility/geometry.h"
#include "Rendering/Manager/NERenderManager.h"

NEIndexBuffer::NEIndexBuffer() : m_NumIndices(0), m_BufferID(0)
{
}

NEIndexBuffer::~NEIndexBuffer()
{
    destroy();
}

uint NEIndexBuffer::create(uint numIndices, void* data /*= 0*/, uint usage /*= kStreamDraw*/ )
{
    //Q_ASSERT_X(numIndices != 0 && m_BufferID == 0, "NEIndexBuffer::create", "The buffer is in use");

    GL->glGenBuffers( 1, &m_BufferID );
    GL->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_BufferID );

    GL->glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(uint), data, usage );

    Q_ASSERT_X(GL->glGetError() == GL_NO_ERROR, "NEVertexBuffer::create", "Cannot allocate data for the buffer");

    m_NumIndices = numIndices;

    //	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    return NE::kReturnSuccess;
}

void NEIndexBuffer::destroy()
{
    // clear the handle and any associated memory
    GL->glDeleteBuffers(1, &m_BufferID);
    m_BufferID = 0;
    m_NumIndices = 0;
}

uint NEIndexBuffer::makeActive()
{
    Q_ASSERT_X(m_BufferID != 0 && m_NumIndices != 0, "NEIndexBuffer::makeActive", "Cannot activate buffer (Maybe it's not created yet?)");

    GL->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_BufferID );
    return NE::kReturnSuccess;
}

void* NEIndexBuffer::beginLoadData()
{
    GL->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_BufferID );
    void* mapPtr = GL->glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    Q_ASSERT_X(mapPtr, "NEIndexBuffer::beginLoadData", "Unable to map index buffer to client memory");
    return mapPtr;
}

uint NEIndexBuffer::endLoadData()
{
    GL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    bool ret = (GL->glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) != GL_FALSE);
    _unused(ret);
    Q_ASSERT_X(ret, "NEVertexBuffer::endLoadData", "Unable to unlock the vertex buffer");
    GL->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    return NE::kReturnSuccess;
}

uint NEIndexBuffer::setData( GLsizeiptr bytes, const void* data /*= 0*/, uint usage /*= kStreamDraw*/ )
{
    GL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    GL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, usage);
    Q_ASSERT_X(GL->glGetError() == GL_NO_ERROR, "NEIndexBuffer::setData", "Cannot allocate data for the buffer");
    return NE::kReturnSuccess;
}



