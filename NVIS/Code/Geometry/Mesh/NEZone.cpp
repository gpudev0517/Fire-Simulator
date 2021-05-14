#include "NEZone.h"
#include "NEZoneItem.h"

#include "Resource/Mesh/NEMeshContainer.h"
#include "Mesh/NEZoneCompartment.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

class NEZonePrivate
{
public:
    NEZonePrivate(NEZone* s) : S(s)
    {
    }

    uint m_ID;
    QString m_Type;
    QString m_Desc;

    QString m_OriginalName;
    QMultiMap<QString, QString> m_ItemNameTree;

    uint applyUnitScale(vec3d unitScale, bool bApplyAfterLocalizing);

private:
    NEZone* S;
};

NE_PROP_DEF(uint, NEZone, ID, 0)
NE_PROP_DEF(QString, NEZone, Type, QString(""))
NE_PROP_DEF(QString, NEZone, Desc, QString(""))

static void initProperties()
{
    NE_INIT_PROP(NEZone, ID );
    NE_INIT_PROP(NEZone, Type);
    NE_INIT_PROP(NEZone, Desc);
}

NEZone::NEZone(NENodeFactory* factory) : NESpatialMesh(factory)
{
    P = new NEZonePrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);
    P->m_ID = NE_DEFAULT(ID);
    P->m_Type = NE_DEFAULT(Type);
    P->m_Desc = NE_DEFAULT(Desc);

    SetRenderMode( NESpatial::Hidden );
}

NEZone::~NEZone()
{
    delete P;
}

uint NEZone::init(uint initMode)
{
    uint ret = NESpatialMesh::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    meshContainer().createCuboidMesh( Color() );
    P->applyUnitScale( vec3d( 1.0f, 1.0f, 1.0f ), false );

    emit initObject();

    setOutputs( outputs() | NE::kIOTriangleMesh );

    return NE::kReturnSuccess;
}

void NEZone::resetProperties()
{
    NESpatialMesh::resetProperties();

    ResetID();
    ResetType();
    ResetDesc();
}

uint NEZonePrivate::applyUnitScale(vec3d unitScale, bool bApplyAfterLocalizing)
{
    std::vector<NECTNPVertex>& vertices =  S->triangleMesh().vertices();
    if (vertices.size() == 0)
        return NE::kReturnSuccess;

    if(bApplyAfterLocalizing)
    {
        for (unsigned i = 0; i != vertices.size(); ++i)
        {
            vec3f& position = vertices[i].position;
            position += S->center();
            position *= vec3f( unitScale.x(), unitScale.y(), unitScale.z()  );
        }

        S->updateAABB();
        S->computeCenter();
    }
    else
    {
        for (unsigned i = 0; i != vertices.size(); ++i)
        {
            vec3f& position = vertices[i].position;
            position -= S->aabb().minPos();
            position *= vec3f( unitScale.x() , unitScale.y() , unitScale.z() );
            position += S->aabb().minPos();
        }
    }

    S->updateAABB();
    S->computeBoundingBox();

    emit S->updateDrawingBuffers();
    return NE::kReturnSuccess;
}

bool NEZone::updateAABB()
{
    std::vector<NECTNPVertex>& v = triangleMesh().vertices();

    if( v.size() <= 0 )
        return false;

    vec3f minPos = v[0].position;
    vec3f maxPos = v[0].position;

    for( unsigned i = 0; i < v.size(); ++i )
    {
        vec3f po = v[i].position;
        if ( po.x() < minPos.x() ) minPos.x() = po.x();
        if ( po.y() < minPos.y() ) minPos.y() = po.y();
        if ( po.z() < minPos.z() ) minPos.z() = po.z();
        if ( po.x() > maxPos.x() ) maxPos.x() = po.x();
        if ( po.y() > maxPos.y() ) maxPos.y() = po.y();
        if ( po.z() > maxPos.z() ) maxPos.z() = po.z();
    }
    AABB val;

    val.insertPoint( minPos );
    val.insertPoint( maxPos );
    setAABB( val );
    return aabb().valid();
}

const QString& NEZone::originalName() const
{
    return P->m_OriginalName;
}

uint NEZone::setOriginalName(const QString& val)
{
    P->m_OriginalName = val;

    return NE::kReturnSuccess;
}

