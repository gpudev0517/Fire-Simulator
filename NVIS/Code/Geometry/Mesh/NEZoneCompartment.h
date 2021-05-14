#pragma once

#include "Spatial/NESpatialGroup.h"
#include "Mesh/NEZoneItem.h"

NE_FWD_DECL_2(class, NEMeshContainer, NE_PRIVATE(NEZoneCompartment))

class NECEXP_GEOMETRY NEZoneCompartment : public NEZoneItem
{
    Q_OBJECT
    NE_CLASS("Zone Container Properties")

    NE_PROP_RW(QString, Desc, "Description", "Compartment Description", true,)
    NE_PROP_RW(vec3f, Thickness, "Thickness", "Thickness - bottom = x, top = y, side = z", true,)

public:
    NEZoneCompartment( NENodeFactory* factory );
    virtual ~NEZoneCompartment();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    ///
    /// \brief updateAABB
    /// \return
    ///
    virtual bool updateAABB();

    virtual void loadZoneFile(const QString &filePath) ;
    virtual void clearZoneFile() ;
    virtual void reloadZoneFile() ;

    virtual QList< QString >& scenarios();
    virtual QList< QString >& otherItems();

    uint SetZonePositionFromWorld(const vec3f& worldPosition);
    uint SetZoneOrientationFromWorld(const quatf& worldRotation);
    uint SetZoneScaleFromWorld(const vec3f& worldScale);

    //void updateZoneTransform();

    virtual QString nodeDescription();

    NEIndexedTriMesh &thickenedMesh();

    static QPair<NEZoneCompartment *, int> s_SelectedCompartment;
    static QPair<NEZoneCompartment *, int> s_TargetCompartment;

    uint SetPosition(const vec3f &val) override;

private:
    NE_DECL_PRIVATE(NEZoneCompartment)

};

//
// The factory class
//
class NEZoneCompartmentFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEZoneCompartmentFactory(NEManager* m = 0);
    ~NEZoneCompartmentFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};
