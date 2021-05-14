#include "NEParticleRigidRenderer.h"

#include "Base/NEBase.h"

#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "Rendering/Utility/NEOsd.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"

#include "Physics/Spatial/Physical/Rigid/NEParticleRigid.h"
#include "Rendering/Spatial/NEOSDParticleRigid.h"

// Particle rigid texture use the latest texture id that is empty.
// Default Material (PBR in max) : MAX_MATERIAL_TEXTURES
// One per Light : 8 in max
// Next one is MAX_MATERIAL_TEXTURES + 8 + 1
#define PARTICLE_RIGID_TEXTURE_ID (MAX_MATERIAL_TEXTURES + 8 + 1)

//
// Particle rigid texture use the latest texture id that is empty.
// Default Material (PBR in max) : MAX_MATERIAL_TEXTURES
// One per Light : 8 in max
// Next one is MAX_MATERIAL_TEXTURES + 8 + 1
#define PARTICLE_RIGID_TEXTURE_ID (MAX_MATERIAL_TEXTURES + 8 + 1)

//
// Particle Rigid
//

NEParticleRigidRenderer::NEParticleRigidRenderer()
{
    m_ParticleVerts = 0;
    if (NEBASE.guiMode())
    {
        m_ParticleVerts = NERENDERMAN.GLManager()->createVertexBuffer(kCPFormat);
    }

    m_ParticlePBOArrayIndex = -1;
    m_ParticleBuffIndex = 0;
    m_ParticleTexData=new float[64*128];

    GL->glGenTextures(1, &m_ParticlePCMapTexture);
    GL->glBindTexture(GL_TEXTURE_2D, m_ParticlePCMapTexture);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 128, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    GL->glBindTexture(GL_TEXTURE_2D, 0);

    GL->glGenBuffers(2, m_ParticlePBOIDs);
    GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_ParticlePBOIDs[0]);
    GL->glBufferData(GL_PIXEL_UNPACK_BUFFER, 64 * 128 * sizeof(float), 0, GL_STREAM_DRAW_ARB);
    GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_ParticlePBOIDs[1]);
    GL->glBufferData(GL_PIXEL_UNPACK_BUFFER, 64 * 128 * sizeof(float), 0, GL_STREAM_DRAW_ARB);

    GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

NEParticleRigidRenderer::~NEParticleRigidRenderer()
{
    if (m_ParticleVerts != 0)
        NERENDERMAN.GLManager()->destroy(m_ParticleVerts);

    if (m_ParticlePCMapTexture != 0)
    {
//        delete [] particlePCBuffer;
        GL->glDeleteTextures(1,&m_ParticlePCMapTexture);
        m_ParticlePCMapTexture = 0;
    }

    for (int i = 0; i < 2; i++)
    {
        GL->glDeleteFramebuffers(1, &m_ParticlePBOIDs[i]);
    }
    delete [] m_ParticleTexData;
}

void NEParticleRigidRenderer::connectToSpatial()
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(m_spatial);
    if (rigid == NULL) return;

    QObject::connect( rigid, SIGNAL(initObject()), this, SLOT(initObject()), Qt::ConnectionType::DirectConnection );
    QObject::connect( rigid, SIGNAL(updateDrawingBuffers()), this, SLOT(updateDrawingBuffers()), Qt::ConnectionType::DirectConnection);
}

void NEParticleRigidRenderer::updateDrawingBuffers()
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(m_spatial);
    if (rigid == NULL) return;

    if(rigid->triangleMesh().numTriangles() > 0)
        rigid->triangleMesh().updateDrawingBuffers(GL_DYNAMIC_DRAW);
}

void NEParticleRigidRenderer::initObject()
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(m_spatial);
    if (rigid == NULL) return;

    if( NERENDERMAN.GLManager()->material( &rigid->triangleMesh() ) == 0)
    {
        NEMaterial* defaultMaterial = NERENDERMAN.materials()["DefaultMaterial"];
        rigid->connectTo(reinterpret_cast<NENode*>(defaultMaterial), NE::kIOTriangleMesh, 0, 0);
    }
}

void NEParticleRigidRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(spatial);
    if (rigid == NULL) return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if(rigid->ShowStatistics())
    {
        NEOSD.add(1, NEOsdMsg("Num. Boundary Particles: ", QString::number(rigid->particlesVector()->objectCount)), rigid);
        NEOSD.add(2, NEOsdMsg("Memory Consumption: ", QString("%1 MB").arg(QString::number(rigid->memoryConsumption()*0.000001))), rigid);
    }

    // Update particle force color map texture.
    // This might need to be moved on another callback that is called
    // on every new frame rather than on render later
    // for performance matter.
    {

    }

    if(!rigid->ShowParticles() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {
        if(rigid->DisplayMode() == NESpatial::Self && rigid->DrawSurfaceNormals() == true)
        {
            NERENDERMAN.GLManager()->beginUnLitDraw(surface);
            GL->glColor3f(1.0f, 0.0f, 1.0f);

            NERENDERMAN.GLManager()->updatePositionUniforms(surface);
            NEPVertex *q3 = new NEPVertex[rigid->triangleMesh().triangles().size() * 6];
            for(uint i = 0; i < rigid->triangleMesh().triangles().size(); i++)
            {
                NECTNPVertex v1 = rigid->triangleMesh().vertices()[rigid->triangleMesh().triangles()[i].p1];
                NECTNPVertex v2 = rigid->triangleMesh().vertices()[rigid->triangleMesh().triangles()[i].p2];
                NECTNPVertex v3 = rigid->triangleMesh().vertices()[rigid->triangleMesh().triangles()[i].p3];
                vec3f normal = vec3f::crossProduct((v2.position - v1.position), (v3.position - v1.position));
                normal = v1.normal;
                q3[i*6 + 0].position = v1.position;
                vec3f pos2 = v1.position + 0.005f * normal;
                //std::cout << "pos2 " << pos2.x() << " " << pos2.y() << " " << pos2.z() << std::endl;
                q3[i*6 + 1].position = pos2;
                q3[i*6 + 2].position = v2.position;
                normal = v2.normal;
                pos2 = v2.position + 0.005f * normal;
                q3[i*6 + 3].position = pos2;
                q3[i*6 + 4].position = v3.position;
                normal = v3.normal;
                pos2 = v3.position + 0.005f * normal;
                q3[i*6 + 5].position = pos2;
            }
            GL->glEnableClientState(GL_VERTEX_ARRAY);
            GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
            GL->glDrawArrays(GL_LINES,0,rigid->triangleMesh().triangles().size() * 6);
            GL->glDisableClientState(GL_VERTEX_ARRAY);
            delete [] q3;

            GL->glColor4f(rigid->Color().redF(), rigid->Color().greenF(), rigid->Color().blueF(), 1.0);
            NERENDERMAN.GLManager()->endUnLitDraw(surface);
        }

        if(rigid->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&rigid->triangleMesh(), surface, false, rigid->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else if(rigid->RenderMode() == NESpatial::NERenderMode::FlatShaded)
        {
            NERENDERMAN.GLManager()->drawFlat(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox);
        }
        else if (rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframe || rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOccluded || rigid->RenderMode() == NESpatial::NERenderMode::Occluded)
        {
            if( rigid->RenderMode() != NESpatial::Occluded)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->Color(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
                GL->glPopAttrib();
            }
        }
        else if (rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeHidden || rigid->RenderMode() == NESpatial::NERenderMode::Hidden )
        {
            if(rigid->RenderMode() != NESpatial::Hidden)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->Color(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, true);
                GL->glPopAttrib();
            }
        }
        else if(rigid->RenderMode() == NESpatial::NERenderMode::SmoothShaded || rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOnShaded)
        {
            NEMaterial*  material = NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh *>( &rigid->triangleMesh() ));
            if(material){

                if(rigid->DisplayMode() == NESpatial::Self && rigid->ShowColorFromParticles())
                {
                    GLint prevFrameBuffer;
                    GL->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);
                    GL->glBindFramebuffer(GL_FRAMEBUFFER, surface->m_PCMapFBO);
                    rigid->SetShowParticles(true);
                    particleVerts()->setStride(rigid->particlesVector()->objectSize);
                    particleVerts()->setData(rigid->particlesVector()->objectCount*rigid->particlesVector()->objectSize, &((NEParticle*)rigid->particlesVector()->vecData)->color(), GL_DYNAMIC_DRAW);
                    NERENDERMAN.GLManager()->beginParticleDraw(surface, rigid->particleSize() * 0.5f, true, false, false, false, true);
                    GLfloat modelview[16];
                    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );

                    matrix44f modelViewMat(modelview, 4, 4);
                    modelViewMat = modelViewMat * rigid->transform().inverted();
                    GL->glLoadMatrixf(modelViewMat.data());


                    GL->glClearColor(0, 0, 0, 0);
                    GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), rigid->particlesVector()->objectCount);
                    NERENDERMAN.GLManager()->endParticleDraw(surface);
                    rigid->SetShowParticles(false);
                    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);

                    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
                    modelViewMat = matrix44f (modelview, 4, 4);
                    modelViewMat = modelViewMat * rigid->transform();
                    GL->glLoadMatrixf(modelViewMat.data());


                    surface->setShaderProgram(material->shaderProgram());
                    GL->glActiveTexture(GL_TEXTURE0 + PARTICLE_RIGID_TEXTURE_ID);
                    GL->glBindTexture(GL_TEXTURE_RECTANGLE, surface->m_PCMapColorTexture);
                    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    GL->glActiveTexture(GL_TEXTURE0 + PARTICLE_RIGID_TEXTURE_ID + 1);
                    GL->glBindTexture(GL_TEXTURE_RECTANGLE, surface->m_PCMapFlatWorldTexture);
                    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "particlePCMapTexture" ), PARTICLE_RIGID_TEXTURE_ID );
                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "particlePCMapFlatWorld" ), PARTICLE_RIGID_TEXTURE_ID + 1 );
                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "particleTextured" ), true );

                    float w, h;
                    GL->glGetTexLevelParameterfv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_WIDTH, &w);
                    GL->glGetTexLevelParameterfv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_HEIGHT, &h);

                    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "PCMapWidth" ), w );
                    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "PCMapHeight" ), h );


                }
                else{
                    surface->setShaderProgram(material->shaderProgram());
                    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "particleTextured" ), false );
                }
            }

            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
            GL->glEnable(GL_BLEND);
            NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, rigid->TwoSidedLighting(),rigid->ReceiveShadows(),rigid->transform());
            GL->glPopAttrib();

            if(rigid->RenderMode() == NESpatial::NERenderMode::QuadWireframeOnShaded)
            {
                GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
                matrix44f modelMat = rigid->transform();
                NERENDERMAN.GLManager()->drawQuadWireframe(&rigid->triangleMesh(), surface, rigid->SolidWireframeColor(), modelMat, rigid->DisplayMode() == NESpatial::BoundingBox, false);
                GL->glPopAttrib();
            }
        }
    }
    else
    {
        particleVerts()->setStride(rigid->particlesVector()->objectSize);
        particleVerts()->setData(rigid->particlesVector()->objectCount*rigid->particlesVector()->objectSize, &((NEParticle*)rigid->particlesVector()->vecData)->color(), GL_DYNAMIC_DRAW);
        NERENDERMAN.GLManager()->beginParticleDraw(surface, rigid->particleSize() * 0.5f, true, false);
        NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), rigid->particlesVector()->objectCount);
        NERENDERMAN.GLManager()->endParticleDraw(surface);
    }

    if(rigid->DrawSolidWireframe() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ))
    {
        GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
        GL->glEnable(GL_LIGHTING);
        GL->glEnable(GL_DEPTH_TEST);
        GL->glDepthFunc(GL_LEQUAL);
        GL->glEnable(GL_BLEND);

        matrix44f modelMat = rigid->transform();
        NERENDERMAN.GLManager()->drawSolidWireframe(&rigid->triangleMesh(), surface, rigid->SolidWireframeColor(), rigid->SolidWireframeThickness(), rigid->ContourThreshold(), modelMat);
        GL->glDisable(GL_BLEND);
        GL->glPopAttrib();
    }

    if(rigid->DrawSurfaceNormals() && rigid->ShowParticles())
    {
        NERENDERMAN.GLManager()->beginUnLitDraw(surface);
        GL->glColor3f(1.0f, 0.0f, 1.0f);

        NERENDERMAN.GLManager()->updatePositionUniforms(surface);
        NEPVertex *q3 = new NEPVertex[rigid->particlesVector()->objectCount * 2];
        int availableCount = 0;
        for_all(NERigidParticle, particle, (*rigid->particlesVector()))
        {
            if(typeid(particle) == typeid(NERigidPciSphParticle))
            {
                NERigidPciSphParticle* p = static_cast<NERigidPciSphParticle*>(particle);
                vec3f pos2 = (p->position() + rigid->particleSize() * p->normal());
                q3[availableCount*2 + 0].position = p->position();
                q3[availableCount*2 + 1].position = pos2;
                availableCount++;
            }
        }
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
        GL->glDrawArrays(GL_LINES,0,availableCount * 2);
        GL->glDisableClientState(GL_VERTEX_ARRAY);
        delete [] q3;

        NERENDERMAN.GLManager()->endUnLitDraw(surface);
    }
}

