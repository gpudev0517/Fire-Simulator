#pragma once

class NEObject;
class NEManager;
class NENodeFactoryPrivate;

//! Public interface of NENode Factory
#define INTERFACE_NENodeFactory_Public(_Terminal_)\
    virtual QString nodeName() _Terminal_\
    virtual uint nodeType() _Terminal_\
    virtual QString nodeInherits() _Terminal_\
    virtual QString objectName() _Terminal_\
    virtual QString nodeVersion() _Terminal_\
    virtual QString nodeVendor() _Terminal_\
    virtual QString nodeHelp() _Terminal_\
    virtual NEObject* createInstance() _Terminal_

class NECEXP_CORE NENodeFactory : public QObject
{
public:
    NENodeFactory(NEManager* m);
    virtual ~NENodeFactory();

    NE_BASE_PURE(NENodeFactory, Public)

    virtual QStringList nodeAltNames();
    virtual NENodeIcon* nodeIcon();


    NEManager* manager();

    int objectNumber();
    //! Resets the object number back to 1
    void resetObjectNumber();
    //! Sets the maximum of val and the current number as the object number
    void setObjectNumber(int val);
    void incrementObjectNumber();

private:
    friend class NEManager;
    friend class NEManagerPrivate;

    void setManager(NEManager* m);
    NE_DECL_PRIVATE(NENodeFactory)
};

Q_DECLARE_INTERFACE(NENodeFactory, "NENodeFactory")
