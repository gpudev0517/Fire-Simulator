#pragma once

#include "Base/NENodeFactory.h"

class NESpatialRenderer;
class NEManager;
class NERendererFactoryPrivate;



//! Public interface of NENode Factory
#define INTERFACE_NERendererFactory_Public(_Terminal_)\
    virtual QString nodeName() _Terminal_\
    virtual NEObject* createInstance() _Terminal_


#define INTERFACE_NERendererFactoryNode_Public(_Terminal_) \
    virtual uint nodeType() { return 0; } _Terminal_\
    virtual QString nodeInherits() {return ""; } _Terminal_\
    virtual QString objectName() {return ""; }_Terminal_\
    virtual QString nodeVersion() {return ""; }_Terminal_\
    virtual QString nodeVendor() {return ""; }_Terminal_\
    virtual QString nodeHelp() {return ""; } _Terminal_

class NECEXP_CORE NERendererFactory : public NENodeFactory
{
public:
    NERendererFactory(NEManager* m = 0);
    virtual ~NERendererFactory();

    NE_BASE_PURE(NERendererFactory, Public)
    NE_BASE(NERendererFactoryNode, Public)


    NEManager* manager();

    int objectNumber();
    //! Resets the object number back to 1
    void resetObjectNumber();
    //! Sets the maximum of val and the current number as the object number
    void setObjectNumber(int val);
    void incrementObjectNumber();
    void setManager(NEManager* m);

private:
    friend class NEManager;
    friend class NEManagerPrivate;

    NE_DECL_PRIVATE(NERendererFactory)
};

Q_DECLARE_INTERFACE(NERendererFactory, "NERendererFactory")
