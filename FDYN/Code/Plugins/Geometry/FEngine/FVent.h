#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

#include "Mesh/NEZoneItem.h"
#include "Utility/NECustomVariants.h"
#include "Utility/NEVentCriteria.h"

NE_FWD_DECL_2(class, FZone, NE_PRIVATE(FVent));
/*!
  Class defining a solid and its physical properties.
*/

class  FVent : public NEZoneItem
{
    Q_OBJECT
    NE_CLASS("Vent Properties")

public:
    enum VentGeometry {
            Rectangular,
            Circular
    };

    NE_PROP_RW(QString, Desc, "Description", "Description String", true, )
    NE_PROP_RW(QString, Issues, "Issues", "Other items not tagged previously", true, )
    NE_PROP_RW(VentGeometry, VentGeometryType, "Vent Geometry", "Vent Geometry", true, )

    NE_PROP_RW(NEDynamicCombo, FirstCompartment, "From", "First compartment", true, )
    NE_PROP_RW(NEDynamicCombo, SecondCompartment, "To", "Second compartment", true, )
    NE_PROP_RW(NEVentCriteria, Criterion, "Criterion", "Criterion", true, )
    NE_PROP_R(vec3f, GhostPosition, "Ghost Position", "Offset position within the associated compartment", true )
    NE_PROP_RW(bool, ShowGhostPosition, "Show Ghost Position", "Show the ghost position", true, )

    NE_PROP_RW(double, VentThickness, "Vent Thickness", "Thickness for Vent", true, )

    Q_ENUMS(VentGeometry)

public:
    FVent(NENodeFactory* factory = 0);
    ~FVent();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

    virtual void copyValues(const FVent* vent);

    const QString& name() const;
    void setName(const QString& name);

    const QString& model3DInfo() const;
    void setModel3DInfo(const QString& model3DInfo);

    // Temporary storage for compartment ids, used when reading json string
    int firstCompartmentId() const;
    void setFirstCompartmentId(int id);
    int secondCompartmentId() const;
    void setSecondCompartmentId(int id);

    void addCompartment(const QString& compName);
    void removeCompartment(const QString& compName);
    void resetCompartments();
    void setFirstCompartmentFromName(const QString& compName, const QString& activeCompName);
    void setSecondCompartmentFromName(const QString& compName, const QString& activeCompName);

    QString categoryType() override;

    // Compute the ghost position within the associated compartment
    void computeGhostPosition(const FZone* comp);
    // Compute the ghost position within the compartment of the parent zone
    void computeGhostPosition();
    // Reset the ghost position to the current position
    void resetGhostPosition();

    void setSnapped(bool m_snapped) override;
    bool isSnapped() override;
    double* getSnappedFaceSize() override;
    void setSnappedFaceSize(double* snappedSize) override;
    void setSnappedPosition(vec3f pos) override;
    vec3f getSnappedPosition();
    void adustFaceXMin(double m);
    void adustFaceYMin(double m);
    void adustFaceZMin(double m);
    void adustFaceXMax(double m);
    void adustFaceYMax(double m);
    void adustFaceZMax(double m);

    //for drawing ghost position
    matrix44f ghostTransform();


private slots:
    void setSnappedAsFalse();
    void adjustSnappedVentPosition();

public slots:
    uint SetPosition(const vec3f& val) override;

private:
    NE_DECL_PRIVATE(FVent)
};

class FVentFactory : public NENodeFactory
{
public:
    FVentFactory(NEManager* m);
    virtual ~FVentFactory();

    NE_DERIVED(NENodeFactory, Public)
};
