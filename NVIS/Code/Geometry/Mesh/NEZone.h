#pragma once

#include "Mesh/NESpatialMesh.h"

NE_FWD_DECL_3(class, NEZoneItem, NEMeshContainer, NE_PRIVATE(NEZone))

class NECEXP_GEOMETRY NEZone : public NESpatialMesh
{
    Q_OBJECT
    NE_CLASS("Zone Properties")

    NE_PROP_RW(uint, ID, "Zone ID", "Zone ID", true,)
    NE_PROP_RW(QString, Type, "Type", "Zone Type", true,)
    NE_PROP_RW(QString, Desc, "Zone Description", "Zone Description String", true,)

public:
    NEZone( NENodeFactory* factory );
    virtual ~NEZone();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    ///
    /// \brief updateAABB
    /// \return
    ///
    virtual bool updateAABB();

    const QString& originalName() const;
    uint setOriginalName(const QString& val);

    virtual QList< QString >& scenarios();
    virtual QList< QString >& otherItems();

    QList<QString> itemNameChildren(const QString& name) const;
    bool itemNameTreeContains(const QString& childName, const QString& parentName) const;
    void addToItemNameTree(const QString& childName, const QString& parentName);
    void removeFromItemNameTree(const QString& childName, const QString& parentName);
    void removeAllFromItemNameTree(const QString& name);
    void renameInItemNameTree(const QString& oldName, const QString& newName);
    void clearItemNameTree();

    virtual QList< QString >& zoneContainers();

    virtual QString nodeDescription();

signals:
#if 0 // Disable for warning duplicate signals.
    void initObject();
    void updateDrawingBuffers();
#endif

private:
    NE_DECL_PRIVATE(NEZone)
};

//
// The factory class
//
class NEZoneFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEZoneFactory(NEManager* m = 0);
    ~NEZoneFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};
