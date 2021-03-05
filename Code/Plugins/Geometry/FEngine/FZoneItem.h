#pragma once
#include "Utility/FilePath.h"
#include "Utility/NESurfaceMaterial.h"
#include "Utility/NEFailureCriteria.h"
#include "Base/NENodeFactory.h"
#include "Mesh/NEZoneItem.h"
#include "Mesh/NESpatialMesh.h"

NE_FWD_DECL_4(class, NEIndexedTriMesh, FilePath, FZone, NE_PRIVATE(FZoneItem))


class FDYNCEXP_PLUGINS FZoneItem : public NEZoneItem
{
    Q_OBJECT
    NE_CLASS("Fire Plant Item")

public:
    enum FItemType
    {
        OtherRaceways = 4,
        Sources= 6,
        Equipments = 7,
        Trays = 2,
        Conduits = 3,
        Cable = 1,
        Component = 5,
        Unknowns = 8,
        Physical = 9,
        Boundary = 10,
        Generic = 11,
        Compartments = 12,
        Vent = 13
    };

    NE_PROP_RW(FItemType, ItemType, "Physical Item Type", "The type of item as modeled in FRI3D (e.g., raceway, components, etc.", true, )
    NE_PROP_R(QString, PlantItemType, "Plant Item Type", "The string type of item Cable/Raceway/Components etc", false )
    NE_PROP_RW(uint, ConnectorID, "Connector ID", "Zone Item Connector ID", false,)
    NE_PROP_RW(NEFailureCriteria, Failure, "Failure Criteria", "Criteria for Failure", true,)
    NE_PROP_R(bool, Failed, "Failed", "Whether failed or not", false)
    NE_PROP_R(double, FailedTime, "Failed Time", "Time when failed", true )
    NE_PROP_RW(QColor, FailedColor, "Failed Color", "Color when failed", true,)
    NE_PROP_RW(QString, Desc, "Description", "Zone item description", true,)
    NE_PROP_RW(QString, Issues, "Issues", "Other items not tagged previously", true,)
    NE_PROP_RW(NESurfaceMaterial, Material, "Material", "Surface material", true,)
    NE_PROP_RW(uint, CompartmentID, "Compartment ID", "Compartment ID where the item is located ", false, )
    NE_PROP_R(vec3f, GhostPosition, "Ghost Position", "Offset position within the associated compartment", true )
    NE_PROP_RW(bool, ShowGhostPosition, "Show Ghost Position", "Show the ghost position", true, )

    Q_ENUMS(FItemType)

public:
    FZoneItem( NENodeFactory* factory = NULL );
    virtual ~FZoneItem();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    virtual QString nodeDescription() override;
    virtual void copyValues(const NEZoneItem* zoneitem);

    int itemType() override;

    // Set to failed status and set failed time
    uint setFailed(double failedTime);
    // Reset to non-failed status and reset failed time to -1.0
    void resetFailed();

    // Compute the ghost position within the associated compartment
    void computeGhostPosition(const FZone* comp);
    // Compute the ghost position within the compartment of the parent zone
    void computeGhostPosition();
    // Reset the ghost position to the current position
    void resetGhostPosition();

    // Return the surface temperature
    double surfTemp(double time) const;
    // Return the minimum and maximum surface temperatures over time
    double minSurfTemp() const;
    double maxSurfTemp() const;

    // Add a surface temperature
    void addSurfTemp(double time, double temperature);
    // Clear the simulation data
    void clearSimData();

public slots:
    uint SetPosition(const vec3f& val) override;

    const QString& zoneItemName() const;
    uint setZoneItemName(const QString& zoneName);

    uint setPlantItemType( const QString& pitemType );

    QString categoryType() override;
    QString otherCategoryType() override;

    const QString& model3DInfo() const;
    void setModel3DInfo(const QString& model3dinfo);

private:
    NE_DECL_PRIVATE(FZoneItem)
};

// The factory class
class FZoneItemFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FZoneItemFactory(NEManager* m = 0);
    ~FZoneItemFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};
