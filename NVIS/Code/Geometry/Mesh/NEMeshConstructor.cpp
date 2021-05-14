#include "Base/NESceneManager.h"

#include "NEMeshConstructor.h"


class NEMeshConstructorPrivate
{
public:
    NEMeshConstructorPrivate(NEMeshConstructor* s) : S(s)
    {

    }
    bool m_ShowTimings;

private:
    NEMeshConstructor* S;
};

NE_PROP_DEF(bool, NEMeshConstructor, ShowTimings, (false))


static void initProperties()
{
    NE_INIT_PROP(NEMeshConstructor, ShowTimings);

}
void NEMeshConstructor::resetProperties()
{
    NESpatial::resetProperties();
    ResetShowTimings();

}

NEMeshConstructor::NEMeshConstructor(NENodeFactory* val) : NESpatial(val)
{
    addNodeTypeToNodeGraphSceneType("NEMeshConstructor", NENode::SceneType::Simulation );

    P = new NEMeshConstructorPrivate(this);

    P->m_ShowTimings = NE_DEFAULT(ShowTimings);
    setSaveVersion(NEMeshSaveVersion::Version_V1);

    // Initializes the properties, called once per class
    CALL_ONCE(initProperties);
}

NEMeshConstructor::~NEMeshConstructor()
{
    delete P;
}

const bool& NEMeshConstructor::ShowTimings() const
{
    return P->m_ShowTimings;
}

uint NEMeshConstructor::SetShowTimings(const bool& b)
{
    NE_PROP_SET_AND_EVAL(ShowTimings, P, b);
    return NE::kReturnSuccess;
}

uint NEMeshConstructor::init(uint initMode)
{
    NESpatial::init(initMode);

    return NE::kReturnSuccess;
}




void NEMeshConstructor::render()
{
}


void NEMeshConstructor::renderPickable()
{
}

void NEMeshConstructor::renderPicked()
{
}

/*!
*/
uint NEMeshConstructor::saveFrameData( QString path )
{
    // If caching is completely off, return early
    if(!CacheData())
        return NE::kReturnSuccess;

    // Get frame number from the current frame
    QString frameNumber;
    frameNumber.sprintf( "/%06d", NESCENE.CurrentFrame() );

    //meshes are always stored in an external file
    QDir meshDataDir( path );

    meshDataDir.mkdir("TriMesh");
    QString meshPath1 = path + "/TriMesh/" + frameNumber + ".obj";
    m_TriangleMesh.saveToFile( meshPath1, false, saveVersion() );


    return NE::kReturnSuccess;
}

void NEMeshConstructor::update()
{

}

/*!
*/
uint NEMeshConstructor::loadFrameData(QString path)
{
    // Compute the frame number from the current frame

    m_TriangleMesh.vertices().clear();
    m_TriangleMesh.triangles().clear();

    QString frameNumber;
    frameNumber.sprintf( "/%06d", NESCENE.CurrentFrame() );

    QString meshPath = path + "/TriMesh/" + frameNumber + ".obj";
    m_TriangleMesh.loadFromFile( meshPath, false, false, 1.0, true );

    //meshPath = path + "/MotionVector/" + frameNumber + ".motion";
    //m_TriangleMesh.loadVelocitiesFromFile( meshPath, false );

    //move this part to an extra method or into trianglemesh
    //if(m_TriangleMesh.numTriangles() > 0)
    //m_TriangleMesh.updateDrawingBuffers( GL_DYNAMIC_DRAW );
    return NE::kReturnSuccess;
}




const NEIndexedTriMesh& NEMeshConstructor::triangleMesh() const
{
    return m_TriangleMesh;
}