void NEParticleRigidRenderer::renderOnlyDepth(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(spatial);
    if (rigid == NULL) return;

    if(rigid->RenderMode() == NESpatial::QuadWireframeOccluded || rigid->RenderMode() == NESpatial::Occluded)
    {
        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        GL->glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, false, rigid->TwoSidedLighting(),rigid->ReceiveShadows(),rigid->transform());
        GL->glPopAttrib();
    }
}

void NEParticleRigidRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(spatial);
    if (rigid == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if(!rigid->ShowParticles() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {

        if(rigid->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&rigid->triangleMesh(), surface, false, rigid->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();

        }

        else
        {
            NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
        }
    }

    else
    {
        particleVerts()->setData(rigid->particlesVector()->objectCount*rigid->particlesVector()->objectSize, &((NEParticle*)rigid->particlesVector()->vecData)->color(), GL_DYNAMIC_DRAW);
        NERENDERMAN.GLManager()->beginParticleDraw(surface, rigid->particleSize() * 0.5f, false, false, false, true);
        NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), rigid->particlesVector()->objectCount);
        NERENDERMAN.GLManager()->endParticleDraw(surface);
    }
}

void NEParticleRigidRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(spatial);
    if (rigid == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if( !rigid->ShowParticles() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) )
    {
        if(rigid->RenderMode() == NESpatial::NERenderMode::Wireframe)
        {
            GL->glPushAttrib(GL_POLYGON_BIT);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            NERENDERMAN.GLManager()->drawRaw(&rigid->triangleMesh(), surface, false, rigid->DisplayMode() == NESpatial::BoundingBox);
            GL->glPopAttrib();
        }
        else
        {
            NERENDERMAN.GLManager()->draw(&rigid->triangleMesh(), surface, rigid->DisplayMode() == NESpatial::BoundingBox, true, true, false, matrix44f());
        }
    }
    else
    {
        particleVerts()->setData(rigid->particlesVector()->objectCount*rigid->particlesVector()->objectSize, &((NEParticle*)rigid->particlesVector()->vecData)->color(), GL_DYNAMIC_DRAW);
        NERENDERMAN.GLManager()->beginParticleDraw(surface, rigid->particleSize() * 0.5f, true, true, false);
        NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), rigid->particlesVector()->objectCount);
        NERENDERMAN.GLManager()->endParticleDraw(surface);
    }
}

