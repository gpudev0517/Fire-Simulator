#pragma once

#include "Core/Spatial/NESpatial.h"

class NECEXP_NEPOSTPROCESSOR NEPostProcessor : public NESpatial
{
	Q_OBJECT
public:
    NEPostProcessor(NENodeFactory *factory) : NESpatial(factory) {}
	virtual ~NEPostProcessor() {}

	//! The main update method of the post processor, called from outside
	virtual uint update() = 0;
	virtual uint init(uint initMode = NE::kInitOnCreate) = 0;
	virtual uint saveFrameData(QString dataPath) = 0;
	virtual uint loadFrameData(QString dataPath) = 0;
    virtual void render() {};
    virtual void renderPicked() {};
    virtual void renderPickable() {};
};


Q_DECLARE_INTERFACE(NEPostProcessor, "NEPostProcessor")
