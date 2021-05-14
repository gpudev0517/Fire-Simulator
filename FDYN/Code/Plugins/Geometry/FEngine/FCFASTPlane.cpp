#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Core/Base/NEBase.h"
#include "Core/Base/NESceneManager.h"
#include "FCFASTPlane.h"
#include "FEngine.h"
#include "FZone.h"


class FCFASTPlanePrivate
{
public:
    FCFASTPlanePrivate(FCFASTPlane* s) :  S(s)
    {
        //m_spatialType = NESpatial::NESpatialType::kRigid;
    }

//    NESpatial::NESpatialType m_spatialType;
//    NEIndexedTriMesh& m_TriMesh;
    FCFASTPlane::FPlaneDirection m_PlaneDirection;


private:
    FCFASTPlane* S;
};

NE_PROP_DEF(FCFASTPlane::FPlaneDirection, FCFASTPlane, PlaneDirection, FCFASTPlane::X)


static void initProperties()
{
    NE_INIT_PROP(FCFASTPlane, PlaneDirection);
}


FCFASTPlane::FCFASTPlane( NENodeFactory* factory ) : NESpatial( factory )
{
    P = new FCFASTPlanePrivate( this );
    CALL_ONCE(initProperties);

    P->m_PlaneDirection = NE_DEFAULT(PlaneDirection);

}

FCFASTPlane::~FCFASTPlane()
{
    if(P){
        delete P;
        P = NULL;
    }
}

uint FCFASTPlane::init( uint initMode )
{
    uint ret = NESpatial::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FCFASTPlane::resetProperties()
{
    NESpatial::resetProperties();

    ResetPlaneDirection();

}

const FCFASTPlane::FPlaneDirection& FCFASTPlane::PlaneDirection() const
{
    return P->m_PlaneDirection;
}

uint FCFASTPlane::SetPlaneDirection(const FCFASTPlane::FPlaneDirection &val){
    NE_PROP_SET_EVAL(PlaneDirection, P, val);

    return NE::kReturnSuccess;
}



//NESpatial::NESpatialType FCFASTPlane::spatialType()
//{
//    return P->m_spatialType;
//}

//void FCFASTPlane::setSpatialType(NESpatial::NESpatialType type)
//{
//    P->m_spatialType = type;
//}

void FCFASTPlane::render()
{
}

void FCFASTPlane::renderPickable()
{
}

void FCFASTPlane::renderPicked()
{
}


//void FCFASTPlane::createMesh()
//{
//    P->m_TriMesh.resize();

//    NECTNPVertex v;


//    v.position = vec3f(-XSize()/2,  FloorElevation(), -ZSize()/2);
//    v.normal = vec3f(0.0f, 1.0f, 0.0f);
//    switch (P->m_Swap)
//    {
//        case 0:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//        case 1:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//        case 2:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//        case 3:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//    }
//    P->m_TriMesh.appendVertex(v);
//    v.position = vec3f(-XSize()/2, FloorElevation(), ZSize()/2);
//    switch (P->m_Swap)
//    {
//        case 0:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//        case 1:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//        case 2:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//        case 3:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//    }
//    P->m_TriMesh.appendVertex(v);
//    v.position = vec3f(XSize()/2, FloorElevation(), ZSize()/2);
//    switch (P->m_Swap)
//    {
//        case 0:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//        case 1:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//        case 2:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//        case 3:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//    }
//    P->m_TriMesh.appendVertex(v);
//    v.position = vec3f(XSize()/2, FloorElevation(), -ZSize()/2);
//    switch (P->m_Swap)
//    {
//        case 0:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//        case 1:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//        case 2:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//        case 3:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//    }
//    P->m_TriMesh.appendVertex(v);

//    //Add triangles now
//    NETriangleIndexed t1(1, 3, 0), t2(1, 2, 3);
//    P->m_TriMesh.appendTriangle(t1);
//    P->m_TriMesh.appendTriangle(t2);

//    if( NEBASE.guiMode() )
//        if(P->m_TriMesh.numTriangles() > 0)
//            P->m_TriMesh.updateDrawingBuffers(GL_DYNAMIC_DRAW);

//    P->m_normal = v.normal;
//}

//NEIndexedTriMesh& FCFASTPlane::mesh()
//{
//    return P->m_TriMesh;
//}



FCFASTPlaneFactory::FCFASTPlaneFactory(NEManager* m) : NENodeFactory(m) {}

FCFASTPlaneFactory::~FCFASTPlaneFactory(){}

QString FCFASTPlaneFactory::nodeName() { return "CFASTPlane"; }

uint FCFASTPlaneFactory::nodeType(){ return NE::kObjectGroup;}

QString FCFASTPlaneFactory::nodeInherits() { return "Visuals";}

QString FCFASTPlaneFactory::objectName() { return "CFASTPlane"; }

QString FCFASTPlaneFactory::nodeVersion() { return "0.1"; }

QString FCFASTPlaneFactory::nodeVendor() { return "F Dynamics"; }

QString FCFASTPlaneFactory::nodeHelp() { return "CFASTPlane"; }

NEObject*FCFASTPlaneFactory::createInstance() { return new FCFASTPlane( this );	}



