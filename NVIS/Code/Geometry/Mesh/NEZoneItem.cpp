#include "NEZoneItem.h"
#include "Mesh/NESpatialMesh.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Resource/Mesh/NEMeshContainer.h"

class NEZoneItemPrivate
{
public:
    NEZoneItemPrivate(NEZoneItem* s) : S(s)
    {
    }

    /* Functions */
    void computeThicknessAndBaseSizeVecs();

    /* Properties */
    uint m_ID;
    QString m_Type;
    vec3f m_DirVec;
    vec3f m_BaseSizeVec;

    /* Other members */
    QString m_OriginalName;
    QList< QString > m_otherItems;

private:
    NEZoneItem* S;
};

NE_PROP_DEF(uint, NEZoneItem, ID, 0)
NE_PROP_DEF(QString, NEZoneItem, Type, QString(""))

static void initProperties()
{
    NE_INIT_PROP(NEZoneItem, ID);
    NE_INIT_PROP(NEZoneItem, Type);
}

NEZoneItem::NEZoneItem(NENodeFactory* factory) : NESpatialMesh(factory)
{
    P = new NEZoneItemPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);

    P->m_ID = NE_DEFAULT(ID);
    P->m_Type = NE_DEFAULT(Type);
}

NEZoneItem::~NEZoneItem()
{
    delete P;
}

QString NEZoneItem::nodeDescription()
{
    return factory().nodeName();
}

uint NEZoneItem::init(uint initMode)
{
    uint ret = NESpatialMesh::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    P->computeThicknessAndBaseSizeVecs();

    return ret;
}

void NEZoneItem::resetProperties()
{
    NESpatialMesh::resetProperties();

    ResetID();
    ResetType();

    P->computeThicknessAndBaseSizeVecs();
}

QList<QString>  &NEZoneItem::otherItems()
{
    return P->m_otherItems;
}

const uint& NEZoneItem::ID() const
{
    return P->m_ID;
}

uint NEZoneItem::SetID(const uint& val)
{
    NE_PROP_SET_EVAL(ID, P, val);

    return NE::kReturnSuccess;
}

const QString& NEZoneItem::Type() const
{
    return P->m_Type;
}

uint NEZoneItem::SetType(const QString& val)
{
    NE_PROP_SET_EVAL(Type, P, val);

    return NE::kReturnSuccess;
}

const vec3f& NEZoneItem::DirVec() const
{
    return P->m_DirVec;
}

const vec3f& NEZoneItem::BaseSizeVec() const
{
    return P->m_BaseSizeVec;
}

const QString& NEZoneItem::originalName() const
{
    return P->m_OriginalName;
}

uint NEZoneItem::setOriginalName(const QString& val)
{
    P->m_OriginalName = val;

    return NE::kReturnSuccess;
}

void NEZoneItem::updateTransform()
{
    updateWorldTransform();
    updateChildrenTransform();
    //updateParentAABB();

    computeBoundingBox();

    P->computeThicknessAndBaseSizeVecs();
}

QString NEZoneItem::categoryType()
{
    return "None";
}

QString NEZoneItem::otherCategoryType()
{
    return "";
}

void NEZoneItem::setItemType(int itype)
{

}
int NEZoneItem::itemType()
{
    return 0;
}

void NEZoneItem::setSnapped(bool m_snapped)
{

}

bool NEZoneItem::isSnapped()
{
    return false;
}


double* NEZoneItem::getSnappedFaceSize()
{
    double* p = new double();
    return p;
}

void NEZoneItem::setSnappedFaceSize(double* p)
{

}

void NEZoneItem::setSnappedPosition(vec3f p)
{

}

void NEZoneItemPrivate::computeThicknessAndBaseSizeVecs()
{
    // All of these should be replaced by the use of the OBB when ready
    m_DirVec = vec3f(S->transform()*vec4f(0.0f, 1.0f, 0.0f, 0.0f));
    m_DirVec.x() = std::abs(m_DirVec.x());
    m_DirVec.y() = std::abs(m_DirVec.y());
    m_DirVec.z() = std::abs(m_DirVec.z());
    if (std::abs(m_DirVec.x()) <= FEps)
        m_DirVec.x() = 0.0f;
    if (std::abs(m_DirVec.y()) <= FEps)
        m_DirVec.y() = 0.0f;
    if (std::abs(m_DirVec.z()) <= FEps)
        m_DirVec.z() = 0.0f;

    vec3f baseSizeVec1 = vec3f(S->transform()*vec4f(1.0f, 0.0f, 0.0f, 0.0f));
    vec3f baseSizeVec2 = vec3f(S->transform()*vec4f(0.0f, 0.0f, 1.0f, 0.0f));
    // Assume the vent is axis-aligned
    m_BaseSizeVec = baseSizeVec1+baseSizeVec2;
    m_BaseSizeVec.x() = std::abs(m_BaseSizeVec.x());
    m_BaseSizeVec.y() = std::abs(m_BaseSizeVec.y());
    m_BaseSizeVec.z() = std::abs(m_BaseSizeVec.z());
    if (m_BaseSizeVec.x() <= FEps)
        m_BaseSizeVec.x() = 0.0f;
    if (m_BaseSizeVec.y() <= FEps)
        m_BaseSizeVec.y() = 0.0f;
    if (m_BaseSizeVec.z() <= FEps)
        m_BaseSizeVec.z() = 0.0f;
}
