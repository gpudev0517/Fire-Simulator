#pragma once

#include "Rendering/Spatial/NEOSDBase.h"
#include "Rendering/Spatial/Renderers/NEOSDBaseRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Base/NERendererFactory.h"

// Measurement Field
class NEMeasurementFieldRenderer : public NESpatialRenderer
{
    Q_OBJECT
public:
    NEMeasurementFieldRenderer();
    ~NEMeasurementFieldRenderer();

    virtual void render(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPickable(NESpatial* spatial, IGLSurface* surface);
    virtual void renderPicked(NESpatial* spatial, IGLSurface* surface);

//    NEVertexBuffer* measurementVerts() const
//    {
//        return m_MeasurementVerts;
//    }

    void updateDrawingBuffers();

private:
    void renderTitle(NESpatial* spatial, IGLSurface* surface);
    void renderBoundingBox(NESpatial* spatial, IGLSurface* surface);

protected:
    GLfloat* m_DataArrowHead;
    GLfloat* m_DataArrowCover;
    GLfloat* m_DataArrowFoot;
};

class NEMeasurementFieldRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEMeasurementFieldRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};



class NEOSDMeasurementRenderer : public NEOSDBaseRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLSurface* surface);
};




class NEOSDMeasurementRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEOSDMeasurementRendererFactory(NEManager* m);

    QString nodeName();

private:
    NEObject* createInstance();
};