QList<QString> &NEZone::scenarios()
{
    static QList<QString> nullList;

    return nullList;
}

QList<QString> &NEZone::otherItems()
{
    static QList<QString> nullList;

    return nullList;
}

QList<QString> NEZone::itemNameChildren(const QString& name) const
{
    // Sort the children, otherwise they would be in reverse order of insertion
    QList<QString> children = P->m_ItemNameTree.values(name);
    std::sort(children.begin(), children.end());

    return children;
}

bool NEZone::itemNameTreeContains(const QString& childName, const QString& parentName) const
{
    return P->m_ItemNameTree.contains(parentName, childName);
}

void NEZone::addToItemNameTree(const QString& childName, const QString& parentName)
{
    if (!P->m_ItemNameTree.contains(parentName, childName))
        P->m_ItemNameTree.insert(parentName, childName);
}

void NEZone::removeFromItemNameTree(const QString& childName, const QString& parentName)
{
    // Clear the sub-tree under child name
    for (auto& grandChildName : P->m_ItemNameTree.values(childName))
        removeFromItemNameTree(grandChildName, childName);
    P->m_ItemNameTree.remove(childName);

    // Remove the branche
    P->m_ItemNameTree.remove(parentName, childName);
}

void NEZone::removeAllFromItemNameTree(const QString& name)
{
    // Clear the sub-tree under name
    for (auto& childName : P->m_ItemNameTree.values(name))
        removeFromItemNameTree(childName, name);
    P->m_ItemNameTree.remove(name);

    // Clear the branches to which name is the child
    for (auto& parentName : P->m_ItemNameTree.keys(name))
        P->m_ItemNameTree.remove(parentName, name);
}

void NEZone::renameInItemNameTree(const QString& oldName, const QString& newName)
{
    // Replace name in values
    for (QMap<QString, QString>::iterator iter = P->m_ItemNameTree.begin(); iter != P->m_ItemNameTree.end(); iter++)
    {
        if (iter.value() == oldName)
            iter.value() = newName;
    }

    // Replace name in keys
    QMultiMap<QString, QString>::iterator iter = P->m_ItemNameTree.find(oldName);
    while (iter != P->m_ItemNameTree.end())
    {
        QString& value = iter.value();
        P->m_ItemNameTree.remove(oldName, value);
        P->m_ItemNameTree.insert(newName, value);

        iter = P->m_ItemNameTree.find(oldName);
    }
}

void NEZone::clearItemNameTree()
{
    P->m_ItemNameTree.clear();
}

QList<QString> &NEZone::zoneContainers()
{
    static QList<QString> nullList;

    return nullList;
}

const uint& NEZone::ID() const
{
    return P->m_ID;
}

uint NEZone::SetID(const uint& val)
{
    NE_PROP_SET_EVAL(ID, P, val);

    return NE::kReturnSuccess;
}

const QString& NEZone::Type() const
{
    return P->m_Type;
}

uint NEZone::SetType(const QString& val)
{
    NE_PROP_SET_EVAL(Type, P, val);

    return NE::kReturnSuccess;
}

const QString& NEZone::Desc() const
{
    return P->m_Desc;
}

uint NEZone::SetDesc(const QString& val)
{
    NE_PROP_SET_EVAL(Desc, P, val);

    return NE::kReturnSuccess;
}

QString NEZone::nodeDescription()
{
    return factory().nodeName();
}

NEZoneFactory::NEZoneFactory(NEManager* m) : NENodeFactory(m)
{

}

NEZoneFactory::~NEZoneFactory()
{

}

QString NEZoneFactory::nodeName()
{
    return "Zone";
}

QStringList NEZoneFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("NEZone");

    return aNames;
}

uint NEZoneFactory::nodeType()
{
    return NE::kObjectGroup;
}

QString NEZoneFactory::nodeInherits()
{
    return "Groups";
}

QString NEZoneFactory::objectName()
{
    return "NEZone";
}

QString NEZoneFactory::nodeVersion()
{
    return "0.1";
}

QString NEZoneFactory::nodeVendor()
{
    return "NDynamics";
}

QString NEZoneFactory::nodeHelp()
{
    return "Describing a Fire Zone";
}

NEObject* NEZoneFactory::createInstance()
{
    return new NEZone(this);
}
