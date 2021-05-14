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


#include "GizmoTransformRotate.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern tvector3 ptd;


IGizmo *CreateRotateGizmo()
{
    return new CGizmoTransformRotate;
}


CGizmoTransformRotate::CGizmoTransformRotate() : CGizmoTransform()
{
    m_RotateType = ROTATE_NONE;
    m_RotateTypePredict = ROTATE_NONE;
    m_Ng2 = 0;
    m_FullAngle = 0;
    m_AngleSnap = 0.f;

}

CGizmoTransformRotate::~CGizmoTransformRotate()
{

}

bool CGizmoTransformRotate::CheckRotatePlan(tvector3 &vNorm, float factor,
    const tvector3 &rayOrig,const tvector3 &rayDir,int id)
{
    tvector3 df, inters;
    m_Axis2 = vNorm;
    m_plan=vector4(GetPivotPosition(), vNorm);
    m_plan.RayInter(inters,rayOrig,rayDir);
    ptd = inters;
    df = inters - GetPivotPosition();
    df/=GetScreenFactor();

    if ( ((df.Length()/factor) >0.9f) && ( (df.Length()/factor) < 1.1f) )
    {
        m_svgMatrix = *m_pMatrix;

        m_LockVertex = df;
        m_LockVertex.Normalize();

        m_Vtx = m_LockVertex;
        m_Vty.Cross( m_LockVertex, vNorm );
        m_Vty.Normalize();
        m_Vtx *= factor;
        m_Vty *= factor;
        m_Vtz.Cross( m_Vtx, m_Vty );
        m_Ng2 = 0;
        m_FullAngle = 0;
        if( id != -1 )
            m_Axis = GetVector( id );

        return true;
    }
    return false;
}

bool CGizmoTransformRotate::GetOpType( ROTATETYPE &type, unsigned int x, unsigned int y )
{
    tvector3 rayOrigin,rayDir, axis;
    tvector3 dir = GetPivotPosition()-m_CamSrc;
    dir.Normalize();

    BuildRay(x, y, rayOrigin, rayDir);
    if (m_bOrthographic)
    {
        dir = rayDir;
    }

//    if (mMask&AXIS_TRACKBALL)
//        if (CheckRotatePlan(dir,1.0f,rayOrigin,rayDir,-1))
//        {
//            tmatrix mt = *m_pMatrix;
//            mt.NoTrans();
//            //mt.Inverse();
//            //m_Axis = m_Axis2 = dir;
//            m_Axis.TransformPoint(mt);
//            /*
//            m_Axis *=tvector3(GetTransformedVector(0).Length(),
//            GetTransformedVector(1).Length(),
//            GetTransformedVector(2).Length());
//            */
//            type = ROTATE_TWIN;
//            return true;
//        }

    // plan 1 : X/Z
    m_Axis = GetTransformedVector(0);
    if (mMask&AXIS_X)
        if (CheckRotatePlan(m_Axis,1.0f,rayOrigin,rayDir,0)) { type = ROTATE_X; return true; }
    m_Axis = GetTransformedVector(1);
    if (mMask&AXIS_Y)
        if (CheckRotatePlan(m_Axis,1.0f,rayOrigin,rayDir,1)) { type = ROTATE_Y; return true; }
    m_Axis = GetTransformedVector(2);
    if (mMask&AXIS_Z)
        if (CheckRotatePlan(m_Axis,1.0f,rayOrigin,rayDir,2)) { type = ROTATE_Z; return true; }

    //m_Axis = GetTransformedVector(dir);
    if (mMask&AXIS_SCREEN)
        if (CheckRotatePlan(dir,1.2f,rayOrigin,rayDir,-1))
        {
            tmatrix mt = *m_pMatrix;
            mt.NoTrans();
            mt.Inverse();
            m_Axis = m_Axis2 = dir;
            m_Axis.TransformPoint(mt);

            m_Axis *=tvector3(GetTransformedVectorLength(0),
                              GetTransformedVectorLength(1),
                              GetTransformedVectorLength(2));

            type = ROTATE_SCREEN;
            return true;
        }

    type = ROTATE_NONE;

    return false;
}

