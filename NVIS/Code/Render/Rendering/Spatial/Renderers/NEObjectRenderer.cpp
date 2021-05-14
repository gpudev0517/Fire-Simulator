#include "NEObjectRenderer.h"

#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Base/NESceneManager.h"


#include "Spatial/NESpatialGroup.h"
#include "Rendering/Spatial/NEClipPlane.h"
#include "Rendering/Spatial/NEFloorPlane.h"
#include "Rendering/Spatial/NENullGizmo.h"
#include "Rendering/Spatial/NEMeasurementItem.h"
#include "Rendering/Spatial/NEInteractiveMeasurementGizmo.h"
#include "Rendering/Spatial/NEOSDGlobalInformation.h"
#include "Rendering/Resource/NEVertexBuffer.h"


//
// ClipPlane
//
void NEClipPlaneRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEClipPlane* clipPlane = qobject_cast<NEClipPlane*>(spatial);

    if(clipPlane->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    AABB aabb = NESCENE.getSceneAABB();
    vec3f diag = (aabb.maxPos()-aabb.minPos());
    clipPlane->SetScaleClip(2*diag);

    // Clip Plane Color
    GL->glColor4f(0.5f, 0.5f, 0.5f, 0.3f);

    if(NERENDERMAN.GLManager()->isBuffered( &clipPlane->mesh()) )
        NERENDERMAN.GLManager()->drawRaw(&clipPlane->mesh(), surface, false, clipPlane->DisplayMode() == NESpatial::BoundingBox);
}

void NEClipPlaneRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEClipPlane* clipPlane = qobject_cast<NEClipPlane*>(spatial);
    if(!clipPlane || clipPlane->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    NERENDERMAN.GLManager()->draw(&clipPlane->mesh(), surface, clipPlane->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
}

void NEClipPlaneRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    renderPickable(spatial, surface);
}


NEClipPlaneRendererFactory::NEClipPlaneRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEClipPlaneRendererFactory::nodeName() { return "Clip Plane Renderer"; }
NEObject *NEClipPlaneRendererFactory::createInstance() { return new NEClipPlaneRenderer(); }


// FloorPlane
//
void NEFloorPlaneRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEFloorPlane* floorPlane = qobject_cast<NEFloorPlane*>(spatial);
//    if(floorPlane->isShow)
//        floorPlane->createMesh();
//    else
//    {
//        floorPlane->triangleMesh().vertices().clear();
//        floorPlane->triangleMesh().triangles().clear();
//    }

    if(NERENDERMAN.GLManager()->isBuffered(&floorPlane->triangleMesh()) && floorPlane->ShowFloorPlan())
    {
        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

        NERENDERMAN.GLManager()->bindFloorDrawingShader(surface);
        NERENDERMAN.GLManager()->updateFloorDrawingUniforms(surface,
                                                            floorPlane->XMin() / floorPlane->XSize(),
                                                            floorPlane->XMax() / floorPlane->XSize(),
                                                            floorPlane->ZMin() / floorPlane->ZSize(),
                                                            floorPlane->ZMax() / floorPlane->ZSize());
        GL->glEnable(GL_TEXTURE_2D);
        if( floorPlane->FloorPlaneEffect() == 0)
        {
            // transparency
            GL->glEnable(GL_BLEND);
            GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            GL->glDisable(GL_BLEND);
        }

        GL->glEnable(GL_POLYGON_OFFSET_FILL);
        GL->glPolygonOffset(-1, 1);

        float r = floorPlane->Color().redF();
        float g = floorPlane->Color().greenF();
        float b = floorPlane->Color().blueF();
        float a = floorPlane->FloorPlanTransparency() ;

        GL->glUniform4f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), r, g, b, a );

        QOpenGLTexture* texture = NULL;

        if( !floorPlane->TextureImage().isNull())
        {
            if( floorPlane->Flip() )
                texture = new QOpenGLTexture(floorPlane->TextureImage().mirrored());
            else
                texture = new QOpenGLTexture(floorPlane->TextureImage());
            texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            texture->setMagnificationFilter(QOpenGLTexture::Linear);
            GLuint texID = texture->textureId();
            GL->glActiveTexture(GL_TEXTURE0);
            GL->glBindTexture(GL_TEXTURE_2D, texID);
            GL->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "useTexture" ), 1 );
            GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_texture" ), 0 );
        }
        else
        {
            GL->glActiveTexture(GL_TEXTURE0);
            GL->glBindTexture(GL_TEXTURE_2D, 0);

            GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "useTexture" ), 0 );

        }


        GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelMatrix"), 1,
                                GL_FALSE,  (GLfloat *)(&floorPlane->transform().data()[0]));
        surface->shaderProgram()->setUniformValue("isReceiveShadow", floorPlane->ReceiveShadows());
        NERENDERMAN.GLManager()->setLightsForShader(surface, surface->shaderProgram());

        NERENDERMAN.GLManager()->draw(kTriangleListPrim, NERENDERMAN.GLManager()->vertexBuffer( &floorPlane->triangleMesh()), NERENDERMAN.GLManager()->indexBuffer(&floorPlane->triangleMesh()));

        if( texture)
            delete(texture);
        GL->glPopAttrib();
    }

}

void NEFloorPlaneRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEFloorPlane* floorPlane = qobject_cast<NEFloorPlane*>(spatial);
    if(!floorPlane || floorPlane->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    NERENDERMAN.GLManager()->draw(&floorPlane->triangleMesh(), surface,
                                  floorPlane->DisplayMode() == NESpatial::BoundingBox,
                                  true, true, false, matrix44f());
}

void NEFloorPlaneRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    renderPickable(spatial, surface);
}

void NEFloorPlaneRenderer::renderShadow(NESpatial* spatial, IGLSurface* surface)
{
    NEFloorPlane* floor = qobject_cast<NEFloorPlane*>(spatial);
    if (floor == NULL) return;

    if( floor->RenderMode() == NESpatial::NERenderMode::Invisible || floor->RenderMode() == NESpatial::NERenderMode::Wireframe || floor->RenderMode() == NESpatial::NERenderMode::QuadWireframe || floor->RenderMode() == NESpatial::NERenderMode::QuadWireframeHidden  || floor->RenderMode() == NESpatial::NERenderMode::Hidden  )
        return;
    if (!floor->OccludeShadows())
        return;
    NERENDERMAN.GLManager()->drawShadowMap(surface, &floor->triangleMesh());
}


NEFloorPlaneRendererFactory::NEFloorPlaneRendererFactory(NEManager* m) : NERendererFactory( m )
{
}

QString NEFloorPlaneRendererFactory::nodeName() { return "Floor Plane Renderer"; }
NEObject *NEFloorPlaneRendererFactory::createInstance() { return new NEFloorPlaneRenderer(); }


// NULL Gizmo
void NENullGizmoRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NENullGizmo* gizmo = qobject_cast<NENullGizmo*>(spatial);

    if(!gizmo || gizmo->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glDisable(GL_DEPTH_TEST);
    GL->glLineWidth( 1.0f );

    GL->glBegin( GL_LINES );
    GL->glVertex3f( -1, 0, 0 );
    GL->glVertex3f( 1, 0, 0 );

    GL->glVertex3f( 0, -1, 0 );
    GL->glVertex3f( 0, 1, 0 );

    GL->glVertex3f( 0, 0, -1 );
    GL->glVertex3f( 0, 0, 1 );
    GL->glEnd();

    GL->glPopAttrib();
}

void NENullGizmoRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

void NENullGizmoRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glEnable(GL_LINE_STIPPLE);
    GL->glLineStipple(4, 0xaaaa);

    render(spatial, surface);

    GL->glPopAttrib();
}

