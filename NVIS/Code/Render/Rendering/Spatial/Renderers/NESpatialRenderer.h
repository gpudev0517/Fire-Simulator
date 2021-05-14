#pragma once

#include "NEObject.h"
#include "Base/NERendererFactory.h"


class IGLSurface;
class NEVertexBuffer;
class NEParticleFluidSolver;
class NEMaterial;
class NESpatial;

class IObjectRenderer : public NEObject
{
    Q_OBJECT
public:
};

class NECEXP_NERENDERING NESpatialRenderer : public IObjectRenderer
{
    Q_OBJECT
public:
    NESpatialRenderer();
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderOnlyDepth(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface) ;
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface) ;
    virtual void renderWater(NESpatial* spatial, IGLSurface* surface);    //!< Render current spatial to the fluid depth and color map
    virtual void renderShadow(NESpatial* spatial, IGLSurface* surface);   //!< Render current spatial to the shadow map
    static void renderStatic(NESpatial* spatial, IGLSurface* surface);
    static void renderPickableStatic(NESpatial* spatial, IGLSurface* surface);
    static void renderPickedStatic(NESpatial* spatial, IGLSurface* surface);

    // Connect signals of spatial to slots on this class
    void setSpatial(NESpatial* spatial);
    virtual void connectToSpatial();

    // fluid material(water material) method for particle fluid node
    void setFluidMaterial(NEMaterial* FluidMaterial);
    NEMaterial* FluidMaterial() const;


public slots:
    virtual bool createVisualizer( NESpatial* spatial );

protected:
    NESpatial* m_spatial; // We can use this for the corresponding object
    NEMaterial* m_pFluidMaterial;
};

