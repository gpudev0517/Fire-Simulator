#pragma once

#include "Base/NENodeFactory.h"

#define INTERFACE_NEMultiNodeFactory_Public(_Terminal_)\
    virtual QString nodeName() _Terminal_\
    virtual uint nodeType() _Terminal_\
    virtual QString nodeInherits() _Terminal_\
    virtual QString objectName() _Terminal_\
    virtual QString nodeVersion() _Terminal_\
    virtual QString nodeVendor() _Terminal_\
    virtual QString nodeHelp() _Terminal_\
    virtual NEObject* createInstance() _Terminal_

class NECEXP_CORE NEMultiNodeFactory : public NENodeFactory
{
    Q_OBJECT

public:
    NEMultiNodeFactory(NEManager* m = 0);
    ~NEMultiNodeFactory();

    virtual QList<QString> &objectNames();
    virtual QList<QString> &nodeNames();

    QList<NENodeFactory *>& factories();
    NE_DERIVED(NENodeFactory, Public)

protected:

    QList<NENodeFactory*> m_factories;

    QList<QString> m_objectNames;
    QList<QString> m_nodeNames;
};