NENullGizmoRendererFactory::NENullGizmoRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NENullGizmoRendererFactory::nodeName() { return "Null Gizmo Renderer"; }
NEObject *NENullGizmoRendererFactory::createInstance() { return new NENullGizmoRenderer(); }

// Measurement Item
void NEMeasurementItemRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEMeasurementItem* measure = qobject_cast<NEMeasurementItem*>(spatial);

    if(!measure || measure->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;
    if(measure->CameraType() != surface->camera()->OrthoType())
        return;

    matrix44f mvMatrix = surface->modelViewMatrix();

    //

    vec3f originFirstPt;
    vec3f originSecondPt;

    if (measure->isMeasurementEdge() )
    {
        originFirstPt = measure->measureEdgeInfo.at(0).first->transform() * measure->measureEdgeInfo.at(0).second.second;
        originSecondPt = measure->measureEdgeInfo.at(1).first->transform() * measure->measureEdgeInfo.at(1).second.second;
    }
    else
    {
        originFirstPt = measure->measureInfoOrigin.at(0).first->transform() * measure->measureInfoOrigin.at(0).first->triangleMesh().vertices()[measure->measureInfoOrigin.at(0).second].position;
        originSecondPt = measure->measureInfoOrigin.at(1).first->transform() * measure->measureInfoOrigin.at(1).first->triangleMesh().vertices()[measure->measureInfoOrigin.at(1).second].position;
    }

    vec3f movedFirstPt = originFirstPt + measure->measureInfoMoved.second *  measure->measureInfoMoved.first;
    vec3f movedSecondPt = originSecondPt +  measure->measureInfoMoved.second *  measure->measureInfoMoved.first;
    vec3f originNorm = (movedFirstPt - originFirstPt).normalized();
    vec3f movedNorm = (movedSecondPt - movedFirstPt).normalized();

    float len = 0;
    vec3f lengthVec ;
    if( measure->isMeasurementEdge() )
    {
        NESpatialMesh* selObj1 = measure->measureEdgeInfo.at(0).first;
        NESpatialMesh* selObj2 = measure->measureEdgeInfo.at(1).first;
        int vertID1 = measure->measureEdgeInfo.at(0).second.first.first;
        int vertID2 = measure->measureEdgeInfo.at(0).second.first.second;
        int vertID3 = measure->measureEdgeInfo.at(1).second.first.first;
        int vertID4 = measure->measureEdgeInfo.at(1).second.first.second;

        vec3f p1 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID1].position;
        vec3f p2 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID2].position;
        vec3f p3 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID3].position;
        vec3f p4 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID4].position;

        vec3f dir1 = (p2-p1).normalized();
        vec3f dir2 = (p4-p3).normalized();
        vec3f dir = vec3f::crossProduct(dir1, dir2);
        if(dir.length() < 1e-3)
        {
            lengthVec = ((p3-p1)-(vec3f::dotProduct(p3-p1, p2-p1)/vec3f::dotProduct(p2-p1, p2-p1))*(p2-p1));
            len = lengthVec.length();
        }
        else
        {
            lengthVec  = vec3f(0,0,0);
            len = vec3f::dotProduct(p3-p1, dir.normalized());
        }
        len = abs(len);
    }
    else
    {
        lengthVec = movedSecondPt - movedFirstPt;
        len = lengthVec.length();
    }

    switch (surface->camera()->OrthoType())
    {
    case NECamera::OrthographicType::Top:
        len = vec3f(lengthVec.x(), 0, lengthVec.z()).length();
        break;
    case NECamera::OrthographicType::Left:
        len = vec3f(0, lengthVec.y(), lengthVec.z()).length();
        break;
    case NECamera::OrthographicType::Front:
        len = vec3f(lengthVec.x(), lengthVec.y(), 0).length();
        break;
    }

    //float len = (movedSecondPt - movedFirstPt).length();

    FTPolygonFont* pFont = new FTPolygonFont("System/Fonts/tahoma.ttf");
    pFont->FaceSize(1);
    FTBBox box = pFont->BBox(QString::number(len).toUtf8().data());
    vec3f upper = vec3f(box.Upper().X(), box.Upper().Y(), box.Upper().Z());
    vec3f lower = vec3f(box.Lower().X(), box.Lower().Y(), box.Lower().Z());
    vec3f letterLen = upper - lower;
    delete(pFont);

    float size = 0.5;
    vec3f offset = surface->viewMatrix().inverted().mapVector((upper+lower) / 2);
    vec3f pos = (movedSecondPt + movedFirstPt) / 2 - offset * size;

    vec3f scale(1.0, 1.0, 1.0);
    eulerf angle = surface->calculateDrawAngle(NULL);

    lower -= vec3f(0.1, 0.1, 0);
    upper += vec3f(0.1, 0.1, 0);
    vec3f clip0 = lower;
    vec3f clip2 = upper;
    vec3f clip1(clip0.x(), clip2.y(), 0);
    vec3f clip3(clip2.x(), clip0.y(), 0);
    vec3f clip[4] = {clip0, clip1, clip2, clip3};
    matrix44f camMat = surface->viewMatrix().inverted();
    vec3f org = (movedSecondPt + movedFirstPt) / 2;
    vec3f center = (lower + upper) / 2;

    vec3f v1 = surface->viewMatrix() * movedFirstPt;
    vec3f v2 = surface->viewMatrix() * movedSecondPt;
    vec3f dir = v2 - v1;
    vec3f dirnorm = dir.normalized();
    int intercnt = 0;
    vec3f interpoints[4];

    for (int n = 0; n < 4; n++)
    {
        clip[n] = camMat.mapVector(clip[n] - center) + org;
        clip[n] = surface->viewMatrix() * clip[n];
    }

    for (int n = 0; n < 4; n++)
    {
        int m = (n + 1) % 4;

        vec3f norm = vec3f::crossProduct(clip[n], clip[m]);
        if (fabs(vec3f::dotProduct(norm, dir)) > 1e-3) {
            vec3f interpt = linePlaneInterPoint(v1, dir, vec3f(), norm);
            if ((vec3f::dotProduct(norm, vec3f::crossProduct(clip[n], interpt)) > 0) &&
                (vec3f::dotProduct(norm, vec3f::crossProduct(interpt, clip[m])) > 0))
            {
                interpoints[intercnt++] = interpt;
            }
        }
    }


    //    movedFirstPt += originNorm * letterLen.y() / 2;
    //    movedSecondPt += originNorm * letterLen.y() / 2;
    vec3f movedHalfFirstPt = (movedFirstPt + movedSecondPt) / 2 - movedNorm * 0.1 - movedNorm * abs(letterLen.x()) / 2 ;
    vec3f movedHalfSecondPt = (movedFirstPt + movedSecondPt) / 2 + movedNorm * 0.1 + movedNorm * abs(letterLen.x()) / 2;


    if (intercnt == 2)
    {
        float t0, t1;
        t0 = vec3f::dotProduct(interpoints[0] - v1, dirnorm) / len;
        t1 = vec3f::dotProduct(interpoints[1] - v1, dirnorm) / len;
        float tx, tn;
        tn = std::max(std::min(t0, t1), 0.0f);
        tx = std::min(std::max(t0, t1), 1.0f);

        movedHalfFirstPt = movedFirstPt + movedNorm * len * tn;
        movedHalfSecondPt = movedFirstPt + movedNorm * len * tx;
    }
    else
    {
        intercnt = 0;

        movedHalfFirstPt = movedFirstPt;
        movedHalfSecondPt = movedSecondPt;
    }

    vec3f arrowPos1 = movedFirstPt + movedNorm * 0.2 - originNorm * 0.15;
    vec3f arrowPos2 = movedFirstPt + movedNorm * 0.2 + originNorm * 0.15;
    vec3f arrowPos3 = movedSecondPt - movedNorm * 0.2 - originNorm * 0.15;
    vec3f arrowPos4 = movedSecondPt - movedNorm * 0.2 + originNorm * 0.15;

    if (movedFirstPt == movedHalfFirstPt)
    {
        arrowPos1 = movedFirstPt;
        arrowPos2 = movedFirstPt;
    }
    if (movedSecondPt == movedHalfSecondPt)
    {
        arrowPos3 = movedSecondPt;
        arrowPos4 = movedSecondPt;
    }

    // draw text
    float fTextSize = measure->TextSize();
    if( measure->TextScaleByLength() )
    {
        vec3f offset = measure->Offset();
        vec3f p2 = movedFirstPt + offset;
        vec3f p3 = movedSecondPt + offset;

        vec3f vA = p3-p2;
        float fLen = vA.length();
        fTextSize += fLen * 0.01;
    }
    scale *= fTextSize;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glDisable(GL_DEPTH_TEST);
    GL->glLineWidth( 1.0f );
