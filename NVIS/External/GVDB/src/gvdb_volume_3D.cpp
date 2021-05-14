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

#include "gvdb_volume_3D.h"
#include "gvdb_render.h"
#include "app_perf.h"

using namespace nvdb;

int Volume3D::mVFBO[2] = {-1, -1};
int Volume3D::mVCLEAR = -1;

Volume3D::Volume3D ( Scene* scn )
{
	mPool = new Allocator;	
	mScene = scn;
}

Volume3D::~Volume3D ()
{
	Clear ();
}

void Volume3D::Resize ( char typ, Vector3DI res, Matrix4F* xform, bool bGL )
{
	mVoxRes = res;				

	mPool->AtlasReleaseAll ();
	mPool->TextureCreate ( 0, typ, res, true, bGL );
}

void Volume3D::SetDomain ( Vector3DF vmin, Vector3DF vmax )
{
	mObjMin = vmin;
	mObjMax = vmax;
}

void Volume3D::Clear ()
{
	mPool->AtlasReleaseAll ();
}

#define max3(a,b,c)		( (a>b) ? ((a>c) ? a : c) : ((b>c) ? b : c) )

void Volume3D::Empty ()
{
	mPool->AtlasFill ( 0 );
}

void Volume3D::CommitFromCPU ( float* src )
{
	mPool->AtlasCommitFromCPU ( 0, (uchar*) src );
}

void Volume3D::RetrieveGL ( char* dest )
{
	#ifdef BUILD_OPENGL
		mPool->AtlasRetrieveGL ( 0, dest );
	#endif 
}

void Volume3D::PrepareRasterGL ( bool start )
{
	#ifdef BUILD_OPENGL
		if ( start ) {
			// Enable opengl state once
            g_GL->glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            g_GL->glDepthMask (GL_FALSE);
            g_GL->glDisable ( GL_DEPTH_TEST );
            g_GL->glDisable ( GL_TEXTURE_2D );
            g_GL->glEnable ( GL_TEXTURE_3D );

			// Set rasterize program once
            g_GL->glUseProgram ( getScene()->getProgram(GLS_VOXELIZE) );
			checkGL ( "glUseProgram(VOX) (PrepareRaster)" );

			// Set raster sampling to major axis						
			int smax = max3(mVoxRes.x, mVoxRes.y, mVoxRes.z);
            g_GL->glViewport(0, 0, smax, smax );

			// Bind texture
			int glid = mPool->getAtlasGLID ( 0 );
            g_GL->glActiveTexture ( GL_TEXTURE0 );
            g_GL->glBindTexture ( GL_TEXTURE_3D, glid );
            checkGL ( "glBindTexture (RasterizeFast)" );

            if ( mVFBO[0] == -1 ) g_GL->glGenFramebuffers(1, (GLuint*) &mVFBO);
            g_GL->glBindFramebuffer(GL_FRAMEBUFFER, mVFBO[0] );
            g_GL->glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glid, 0);
				
			switch ( mPool->getAtlas(0).type ) {
            case T_UCHAR:	g_GL43->glBindImageTexture( 0, glid, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8 );		break;
            case T_FLOAT:	g_GL43->glBindImageTexture( 0, glid, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F );	break;
			};
			checkGL ( "glBindImageTexture (RasterizeFast)" );	

			// Indicate res of major axis
            g_GL43->glProgramUniform1i ( getScene()->getProgram(GLS_VOXELIZE), getScene()->getParam(GLS_VOXELIZE, USAMPLES), smax );

		} else {
			// Restore state
            g_GL->glUseProgram ( 0 );
            g_GL->glEnable (GL_DEPTH_TEST);
            g_GL->glFramebufferTexture (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
            g_GL->glBindFramebuffer (GL_FRAMEBUFFER, 0);
            g_GL->glDepthMask ( GL_TRUE);
            g_GL->glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			checkGL ( "glUseProgram(0) (PrepareRaster)" );			
		}
	#endif 
}

void Volume3D::SurfaceVoxelizeFastGL ( Vector3DF vmin, Vector3DF vmax, Matrix4F* model )
{
	mObjMin = vmin;
	mObjMax = vmax;

	#ifdef BUILD_OPENGL

		// Clear texture. 		
		Empty ();												// using cuda kernel						
		cudaCheck ( cuCtxSynchronize(), "Volume3D", "SurfaceVoxelizeFastGL", "cuCtxSynchronize", "", false );		// must sync for opengl to use		
		
		//glClearColor ( 0, 0, 0, 0);			// using FBO, see PrepareRasterGL setup
		//glClear(GL_COLOR_BUFFER_BIT);	

		// Setup transform matrix
		Matrix4F mw;
		mw.Translate ( -mObjMin.x, -mObjMin.y, -mObjMin.z );
		mw *= (*model);
		mw *= Vector3DF( 2.0/(mObjMax.x-mObjMin.x), 2.0/(mObjMax.y-mObjMin.y), 2.0/(mObjMax.z-mObjMin.z) );		
		renderSetUW ( getScene(), GLS_VOXELIZE, &mw, mVoxRes );

		// Rasterize		
		renderSceneGL ( getScene(), GLS_VOXELIZE, false );
		checkGL ( "renderSceneGL (RasterizeFast)" );

        g_GL->glFinish ();

	#endif	
}


