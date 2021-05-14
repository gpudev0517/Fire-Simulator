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
#include "gvdb_render.h"


#ifdef BUILD_OPENGL

QOpenGLFunctions_3_3_Compatibility *g_GL = NULL;
QOpenGLFunctions_4_3_Compatibility *g_GL43 = NULL;
QOpenGLExtension_EXT_direct_state_access *g_GLEXTDSA = NULL;

	void checkGL( const char* msg )
	{
        if(!g_GL)
        {
            g_GL = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
            g_GL43 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Compatibility>();

            if(QOpenGLContext::currentContext()->hasExtension("GL_EXT_direct_state_access")){
                g_GLEXTDSA = new QOpenGLExtension_EXT_direct_state_access();
                bool bSucess = g_GLEXTDSA->initializeOpenGLFunctions();
                if(!bSucess)
                {
                    std::cout << "Current Context is not supported Extension : GL_EXT_direct_state_access" << std::endl;
                }

            }
        }
		GLenum errCode;
		//const GLubyte* errString;
        errCode = g_GL->glGetError();
		if (errCode != GL_NO_ERROR) {
			//printf ( "%s, ERROR: %s\n", msg, gluErrorString(errCode) );
			gprintf ("%s, ERROR: 0x%x\n", msg, errCode );
		}
	}

	void makeSimpleShaderGL ( Scene* scene, char* vertname, char* fragname )
	{
		scene->AddShader ( GLS_SIMPLE, vertname, fragname );
		scene->AddParam ( GLS_SIMPLE, UVIEW, (char*)"uView" );
		scene->AddParam ( GLS_SIMPLE, UPROJ, (char*)"uProj" );
		scene->AddParam ( GLS_SIMPLE, UMODEL, (char*)"uModel" );	
		scene->AddParam ( GLS_SIMPLE, ULIGHTPOS, (char*)"uLightPos" );
		scene->AddParam ( GLS_SIMPLE, UCLRAMB, (char*)"uClrAmb" );
		scene->AddParam ( GLS_SIMPLE, UCLRDIFF, (char*)"uClrDiff" );		
	}
	void makeOutlineShader ( Scene* scene, char* vertname, char* fragname )
	{
		scene->AddShader (GLS_OUTLINE, vertname, fragname );
		scene->AddParam ( GLS_OUTLINE, UVIEW, (char*)"uView" );
		scene->AddParam ( GLS_OUTLINE, UPROJ, (char*)"uProj" );
		scene->AddParam ( GLS_OUTLINE, UMODEL, (char*)"uModel" );	
		scene->AddParam ( GLS_OUTLINE, UCLRAMB, (char*)"uClrAmb" );
	}

	void makeSliceShader ( Scene* scene, char* vertname, char* fragname )
	{
		scene->AddShader ( GLS_SLICE, vertname, fragname );
		scene->AddParam ( GLS_SLICE, UVIEW, (char*)"uView" );
		scene->AddParam ( GLS_SLICE, UPROJ, (char*)"uProj" );
		scene->AddParam ( GLS_SLICE, ULIGHTPOS, (char*)"uLightPos" );
		scene->AddParam ( GLS_SLICE, UTEX, (char*)"uTex" );	
	}
	void makeVoxelizeShader ( Scene* scene, char* vertname, char* fragname, char* geomname )
	{
		scene->AddShader ( GLS_VOXELIZE, vertname, fragname, geomname );
		scene->AddParam ( GLS_VOXELIZE, UW, (char*)"uW" );
		scene->AddParam ( GLS_VOXELIZE, UTEXRES, (char*)"uTexRes" );
		scene->AddParam ( GLS_VOXELIZE, USAMPLES, (char*)"uSamples" );
	}
	void makeRaycastShader ( Scene* scene, char* vertname, char* fragname )
	{
		scene->AddShader ( GLS_RAYCAST, vertname, fragname );
		scene->AddParam ( GLS_RAYCAST, UINVVIEW, (char*)"uInvView" );
		scene->AddParam ( GLS_RAYCAST, UCAMPOS,  (char*)"uCamPos" );
		scene->AddParam ( GLS_RAYCAST, UCAMDIMS, (char*)"uCamDims" );
		scene->AddParam ( GLS_RAYCAST, UVOLMIN,  (char*)"uVolMin" );
		scene->AddParam ( GLS_RAYCAST, UVOLMAX,  (char*)"uVolMax" );
		scene->AddParam ( GLS_RAYCAST, UTEX,     (char*)"uTex" );
		scene->AddParam ( GLS_RAYCAST, UTEXRES,  (char*)"uTexRes" );
		scene->AddParam ( GLS_RAYCAST, ULIGHTPOS, (char*)"uLightPos" );
		//scene->AddParam ( GLS_RAYCAST, USAMPLES, (char*)"uSamples" );
	}
	void makeInstanceShader ( Scene* scene, char* vertname, char* fragname )
	{
		scene->AddShader ( GLS_INSTANCE, vertname, fragname );
		scene->AddParam ( GLS_INSTANCE, UVIEW, (char*)"uView" );
		scene->AddParam ( GLS_INSTANCE, UPROJ, (char*)"uProj" );	
	
		/*scene->AddAttrib ( GLS_INSTANCE, UVOLMIN, "instVMin" );		// instance attributes
		scene->AddAttrib ( GLS_INSTANCE, UVOLMAX, "instVMax" );
		scene->AddAttrib ( GLS_INSTANCE, UTEXMIN, "instTMin" );
		scene->AddAttrib ( GLS_INSTANCE, UTEXMAX, "instTMax" );
		scene->AddAttrib ( GLS_INSTANCE, UCLRAMB, "instClr" );*/
	}
	void makeScreenShader ( Scene* scene, char* vertname, char* fragname )
	{
		scene->AddShader ( GLS_SCREENTEX, vertname, fragname );
		scene->AddParam ( GLS_SCREENTEX, UTEX,     (char*)"uTex" );	
	}

	
	void renderCamSetupGL ( Scene* scene, int prog_id, Matrix4F* model )
	{
		int prog = scene->getProgram(prog_id);
		int attr_model = scene->getParam(prog_id, UMODEL);
		int attr_cam = scene->getParam(prog_id, UCAMPOS);

		// Set model, view, projection matrices
		if ( attr_model != -1 ) {		
			if ( model == 0x0 ) {
				Matrix4F ident; ident.Identity ();
                g_GL43->glProgramUniformMatrix4fv( prog, attr_model, 1, GL_FALSE, ident.GetDataF() );
			} else  {
                g_GL43->glProgramUniformMatrix4fv( prog, attr_model, 1, GL_FALSE, model->GetDataF() );
			}
		}
		Camera3D* cam = scene->getCamera ();		
        g_GL43->glProgramUniformMatrix4fv( prog, scene->getParam(prog_id, UVIEW), 1, GL_FALSE, cam->getViewMatrix().GetDataF() );
        g_GL43->glProgramUniformMatrix4fv( prog, scene->getParam(prog_id, UPROJ), 1, GL_FALSE, cam->getProjMatrix().GetDataF() );
        if ( attr_cam != -1 ) g_GL43->glProgramUniform3fv ( prog, attr_cam, 1, &cam->getPos().x );
	}

    void renderLightSetupGL ( Scene* scene, int prog_id ) 
	{
		Light* light = scene->getLight ();	
        g_GL43->glProgramUniform3fv  (scene->getProgram(prog_id), scene->getParam(prog_id, ULIGHTPOS), 1, &light->getPos().x );
	}
	void renderSetMaterialGL(Scene* scene, int prog_id, Vector4DF amb, Vector4DF diff, Vector4DF spec)
	{
		int prog = scene->getProgram(prog_id);
        g_GL43->glProgramUniform4fv(prog, scene->getParam(prog_id, UCLRAMB), 1, &amb.x);
        g_GL43->glProgramUniform4fv(prog, scene->getParam(prog_id, UCLRDIFF), 1, &diff.x);
        g_GL43->glProgramUniform4fv(prog, scene->getParam(prog_id, UCLRSPEC), 1, &spec.x);
	}
	void renderSetTex3D ( Scene* scene, int prog_id, int tex, Vector3DF res )
	{
		int prog = scene->getProgram(prog_id);
		if ( scene->getParam(prog_id, UTEXRES) != -1 )
            g_GL43->glProgramUniform3fv ( prog, scene->getParam(prog_id, UTEXRES), 1, &res.x );

        g_GL43->glProgramUniform1i ( prog, scene->getParam(prog_id, UTEX), 0 );
        g_GL->glActiveTexture ( GL_TEXTURE0 );
        g_GL->glBindTexture ( GL_TEXTURE_3D, tex );
	}
	void renderSetTex2D ( Scene* scene, int prog_id, int tex )
	{
        g_GL43->glProgramUniform1i (scene->getProgram(prog_id), scene->getParam(prog_id, UTEX), 0 );
        g_GL->glActiveTexture ( GL_TEXTURE0 );
        g_GL->glBindTexture ( GL_TEXTURE_2D, tex );
	}
	void renderSetUW ( Scene* scene, int prog_id, Matrix4F* model, Vector3DF res )
	{
        g_GL43->glProgramUniformMatrix4fv( scene->getProgram(prog_id), scene->getParam(prog_id, UW ), 1, GL_FALSE, model->GetDataF() );
        g_GL43->glProgramUniform3fv ( scene->getProgram(prog_id), scene->getParam(prog_id, UTEXRES ), 1, &res.x );
	}

	void renderSceneGL ( Scene* scene, int prog_id )
	{
        g_GL->glEnable ( GL_CULL_FACE );
        g_GL->glEnable ( GL_DEPTH_TEST );
		renderSceneGL ( scene, prog_id, true );
	}

	void renderSceneGL ( Scene* scene, int prog_id, bool bMat )
	{	
		// Render each model
		Model* model;
		if ( scene->useOverride() && bMat )
			renderSetMaterialGL ( scene, prog_id, scene->clrAmb, scene->clrDiff, scene->clrSpec );

		for (int n = 0; n < scene->getNumModels(); n++ ) {
			model = scene->getModel( n );		
			if ( !scene->useOverride() && bMat ) 
				renderSetMaterialGL ( scene, prog_id, model->clrAmb, model->clrDiff, model->clrSpec );
			
            g_GL->glBindVertexArray ( model->vertArrayID );
            g_GL->glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, model->elemBufferID );
            g_GL->glDrawElements ( GL_TRIANGLES, model->elemCount * 3, GL_UNSIGNED_INT, 0 );
		}
        g_GL->glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
        g_GL->glBindVertexArray ( 0 );
        g_GL->glDisable ( GL_DEPTH_TEST );
        g_GL->glDisable ( GL_CULL_FACE );

		// OpenGL 3.3
		/* glEnableClientState ( GL_VERTEX_ARRAY );
		glBindBuffer ( GL_ARRAY_BUFFER, model->vertBufferID );	
		glVertexPointer ( model->vertComponents, GL_FLOAT, model->vertStride, (char*) model->vertOffset );
		glNormalPointer ( GL_FLOAT, model->vertStride, (char*) model->normOffset );
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, model->elemBufferID );	 
		glDrawElements ( model->elemDataType, model->elemCount*3, GL_UNSIGNED_INT, 0 );	*/
	}

	void renderScreenspaceGL ( Scene* scene, int prog_id )
	{
		int prog = scene->getProgram(prog_id);
		Camera3D* cam = scene->getCamera ();		

		if ( scene->getParam(prog_id, UINVVIEW) != -1 ) 
            g_GL43->glProgramUniformMatrix4fv( prog, scene->getParam(prog_id, UINVVIEW ), 1, GL_FALSE, cam->getInvView().GetDataF() );
	
		if ( scene->getParam(prog_id, UCAMPOS) != -1 ) {
            g_GL43->glProgramUniform3fv ( prog, scene->getParam(prog_id, UCAMPOS), 1, &cam->getPos().x );
			Vector3DF cd;
			cd.x = tan ( cam->getFov() * 0.5 * 3.141592/180.0f );
			cd.y = cd.x / cam->getAspect();
			cd.z = cam->getNear();
            g_GL43->glProgramUniform3fv	( prog, scene->getParam(prog_id, UCAMDIMS), 1, &cd.x );
		}

		//scene->getScreenquad().SelectVBO ();
		//scene->getScreenquad().Draw(1);
	}

#endif