bool CGizmoTransformRotate::OnMouseDown(unsigned int x, unsigned int y)
{
    if (m_pMatrix)
    {
        return GetOpType(m_RotateType, x, y);
    }

    m_RotateType = ROTATE_NONE;
    return false;
}

void CGizmoTransformRotate::getRotation(float* rot)
{
    tmatrix newRot;
    newRot.RotationAxis( m_Axis, m_Ng2 );
    tmatrix mt;
    mt.Identity();
    if (mLocation == LOCATE_WORLD)
    {
        mt.Multiply( m_svgMatrix.GetNoScaleTransform() );
        mt.Multiply(newRot);
    }
    else
    {
        mt.Multiply(newRot);
        mt.Multiply( m_svgMatrix.GetNoScaleTransform() );
    }

    for( int i = 0; i < 3; i++)
    {
        for( int j = 0; j < 3; j++)
            rot[i*3+j] = mt.m[i][j];
    }
}


void CGizmoTransformRotate::Rotate1Axe(const tvector3& rayOrigin,const tvector3& rayDir)
{
    tvector3 inters;
    m_plan=vector4(GetPivotPosition(), m_Axis2);
    m_plan.RayInter( inters, rayOrigin, rayDir );
    ptd = inters;

    tvector3 df = inters - GetPivotPosition();

    df.Normalize();
    m_LockVertex2 = df;

    float acosng = df.Dot( m_LockVertex );
    if( (acosng<-0.99999f) || (acosng> 0.99999f) )
    {
        m_Ng2 = 0.f;
        m_FullAngle = 0.0f;
    }
    else
    {
        m_Ng2 = ( float )acos( acosng );
        m_FullAngle = ( float ) acos( acosng );
    }


    if( df.Dot( m_Vty ) > 0 )
        m_Ng2 = -m_Ng2;

    tmatrix mt,mt2;

    if( m_bUseSnap )
    {
        m_Ng2*=(360.0f/ZPI);
        SnapIt( m_Ng2,m_AngleSnap );
        m_Ng2/=(360.0f/ZPI);
    }

    *mEditPos = m_svgMatrix.V4.position;

    if (m_Axis == tvector3::ZAxis)
    {
        if (mEditQT)
        {
#if USE_QUATERNION
            tquaternion gth(*m_pMatrix);

            gth.Normalize();
            gth.UnitInverse();

            tquaternion qtg;
            qtg.RotationAxis(m_Axis,m_Ng2);
            *mEditQT = gth;//gth+qtg;//tquaternion(mt2);
            mEditQT->Normalize();
#else
            mEditQT->z = m_Ng2;
#endif
        }
    }
    tquaternion gth( mt2 );
    *mEditQT = gth;
    mEditQT->Normalize();

    mEditM->Set( mt2 );

}

void CGizmoTransformRotate::OnMouseMove(unsigned int x, unsigned int y)
{
    tvector3 rayOrigin, rayDir, axis;

    BuildRay(x, y, rayOrigin, rayDir);

    if (m_RotateType != ROTATE_NONE)
    {
        Rotate1Axe(rayOrigin, rayDir);
    }
    else
    {
        // predict move
        if (m_pMatrix)
        {
            GetOpType(m_RotateTypePredict, x, y);
        }
    }

}

void CGizmoTransformRotate::OnMouseUp(unsigned int x, unsigned int y)
{
    m_RotateType = ROTATE_NONE;
}

