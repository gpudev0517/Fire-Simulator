#pragma once

#include "NENode.h"


class NECEXP_CORE NEResource : public NENode
{
	Q_OBJECT
public:
    NEResource(NENodeFactory* factory);
    virtual ~NEResource();

    // NEObject interface
public:
    virtual uint init(uint initMode) override;
    virtual void propertyTweak(QtPropertyBase* p) override;

public slots:
    virtual void resetProperties() override;

protected:
    virtual void initializeGUIActions() override;
};

Q_DECLARE_INTERFACE(NEResource, "NEResource")
