#pragma once

#include "Rendering/Spatial/NEOSDBase.h"
#include "NEOSDBaseRenderer.h"

#include "NESpatialRenderer.h"
#include "Base/NERendererFactory.h"

class IGLSurface;
class NEVertexBuffer;
class NEParticleFluidSolver;

// ClipPlane
class NEClipPlaneRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NEClipPlaneRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEClipPlaneRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


// FloorPlane
class NEFloorPlaneRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    virtual void renderShadow(NESpatial* spatial, IGLSurface* surface);
};

class NEFloorPlaneRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEFloorPlaneRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


// NULL Gizmo
class NENullGizmoRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NENullGizmoRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NENullGizmoRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

// Measurement Item
class NEMeasurementItemRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    bool m_bPickable = false;
};

class NEMeasurementItemRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEMeasurementItemRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

// Interactive Measurement Gizmo
class NEInteractiveMeasurementGizmoRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NEInteractiveMeasurementGizmoRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEInteractiveMeasurementGizmoRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};



class NEOSDGlobalInformationRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};




class NEOSDGlobalInformationRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEOSDGlobalInformationRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};




