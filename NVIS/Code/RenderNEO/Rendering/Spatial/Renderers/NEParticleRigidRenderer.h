#pragma once

#include "Rendering/Spatial/NEOSDBase.h"
#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"

class IGLSurface;
class NEVertexBuffer;
class NEParticleFluidSolver;


// Particle Rigid
class NECEXP_NERENDERNEO NEParticleRigidRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    NEParticleRigidRenderer();
    ~NEParticleRigidRenderer();
    virtual void connectToSpatial();

    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderOnlyDepth(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    virtual void renderShadow(NESpatial* spatial, IGLSurface* surface);
    virtual void renderInMotion(NESpatial* spatial, IGLSurface* surface);

public slots:
    void initObject();
    void updateDrawingBuffers();

protected:
    NEVertexBuffer* particleVerts() const;
    NEVertexBuffer* m_ParticleVerts;

    // Particle force position-coloring map
    GLuint m_ParticlePCMapTexture;

    GLuint m_ParticlePBOIDs[2];

    int m_ParticlePBOArrayIndex;
    float *m_ParticleTexData;
    int m_ParticleBuffIndex;
};

class NEOSDParticleRigidRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};



class NEOSDParticleRigidRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEOSDParticleRigidRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};
