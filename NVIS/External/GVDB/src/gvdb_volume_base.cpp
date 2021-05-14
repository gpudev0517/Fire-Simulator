//--------------------------------------------------------------------------------
// NVIDIA(R) GVDB VOXELS
// Copyright 2017, NVIDIA Corporation. 
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
//    in the documentation and/or  other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived 
//    from this software without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Version 1.0: Rama Hoetzlein, 5/1/2017
//----------------------------------------------------------------------------------

#include "gvdb_volume_base.h"
#include "gvdb_render.h"
#include "gvdb_allocator.h"
#include "gvdb_scene.h"

using namespace nvdb;

void VolumeBase::getDimensions ( Vector3DF& objmin, Vector3DF& objmax, Vector3DF& voxmin, Vector3DF& voxmax, Vector3DF& voxsize, Vector3DF& voxres )
{
	objmin = mObjMin;
	objmax = mObjMax;
	voxmin = mVoxMin;
	voxmax = mVoxMax;
	voxsize = mVoxsize * 1000;
	voxres = mVoxRes;
}

void VolumeBase::getTiming ( float& render_time )
{
	render_time = mRenderTime.x;
}

void VolumeBase::ClearGeometry ( Model* m )
{
    if ( m->vertArrayID != -1 ) g_GL->glDeleteVertexArrays ( 1,  (GLuint*) &m->vertArrayID );
    if ( m->vertBufferID != -1 ) g_GL->glDeleteBuffers ( 1,  (GLuint*) &m->vertBufferID );
    if ( m->elemBufferID != -1 ) g_GL->glDeleteBuffers ( 1,  (GLuint*) &m->elemBufferID );
}

void VolumeBase::CommitGeometry ( int model_id )
{
	Model* m = mScene->getModel( model_id );
	CommitGeometry ( m );
}

void VolumeBase::CommitGeometry ( Model* m )
{
	#ifdef BUILD_OPENGL
		// Create VAO
        if ( m->vertArrayID == -1 )  g_GL->glGenVertexArrays ( 1, (GLuint*) &m->vertArrayID );
        g_GL->glBindVertexArray ( m->vertArrayID );

		// Update Vertex VBO
        if ( m->vertBufferID == -1 ) g_GL->glGenBuffers( 1, (GLuint*) &m->vertBufferID );
		//glBindBuffer ( GL_ARRAY_BUFFER, vertBufferID );	
		//glBufferData ( GL_ARRAY_BUFFER, vertCount * vertStride, vertBuffer, GL_STATIC_DRAW );
	
        g_GLEXTDSA->glNamedBufferDataEXT( m->vertBufferID, m->vertCount * m->vertStride, m->vertBuffer, GL_STATIC_DRAW );
        g_GL->glEnableVertexAttribArray ( 0 );
        g_GL43->glBindVertexBuffer ( 0, m->vertBufferID, 0, m->vertStride );
        g_GL43->glVertexAttribFormat ( 0, m->vertComponents, GL_FLOAT, false, m->vertOffset );
        g_GL43->glVertexAttribBinding ( 0, 0 );
        g_GL->glEnableVertexAttribArray ( 1 );
        g_GL43->glVertexAttribFormat ( 1, m->normComponents, GL_FLOAT, false, m->normOffset );
        g_GL43->glVertexAttribBinding ( 1, 0 );
	
		// Update Element VBO
        if ( m->elemBufferID == -1 ) g_GL->glGenBuffers( 1, (GLuint*) &m->elemBufferID );
		//glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, elemBufferID );
		//glBufferData( GL_ELEMENT_ARRAY_BUFFER, elemCount * elemStride, elemBuffer, GL_STATIC_DRAW );	
        g_GLEXTDSA->glNamedBufferDataEXT( m->elemBufferID, m->elemCount * m->elemStride, m->elemBuffer, GL_STATIC_DRAW );

        g_GL->glBindVertexArray ( 0 );
	
        g_GL->glBindBuffer ( GL_ARRAY_BUFFER, 0 );
        g_GL->glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

	#endif
}
	
	
