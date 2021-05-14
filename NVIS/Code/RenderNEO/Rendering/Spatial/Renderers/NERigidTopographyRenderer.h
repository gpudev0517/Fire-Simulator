#pragma once


#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "NEParticleRigidRenderer.h"

#include "Base/NERendererFactory.h"

class NEVertexBuffer;


class NERigidTopographyRenderer : public NEParticleRigidRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};

class NERigidTopographyRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NERigidTopographyRendererFactory(NEManager* m);
    QString nodeName();

private:
    NEObject* createInstance();
};

class NEOSDRigidTopographyRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};


class NEOSDRigidTopographyRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEOSDRigidTopographyRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


