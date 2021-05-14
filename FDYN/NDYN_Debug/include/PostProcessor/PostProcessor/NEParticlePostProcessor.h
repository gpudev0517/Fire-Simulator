#pragma once

#include "PostProcessor/NEPostProcessor.h"

class NEParticlePostProcessor : public NEPostProcessor
{
public:
    NEParticlePostProcessor(NENodeFactory* factory);
	virtual ~NEParticlePostProcessor();

	//! The main update method of the post processor, called from outside
	virtual uint postProcess();
};
