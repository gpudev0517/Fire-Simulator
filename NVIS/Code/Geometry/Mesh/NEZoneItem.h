#pragma once
#include "Utility/FilePath.h"
#include "Mesh/NESpatialMesh.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(NEZoneItem))

class NECEXP_GEOMETRY NEZoneItem : public NESpatialMesh
{
    Q_OBJECT
    NE_CLASS("Zone Item Properties")

    NE_PROP_RW(uint, ID, "ID", "Zone Item ID", true,)
    NE_PROP_RW(QString, Type, "Type", "Type String", true,)
    NE_PROP_R(vec3f, DirVec, "Direction Size", "Vector representing the thickness and its direction", false)
    NE_PROP_R(vec3f, BaseSizeVec, "Base Size", "Vector representing the base size and its two directions", false)

public:
    NEZoneItem( NENodeFactory* factory );
    virtual ~NEZoneItem();

    uint init(uint initMode) override;
    void resetProperties() override;

    const QString& originalName() const;
    uint setOriginalName(const QString& val);

    virtual QList< QString >& otherItems();

    virtual QString nodeDescription();

    virtual QString categoryType();
    virtual QString otherCategoryType();

    virtual void setItemType(int itype);
    virtual int itemType();

    virtual void updateTransform() override;

    virtual bool isSnapped();
    virtual void setSnapped(bool m_snapped);
    virtual double* getSnappedFaceSize();
    virtual void setSnappedFaceSize(double *p);
    virtual void setSnappedPosition(vec3f p);

private:
    NE_DECL_PRIVATE(NEZoneItem)
};