void Volume3D::SurfaceVoxelizeGL ( uchar chan, Model* model, Matrix4F* xform )
{
	#ifdef BUILD_OPENGL

		// Full rasterize
		// ** Note ** This is slow if called repeatedly, 
		// since it prepares all the necessary opengl/shader state.
		// See PrepareRaster and RasterizeFast above when using for staging.

		// Configure model
		model->ComputeBounds ( *xform, 0.05 );
		mObjMin = model->objMin; mObjMax = model->objMax;
		mVoxMin = mObjMin;	mVoxMin /= mVoxsize;
		mVoxMax = mObjMax;  mVoxMax /= mVoxsize;
		mVoxRes = mVoxMax;	mVoxRes -= mVoxMin;

		// Create atlas if none exists
		mPool->AtlasReleaseAll ();
		mPool->AtlasCreate ( chan, T_FLOAT, mVoxRes, Vector3DI(1,1,1), 0, 0, true, true );		

        if ( mVFBO[0] == -1 ) g_GL->glGenFramebuffers(1, (GLuint*) &mVFBO);

		// Bind frame buffer to 3D texture to clear it
		int glid = mPool->getAtlasGLID ( chan );
        g_GL->glBindFramebuffer(GL_FRAMEBUFFER, mVFBO[0] );
        g_GL->glFramebufferTexture (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glid, 0);
        g_GL->glClearColor ( 0, 0, 0, 0);
        g_GL->glClear(GL_COLOR_BUFFER_BIT);
        g_GL->glFramebufferTexture (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
        g_GL->glBindFramebuffer (GL_FRAMEBUFFER, 0);

		// Set raster sampling to major axis
		int s = max3( mVoxRes.x, mVoxRes.y, mVoxRes.z );
        g_GL->glViewport(0, 0, s , s );

		// Not using ROP to write to FB maks out all color/depth
        g_GL->glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        g_GL->glDepthMask (GL_FALSE);
        g_GL->glDisable (GL_DEPTH_TEST);
	
		// Bind 3D texture for write. Layered is set to true for 3D texture
        g_GL->glEnable ( GL_TEXTURE_3D );
        g_GL->glActiveTexture ( GL_TEXTURE0 );
        g_GL->glBindTexture ( GL_TEXTURE_3D, glid );
        g_GL43->glBindImageTexture( 0, glid, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F );
    
        g_GL->glUseProgram ( getScene()->getProgram(GLS_VOXELIZE) );

        g_GL43->glProgramUniform1i ( getScene()->getProgram(GLS_VOXELIZE), getScene()->getParam(GLS_VOXELIZE, USAMPLES), s );	// indicate res of major axis
    
		// Send model orientation, scaled to fit in volume
		Matrix4F mw;
		mw.Translate ( -mObjMin.x, -mObjMin.y, -mObjMin.z );
		mw *= (*xform);
		mw *= Vector3DF( 2.0/(mObjMax.x-mObjMin.x), 2.0/(mObjMax.y-mObjMin.y), 2.0/(mObjMax.z-mObjMin.z) );		
		renderSetUW ( getScene(), getScene()->getProgram(GLS_VOXELIZE), &mw, mVoxRes );		// this sets uTexRes in shader

		renderSceneGL ( getScene(), getScene()->getProgram(GLS_VOXELIZE), false );

        g_GL->glUseProgram ( 0 );
	
		// restore screen raster 
        g_GL43->glBindImageTexture (0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F );
        g_GL->glEnable (GL_DEPTH_TEST);
        g_GL->glDepthMask ( GL_TRUE);
        g_GL->glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        g_GL->glViewport ( 0, 0, getScene()->mXres, getScene()->mYres );

	#endif
}

void Volume3D::getMemory ( float& voxels, float& overhead, float& effective )
{
	// all measurements in MB
	voxels = float(mVoxRes.x*mVoxRes.y*mVoxRes.z*4.0) / (1024.0*1024.0);
	overhead = (float) 0.0;
	effective = float(mVoxRes.x*mVoxRes.y*mVoxRes.z*4.0) / (1024.0*1024.0);
}
