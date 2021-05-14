#include "Rendering/Resource/NEVertexBuffer.h"
#include "Utility/geometry.h"
#include "Rendering/Manager/NERenderManager.h"


NEVertexBuffer::NEVertexBuffer() : m_NumVertices(0), m_BufferID(0), m_Stride(0)
{

}

NEVertexBuffer::~NEVertexBuffer()
{
    destroy();
}

//uint NEVertexBuffer::generate()
//{
//
//}
//


uint NEVertexBuffer::create( NEVertexFormat format, uint numVertices, void* data /*= 0*/, uint usage /*= kStreamDraw*/ )
{
    Q_ASSERT_X(/*numVertices != 0 && */m_BufferID == 0, "NEVertexBuffer::create", "The buffer is in use");

    create(format);

    GL->glBindBuffer( GL_ARRAY_BUFFER, m_BufferID );

    //std::cout << "k size " << format << " " << kNEVFSize[format] << std::endl;
    GL->glBufferData( GL_ARRAY_BUFFER, numVertices*kNEVFSize[format], data, usage );

    GLenum error = GL->glGetError();
    _unused(error);

    //	Q_ASSERT_X(error == GL_NO_ERROR, "NEVertexBuffer::create", "Cannot allocate data for the buffer");

    m_NumVertices = numVertices;

    GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return NE::kReturnSuccess;
}

uint NEVertexBuffer::create( NEVertexFormat format)
{
    GL->glGenBuffers( 1, &m_BufferID );

    m_VertexFormat = format;

    switch(m_VertexFormat)
    {
    case kPFormat:
        setStride(sizeof(NEPVertex));
        break;
    case kCPFormat:
    default:
        setStride(sizeof(NECPVertex));
        break;

    case kTPFormat:
        setStride(sizeof(NETPVertex));
        break;

    case kNPFormat:
        setStride(sizeof(NENPVertex));
        break;

    case kCNPFormat:
        setStride(sizeof(NECNPVertex));
        break;
    case kTCPFormat:
        setStride(sizeof(NETCPVertex));
        break;

    case kTNPFormat:
        setStride(sizeof(NETNPVertex));
        break;

    case kCTNPFormat:
        setStride(sizeof(NECTNPVertex));
        break;
    case kPBFormat:
        setStride(sizeof(NEPBVertex));
        break;
    }

    return NE::kReturnSuccess;
}

void NEVertexBuffer::destroy()
{
    // clear the handle and any associated memory
    GL->glDeleteBuffers(1, &m_BufferID);
    m_BufferID = 0;
    m_NumVertices = 0;
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

uint NEVertexBuffer::makeActive()
{
    Q_ASSERT_X(m_BufferID != 0 /*&& m_NumVertices != 0*/,
               "NEVertexBuffer::makeActive", "Cannot activate buffer (Maybe it's not created yet?)");

    GL->glBindBuffer( GL_ARRAY_BUFFER, m_BufferID );

    switch (m_VertexFormat)
    {
    case kPFormat:
        GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)0);
        GL->glEnableVertexAttribArray(0);
        break;
    case kCPFormat:
    default:
        GL->glEnableClientState(GL_COLOR_ARRAY);
        GL->glColorPointer(4, GL_UNSIGNED_BYTE, m_Stride, BUFFER_OFFSET(0));
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(Color32)) );
        break;
    case kCPAFormat:
        GL->glEnableClientState(GL_COLOR_ARRAY);
        GL->glColorPointer(4, GL_UNSIGNED_BYTE, m_Stride, BUFFER_OFFSET(0));
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(Color32)) );
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(matrix33f)) );
        break;
    case kTPFormat:
        GL->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GL->glTexCoordPointer(2, GL_FLOAT, m_Stride, BUFFER_OFFSET(sizeof(float)*3));
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET(0) );
//        GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)0);
//        GL->glEnableVertexAttribArray(0);
//        GL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*3));
//        GL->glEnableVertexAttribArray(1);
        break;

    case kNPFormat:
        //glInterleavedArrays( GL_N3F_V3F, m_Stride, NULL );
        GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)0);
        GL->glEnableVertexAttribArray(0);
        GL->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*3));
        GL->glEnableVertexAttribArray(1);
        break;

    case kCNPFormat:
        GL->glEnableClientState(GL_COLOR_ARRAY);
        GL->glColorPointer(4, GL_UNSIGNED_BYTE, m_Stride, BUFFER_OFFSET( 0 ));
        GL->glEnableClientState(GL_NORMAL_ARRAY);
        GL->glNormalPointer(GL_FLOAT, m_Stride, BUFFER_OFFSET( 4 ));
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, m_Stride, BUFFER_OFFSET( 16 ));
        break;
    case kTCPFormat:
        GL->glInterleavedArrays( GL_T2F_C4UB_V3F, m_Stride, NULL );
        break;

    case kTNPFormat:
        GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*5));
        GL->glEnableVertexAttribArray(0);
        GL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*0));
        GL->glEnableVertexAttribArray(1);
        GL->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*2));
        GL->glEnableVertexAttribArray(2);
        break;

    case kCTNPFormat:
        GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*9));
        GL->glEnableVertexAttribArray(0);
        GL->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*0));
        GL->glEnableVertexAttribArray(1);
        GL->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*6));
        GL->glEnableVertexAttribArray(2);
        GL->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*2));
        GL->glEnableVertexAttribArray(3);
        break;
    case kPBFormat: //for quad wireframe
        GL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)0);
        GL->glEnableVertexAttribArray(0);
        GL->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_Stride, (void*)(sizeof(float)*3));
        GL->glEnableVertexAttribArray(1);
        break;
    }

    return NE::kReturnSuccess;
}

void* NEVertexBuffer::beginLoadData()
{
    GL->glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    void* mapPtr = GL->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    Q_ASSERT_X(mapPtr, "NEVertexBuffer::beginLoadData", "Unable to map vertex buffer to client memory");
    return mapPtr;
}

uint NEVertexBuffer::endLoadData()
{
    GL->glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    bool ret = (GL->glUnmapBuffer(GL_ARRAY_BUFFER) != GL_FALSE);
    _unused(ret);
    Q_ASSERT_X(ret, "NEVertexBuffer::endLoadData", "Unable to unlock the vertex buffer");
    GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );
    return NE::kReturnSuccess;
}

uint NEVertexBuffer::setData(GLsizeiptr bytes, const void* data /*= 0*/, uint usage /*= kStreamDraw*/)
{
    GL->glGetError();
    GL->glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    GL->glBufferData(GL_ARRAY_BUFFER, bytes, data, usage);
    Q_ASSERT_X(GL->glGetError() == GL_NO_ERROR, "NEVertexBuffer::setData", "Cannot allocate data for the buffer");
    GL->glBindBuffer( GL_ARRAY_BUFFER, 0 );
    return NE::kReturnSuccess;
}

void NEVertexBuffer::setStride( uint stride )
{
    m_Stride = stride;
}

uint NEVertexBuffer::getVertexCount()
{
    return m_NumVertices;
}
