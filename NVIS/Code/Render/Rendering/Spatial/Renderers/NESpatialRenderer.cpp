#include "NESpatialRenderer.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"


//
// Spatial
//
NESpatialRenderer::NESpatialRenderer()
{
    m_pFluidMaterial = 0;
}

void NESpatialRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
}

void NESpatialRenderer::renderOnlyDepth(NESpatial* spatial, IGLSurface* surface)
{
}

void NESpatialRenderer::renderPickable(NESpatial *spatial, IGLSurface* surface)
{
}

void NESpatialRenderer::renderPicked(NESpatial *spatial, IGLSurface* surface)
{
}

void NESpatialRenderer::renderWater(NESpatial* spatial, IGLSurface* surface)
{
}

void NESpatialRenderer::renderShadow(NESpatial* spatial, IGLSurface* surface)
{
}

void NESpatialRenderer::renderStatic(NESpatial* spatial, IGLSurface* surface)
{
    spatial->render();
}

void NESpatialRenderer::renderPickableStatic(NESpatial* spatial, IGLSurface* surface)
{
    spatial->renderPickable();
}

void NESpatialRenderer::renderPickedStatic(NESpatial* spatial, IGLSurface* surface)
{
    spatial->renderPicked();
}

void NESpatialRenderer::setSpatial(NESpatial* spatial)
{
    m_spatial = spatial;
    connectToSpatial();
}

void NESpatialRenderer::connectToSpatial()
{
    //
}

bool NESpatialRenderer::createVisualizer(NESpatial *spatial)
{
    return true;
}

void NESpatialRenderer::setFluidMaterial(NEMaterial* FluidMaterial)
{
    m_pFluidMaterial = FluidMaterial;
}

NEMaterial* NESpatialRenderer::FluidMaterial() const
{
    return m_pFluidMaterial;
}