void CGizmoTransformRotate::Draw(int programId)
{
    if (m_pMatrix)
    {
        ComputeScreenFactor();

        tvector3 right,up,frnt,dir;

        //glDisable(GL_DEPTH_TEST);
        tvector3 orig(GetPivotPosition());
        
        tvector3 plnorm(m_CamSrc-orig);
            

        plnorm.Normalize();





        tplane plCam = vector4(plnorm,0);


		dir = orig-m_CamSrc;
        dir.Normalize();
		if (m_bOrthographic)
        {
            switch (m_orthoType)
            {
            case 0:
                dir = tvector3(0, -1, 0);
                break;
            case 1:
                dir = tvector3(1, 0, 0);
                break;
            case 2:
                dir = tvector3(0, 0, -1);
                break;
            }
        }

        right.Cross(dir,GetTransformedVector(1));
        right.Normalize();

        up.Cross(dir,right);
        up.Normalize();

        right.Cross(dir,up);
        right.Normalize();

        tvector3 axeX(1,0,0),axeY(0,1,0),axeZ(0,0,1);


        if (mLocation == LOCATE_LOCAL)
        {
            axeX.TransformVector(*m_pMatrix);
            axeY.TransformVector(*m_pMatrix);
            axeZ.TransformVector(*m_pMatrix);
            axeX.Normalize();
            axeY.Normalize();
            axeZ.Normalize();
        }

        this->programId = programId;

        // Twin
        if (mMask&AXIS_TRACKBALL)
        {

            if (m_RotateTypePredict != ROTATE_TWIN)
                DrawCircle(orig, 0.2f,0.2f,0.2f,right*GetScreenFactor(),up*GetScreenFactor());
            else
                DrawCircle(orig, 1,1,1,right*GetScreenFactor(),up*GetScreenFactor());
        }

        // Screen
        if (mMask&AXIS_SCREEN)
        {
            if (m_RotateTypePredict != ROTATE_SCREEN)
                DrawCircle(orig, 1.0f,0.3f,1.0f,up*1.2f*GetScreenFactor(),right*1.2f*GetScreenFactor());
            else
                DrawCircle(orig, 1,1,1,up*1.2f*GetScreenFactor(),right*1.2f*GetScreenFactor());
        }

        // X
        right.Cross(dir, axeX);
        right.Normalize();
        frnt.Cross(right, axeX);

        frnt.Normalize();

        if (mMask&AXIS_X)
        {
            if (m_RotateTypePredict != ROTATE_X)
                DrawCircleHalf(orig, 1,0,0,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
            else
                DrawCircleHalf(orig, 1,1,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
        }

        // Y

        right.Cross(dir, axeY);
        right.Normalize();
        frnt.Cross(right, axeY);

        frnt.Normalize();

        if (mMask&AXIS_Y)
        {

            if (m_RotateTypePredict != ROTATE_Y)
                DrawCircleHalf(orig, 0,1,0,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
            else
                DrawCircleHalf(orig, 1,1,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
        }

        // Z
        right.Cross(dir, axeZ);
        right.Normalize();
        frnt.Cross(right, axeZ);

        frnt.Normalize();

        if (mMask&AXIS_Z)
        {
            if (m_RotateTypePredict != ROTATE_Z)
                DrawCircleHalf(orig, 0,0,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
            else
                DrawCircleHalf(orig, 1,1,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
        }
        // camembert
        if ( (m_RotateType != ROTATE_NONE) && (m_RotateType != ROTATE_TWIN ) )
           DrawCamem(orig,m_Vtx*GetScreenFactor(),m_Vty*GetScreenFactor(),-m_Ng2);
        /*
        // debug
        glPointSize(20);
        glBegin(GL_POINTS);
        glVertex3fv(&ptd.x);
        glEnd();

        glEnable(GL_DEPTH_TEST);
        */
#if 0
#ifdef WIN32
        GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        GDD->GetD3D9Device()->SetRenderState(D3DRS_CULLMODE , D3DCULL_NONE );
        GDD->GetD3D9Device()->SetRenderState(D3DRS_ZENABLE , D3DZB_TRUE);
        GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHATESTENABLE , FALSE);
        GDD->GetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE , TRUE);
#endif
        extern RenderingState_t GRenderingState;
        GRenderingState.mAlphaTestEnable = 0;
        GRenderingState.mZWriteEnable = 1;
        GRenderingState.mBlending = 0;
        GRenderingState.mCulling = 0;
        GRenderingState.mZTestType = 1;
#endif
    }


}
