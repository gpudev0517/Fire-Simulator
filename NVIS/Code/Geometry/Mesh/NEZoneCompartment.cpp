#include "NEZoneCompartment.h"

#include "Resource/Mesh/NEMeshContainer.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Utility/geometry.h"

NE_PROP_DEF(QString, NEZoneCompartment, Desc, QString(""))
NE_PROP_DEF(vec3f, NEZoneCompartment, Thickness, vec3f( 0.0f, 0.0f, 0.f ))

QPair<NEZoneCompartment *, int> NEZoneCompartment::s_SelectedCompartment = QPair<NEZoneCompartment *, int>();
QPair<NEZoneCompartment *, int> NEZoneCompartment::s_TargetCompartment = QPair<NEZoneCompartment *, int>();

class NEZoneCompartmentPrivate
{
public:
    NEZoneCompartmentPrivate(NEZoneCompartment* s) : S(s)
    {
    }

    QString m_Desc;
    vec3f m_Thickness;

    uint applyUnitScale(vec3d unitScale, bool bApplyAfterLocalizing);
    bool applyThickness();

private:
    NEZoneCompartment* S;
};

static void initProperties()
{
    NE_INIT_PROP(NEZoneCompartment, Desc);
    NE_INIT_PROP(NEZoneCompartment, Thickness);
}

NEZoneCompartment::NEZoneCompartment(NENodeFactory* factory) : NEZoneItem(factory)
{
    P = new NEZoneCompartmentPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);
    P->m_Desc = NE_DEFAULT(Desc);
    P->m_Thickness = NE_DEFAULT(Thickness);

    hideProperty( "Thickness", true );

    SetRenderMode( NESpatial::QuadWireframe );
}

NEZoneCompartment::~NEZoneCompartment()
{
    delete P;
}

uint NEZoneCompartment::init(uint initMode)
{
    uint ret = NEZoneItem::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    meshContainer().createCuboidMesh( Color() );
    P->applyUnitScale( vec3d( 1.0f, 1.0f, 1.0f ), false );
    P->applyThickness( );

    emit initObject();

    setOutputs( outputs() | NE::kIOTriangleMesh );

    return NE::kReturnSuccess;
}

void NEZoneCompartment::resetProperties()
{
    NEZoneItem::resetProperties();

    ResetDesc();
    ResetThickness();
}

uint NEZoneCompartment::SetPosition(const vec3f &val)
{
    return NEZoneItem::SetPosition(val);

//    if(s_TargetCompartment.first == NULL ){
//        return NEZoneItem::SetPosition(val);
//    }
//    else {
//        NEZoneCompartment *targetCompart = s_TargetCompartment.first;
//        int faceId = s_TargetCompartment.second;

//        NEIndexedTriMesh &targetTriMesh = targetCompart->triangleMesh();
//        std::vector<NEPrimitivePolygon> &targetFaces = targetCompart->quadFaces();

//        vec3f p1 = targetCompart->transform() * targetTriMesh.vertices()[targetFaces[faceId].triangles[0].p1].position;
//        vec3f p2 = targetCompart->transform() * targetTriMesh.vertices()[targetFaces[faceId].triangles[0].p2].position;
//        vec3f p3 = targetCompart->transform() * targetTriMesh.vertices()[targetFaces[faceId].triangles[0].p3].position;

//        vec3f targetNormal = vec3f::crossProduct(p2-p1, p3-p1);
//        targetNormal.normalize();

//        vec3f delta = val - Position();

//        float t = vec3f::dotProduct(delta, targetNormal);

//        if( t == 0.0f){
//            return NEZoneItem::SetPosition(val);
//        }
//        else{
//            targetNormal = targetNormal * t;

//            vec3f deltaOnPlane = delta - targetNormal;
//            return NEZoneItem::SetPosition(deltaOnPlane + Position());
//        }
//    }
}

uint NEZoneCompartmentPrivate::applyUnitScale(vec3d unitScale, bool bApplyAfterLocalizing)
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
            position *= vec3f( unitScale.x(), unitScale.y(), unitScale.z());
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
            position *= vec3f( unitScale.x(), unitScale.y(), unitScale.z());
            position += S->aabb().minPos();
        }
    }

    S->updateAABB();
    S->computeBoundingBox();

    emit S->updateDrawingBuffers();
    return NE::kReturnSuccess;
}

bool NEZoneCompartmentPrivate::applyThickness()
{
    return true;
}

bool NEZoneCompartment::updateAABB()
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

void NEZoneCompartment::loadZoneFile(const QString &filePath)
{
    return;
}

void NEZoneCompartment::clearZoneFile()
{
    return;
}

void NEZoneCompartment::reloadZoneFile()
{
    return;
}

QList<QString> &NEZoneCompartment::scenarios()
{
    static QList<QString> nullList;

    return nullList;
}

QList<QString> &NEZoneCompartment::otherItems()
{
    static QList<QString> nullList;

    return nullList;
}

const QString& NEZoneCompartment::Desc() const
{
    return P->m_Desc;
}

uint NEZoneCompartment::SetDesc(const QString& val)
{
    NE_PROP_SET_EVAL(Desc, P, val);

    return NE::kReturnSuccess;
}

QString NEZoneCompartment::nodeDescription()
{
    return factory().nodeName();
}

uint NEZoneCompartment::SetThickness(const vec3f& val)
{
    NE_PROP_SET_EVAL(Thickness, P, val);

    return NE::kReturnSuccess;
}

const vec3f& NEZoneCompartment::Thickness() const
{
    return P->m_Thickness;
}

uint NEZoneCompartment::SetZonePositionFromWorld(const vec3f& worldPosition)
{
    return NE::kReturnSuccess;
}

uint NEZoneCompartment::SetZoneOrientationFromWorld(const quatf& worldRotation)
{
    return NE::kReturnSuccess;
}

uint NEZoneCompartment::SetZoneScaleFromWorld(const vec3f& worldScale)
{
    return NE::kReturnSuccess;
}

NEZoneCompartmentFactory::NEZoneCompartmentFactory(NEManager* m) : NENodeFactory(m)
{

}

NEZoneCompartmentFactory::~NEZoneCompartmentFactory()
{

}

QString NEZoneCompartmentFactory::nodeName()
{
    return "Zone Compartment";
}

QStringList NEZoneCompartmentFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("NEZoneCompartment");

    return aNames;
}

uint NEZoneCompartmentFactory::nodeType()
{
    return NE::kObjectGroup;
}

QString NEZoneCompartmentFactory::nodeInherits()
{
    return "Groups";
}

QString NEZoneCompartmentFactory::objectName()
{
    return "NEZoneCompartment";
}

QString NEZoneCompartmentFactory::nodeVersion()
{
    return "0.1";
}

QString NEZoneCompartmentFactory::nodeVendor()
{
    return "NDynamics";
}

QString NEZoneCompartmentFactory::nodeHelp()
{
    return "Describing a Fire Zone";
}

NEObject* NEZoneCompartmentFactory::createInstance()
{
    return new NEZoneCompartment(this);
}
