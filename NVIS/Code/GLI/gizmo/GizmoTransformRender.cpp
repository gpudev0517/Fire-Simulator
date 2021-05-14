///////////////////////////////////////////////////////////////////////////////////////////////////
// LibGizmo
// File Name : 
// Creation : 10/01/2012
// Author : Cedric Guillemet
// Description : LibGizmo
//
///Copyright (C) 2012 Cedric Guillemet
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
///so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 


#include "GizmoTransformRender.h"

#include <QOpenGLContext>
#include <QtOpenGLExtensions/QOpenGLExtensions>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>

CGizmoTransformRender::CGizmoTransformRender()
{
    m_GL = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
}

void CGizmoTransformRender::updateShaderParams()
{

    GLfloat modelview[16];
    m_GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    m_GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );
    m_GL->glUniformMatrix4fv( m_GL->glGetUniformLocation( programId, "projection"), 1, GL_FALSE, proj );
    m_GL->glUniformMatrix4fv( m_GL->glGetUniformLocation( programId, "modelview"), 1, GL_FALSE,  modelview );

    float currentColor[4];
    m_GL->glGetFloatv( GL_CURRENT_COLOR, currentColor );
    m_GL->glUniform4fv( m_GL->glGetUniformLocation( programId, "meshColor" ), 1, currentColor );
}

void CGizmoTransformRender::DrawCircle(const tvector3 &orig,float r,float g,float b,const tvector3 &vtx,const tvector3 &vty)
{
    m_GL->glDisable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_LIGHTING);
    m_GL->glColor4f(r,g,b,1);
    m_GL->glLineWidth( 2.0 );

    updateShaderParams();


    tvector3 q3[50];
	for (int i = 0; i < 50 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos((2*ZPI/50)*i);
		vt += vty * sin((2*ZPI/50)*i);
		vt += orig;
        q3[i] = vt;
    }
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
    m_GL->glDrawArrays(GL_LINE_LOOP,0,50);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);
}


void CGizmoTransformRender::DrawCircleHalf(const tvector3 &orig,float r,float g,float b,const tvector3 &vtx,const tvector3 &vty,tplane &camPlan)
{
    m_GL->glDisable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_LIGHTING);
    m_GL->glColor4f(r,g,b,1);

    updateShaderParams();

    tvector3 q3[50];
    int ptCount = 0;
	for (int i = 0; i < 30 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI/30)*i);
		vt += vty * sin((ZPI/30)*i);
		vt +=orig;
		if (camPlan.DotNormal(vt))
        {
            q3[ptCount++] = vt;
        }
	}
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
    m_GL->glDrawArrays(GL_LINE_STRIP,0,ptCount);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);
}

void CGizmoTransformRender::DrawAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx,const tvector3 &vty, float fct,float fct2,const tvector4 &col)
{
    m_GL->glDisable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_LIGHTING);
    m_GL->glColor4fv(&col.x);

    updateShaderParams();

    tvector3 q3[2] = {orig, orig + axis};
    tvector3 q3_fans[31*3];
	for (int i=0;i<=30;i++)
	{
		tvector3 pt;
		pt = vtx * cos(((2*ZPI)/30.0f)*i)*fct;
		pt+= vty * sin(((2*ZPI)/30.0f)*i)*fct;
		pt+=axis*fct2;
		pt+=orig;
        q3_fans[i*3 + 0] = pt;
		pt = vtx * cos(((2*ZPI)/30.0f)*(i+1))*fct;
		pt+= vty * sin(((2*ZPI)/30.0f)*(i+1))*fct;
		pt+=axis*fct2;
		pt+=orig;
        q3_fans[i*3 + 1] = pt;
        q3_fans[i*3 + 2] = orig + axis;
	}

    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
    m_GL->glDrawArrays(GL_LINES,0,2);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_fans);
    m_GL->glDrawArrays(GL_TRIANGLE_FAN,0,31*3);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);
}

void CGizmoTransformRender::DrawCamem(const tvector3& orig,const tvector3& vtx,const tvector3& vty,float ng)
{
    m_GL->glDisable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_LIGHTING);
	int i = 0 ;
    m_GL->glEnable(GL_BLEND);
    m_GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_GL->glDisable(GL_CULL_FACE);




    tvector3 q3_fan[52];
    q3_fan[0] = orig;
	for (i = 0 ; i <= 50 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng)/50)*i);
		vt += vty * sin(((ng)/50)*i);
		vt+=orig;
        q3_fan[i+1] = vt;
    }
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);

    m_GL->glColor4f(1,1,0,0.5f);
    updateShaderParams();

    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_fan);
    m_GL->glDrawArrays(GL_TRIANGLE_FAN,0,52);

    m_GL->glDisable(GL_BLEND);
    m_GL->glColor4f(1,1,0.2f,1);
    updateShaderParams();

    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_fan);
    m_GL->glDrawArrays(GL_LINE_LOOP,0,52);

    m_GL->glDisableClientState(GL_VERTEX_ARRAY);
}

void CGizmoTransformRender::DrawQuad(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3 &axisV)
{
    m_GL->glDisable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_LIGHTING);
    m_GL->glEnable(GL_BLEND);
    m_GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_GL->glDisable(GL_CULL_FACE);

	tvector3 pts[4];
	pts[0] = orig;
	pts[1] = orig + (axisU * size);
	pts[2] = orig + (axisU + axisV)*size;
	pts[3] = orig + (axisV * size);

	if (!bSelected)
        m_GL->glColor4f(1,1,0,0.5f);
	else
        m_GL->glColor4f(1,1,1,0.6f);

    updateShaderParams();
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&pts);
    m_GL->glDrawArrays(GL_TRIANGLE_FAN,0,4);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);

	if (!bSelected)
        m_GL->glColor4f(1,1,0.2f,1);
	else
        m_GL->glColor4f(1,1,1,0.6f);

    updateShaderParams();
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&pts);
    m_GL->glDrawArrays(GL_LINE_STRIP,0,4);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);

    m_GL->glDisable(GL_BLEND);
}


void CGizmoTransformRender::DrawTri(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV)
{
    m_GL->glDisable(GL_DEPTH_TEST);
    m_GL->glDisable(GL_LIGHTING);
    m_GL->glEnable(GL_BLEND);
    m_GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_GL->glDisable(GL_CULL_FACE);

	tvector3 pts[3];
	pts[0] = orig;

	pts[1] = (axisU );
	pts[2] = (axisV );

	pts[1]*=size;
	pts[2]*=size;
	pts[1]+=orig;
	pts[2]+=orig;

	if (!bSelected)
        m_GL->glColor4f(1,1,0,0.5f);
	else
        m_GL->glColor4f(1,1,1,0.6f);

    updateShaderParams();
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&pts);
    m_GL->glDrawArrays(GL_TRIANGLES,0,4);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);

	if (!bSelected)
        m_GL->glColor4f(1,1,0.2f,1);
	else
        m_GL->glColor4f(1,1,1,0.6f);

    updateShaderParams();
    m_GL->glEnableClientState(GL_VERTEX_ARRAY);
    m_GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&pts);
    m_GL->glDrawArrays(GL_LINE_STRIP,0,3);
    m_GL->glDisableClientState(GL_VERTEX_ARRAY);

    m_GL->glDisable(GL_BLEND);
}
