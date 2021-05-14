#pragma once

#include "Physics/Spatial/NEPhysical.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NELink))
NE_FWD_DECL_1(class, NERigid )

class NECEXP_PHYSICS NELink : public NEPhysical
{
    Q_OBJECT
    NE_CLASS("General Rigid Body Options")

public:
    enum NELinkType { Spherical, Cylindrical, Revolute, Prismatic, Distance, Engine };
    NE_PROP_RW(NELinkType, LinkType, "Link Type", "Type of the constraint between two rigid bodies", true,)
    Q_ENUMS(NELinkType)

public:
    NELink(NENodeFactory* factory);
    virtual ~NELink();
    virtual uint init(uint initMode = NE::kInitOnCreate);
    NERigid* rigid( uint rigidNum ) const;

public slots:
    uint SetPosition(const vec3f& val) override;

 signals:
    void linkChanged(const NELink* val);

protected:
    virtual bool addSource(NENode* node, NEConn& conn);
    virtual void removeSource(NENode* node, uint type);

private:
    NE_DECL_PRIVATE(NELink)
};

// The factory class

class NELinkFactory : public NENodeFactory
{
public:
    NELinkFactory(NEManager* m);
    ~NELinkFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames();
};



Q_DECLARE_INTERFACE(NELink, "NELink")
