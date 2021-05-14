#pragma once

#include "Spatial/NESpatial.h"

class NECEXP_PHYSICS NEDataCollector: public NESpatial
{
    Q_OBJECT

public:

    NEDataCollector( NENodeFactory* factory  );
    virtual ~NEDataCollector();

    virtual uint init(uint initMode = NE::kInitOnCreate);
    virtual void update();
    virtual void archive();

    virtual void restore();

    virtual void render();
    virtual void renderPicked();
    virtual void renderPickable() ;

};


