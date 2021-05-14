#include "Utility/geometry.h"
#include "NEVolumeNode.h"
#include "NEVolume.h"
#include "Manager/NESimulationNodeGraph.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Utility/NEColorRamp.h"
#include "Base/NENodeFactory.h"

class NEVolumeNodePrivate
{

public:
    std::shared_ptr< NEVolume >    m_Volume;

    double m_EvalStart ;
    double m_EvalEnd  ;
    NEVolumeNode::DisplayMode m_DispMode;

    vec3f* m_vertices;
    vec3f* m_indices;
    bool    m_FlipXAxis;
    bool    m_FlipYAxis;
    bool    m_FlipZAxis;
};




NE_PROP_DEF(NEVolumeNode::DisplayMode, NEVolumeNode, DispMode, NEVolumeNode::DisplayMode::Points)
NE_PROP_DEF(double, NEVolumeNode, EvalStart, 0.0f)
NE_PROP_DEF(double, NEVolumeNode, EvalEnd, 10.0f)
NE_PROP_DEF(NEBool, NEVolumeNode, Eval, NEBool(1))
NE_PROP_DEF(bool, NEVolumeNode, FlipXAxis, false)
NE_PROP_DEF(bool, NEVolumeNode, FlipYAxis, false)
NE_PROP_DEF(bool, NEVolumeNode, FlipZAxis, false)

static void initProperties()
{
    NE_INIT_PROP(NEVolumeNode,DispMode);
    NE_INIT_PROP(NEVolumeNode,EvalStart);
    NE_INIT_PROP(NEVolumeNode,EvalEnd);
    NE_INIT_PROP(NEVolumeNode, FlipXAxis);
    NE_INIT_PROP(NEVolumeNode, FlipYAxis);
    NE_INIT_PROP(NEVolumeNode, FlipZAxis);
}

///
/// \brief NEVolumeNode::NEVolumeNode
/// \param factory
///


NEVolumeNode::NEVolumeNode( NENodeFactory *factory ): NESpatial(factory)
{

    addNodeTypeToNodeGraphSceneType("Volumes", NENode::SceneType::Modeling );
    addNodeTypeToNodeGraphSceneType("Volumes", NENode::SceneType::Simulation );
    addNodeTypeToNodeGraphSceneType("Volumes", NENode::SceneType::Visualization );

    P = new NEVolumeNodePrivate;
    CALL_ONCE(initProperties);

    P->m_Volume = std::shared_ptr< NEVolume >( new NEVolume );
    P->m_EvalStart = NE_DEFAULT(EvalStart);
    P->m_EvalEnd   = NE_DEFAULT(EvalEnd);
    P->m_DispMode = NE_DEFAULT(DispMode);
    P->m_FlipXAxis  = NE_DEFAULT(FlipXAxis);
    P->m_FlipYAxis  = NE_DEFAULT(FlipYAxis);
    P->m_FlipZAxis  = NE_DEFAULT(FlipZAxis);
}


///
/// \brief NEVolumeNode::~NEVolumeNode
///

NEVolumeNode::~NEVolumeNode()
{
    delete P;

}

const double& NEVolumeNode::EvalStart() const
{
    return P->m_EvalStart;
}

uint NEVolumeNode::SetEvalStart( const double& arg )
{
    P->m_EvalStart = arg;
    return 1;
}


const double& NEVolumeNode:: EvalEnd() const
{
    return P->m_EvalEnd;
}

uint NEVolumeNode::SetEvalEnd( const double& arg )
{
    P->m_EvalEnd = arg;
    return 1;
}


uint NEVolumeNode::SetDispMode( const NEVolumeNode::DisplayMode& arg )
{
    P->m_DispMode = arg;

    if( NEBASE.initialized() && P->m_Volume )
        emit createVisualizer( this );

    return 1;
}

const NEVolumeNode::DisplayMode&  NEVolumeNode::DispMode() const
{
    return P->m_DispMode;
}


uint NEVolumeNode::SetEval(const NEBool& reset)
{
    if( P->m_Volume )
    {
        if( P->m_Volume->Volume() )
            NESCENE.NodeGraph()->evaluate( this );
    }

    return 1;
}

const NEBool& NEVolumeNode::Eval() const
{
    static NEBool m = NEBool(1);
    return m;
}

uint NEVolumeNode::SetFlipXAxis(const bool& flipXAxis)
{
    NE_PROP_SET_EVAL(FlipXAxis, P, flipXAxis);
    return NE::kReturnSuccess;
}

uint NEVolumeNode::SetFlipYAxis(const bool& flipYAxis)
{
    NE_PROP_SET_EVAL(FlipYAxis, P, flipYAxis);
    return NE::kReturnSuccess;
}

uint NEVolumeNode::SetFlipZAxis(const bool& flipZAxis)
{
    NE_PROP_SET_EVAL(FlipZAxis, P, flipZAxis);
    return NE::kReturnSuccess;
}

const bool& NEVolumeNode::FlipXAxis() const
{
    return P->m_FlipXAxis;
}

const bool& NEVolumeNode::FlipYAxis() const
{
    return P->m_FlipYAxis;
}

const bool& NEVolumeNode::FlipZAxis() const
{
    return P->m_FlipZAxis;
}



