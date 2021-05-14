#pragma once

#include <QObject>

#include "Rendering/Spatial/NEObjectRenderer.h"
#include "Core/Base/NERendererFactory.h"


//
//
//

class  NEBoundedForceRenderer: public NEObjectRenderer
{
    Q_OBJECT
public:
    virtual void render(NESpatial* spatial, IGLWidget* widget);
    virtual void renderPickable(NESpatial* spatial, IGLWidget* widget);
    virtual void renderPicked(NESpatial* spatial, IGLWidget* widget);
};

class NEBoundedForceRendererFactory : public NERendererFactory
{
    Q_OBJECT
    Q_INTERFACES(NERendererFactory)

public:
    NEBoundedForceRendererFactory(NEManager* m);

    QString nodeName();

private:
    NESpatialRenderer* createInstance();
};