void NEParticleRigidRenderer::renderShadow(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(spatial);
    if (rigid == NULL) return;

    if( rigid->RenderMode() == NESpatial::NERenderMode::Invisible || rigid->RenderMode() == NESpatial::NERenderMode::Wireframe )
        return;
    if (!rigid->OccludeShadows())
        return;
    NERENDERMAN.GLManager()->drawShadowMap(surface, &rigid->triangleMesh());
}

void NEParticleRigidRenderer::renderInMotion(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleRigid* rigid = qobject_cast<NEParticleRigid*>(spatial);
    if (rigid == NULL) return;

    if(rigid->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    if(!rigid->ShowParticles() && NERENDERMAN.GLManager()->isBuffered( &rigid->triangleMesh() ) && rigid->ComputePerVertexVelocity())
    {
        if(rigid->RenderMode() != NESpatial::NERenderMode::Wireframe)
        {
            NERENDERMAN.GLManager()->drawInMotion(surface, &rigid->triangleMesh());
        }
    }
}

NEVertexBuffer* NEParticleRigidRenderer::particleVerts() const
{
    return m_ParticleVerts;
}

#define USE_TICK_LARGE_MIDDLE_SHORT 1

//
// OSDParticleRigidRenderer
//
void NEOSDParticleRigidRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDParticleRigid* osdParticleRigid = qobject_cast<NEOSDParticleRigid*>(spatial);
    if (osdParticleRigid == NULL) return;
    const NEParticleRigid* rigid = osdParticleRigid->particleRigid();
    if (!rigid) return;
    if(osdParticleRigid->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    float minVal = rigid->MinColoringRange();
    float maxVal = rigid->MaxColoringRange();

    if (rigid->ColoringOptions() == NEParticleRigid::Fixed)
    {
        QColor minColor = rigid->MinRangeColor();
        QColor maxColor = rigid->MaxRangeColor();
        vec3f minColorv = vec3f(minColor.red() / 255.0, minColor.green() / 255.0, minColor.blue() / 255.0);
        vec3f maxColorv = vec3f(maxColor.red() / 255.0, maxColor.green() / 255.0, maxColor.blue() / 255.0);

        // Render Color Bar
        surface->setShaderProgram(osdParticleRigid->getColorBarProgram());
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), 2 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[0]").toUtf8() ), 1, &minColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[0]").toUtf8() ), 0.0 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[1]").toUtf8() ), 1, &maxColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[1]").toUtf8() ), 1.0 );
    }
    else if (rigid->ColoringOptions() == NEParticleRigid::Ramp)
    {
        QVector<QPair<qreal, QColor>> colorList = rigid->getColorList();
        int nSize = std::min(colorList.size(), MAX_COLORRAMP_SIZE);

        // Render Color Bar
        surface->setShaderProgram(osdParticleRigid->getColorBarProgram());
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), 0 );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), nSize );
        for(int i = 0; i < nSize; i++)
        {
            float pos = colorList.at(i).first;
            QColor color = colorList.at(i).second;
            vec3f colorv = vec3f(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
            GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[" + QString::number(i) + "]").toUtf8() ), 1, &colorv[0] );
            GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[" + QString::number(i) + "]").toUtf8() ), pos );
        }
    }
    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdParticleRigid->Positionx();
    int yRes = height * osdParticleRigid->Positiony();
    int wRes = OSD_PARTICLE_COLORBAR_WIDTH * osdParticleRigid->Scalex();      //!< Width of color bar
    int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * osdParticleRigid->Scaley();     //!< Height of color bar
    int hTitle = osdParticleRigid->fontHeight() * 2;
    int wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdParticleRigid->Scalex();   //!< Width of margin between color bar and measure
    unsigned int widthU = 0;   //!< Width of measurement text
    NEOSDParticleRigid::NEOSDOrientation enumOrientation = osdParticleRigid->OSDOrientation();
    if (enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Top ||
        enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Bottom)
    {
        if (osdParticleRigid->FontSize() == 20)
            osdParticleRigid->SetFontSize(12);
    } else
    {
        if (osdParticleRigid->FontSize() == 12)
            osdParticleRigid->SetFontSize(20);
    }
    vec2f OsdSize = osdParticleRigid->osdSize();
    if (enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Top ||
        enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Bottom)
    {
        wRes = OsdSize.x() - (int)(osdParticleRigid->fontWidth());
        hRes = OSD_PARTICLE_COLORBAR_WIDTH * osdParticleRigid->Scaley();
        wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdParticleRigid->Scaley();
    }

    int xPos = 0, yPos = 0;
    vec3f colorBarRect[4];
    vec3f colorBarRect_2D[4];
    vec2f widgetSize = vec2f(width, height);
    vec3f offsetOfOrientation;
    switch(enumOrientation)
    {
    case NEOSDParticleRigid::NEOSDOrientation::Top:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes - hTitle, 0);
        colorBarRect[1] = vec3f(xRes, yRes - hTitle - hRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes - hTitle - hRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes - hTitle, 3);
        break;
    case NEOSDParticleRigid::NEOSDOrientation::Bottom:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes + hRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes + hRes, 3);
        break;
    case NEOSDParticleRigid::NEOSDOrientation::Right:
        offsetOfOrientation = vec3f(OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes - wRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes - wRes, yRes + hRes, 3);
        break;
    case NEOSDParticleRigid::NEOSDOrientation::Left:
    default:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes + wRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes, yRes + hRes, 3);
        break;
    }
    for (int nIndex = 0; nIndex < 4; nIndex++)
    {
        colorBarRect[nIndex] = (colorBarRect[nIndex] + offsetOfOrientation); // widgetSize;
        colorBarRect_2D[nIndex] = vec3f(colorBarRect[nIndex].x(), colorBarRect[nIndex].y(), 0);
        colorBarRect[nIndex].setX(colorBarRect[nIndex].x() / widgetSize.x());
        colorBarRect[nIndex].setY(colorBarRect[nIndex].y() / widgetSize.y());
        //std::cout << "NEOSDParticleFluidRenderer::render colorBarRect x=" << colorBarRect[nIndex].x() << ", y=" << colorBarRect[nIndex].y() << ", z=" << colorBarRect[nIndex].z() << ", index=" << nIndex << std::endl;
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

#if USE_TICK_LARGE_MIDDLE_SHORT
    // Render Ruler
    int rulerLeft   = colorBarRect_2D[0].x(); // point of index 0 is bottomleft(or topleft)
    int rulerTop    = colorBarRect_2D[0].y();
    int rulerRight  = colorBarRect_2D[1].x(); // point of index 1 is bottomright(or bottomleft)
    int rulerbottom = colorBarRect_2D[1].y();
    int tickWidthL  = 5; // wRes / 10 + 1;
    int tickWidthM  = 3; // wRes / 20 + 1;
    int tickWidthS  = 3; // wRes / 25 + 1;
    int tickHeightL = 0;
    int tickHeightM = 0;
    int tickHeightS = 0;
    if (enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Top ||
        enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Bottom)
    {
        tickWidthL = 0;
        tickWidthM = 0;
        tickWidthS = 0;
        tickHeightL = 5; // hRes / 10 + 1;
        tickHeightM = 3; // hRes / 20 + 1;
        tickHeightS = 3; // hRes / 25 + 1;
    }
//    glLineWidth(2.0f);
    vec3f q3_1[2], q3_2[2];
    float offsetX, offsetY;

    GL->glEnableClientState(GL_VERTEX_ARRAY);

    for( int i = 0; i <= 10 * osdParticleRigid->TickCount(); i++)
    {
        float s = i / 10.0 / osdParticleRigid->TickCount();
        if (enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Top ||
            enumOrientation == NEOSDParticleRigid::NEOSDOrientation::Bottom)
        {
            offsetX = s * wRes; offsetY = 0;
        } else
        {
            offsetX = 0; offsetY = s * hRes;
        }

        q3_1[0] = vec2f(rulerLeft + offsetX, rulerTop + offsetY);
        q3_2[0] = vec2f(rulerRight + offsetX, rulerbottom + offsetY);
        if (i % 10 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthL, rulerTop + offsetY - tickHeightL);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthL, rulerbottom + offsetY + tickHeightL);
        }
        else if (i % 5 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthM, rulerTop + offsetY - tickHeightM);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthM, rulerbottom + offsetY + tickHeightM);
        }
        else
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthS, rulerTop + offsetY - tickHeightS);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthS, rulerbottom + offsetY + tickHeightS);
        }
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_1);
        GL->glDrawArrays(GL_LINE_LOOP,0,2);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_2);
        GL->glDrawArrays(GL_LINE_LOOP,0,2);