uint NEVolumeNode::init( uint initMode )
{
    NESpatial::init( initMode );
    emit createVisualizer( this );

    return NE::kReturnSuccess;
}

void NEVolumeNode::resetProperties()
{
    NESpatial::resetProperties();

    ResetEvalStart();
    ResetEvalEnd();
    ResetDispMode();
    ResetEval();
    ResetFlipXAxis();
    ResetFlipYAxis();
    ResetFlipZAxis();
}

///
/// \brief NEVolumeNode::inputVolume
/// \param source
/// \param type
/// \param slot
/// \return
///

std::shared_ptr<NEVolume> NEVolumeNode::inputVolume( NENode *source, uint type, int slot )
{
    //std::cout << "Type " << tydpe << std::endl;
    if( type != NE::kIOVolume )
    {
        return NULL;
    }

    if( qobject_cast< NEVolumeNode* >( source ) )
    {
        //std::cout << "Casting " << std::endl;
        std::shared_ptr< NEVolume > vol;
        NEVolumeNode* node = qobject_cast< NEVolumeNode* >( source );
        if( node )
            vol = node->Volume();
        //std::cout << "Vol: " << vol << std::endl;
        return vol;
    }
    else
    {
        return NULL;
    }
}

///
/// \brief NEVolumeNode::update
/// \return
///

uint NEVolumeNode::update()
{
    return NE::kReturnSuccess;
}

std::shared_ptr<NEVolume> NEVolumeNode::Volume()
{
    return P->m_Volume;
}

void NEVolumeNode::SetVolume(const std::shared_ptr<NEVolume> &arg)
{
    P->m_Volume = arg;
}


bool NEVolumeNode::updateAABB()
{
    if( P->m_Volume == NULL )
        return false;

    openvdb::FloatGrid::Ptr grid = P->m_Volume->Volume();

    openvdb::CoordBBox bbox;
    if( grid->empty() )
        return false;

    grid->tree().evalLeafBoundingBox( bbox );

    AABB val;

    vec3f pMin( bbox.min().x()*0.5, bbox.min().y()*0.5, bbox.min().z()*0.5 );
    vec3f pMax( bbox.max().x()*0.5, bbox.max().y()*0.5, bbox.max().z()*0.5 );

    val.insertPoint(pMin);
    val.insertPoint(pMax);

    setAABB(val);

    return aabb().valid();
}



void NEVolumeNode::removeSource(NENode* source, uint type, uint sIndex, uint tIndex)
{
    if( P->m_Volume )
    {
        P->m_Volume->SetVolume( openvdb::FloatGrid::create() );
    }
    emit createVisualizer( this );

    return NENode::removeSource( source, type, sIndex, tIndex );
}




uint NEVolumeNode::eval()
{
    emit createVisualizer( this );
    return NE::kReturnSuccess;
}



bool NEVolumeNode::Active()
{
    if (!(Behavior() == NENode::Active))
        return false;

    const double startTime = EvalStart() ;
    const double endTime   = EvalEnd();
    if( NESCENE.ElapsedTime() >= startTime && NESCENE.ElapsedTime() <= endTime )
    {
        return true;
    }

    return false;
}


///
/// \brief NEVolumeNode::render
///

void NEVolumeNode::render()
{
}



void NEVolumeNode::renderPickable()
{
}


void NEVolumeNode::renderPicked()
{
}

/*!
*/
uint NEVolumeNode::saveFrameData( QString path , const int framenum )
{
    // If caching is completely off, return early
    if(!CacheData())
        return NE::kReturnSuccess;

    // Get frame number from the current frame
    QString frameNumber;
    frameNumber.sprintf("/%06d", framenum );

    //volumes are always stored in an external file
    QDir meshDataDir( path );

    meshDataDir.mkdir( "Volume" );
    QString meshPath1 = path + "/Volume/" + frameNumber + ".vdb";

    // Create a VDB file object.
    openvdb::io::File file( meshPath1.toStdString().c_str() );
    //if( m_volume )
    //    file.write( m_volume->volume() );

    file.close();

    return NE::kReturnSuccess;
}

/*!
*/
uint NEVolumeNode::loadFrameData(QString path, const int framenum)
{
    // Compute the frame number from the current frame

    QString frameNumber;
    frameNumber.sprintf("/%06d", framenum );

    QString meshPath = path + "/Volume/" + frameNumber + ".vdb";

    // Create a VDB file object.
    openvdb::io::File file( meshPath.toStdString().c_str() );

    QFile vfile( meshPath );
    if( !vfile.exists() )
        return false;

    //
    // Open the file.  This reads the file header, but not any grids.
    //
    if( file.open() == false )
        return false;

    //
    // Loop over all grids in the file and retrieve a shared pointer
    // to the first one
    //

    openvdb::GridBase::Ptr baseGrid;
    for( openvdb::io::File::NameIterator nameIter = file.beginName(); nameIter != file.endName(); ++nameIter )
    {
        P->m_Volume = std::shared_ptr< NEVolume >( new NEVolume );
        baseGrid = file.readGrid( nameIter.gridName() );
        P->m_Volume->SetVolume(  openvdb::gridPtrCast< openvdb::FloatGrid >(baseGrid) );
        break;
    }

    file.close();

    return NE::kReturnSuccess;
}



