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


#include "GizmoTransformScale.h"

#include <float.h>
extern tvector3 ptd;


IGizmo *CreateScaleGizmo()
{
    return new CGizmoTransformScale;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGizmoTransformScale::CGizmoTransformScale() : CGizmoTransform()
{
	m_ScaleType = SCALE_NONE;
}

CGizmoTransformScale::~CGizmoTransformScale()
{

}



bool CGizmoTransformScale::GetOpType(SCALETYPE &type, unsigned int x, unsigned int y)
{
    if (mLocation == LOCATE_2D)
    {
        x += mScreenWidth / 2;
        y -= mScreenHeight / 2;
    }

	// init
	tvector3 trss(GetTransformedVectorLength(0),
		GetTransformedVectorLength(1),
		GetTransformedVectorLength(2));

	m_LockX = x;
	m_LockY = y;
	m_svgMatrix = *m_pMatrix;

	tmatrix mt;
    if (mLocation == LOCATE_LOCAL)
    {
        mt = m_pMatrix->GetNoScaleTransform();
        tmatrix transMat;
        transMat.Translation(m_Pivot);
        mt.Multiply(transMat);
        mt.Inverse();
    }
    else
    {
        // world
        mt.Translation(-GetPivotPosition());
    }

	// ray casting
	tvector3 rayOrigin,rayDir,df2;
    BuildRay(x, y, rayOrigin, rayDir);


    if (mLocation == LOCATE_2D)
    {
        //plan 2 : X/Y
        df2 = RayTrace2(rayOrigin, rayDir, GetTransformedVector(2), mt, trss, false);

        if ( ( df2.x >= 0 ) && (df2.x <= 1) && ( fabs(df2.y) < 0.1f ) ) { type = SCALE_X; return true;	}
        else if ( ( df2.y >= 0 ) && (df2.y <= 1) && ( fabs(df2.x) < 0.1f ) ) { type = SCALE_Y; return true; }
        else if ( (df2.x<0.5f) && (df2.y<0.5f) && (df2.x>0) && (df2.y>0)) { type = SCALE_XY; return true; }
    }
    else
    {
        // plan 1 : X/Z
        df2 = RayTrace2(rayOrigin, rayDir, GetTransformedVector(1), mt, trss, false);

        if ( (df2.x<0.2f) && (df2.z<0.2f) && (df2.x>0) && (df2.z>0)) { type = SCALE_XYZ; return true; }
        else if ( ( df2.x >= 0 ) && (df2.x <= 1) && ( fabs(df2.z) < 0.1f ) ) { type = SCALE_X; return true;	}
        else if ( ( df2.z >= 0 ) && (df2.z <= 1) && ( fabs(df2.x) < 0.1f ) ) { type = SCALE_Z; return true;	}
        else if ( (df2.x<0.5f) && (df2.z<0.5f) && (df2.x>0) && (df2.z>0)) {	type = SCALE_XZ; return true; }
        else
        {
            //plan 2 : X/Y
            df2 = RayTrace2(rayOrigin, rayDir, GetTransformedVector(2), mt, trss, false);

            if ( (df2.x<0.2f) && (df2.y<0.2f) && (df2.x>0) && (df2.y>0)) { type = SCALE_XYZ; return true; }
            else if ( ( df2.x >= 0 ) && (df2.x <= 1) && ( fabs(df2.y) < 0.1f ) ) { type = SCALE_X; return true;	}
            else if ( ( df2.y >= 0 ) && (df2.y <= 1) && ( fabs(df2.x) < 0.1f ) ) { type = SCALE_Y; return true; }
            else if ( (df2.x<0.5f) && (df2.y<0.5f) && (df2.x>0) && (df2.y>0)) { type = SCALE_XY; return true; }
            else
            {
                //plan 3: Y/Z
                df2 = RayTrace2(rayOrigin, rayDir, GetTransformedVector(0), mt, trss, false);

                if ( (df2.y<0.2f) && (df2.z<0.2f) && (df2.y>0) && (df2.z>0)) { type = SCALE_XYZ; return true; }
                else if ( ( df2.y >= 0 ) && (df2.y <= 1) && ( fabs(df2.z) < 0.1f ) ) { type = SCALE_Y; return true;	}
                else if ( ( df2.z >= 0 ) && (df2.z <= 1) && ( fabs(df2.y) < 0.1f ) ) { type = SCALE_Z; return true;	}
                else if ( (df2.y<0.5f) && (df2.z<0.5f) && (df2.y>0) && (df2.z>0)) { type = SCALE_YZ; return true; }
            }
        }
    }

	type = SCALE_NONE;
	return false;
}


bool CGizmoTransformScale::OnMouseDown(unsigned int x, unsigned int y)
{
	if (m_pMatrix)
	{
		return GetOpType(m_ScaleType, x, y);
	}

	m_ScaleType = SCALE_NONE;
	return false;
}

void CGizmoTransformScale::SnapScale(float &val)
{
	if (m_bUseSnap)
	{
		val*=(100.0f);
		SnapIt(val,m_ScaleSnap);
		val/=(100.0f);
	}
}

float checkFloatValidaty(float scaleValue)
{
    float newScaleValue = scaleValue;
    if (newScaleValue < 8 * FLT_EPSILON)
        newScaleValue = 8 * FLT_EPSILON;
    if (newScaleValue >= FLT_MAX / 10)
        newScaleValue = FLT_MAX / 10;
    return newScaleValue;
}

void CGizmoTransformScale::OnMouseMove(unsigned int x, unsigned int y)
{
	if (m_ScaleType != SCALE_NONE)
	{
        if (mLocation == LOCATE_2D)
        {
            x += mScreenWidth / 2;
            y -= mScreenHeight / 2;
        }

		tvector3 rayOrigin,rayDir,df, inters, machin;
		tvector3 scVect,scVect2;

		BuildRay(x, y, rayOrigin, rayDir);
		m_plan.RayInter(inters,rayOrigin,rayDir);

		switch (m_ScaleType)
		{
		case SCALE_XZ: scVect = tvector3(1,0,1); break;
		case SCALE_X:  scVect = tvector3(1,0,0); break;
		case SCALE_Z:  scVect = tvector3(0,0,1); break;
		case SCALE_XY: scVect = tvector3(1,1,0); break;
		case SCALE_YZ: scVect = tvector3(0,1,1); break;
		case SCALE_Y:  scVect = tvector3(0,1,0); break;
		case SCALE_XYZ:scVect = tvector3(1,1,1); break;
		}

		// Get Mouse moving axis
		tvector3 axisVector = scVect;
        //if (mLocation == LOCATE_LOCAL)
		{
			axisVector.TransformVector(*m_pMatrix);
			axisVector.Normalize();
		}

        df = inters - m_LockVertex;
		float diff = df.Dot(axisVector);

        tvector4 modelPosition = m_pMatrix->V4.position;
        tvector4 cameraPosition = m_invmodel.V4.position;
        tvector3 sceneSize (modelPosition.x - cameraPosition.x, modelPosition.y - cameraPosition.y, modelPosition.z - cameraPosition.z);
        diff *= (10 / sceneSize.Length());

		scVect2 = tvector3(1,1,1) - scVect;
        float lng2;
        if (diff < 0)
            lng2 = pow(1.3f, diff);
        else
            lng2 = 1 + diff;

		if (m_ScaleType == SCALE_XYZ)
		{
			SnapScale(lng2);
			scVect *= lng2;
		}
		else
		{
			SnapScale(lng2);
			scVect *= lng2;
			scVect += scVect2;
		}

		tmatrix mt;
		mt.Scaling(scVect);
		*m_pMatrix = mt * m_svgMatrix;

		mEditScale->x = checkFloatValidaty(m_pMatrix->GetLine(0).Length());
		mEditScale->y = checkFloatValidaty(m_pMatrix->GetLine(1).Length());
		mEditScale->z = checkFloatValidaty(m_pMatrix->GetLine(2).Length());

		// Transform = Scale * Rotation * Translation
		*mEditPos = m_pMatrix->V4.position;
	}
	else
	{
		// predict move
		if (m_pMatrix)
		{
			GetOpType(m_ScaleTypePredict, x, y);
		}
	}

}

void CGizmoTransformScale::OnMouseUp(unsigned int x, unsigned int y)
{
	m_ScaleType = SCALE_NONE;
}

void CGizmoTransformScale::Draw(int programId)
{
	if (m_pMatrix)
	{
		ComputeScreenFactor();

		//glDisable(GL_DEPTH_TEST);
		tvector3 orig = GetPivotPosition();


		// axis
		tvector3 axeX(1,0,0),axeY(0,1,0),axeZ(0,0,1);
        //if (mLocation == LOCATE_LOCAL)
        {
            axeX.TransformVector(*m_pMatrix);
		    axeY.TransformVector(*m_pMatrix);
		    axeZ.TransformVector(*m_pMatrix);
		    axeX.Normalize();
		    axeY.Normalize();
		    axeZ.Normalize();
        }

        this->programId = programId;

		DrawTri(orig, 0.5f*GetScreenFactor(),((m_ScaleTypePredict==SCALE_XY)||(m_ScaleTypePredict==SCALE_XYZ)), axeX, axeY);
        if (mLocation != LOCATE_2D)
        {
            DrawTri(orig, 0.5f*GetScreenFactor(),((m_ScaleTypePredict==SCALE_XZ)||(m_ScaleTypePredict==SCALE_XYZ)), axeX, axeZ);
            DrawTri(orig, 0.5f*GetScreenFactor(),((m_ScaleTypePredict==SCALE_YZ)||(m_ScaleTypePredict==SCALE_XYZ)), axeY, axeZ);
        }


		axeX*=GetScreenFactor();
		axeY*=GetScreenFactor();
		axeZ*=GetScreenFactor();


		// plan1
		if (m_ScaleTypePredict != SCALE_X)
			DrawAxis(orig,axeX,axeY,axeZ,0.05f,0.83f,vector4(1,0,0,1));
		else
			DrawAxis(orig,axeX,axeY,axeZ,0.05f,0.83f,vector4(1,1,1,1));

		//plan2
		if (m_ScaleTypePredict != SCALE_Y)
			DrawAxis(orig,axeY,axeX,axeZ,0.05f,0.83f,vector4(0,1,0,1));
		else
			DrawAxis(orig,axeY,axeX,axeZ,0.05f,0.83f,vector4(1,1,1,1));

		//plan3
        if (mLocation != LOCATE_2D)
        {
            if (m_ScaleTypePredict != SCALE_Z)
                DrawAxis(orig,axeZ,axeX,axeY,0.05f,0.83f,vector4(0,0,1,1));
            else
                DrawAxis(orig,axeZ,axeX,axeY,0.05f,0.83f,vector4(1,1,1,1));
        }
	}
}