//        std::cout << "new index=" << i << std::endl;
//        std::cout << "q3_1[0] x=" << q3_1[0].x() << ", y=" << q3_1[0].y() << ", z=" << q3_1[0].z() << std::endl;
//        std::cout << "q3_1[1] x=" << q3_1[1].x() << ", y=" << q3_1[1].y() << ", z=" << q3_1[1].z() << std::endl << std::endl;
    }
//    glLineWidth(1.0f);
    GL->glDisableClientState(GL_VERTEX_ARRAY);
#endif

    // Draw Title Text
    surface->setShaderProgram(0);
    GL->glDisable(GL_BLEND);
    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );
    float titleTextWidth = osdParticleRigid->TitleText().length() * osdParticleRigid->fontWidth();
    if (enumOrientation != NEOSDParticleRigid::NEOSDOrientation::Top &&
        enumOrientation != NEOSDParticleRigid::NEOSDOrientation::Bottom)
    {
        glPrint(osdParticleRigid->TitleText(),
                xRes - titleTextWidth / 2,
                yRes + hRes + hTitle - OsdSize.y() / 2,
                osdParticleRigid->fontColor(),
                osdParticleRigid->getTextureFont(), osdParticleRigid->OutlineColor(), osdParticleRigid->getTextureOutlineFont());
    } else {
        glPrint(osdParticleRigid->TitleText(),
                xRes - titleTextWidth / 2,
                yRes - osdParticleRigid->fontHeight() + OsdSize.y() / 2,
                osdParticleRigid->fontColor(),
                osdParticleRigid->getTextureFont(), osdParticleRigid->OutlineColor(), osdParticleRigid->getTextureOutlineFont());
    }

    // Draw measurement values (1..step)
    for (int i = 0; i <= osdParticleRigid->TickCount(); i++)
    {
        float s = i / (float)osdParticleRigid->TickCount();
        float val = minVal * (1-s) + maxVal * s;
        QString text;
        if( osdParticleRigid->DisplayNotation() == NEOSDBase::Exponent )
        {
            text = QString("%1").arg( (double)val, 2, 'E', 2 );
        }
        else if( osdParticleRigid->DisplayNotation() == NEOSDBase::FloatingPoint )
        {
            text = QString("%1").arg( (double)val, osdParticleRigid->TotalNumberLength(), 'f', osdParticleRigid->FloatTypePrecision() );
        }
        widthU = (float)osdParticleRigid->TotalNumberLength() * osdParticleRigid->fontWidth();
        switch(enumOrientation)
        {
        case NEOSDParticleRigid::NEOSDOrientation::Top:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - OsdSize.y()/2;
            break;
        case NEOSDParticleRigid::NEOSDOrientation::Bottom:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - hTitle - (int)(osdParticleRigid->fontHeight()) + OsdSize.y()/2;
            break;
        case NEOSDParticleRigid::NEOSDOrientation::Right:
            xPos = xRes + (int)(osdParticleRigid->fontWidth()) -  OsdSize.x()/2;
            yPos = yRes + s * hRes - OsdSize.y()/2;
            break;
        case NEOSDParticleRigid::NEOSDOrientation::Left:
        default:
            xPos = xRes + wRes + wMargin -  OsdSize.x()/2;
            yPos = yRes + s * hRes - OsdSize.y()/2;
            break;
        }
        glPrint(text, xPos, yPos,
                osdParticleRigid->fontColor(),
                osdParticleRigid->getTextureFont(), osdParticleRigid->OutlineColor(), osdParticleRigid->getTextureOutlineFont());
    }

    GL->glPopAttrib();
}



NEOSDParticleRigidRendererFactory::NEOSDParticleRigidRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEOSDParticleRigidRendererFactory::nodeName() { return "Particle Rigid OSD Renderer"; }
NEObject *NEOSDParticleRigidRendererFactory::createInstance() { return new NEOSDParticleRigidRenderer(); }
