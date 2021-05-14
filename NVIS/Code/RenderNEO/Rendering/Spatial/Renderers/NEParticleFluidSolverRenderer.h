#pragma once

#include "Rendering/Spatial/NEOSDBase.h"
#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"

#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"

class IGLSurface;
class NEVertexBuffer;
class NEParticleFluidSolver;

// Fluid
class NECEXP_NERENDERNEO NEFluidSolverRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    NEFluidSolverRenderer();
    ~NEFluidSolverRenderer();

    //
    virtual void connectToSpatial();
    NEVertexBuffer* particleVerts();

public slots:
    void updateDrawingBuffers();

protected:
    NEVertexBuffer* m_ParticleVerts;
};

// Particle Fluid
class NECEXP_NERENDERNEO NEParticleFluidSolverRenderer : public NEFluidSolverRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    virtual void renderWater(NESpatial* spatial, IGLSurface* surface);
    void zOrderParticles(NEParticleFluidSolver *fluidSolver);
public slots:
    void initObject();
protected:
    std::vector< IdPointerPair > m_IdPairs;
};

class NEParticleFluidSolverRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEParticleFluidSolverRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};



class NEOSDParticleFluidRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};


class NEOSDParticleFluidRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEOSDParticleFluidRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