//    if (!m_bPickable)
//        GL->glColor3f(0.24f, 0.24f, 0.24f);
//    else
//        GL->glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

    NEVertexBuffer* lineVerts;
    lineVerts = 0;
    lineVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 16, 0, GL_STATIC_DRAW);
    NEPVertex* vertexPtr = (NEPVertex*) lineVerts->beginLoadData();

    vertexPtr[0].position = movedFirstPt;
    vertexPtr[1].position = movedHalfFirstPt;
    vertexPtr[2].position = movedHalfSecondPt;
    vertexPtr[3].position = movedSecondPt;
    vertexPtr[4].position = movedFirstPt;
    vertexPtr[5].position = originFirstPt;
    vertexPtr[6].position = movedSecondPt;
    vertexPtr[7].position = originSecondPt;
    //draw Arrow
    vertexPtr[8].position = movedFirstPt;
    vertexPtr[9].position = arrowPos1;
    vertexPtr[10].position = movedFirstPt;
    vertexPtr[11].position = arrowPos2;
    vertexPtr[12].position = movedSecondPt;
    vertexPtr[13].position = arrowPos3;
    vertexPtr[14].position = movedSecondPt;
    vertexPtr[15].position = arrowPos4;

    lineVerts->endLoadData();
    NERENDERMAN.GLManager()->draw( kLineListPrim, lineVerts );

    surface->setShaderProgram(0);
    if( !m_bPickable )
        //surface->drawText3D(QString::number(len), pos, angle, size, scale,Color32(60,60,60,255));
        surface->drawText3D(QString::number(len), pos, angle, size, scale,
                            Color32(measure->Color().red(),measure->Color().green(),
                                    measure->Color().blue(),measure->Color().alpha()));
    else
        surface->drawText3D(QString::number(len), pos, angle, size, scale,
                            Color32(0,0,255,255));
    NERENDERMAN.GLManager()->bindPositionShader(surface);
    GL->glPopAttrib();
    m_bPickable = false;

    surface->setModelViewMatrix(mvMatrix);
}

void NEMeasurementItemRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    render(spatial, surface);
    GL->glPopAttrib();
}

void NEMeasurementItemRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    m_bPickable = true;
    render(spatial, surface);
    GL->glPopAttrib();
}


NEMeasurementItemRendererFactory::NEMeasurementItemRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEMeasurementItemRendererFactory::nodeName() { return "Measurement Item Renderer"; }
NEObject *NEMeasurementItemRendererFactory::createInstance() { return new NEMeasurementItemRenderer(); }

// Interactive Measurement Gizmo
void NEInteractiveMeasurementGizmoRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEInteractiveMeasurementGizmo* gizmo = qobject_cast<NEInteractiveMeasurementGizmo*>(spatial);

    if(!gizmo || gizmo->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    QMap<uint,NENode::NEConn>& srcConns = gizmo->sourceConnections();
    if(srcConns.size() < 2)
        return;

    NESpatial* pSrc = qobject_cast< NESpatial *>(srcConns[0].source);
    vec3f p0 = pSrc->Position();
    if(NESpatialGroup* spatialGroup = qobject_cast<NESpatialGroup*>(pSrc->parent())){
        p0 = spatialGroup->transform() * p0;
    }

    pSrc = qobject_cast< NESpatial *>(srcConns[1].source);
    vec3f p1 = pSrc->Position();
    if(NESpatialGroup* spatialGroup = qobject_cast<NESpatialGroup*>(pSrc->parent())){
        p1 = spatialGroup->transform() * p1;
    }

    vec3f offset = gizmo->Offset();
    vec3f p2 = p0 + offset;
    vec3f p3 = p1 + offset;

    vec3f vA = p3-p2;
    float fLen = vA.length();
    vA.normalize();
    vec3f vN = vec3f(-vA.y(), vA.x(), 0);
    if(vA.x() == 0 && vA.y() == 0)
        vN = vec3f(0, -vA.z(), vA.y());
    vN.normalize();

    float fArrowSize = 0.2;
    vec3f p4 = p2 + vA * fArrowSize;
    vec3f p5 = p3 - vA * fArrowSize;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    surface->setShaderProgram(0);

    GL->glDisable(GL_DEPTH_TEST);
    GL->glLineWidth( 1.0f );

//    // draw arrow as cone
//    int division = 32;
//    glBegin( GL_LINES );
//    for(int i = 0; i < division; i++) {
//        float angle = float(i) * 360.f / (float)division;
//        matrix33f mat;
//        mat.rotate(angle, vA);
//        vec3f p = p4 + (mat * vN) * fArrowSize / 2;
//        glVertex3f(p2.x(), p2.y(), p2.z());
//        glVertex3f(p.x(), p.y(), p.z());

//        glVertex3f(p.x(), p.y(), p.z());
//        angle = float(i+1) * 360.f / (float)division;
//        mat.setToIdentity();
//        mat.rotate(angle, vA);
//        p = p4 + (mat * vN) * fArrowSize / 2;
//        glVertex3f(p.x(), p.y(), p.z());
//    }
//    glEnd();

//    glBegin( GL_LINES );
//    for(int i = 0; i < division; i++) {
//        float angle = float(i) * 360.f / (float)division;
//        matrix33f mat;
//        mat.rotate(angle, vA);
//        vec3f p = p5 + (mat * vN) * fArrowSize / 2;
//        glVertex3f(p3.x(), p3.y(), p3.z());
//        glVertex3f(p.x(), p.y(), p.z());

//        glVertex3f(p.x(), p.y(), p.z());
//        angle = float(i+1) * 360.f / (float)division;
//        mat.setToIdentity();
//        mat.rotate(angle, vA);
//        p = p5 + (mat * vN) * fArrowSize / 2;
//        glVertex3f(p.x(), p.y(), p.z());
//    }
//    glEnd();

    // draw main line
    GL->glBegin( GL_LINES );
    GL->glVertex3f(p2.x(), p2.y(), p2.z());
    GL->glVertex3f(p3.x(), p3.y(), p3.z());
    GL->glEnd();

    // draw leading line
    GL->glEnable(GL_LINE_STIPPLE);
    GL->glLineStipple(4, 0xaaaa);
    GL->glBegin( GL_LINES );
    GL->glVertex3f(p0.x(), p0.y(), p0.z());
    GL->glVertex3f(p2.x(), p2.y(), p2.z());

    GL->glVertex3f(p1.x(), p1.y(), p1.z());
    GL->glVertex3f(p3.x(), p3.y(), p3.z());
    GL->glEnd();

    GL->glDisable(GL_LINE_STIPPLE);

    // draw text
    float fTextSize = gizmo->TextSize();
    if(gizmo->TextScaleByLength())
    {
        fTextSize += fLen * 0.01;
    }
    QString lenStr;
    lenStr.setNum(fLen, 'f', 4);

    float yaw = -atan2(vA.z(), vA.x());
    float roll = atan2(vA.y(), sqrt((vA.x() * vA.x()) + (vA.z() * vA.z())));
    eulerf eAngle = eulerf(0, RADIAN_TO_DEGREE(yaw), RADIAN_TO_DEGREE(roll));
    eulerf eOrient = gizmo->Orientation();
    eAngle = eAngle + eOrient;

    vec3f pM = (p2 + p3) / 2;
    pM -= vA * (fTextSize * lenStr.length() / 4);

    matrix44f letterMatrix;
    letterMatrix.translate(pM.x(),pM.y(),pM.z());

    quatf quatX, quatY, quatZ, quatXYZ;
    quatX = quatX.fromAxisAndAngle( 1,0,0, eAngle.phi() );
    quatY = quatY.fromAxisAndAngle( 0,1,0, eAngle.theta() );
    quatZ = quatZ.fromAxisAndAngle( 0,0,1, eAngle.psi() );
    quatXYZ = quatY*quatX*quatZ;
    letterMatrix.rotate( quatXYZ );

    letterMatrix.scale(fTextSize);
    GL->glMatrixMode(GL_MODELVIEW);
    GL->glPushMatrix();
    GL->glMultMatrixf(letterMatrix.data());
    NERENDERMAN.GLManager()->glPrint3D(lenStr.toUtf8(), -1);
    GL->glPopMatrix();

    GL->glPopAttrib();
}

void NEInteractiveMeasurementGizmoRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

void NEInteractiveMeasurementGizmoRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    surface->setShaderProgram(0);

    GL->glEnable(GL_LINE_STIPPLE);
    GL->glLineStipple(4, 0xaaaa);

    render(spatial, surface);

    GL->glPopAttrib();
}

NEInteractiveMeasurementGizmoRendererFactory::NEInteractiveMeasurementGizmoRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEInteractiveMeasurementGizmoRendererFactory::nodeName() { return "Interactive Measurement Gizmo Renderer"; }
NEObject *NEInteractiveMeasurementGizmoRendererFactory::createInstance() { return new NEInteractiveMeasurementGizmoRenderer(); }



//
// OSDGlobalInformation
//
void NEOSDGlobalInformationRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDGlobalInformation* osdGlobalInformation = qobject_cast<NEOSDGlobalInformation*>(spatial);
    if (osdGlobalInformation == NULL) return;
    if(osdGlobalInformation->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    surface->setShaderProgram(0);
    //glDisable(GL_BLEND);
    //NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    QStringList osdMessage;
    osdMessage << osdGlobalInformation->OsdText();

    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdGlobalInformation->Positionx();
    int yRes = height * osdGlobalInformation->Positiony();

    int posyU = yRes;

    for (int i = osdMessage.size() - 1; i >= 0; i--)
    {
        QString text = osdMessage[i];
        glPrint(text, xRes - osdGlobalInformation->osdSize().x() / 2, posyU - osdGlobalInformation->osdSize().y() / 2,
                osdGlobalInformation->fontColor(),
                osdGlobalInformation->getTextureFont(), osdGlobalInformation->OutlineColor(), osdGlobalInformation->getTextureOutlineFont());
        posyU += osdGlobalInformation->fontHeight();
    }

    GL->glPopAttrib();
}





NEOSDGlobalInformationRendererFactory::NEOSDGlobalInformationRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEOSDGlobalInformationRendererFactory::nodeName() { return "Global OSD Renderer"; }
NEObject*NEOSDGlobalInformationRendererFactory::createInstance() { return new NEOSDGlobalInformationRenderer(); }

