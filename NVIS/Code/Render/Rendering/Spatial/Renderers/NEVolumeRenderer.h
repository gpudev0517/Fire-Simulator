#pragma once

#include "NEOSDBaseRenderer.h"
#include "NESpatialRenderer.h"
#include "Base/NERendererFactory.h"

class NEVolumeNodeRenderer : public NESpatialRenderer
{
public:
    virtual float MinColoringRange() { return 0; }
    virtual float MaxColoringRange() { return 1; }
};

//
//
//

class NEVolumeRendererPrivate;

class  NEVolumeRenderer: public NESpatialRenderer
{
    Q_OBJECT
public:
    NEVolumeRenderer();
    ~NEVolumeRenderer();
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
    bool createVisualizer( NESpatial* spatial );
private:
    NEVolumeRendererPrivate *P;
};

class NEVolumeRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEVolumeRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


#if 0
//
// VolumeSource
//
class NEVolumeSourceRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);
};

class NEVolumeSourceRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEVolumeSourceRendererFactory(NEManager* m);

    QString nodeName();

private:
    NESpatialRenderer* createInstance();
};
#endif

#if NE_VOLUME
//
// various Volume class renderer factory
//

class NEMeshToVolumeRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEMeshToVolumeRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

class NEPartToVolumeRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEPartToVolumeRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

class NEVolumeCombineRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEVolumeCombineRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

class NEVolumeFileRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEVolumeFileRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

class NEVolumeFilterRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEVolumeFilterRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};


class NEOSDVolumeRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};

class NEOSDVolumeRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEOSDVolumeRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};

#endif
