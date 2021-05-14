#include "FCFASTRenderer.h"
#include "FCFAST.h"
#include "FCFASTPlane.h"
#include "FZone.h"
#include "FEngine.h"
#include "Core/Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"

#include "Rendering/Resource/NEVertexBuffer.h"

#include "Mesh/NESpatialMesh.h"
#include "FZoneItem.h"
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLFunctions_3_3_Compatibility>



//
// Zone Item Renderer
//
void FCFASTRenderer::renderColorRamp(NESpatial *spatial, IGLSurface *surface)
{

}

void FCFASTRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    return;
    spatial->SetColor( QColor( 38, 77, 57 ) );



    //qDebug() << "FCFASTRenderer::render is called " << spatial->objectName();
    NERENDERMAN.GLManager()->setDepthTest(kDisableDepthTest);
    if(FCFAST *fCFAST = qobject_cast<FCFAST *>(spatial))
    {
        FEngine* engine = qobject_cast<FEngine *>(spatial->parent());
        if(engine)
        {
            FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(engine->activeZoneName()));
            if(zone)
            {
                double height = zone->layerHeight(NESCENE.ElapsedTime());
                double higherTemp = zone->higherLayerTemp(NESCENE.ElapsedTime());
                double lowerTemp = zone->lowerLayerTemp(NESCENE.ElapsedTime());

                //qDebug() << "time" <<NESCENE.ElapsedTime() <<"height" << height << "higherTemp" <<higherTemp << "lowerTemp" <<lowerTemp;

                float minVal = zone->minLayerTemp();
                float maxVal = zone->maxLayerTemp();

                float upperVal = (higherTemp - minVal)/(maxVal - minVal);
                float lowerVal = (lowerTemp - minVal)/(maxVal - minVal);

                QColor minColor = QColor(20, 20, 255, 255);
                QColor maxColor = QColor(255, 0, 0, 255);

                //qDebug() << "minVal" <<minVal << "maxVal" <<maxVal << "upperVal" <<upperVal << "lowerVal" <<lowerVal;

                if(fCFAST->Visualize())
                {
                    if(fCFAST->XVisPlane())
                    {
                        if(fCFAST->XVisPlanePos() > zone->compAABB().minPos().x() && fCFAST->XVisPlanePos() < zone->compAABB().maxPos().x())
                        {
                            surface->setShaderProgram(0);

                            NERENDERMAN.GLManager()->bindRectYFilterShader(surface);
                            NERENDERMAN.GLManager()->updateRectYFilterUniforms(surface, minColor, maxColor, upperVal, lowerVal, height);

                            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                            GL->glEnable(GL_BLEND);
                            GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                            //GL->glColor4f(1, 0, 0, 0.5);
                            NEVertexBuffer* planeVerts;
                            planeVerts = 0;
                            planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                            NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                            vertexPtr[0].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().minPos().z());
                            vertexPtr[1].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().maxPos().z());
                            vertexPtr[2].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().maxPos().z());
                            vertexPtr[3].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().minPos().z());

                            planeVerts->endLoadData();

                            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                            GL->glDisable(GL_BLEND);

                            GL->glPopAttrib();
                        }
                    }

                    if(fCFAST->YVisPlane())
                    {
                        if(fCFAST->YVisPlanePos() > zone->compAABB().minPos().y() && fCFAST->YVisPlanePos() < zone->compAABB().maxPos().y())
                        {
                            surface->setShaderProgram(0);

                            NERENDERMAN.GLManager()->bindRectYFilterShader(surface);
                            NERENDERMAN.GLManager()->updateRectYFilterUniforms(surface, minColor, maxColor, upperVal, lowerVal, height);

                            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                            GL->glEnable(GL_BLEND);
                            GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                            GL->glColor4f(0, 1, 0, 0.5);
                            NEVertexBuffer* planeVerts;
                            planeVerts = 0;
                            planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                            NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                            vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());
                            vertexPtr[1].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                            vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                            vertexPtr[3].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());

                            planeVerts->endLoadData();

                            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                            GL->glDisable(GL_BLEND);

                            GL->glPopAttrib();
                        }
                    }

                    if(fCFAST->ZVisPlane())
                    {
                        if(fCFAST->ZVisPlanePos() > zone->compAABB().minPos().z() && fCFAST->ZVisPlanePos() < zone->compAABB().maxPos().z())
                        {
                            surface->setShaderProgram(0);

                            NERENDERMAN.GLManager()->bindRectYFilterShader(surface);
                            NERENDERMAN.GLManager()->updateRectYFilterUniforms(surface, minColor, maxColor, upperVal, lowerVal, height);

                            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                            GL->glEnable(GL_BLEND);
                            GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                            GL->glColor4f(0, 0, 1, 0.5);
                            NEVertexBuffer* planeVerts;
                            planeVerts = 0;
                            planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                            NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                            vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                            vertexPtr[1].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                            vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());
                            vertexPtr[3].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());

                            planeVerts->endLoadData();

                            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                            GL->glDisable(GL_BLEND);

                            GL->glPopAttrib();
                        }
                    }

                    //render color Ramp
                    renderColorRamp(spatial, surface);
                }
            }
        }
    }
}

void FCFASTRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    return;
    if(FCFAST *fCFAST = qobject_cast<FCFAST *>(spatial))
    {
        FEngine* engine = qobject_cast<FEngine *>(spatial->parent());
        if(engine)
        {
            FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(engine->activeZoneName()));
            if(zone)
            {
                double height = zone->layerHeight(NESCENE.ElapsedTime());
                double higherTemp = zone->higherLayerTemp(NESCENE.ElapsedTime());
                double lowerTemp = zone->lowerLayerTemp(NESCENE.ElapsedTime());

                //qDebug() << "time" <<NESCENE.ElapsedTime() <<"height" << height << "higherTemp" <<higherTemp << "lowerTemp" <<lowerTemp;

                float minVal = zone->minLayerTemp();
                float maxVal = zone->maxLayerTemp();

                float upperVal = (higherTemp - minVal)/(maxVal - minVal);
                float lowerVal = (lowerTemp - minVal)/(maxVal - minVal);

                QColor minColor = QColor(20, 20, 255, 255);
                QColor maxColor = QColor(255, 0, 0, 255);

                //qDebug() << "minVal" <<minVal << "maxVal" <<maxVal << "upperVal" <<upperVal << "lowerVal" <<lowerVal;

                if(fCFAST->Visualize())
                {
                    if(fCFAST->XVisPlane())
                    {
                        if(fCFAST->XVisPlanePos() > zone->compAABB().minPos().x() && fCFAST->XVisPlanePos() < zone->compAABB().maxPos().x())
                        {

                            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                            //GL->glColor4f(1, 0, 0, 0.5);
                            NEVertexBuffer* planeVerts;
                            planeVerts = 0;
                            planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                            NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                            vertexPtr[0].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().minPos().z());
                            vertexPtr[1].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().maxPos().z());
                            vertexPtr[2].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().maxPos().z());
                            vertexPtr[3].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().minPos().z());

                            planeVerts->endLoadData();

                            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                            GL->glPopAttrib();
                        }
                    }

                    if(fCFAST->YVisPlane())
                    {
                        if(fCFAST->YVisPlanePos() > zone->compAABB().minPos().y() && fCFAST->YVisPlanePos() < zone->compAABB().maxPos().y())
                        {

                            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                            NEVertexBuffer* planeVerts;
                            planeVerts = 0;
                            planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                            NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                            vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());
                            vertexPtr[1].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                            vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                            vertexPtr[3].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());

                            planeVerts->endLoadData();

                            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                            GL->glPopAttrib();
                        }
                    }

                    if(fCFAST->ZVisPlane())
                    {
                        if(fCFAST->ZVisPlanePos() > zone->compAABB().minPos().z() && fCFAST->ZVisPlanePos() < zone->compAABB().maxPos().z())
                        {

                            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                            NEVertexBuffer* planeVerts;
                            planeVerts = 0;
                            planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                            NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                            vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                            vertexPtr[1].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                            vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());
                            vertexPtr[3].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());

                            planeVerts->endLoadData();

                            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);
                            GL->glPopAttrib();
                        }
                    }

                    //render color Ramp
                    renderColorRamp(spatial, surface);
                }
            }
        }
    }

}

void FCFASTRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
    //NESpatialRenderer::renderPicked(spatial, surface);
}

void FCFASTRenderer::updateDrawingBuffers()
{
    NESpatialMesh* rigid = qobject_cast<NESpatialMesh*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

FCFASTRendererFactory::FCFASTRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FCFASTRendererFactory::nodeName() { return "CFAST Renderer"; }

NEObject *FCFASTRendererFactory::createInstance() { return new FCFASTRenderer(); }

void FCFASTPlaneRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    spatial->SetColor( QColor( 38, 77, 57 ) );



    //qDebug() << "FCFASTRenderer::render is called " << spatial->objectName();
    NERENDERMAN.GLManager()->setDepthTest(kDisableDepthTest);
    if(FCFASTPlane *fCFASTPlane = qobject_cast<FCFASTPlane *>(spatial))
    {

        FCFAST *fCFAST = qobject_cast<FCFAST *>(spatial->parent());
        FEngine* engine = qobject_cast<FEngine *>(spatial->parent()->parent());
        if(engine)
        {
            FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(engine->activeZoneName()));
            if(zone)
            {
                double height = zone->layerHeight(NESCENE.ElapsedTime());
                double higherTemp = zone->higherLayerTemp(NESCENE.ElapsedTime());
                double lowerTemp = zone->lowerLayerTemp(NESCENE.ElapsedTime());

                //qDebug() << "time" <<NESCENE.ElapsedTime() <<"height" << height << "higherTemp" <<higherTemp << "lowerTemp" <<lowerTemp;

                float minVal = zone->minLayerTemp();
                float maxVal = zone->maxLayerTemp();

                float upperVal = (higherTemp - minVal)/(maxVal - minVal);
                float lowerVal = (lowerTemp - minVal)/(maxVal - minVal);

                QColor minColor = QColor(20, 20, 255, 255);
                QColor maxColor = QColor(255, 0, 0, 255);

                //qDebug() << "minVal" <<minVal << "maxVal" <<maxVal << "upperVal" <<upperVal << "lowerVal" <<lowerVal;

                if(fCFAST->Visualize())
                {
                    if( fCFASTPlane->PlaneDirection() == FCFASTPlane::X){
                        if(fCFAST->XVisPlane() )
                        {
                            if(fCFAST->XVisPlanePos() > zone->compAABB().minPos().x() && fCFAST->XVisPlanePos() < zone->compAABB().maxPos().x())
                            {
                                surface->setShaderProgram(0);

                                NERENDERMAN.GLManager()->bindRectYFilterShader(surface);
                                NERENDERMAN.GLManager()->updateRectYFilterUniforms(surface, minColor, maxColor, upperVal, lowerVal, height);

                                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                                GL->glEnable(GL_BLEND);
                                GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                                //GL->glColor4f(1, 0, 0, 0.5);
                                NEVertexBuffer* planeVerts;
                                planeVerts = 0;
                                planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                                NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                                vertexPtr[0].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().minPos().z());
                                vertexPtr[1].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().maxPos().z());
                                vertexPtr[2].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().maxPos().z());
                                vertexPtr[3].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().minPos().z());

                                planeVerts->endLoadData();

                                NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                                GL->glDisable(GL_BLEND);

                                GL->glPopAttrib();
                            }
                        }
                    }
                    else if (fCFASTPlane->PlaneDirection() == FCFASTPlane::Y) {
                        if(fCFAST->YVisPlane())
                        {
                            if(fCFAST->YVisPlanePos() > zone->compAABB().minPos().y() && fCFAST->YVisPlanePos() < zone->compAABB().maxPos().y())
                            {
                                surface->setShaderProgram(0);

                                NERENDERMAN.GLManager()->bindRectYFilterShader(surface);
                                NERENDERMAN.GLManager()->updateRectYFilterUniforms(surface, minColor, maxColor, upperVal, lowerVal, height);

                                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                                GL->glEnable(GL_BLEND);
                                GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                                GL->glColor4f(0, 1, 0, 0.5);
                                NEVertexBuffer* planeVerts;
                                planeVerts = 0;
                                planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                                NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                                vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());
                                vertexPtr[1].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                                vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                                vertexPtr[3].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());

                                planeVerts->endLoadData();

                                NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                                GL->glDisable(GL_BLEND);

                                GL->glPopAttrib();
                            }
                        }
                    }
                    else if (fCFASTPlane->PlaneDirection() == FCFASTPlane::Z){
                        if(fCFAST->ZVisPlane())
                        {
                            if(fCFAST->ZVisPlanePos() > zone->compAABB().minPos().z() && fCFAST->ZVisPlanePos() < zone->compAABB().maxPos().z())
                            {
                                surface->setShaderProgram(0);

                                NERENDERMAN.GLManager()->bindRectYFilterShader(surface);
                                NERENDERMAN.GLManager()->updateRectYFilterUniforms(surface, minColor, maxColor, upperVal, lowerVal, height);

                                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                                GL->glEnable(GL_BLEND);
                                GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                                GL->glColor4f(0, 0, 1, 0.5);
                                NEVertexBuffer* planeVerts;
                                planeVerts = 0;
                                planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                                NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                                vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                                vertexPtr[1].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                                vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());
                                vertexPtr[3].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());

                                planeVerts->endLoadData();

                                NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                                GL->glDisable(GL_BLEND);

                                GL->glPopAttrib();
                            }
                        }
                    }
                }
            }
        }
    }
}

void FCFASTPlaneRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    if(FCFASTPlane *fCFASTPlane = qobject_cast<FCFASTPlane *>(spatial))
    {
        FCFAST *fCFAST = qobject_cast<FCFAST* >(spatial->parent());
        FEngine* engine = qobject_cast<FEngine *>(spatial->parent()->parent());
        if(engine)
        {
            FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(engine->activeZoneName()));
            if(zone)
            {
                double height = zone->layerHeight(NESCENE.ElapsedTime());
                double higherTemp = zone->higherLayerTemp(NESCENE.ElapsedTime());
                double lowerTemp = zone->lowerLayerTemp(NESCENE.ElapsedTime());

                //qDebug() << "time" <<NESCENE.ElapsedTime() <<"height" << height << "higherTemp" <<higherTemp << "lowerTemp" <<lowerTemp;

                float minVal = zone->minLayerTemp();
                float maxVal = zone->maxLayerTemp();

                float upperVal = (higherTemp - minVal)/(maxVal - minVal);
                float lowerVal = (lowerTemp - minVal)/(maxVal - minVal);

                QColor minColor = QColor(20, 20, 255, 255);
                QColor maxColor = QColor(255, 0, 0, 255);

                //qDebug() << "minVal" <<minVal << "maxVal" <<maxVal << "upperVal" <<upperVal << "lowerVal" <<lowerVal;

                if(fCFAST->Visualize())
                {
                    if (fCFASTPlane->PlaneDirection() == FCFASTPlane::X){
                        if(fCFAST->XVisPlane())
                        {
                            if(fCFAST->XVisPlanePos() > zone->compAABB().minPos().x() && fCFAST->XVisPlanePos() < zone->compAABB().maxPos().x())
                            {

                                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                                //GL->glColor4f(1, 0, 0, 0.5);
                                NEVertexBuffer* planeVerts;
                                planeVerts = 0;
                                planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                                NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                                vertexPtr[0].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().minPos().z());
                                vertexPtr[1].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().minPos().y(), zone->compAABB().maxPos().z());
                                vertexPtr[2].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().maxPos().z());
                                vertexPtr[3].position = vec3f(fCFAST->XVisPlanePos(), zone->compAABB().maxPos().y(), zone->compAABB().minPos().z());

                                planeVerts->endLoadData();

                                NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                                GL->glPopAttrib();
                            }
                        }
                    }
                    else if(fCFASTPlane->PlaneDirection() == FCFASTPlane::Y){
                        if(fCFAST->YVisPlane())
                        {
                            if(fCFAST->YVisPlanePos() > zone->compAABB().minPos().y() && fCFAST->YVisPlanePos() < zone->compAABB().maxPos().y())
                            {

                                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                                NEVertexBuffer* planeVerts;
                                planeVerts = 0;
                                planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                                NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                                vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());
                                vertexPtr[1].position = vec3f(zone->compAABB().minPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                                vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().maxPos().z());
                                vertexPtr[3].position = vec3f(zone->compAABB().maxPos().x(), fCFAST->YVisPlanePos(), zone->compAABB().minPos().z());

                                planeVerts->endLoadData();

                                NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

                                GL->glPopAttrib();
                            }
                        }
                    }
                    else if(fCFASTPlane->PlaneDirection() == FCFASTPlane::Z){
                        if(fCFAST->ZVisPlane())
                        {
                            if(fCFAST->ZVisPlanePos() > zone->compAABB().minPos().z() && fCFAST->ZVisPlanePos() < zone->compAABB().maxPos().z())
                            {

                                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                                NEVertexBuffer* planeVerts;
                                planeVerts = 0;
                                planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
                                NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

                                vertexPtr[0].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                                vertexPtr[1].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().minPos().y(), fCFAST->ZVisPlanePos());
                                vertexPtr[2].position = vec3f(zone->compAABB().maxPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());
                                vertexPtr[3].position = vec3f(zone->compAABB().minPos().x(), zone->compAABB().maxPos().y(), fCFAST->ZVisPlanePos());

                                planeVerts->endLoadData();

                                NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);
                                GL->glPopAttrib();
                            }
                        }
                    }
                }
            }
        }
    }

}

void FCFASTPlaneRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, surface);
}

FCFASTPlaneRendererFactory::FCFASTPlaneRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FCFASTPlaneRendererFactory::nodeName() { return "CFASTPlane Renderer"; }

NEObject *FCFASTPlaneRendererFactory::createInstance() { return new FCFASTPlaneRenderer(); }




//
// OSD
//
void FOSDCFASTRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    if(FOSDCFAST *fOSDCFAST = qobject_cast<FOSDCFAST *>(spatial))
    {
        if(fOSDCFAST->RenderMode() == NESpatial::NERenderMode::Invisible)
            return;

        NESpatial* spatialParent = qobject_cast< NESpatial *>( spatial->parent() );


        FCFAST *fCFAST = nullptr;
        if( spatialParent )
            fCFAST = qobject_cast< FCFAST *>( spatialParent );
        else
            return;

        //
        // Create the color ramp node FOSDCFASTRenderer here...and pass the zone to that
        //

        if( fCFAST && fCFAST->Visualize() )
        {

            FEngine* engine = qobject_cast<FEngine *>(fCFAST->parent());
            if( engine )
            {
                FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(engine->activeZoneName()));
                if(zone)
                {
                    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

                    GL->glMatrixMode(GL_MODELVIEW);
                    GL->glLoadIdentity();

                    float minVal = zone->minLayerTemp();
                    float maxVal = zone->maxLayerTemp();

                    QColor minColor = QColor(20, 20, 255, 255);
                    QColor maxColor = QColor(255, 0, 0, 255);

                    NERENDERMAN.GLManager()->bindColorRampOSDShader(surface);
                    NERENDERMAN.GLManager()->updateColorRampOSDUniforms(surface, minColor, maxColor);

                    int width = surface->getWidth();
                    int height = surface->getHeight();

                    //Data for render OSD, it should be added on UI
                    //int tickCount = 4; //text count on colorRamp
                    int tickCount = fOSDCFAST->TickCount();
                    QString titleText = fOSDCFAST->TitleText();

                    int xRes = width * fOSDCFAST->Positionx();
                    int yRes = height * fOSDCFAST->Positiony();

                    int wRes = OSD_PARTICLE_COLORBAR_WIDTH * fOSDCFAST->Scalex();      //!< Width of color bar
                    int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * fOSDCFAST->Scaley();     //!< Height of color bar
                    int hTitle = fOSDCFAST->fontHeight() * 2;
                    int wMargin = OSD_PARTICLE_MEASURE_MARGIN * fOSDCFAST->Scalex();   //!< Width of margin between color bar and measure
                    unsigned int widthU = 0;   //!< Width of measurement text

                    for (int i = 0; i <= tickCount; i++)
                    {
                        float s = i / (float)tickCount;
                        float val = minVal * (1-s) + maxVal * s;
                        QString text;
                        if( fOSDCFAST->DisplayNotation() == NEOSDBase::Exponent )
                        {
                            text = QString("%1").arg( (double)val, 2, 'E', 2 );
                        }
                        else if( fOSDCFAST->DisplayNotation() == NEOSDBase::FloatingPoint )
                        {
                            text = QString("%1").arg( (double)val, fOSDCFAST->TotalNumberLength(), 'f', fOSDCFAST->FloatTypePrecision() );
                        }
                        widthU = MAX(widthU, (float)text.length() * fOSDCFAST->fontWidth() + fOSDCFAST->fontWidth());
                    }

                    vec2f OsdSize = fOSDCFAST->osdSize();

                    //calculate total OSD size incule colorRamp and text
                    FOSDCFAST::FOSDCFASTOrientation enumOrientation = fOSDCFAST->OSDOrientation();

                    if (enumOrientation == FOSDCFAST::FOSDCFASTOrientation::Top ||
                        enumOrientation == FOSDCFAST::FOSDCFASTOrientation::Bottom)
                    {
                        wRes = OSD_PARTICLE_COLORBAR_HEIGHT * fOSDCFAST->Scalex();
                        hRes = OSD_PARTICLE_COLORBAR_WIDTH * fOSDCFAST->Scaley();
                        wMargin = OSD_PARTICLE_MEASURE_MARGIN * fOSDCFAST->Scaley();
                    }

                    int xPos = 0, yPos = 0;
                    vec3f colorBarRect[4];
                    vec3f colorBarRect_2D[4];
                    vec2f widgetSize = vec2f(width, height);


                    switch(enumOrientation)
                    {
                    case FOSDCFAST::FOSDCFASTOrientation::Top:
                        colorBarRect[0] = vec3f(xRes - wRes, yRes + OsdSize.y() - hTitle, 0);
                        colorBarRect[1] = vec3f(xRes - wRes, yRes + OsdSize.y() - hTitle - hRes, 1);
                        colorBarRect[2] = vec3f(xRes, yRes + OsdSize.y() - hTitle - hRes, 2);
                        colorBarRect[3] = vec3f(xRes, yRes + OsdSize.y() - hTitle, 3);
                        break;
                    case FOSDCFAST::FOSDCFASTOrientation::Bottom:
                        colorBarRect[0] = vec3f(xRes - wRes, yRes + hRes, 0);
                        colorBarRect[1] = vec3f(xRes - wRes, yRes, 1);
                        colorBarRect[2] = vec3f(xRes, yRes, 2);
                        colorBarRect[3] = vec3f(xRes, yRes + hRes, 3);

                        break;
                    case FOSDCFAST::FOSDCFASTOrientation::Right:
                        colorBarRect[0] = vec3f(xRes - wRes, yRes, 0);
                        colorBarRect[1] = vec3f(xRes, yRes, 1);
                        colorBarRect[2] = vec3f(xRes, yRes + hRes, 2);
                        colorBarRect[3] = vec3f(xRes - wRes, yRes + hRes, 3);
                        break;
                    case FOSDCFAST::FOSDCFASTOrientation::Left:
                    default:
                        colorBarRect[0] = vec3f(xRes - OsdSize.x(), yRes, 0);
                        colorBarRect[1] = vec3f(xRes - OsdSize.x()+ wRes, yRes, 1);
                        colorBarRect[2] = vec3f(xRes - OsdSize.x()+ wRes, yRes + hRes, 2);
                        colorBarRect[3] = vec3f(xRes - OsdSize.x(), yRes + hRes, 3);

                        break;
                    }

                    for (int nIndex = 0; nIndex < 4; nIndex++)
                    {
                        colorBarRect_2D[nIndex] = vec3f(colorBarRect[nIndex].x(), colorBarRect[nIndex].y(), 0);
                        colorBarRect[nIndex].setX(colorBarRect[nIndex].x() / widgetSize.x());
                        colorBarRect[nIndex].setY(colorBarRect[nIndex].y() / widgetSize.y());
                    }

                    GL->glEnableClientState(GL_VERTEX_ARRAY);
                    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect);
                    GL->glDrawArrays(GL_TRIANGLE_FAN,0,4);
                    GL->glDisableClientState(GL_VERTEX_ARRAY);

                    // Render black colorbar border
                    NERENDERMAN.GLManager()->bindPositionShader(surface);
                    GL->glEnableClientState(GL_VERTEX_ARRAY);
                    GL->glColor4f(0,0,0,1);
                    NERENDERMAN.GLManager()->updatePositionUniforms(surface);
                    GL->glLineWidth(1.5f);
                    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect_2D);
                    GL->glDrawArrays(GL_LINE_LOOP,0,4);
                    GL->glLineWidth(1.0f);
                    GL->glDisableClientState(GL_VERTEX_ARRAY);


                    // Draw Title Text
                    surface->setShaderProgram(0);
                    GL->glDisable(GL_BLEND);
                    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );
                    float titleTextWidth = titleText.length() * fOSDCFAST->fontWidth();

                    glPrint(titleText,
                            xRes - OsdSize.x() / 2 - titleTextWidth / 2,
                            yRes + OsdSize.y() - fOSDCFAST->fontHeight(),
                            fOSDCFAST->fontColor(),
                            fOSDCFAST->getTextureFont(), fOSDCFAST->OutlineColor(), fOSDCFAST->getTextureOutlineFont());

                    // Draw measurement values (1..step)
                    for (int i = 0; i <= tickCount; i++)
                    {
                        float s = i / (float)tickCount;
                        float val = minVal * (1-s) + maxVal * s;
                        QString text;
                        if( fOSDCFAST->DisplayNotation() == NEOSDBase::Exponent )
                        {
                            text = QString("%1").arg( (double)val, 2, 'E', 2 );
                        }
                        else if( fOSDCFAST->DisplayNotation() == NEOSDBase::FloatingPoint )
                        {
                            text = QString("%1").arg( (double)val, fOSDCFAST->TotalNumberLength(), 'f', fOSDCFAST->FloatTypePrecision() );
                        }

                        switch(enumOrientation)
                        {
                        case FOSDCFAST::FOSDCFASTOrientation::Top:
                            xPos = xRes - OsdSize.x() + s * wRes - (widthU/2);
                            yPos = yRes ;
                            break;
                        case FOSDCFAST::FOSDCFASTOrientation::Bottom:
                            xPos = xRes - OsdSize.x() + s * wRes - (widthU/2);
                            yPos = yRes + OsdSize.y() - hTitle - (int)(fOSDCFAST->fontHeight());
                            break;
                        case FOSDCFAST::FOSDCFASTOrientation::Right:
                            xPos = xRes - OsdSize.x();
                            yPos = yRes + s * hRes;
                            break;
                        case FOSDCFAST::FOSDCFASTOrientation::Left:
                        default:
                            xPos = xRes - widthU;
                            yPos = yRes + s * hRes;
                            break;
                        }

                        glPrint(text, xPos, yPos,
                                fOSDCFAST->fontColor(),
                                fOSDCFAST->getTextureFont(), fOSDCFAST->OutlineColor(), fOSDCFAST->getTextureOutlineFont());
                    }

                    GL->glPopAttrib();

                }
            }
        }
    }
}


void FOSDCFASTRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    FOSDCFAST* osdBase = qobject_cast<FOSDCFAST*>(spatial);
    if (osdBase == NULL) return;
    if(osdBase->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    NERENDERMAN.GLManager()->bindPositionShader(surface);
    NERENDERMAN.GLManager()->updateCPUniforms(surface);

    int wRes = osdBase->osdSize().x();      //!< Width of color bar
    int hRes = osdBase->osdSize().y();     //!< Height of color bar

    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdBase->Positionx();
    int yRes = height * osdBase->Positiony();


    int left = xRes - wRes;
    int right = xRes;
    int top = yRes;
    int bottom = yRes + hRes;

    drawOSDRect(left, top, right, bottom, surface);
    //render( spatial, surface );

    GL->glPopAttrib();

}



void FOSDCFASTRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDBase* osdBase = qobject_cast<NEOSDBase*>(spatial);
    if (osdBase == NULL) return;
    if(osdBase->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    NERENDERMAN.GLManager()->bindPositionShader(surface);
    NERENDERMAN.GLManager()->updateCPUniforms(surface);

    static Color32 OsdLine(0, 255, 0, 255);

    NERENDERMAN.GLManager()->setFillMode(kWireframeFrontAndBack);
    GL->glColor4ubv((GLubyte*)&OsdLine);

    int wRes = osdBase->osdSize().x();      //!< Width of color bar
    int hRes = osdBase->osdSize().y();     //!< Height of color bar

    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdBase->Positionx();
    int yRes = height * osdBase->Positiony();


    int left = xRes - wRes;
    int right = xRes;
    int top = yRes;
    int bottom = yRes + hRes;

    drawOSDRect(left, top, right, bottom, surface);
    //render( spatial, surface );

    GL->glPopAttrib();


}


FOSDCFASTRendererFactory::FOSDCFASTRendererFactory(NEManager* m) : NERendererFactory( m )
{
    NERenderManager*rm = qobject_cast< NERenderManager* >(m);
    if( rm )
    {
        rm->addRendererToMaps( this );
    }
}

QString FOSDCFASTRendererFactory::nodeName() { return "CFASTOSD Renderer"; }
NEObject*FOSDCFASTRendererFactory::createInstance() { return new FOSDCFASTRenderer(); }


