#include "NEVolumeRenderer.h"

#include "Base/NEBase.h"

#include "Volume/Core/NEVolume.h"
#include "Volume/Core/NEVolumeNode.h"

#include "Rendering/Spatial/NEOSDVolume.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"

#include "Rendering/NEGLRenderer.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Rendering/Resource/NEIndexBuffer.h"

#include "Utility/geometry.h"
#include "Utility/NEColorRamp.h"



template <typename RenderType>
struct RenderTilesAndLeafs
{
    RenderTilesAndLeafs( RenderType& r ) : render( r ) {}
    template<openvdb::Index LEVEL>
    inline bool descent()
    {
    return LEVEL>0;
    } // only descend to leaf nodes

    template<openvdb::Index LEVEL>
    inline void operator()( const openvdb::CoordBBox &bbox )
    {
    //render.addWireBox(bbox,SOP_OpenVDB_Visualize::colorLevel(LEVEL));
    }
    RenderType& render;
};


////////////////////////////////////////


class VDBTopologyVisualizer
{
public:

    enum { NODE_WIRE_BOX = 1, NODE_SOLID_BOX = 2};
    enum { POINTS = 1, POINTS_WITH_VALUES = 2, WIRE_BOX = 3, SOLID_BOX = 4};

    VDBTopologyVisualizer( NEVolumeNode &n,
               int nodeRenderMode, int tileRenderMode, int voxelRenderMode,
               bool ignoreStaggeredVectors = false );

    ~VDBTopologyVisualizer()
    {
    m_indices.clear();
    m_vertices.clear();
    if( m_vertexBuffer )
        NERENDERMAN.GLManager()->destroy( m_vertexBuffer );
    if( m_indexBuffer )
        NERENDERMAN.GLManager()->destroy( m_indexBuffer );
    }


    template<typename GridType>
    void operator()( const GridType& );

    void addWireBox( const openvdb::CoordBBox&, const vec3f& color );

    void drawBox();
    void drawPoints();

    void clear();

    void createBox( NEVolumeNode &geo, const openvdb::math::Transform &xform, const openvdb::CoordBBox &bbox, const vec3f &color, bool solid = false );
    int  createPoint( const openvdb::Vec3d &pos );
    void createPolyBox( NEVolumeNode& geo, const openvdb::math::Transform& xform, const openvdb::CoordBBox& bbox, const vec3f& color, bool solid  );

    int createIndex(uint index);
private:

    int  mNodeRenderMode, mTileRenderMode, mVoxelRenderMode;
    bool mIgnoreStaggered;

    const openvdb::math::Transform* mXform;
    NEVolumeNode* mGeo;

    ///
    /// @param pos position in index coordinates
    ///
    int createPoint(const vec3f pos , const vec3f &color);

    int createPoint( const openvdb::CoordBBox&, const vec3f& color );

    void addPoint( const openvdb::CoordBBox&, const vec3f& color, bool staggered );

    void addBox( const openvdb::CoordBBox&, const vec3f& color, bool solid );



    //std::vector< NECPVertex > m_vertices;
    std::vector< NEPVertex > m_vertices;
    std::vector< uint > m_indices;

    NEVertexBuffer* m_vertexBuffer;
    NEIndexBuffer*  m_indexBuffer;

};


VDBTopologyVisualizer::VDBTopologyVisualizer( NEVolumeNode &n,
                          int  nodeRenderMode,
                          int  tileRenderMode,
                          int  voxelRenderMode,
                          bool ignoreStaggeredVectors )
    : mNodeRenderMode( nodeRenderMode )
    , mTileRenderMode( tileRenderMode )
    , mVoxelRenderMode( voxelRenderMode )
    , mIgnoreStaggered( ignoreStaggeredVectors )
    , mGeo( &n )
    , mXform( NULL )
{

    m_vertexBuffer = NULL ;
    m_indexBuffer  = NULL ;

}


void
VDBTopologyVisualizer::addWireBox( const openvdb::CoordBBox& bbox, const vec3f &color )
{
    addBox( bbox, color, false );
}

void VDBTopologyVisualizer::drawBox()
{

    //NERENDERMAN.GLManager()->draw( kLineListPrim, m_vertexBuffer );
    if( m_vertexBuffer  )
    NERENDERMAN.GLManager()->draw( kQuadListPrim, m_vertexBuffer, m_indexBuffer );

}

void VDBTopologyVisualizer::drawPoints()
{

    if( m_vertexBuffer  )
    NERENDERMAN.GLManager()->draw( kPointListPrim, m_vertexBuffer );

}


template<typename GridType>
void
VDBTopologyVisualizer::operator()( const GridType& grid )
{
    typedef typename GridType::TreeType TreeType;

    openvdb::CoordBBox bbox;
    mXform = &grid.transform();

    const std::string valueType = grid.valueType();


    //
    // Tile and voxel rendering
    //
    if( mTileRenderMode || mVoxelRenderMode )
    {
    const bool staggered = !mIgnoreStaggered && ( grid.getGridClass() == openvdb::GRID_STAGGERED );

    //
    // For each active value..
    //

    vec3f bounds = mGeo->aabb().maxPos() - mGeo->aabb().minPos();
    //float maxbounds = ( bounds.x() > bounds.y() ? bounds.x() : ( bounds.y() > bounds.z() ? bounds.y() : bounds.z() ) );

    for( typename TreeType::ValueOnCIter iter = grid.cbeginValueOn(); iter; iter.next() )
    {
        const int renderMode = iter.isVoxelValue() ? mVoxelRenderMode : mTileRenderMode;
        if( renderMode == 0 )
        continue; // nothing to do!


        openvdb::Vec3d vs = mXform->voxelSize();
        float fvs = ( vs.x() + vs.y()  + vs.z() ) /3.0f;
        const bool negative = iter.getValue() < openvdb::zeroVal< typename TreeType::ValueType >();
        //const bool negative = iter.getValue() < fvs;
        float dist          = iter.getValue();

        //float colorx        = -2.0f * ( dist ) / maxbounds;
        //float colory        = -2.0f * ( dist ) / maxbounds;
        //float colorz        = -2.0f * ( dist ) / maxbounds;
        const vec3f& color  = vec3f ( 0.25f, 0.25f, 0.25f );

        iter.getBoundingBox( bbox );

        //std::cout << "Dist " << dist << std::endl;
        //if( dist <= 0.1f )
        if( negative )
        {
        switch( renderMode )
        {
        case POINTS:
            addPoint( bbox, color, staggered );
            break;
        case WIRE_BOX:
            addBox( bbox, color, renderMode == SOLID_BOX );
            break;
        }
        }
    }
    }


    if( NEBASE.guiMode() )
    {
    uint numVertices = m_vertices.size();
    m_vertexBuffer = NERENDERMAN.GLManager()->createVertexBuffer( kPFormat, numVertices, m_vertices.data() );

    uint numIndices = m_indices.size();
    m_indexBuffer = NERENDERMAN.GLManager()->createIndexBuffer( numIndices, m_indices.data() );
    }

}


inline int
VDBTopologyVisualizer::createPoint( const openvdb::Vec3d& pos )
{
    openvdb::Vec3d wpos = mXform->indexToWorld( pos );

    /*
    NECPVertex cv;

    cv.position = vec3f( pos.x(), pos.y(), pos.z() );
    cv.color  = Color32( 255.0, 255.0, 255.0, 255.0 );

    m_vertices.push_back( cv );
    */
    NEPVertex cv;
    cv.position = vec3f( pos.x(), pos.y(), pos.z() );
    m_vertices.push_back( cv );

    return (m_vertices.size() - 1 );
}

inline int
VDBTopologyVisualizer::createIndex( uint index )
{
    this->m_indices.push_back( index );
    return m_indices.size();
}

inline int
VDBTopologyVisualizer::createPoint( const vec3f pos, const vec3f& color )
{
    openvdb::Vec3d wpos = mXform->indexToWorld( openvdb::Vec3d( pos.x(), pos.y(), pos.z() ) );

    /*
    NECPVertex cv;

    cv.position = vec3f( pos.x(), pos.y(), pos.z() );
    cv.color = Color32( color.x() * 255.0, color.y() * 255.0f, color.z() * 255.0f, 255.0 );

    m_vertices.push_back( cv );
    */
    NEPVertex cv;
    cv.position = vec3f( pos.x(), pos.y(), pos.z() );
    m_vertices.push_back( cv );

    return ( m_vertices.size() - 1);
}


inline int
VDBTopologyVisualizer::createPoint( const openvdb::CoordBBox& bbox,
                    const vec3f &color )
{
    openvdb::Vec3d pos = openvdb::Vec3d( 0.5 * ( bbox.min().x() + bbox.max().x() ),
                     0.5 * ( bbox.min().y() + bbox.max().y() ),
                     0.5 * ( bbox.min().z() + bbox.max().z() ));



    pos = mXform->indexToWorld( pos );

    /*
    NECPVertex cv;
    cv.position = vec3f( pos.x(), pos.y(), pos.z() );
    cv.color = Color32( color.x() * 255.0f , color.y() * 255.0f, color.z() * 255.0f, 255.0f );
    m_vertices.push_back( cv );
    */

    NEPVertex cv;
    cv.position = vec3f( pos.x(), pos.y(), pos.z() );
    m_vertices.push_back( cv );

    int offset = m_vertices.size() - 1;
    return offset;
}



void
VDBTopologyVisualizer::addPoint( const openvdb::CoordBBox& bbox,
                 const vec3f& color, bool staggered )
{
    if( !staggered )
    {
    createPoint( bbox, color );
    }
    else
    {
    openvdb::Vec3d pos = openvdb::Vec3d( 0.5 * ( bbox.min().x() + bbox.max().x() ),
                         0.5 * ( bbox.min().y() + bbox.max().y() ),
                         0.5 * ( bbox.min().z() + bbox.max().z() ) );
    pos[0] -= 0.5; // -x
    int offset = createPoint( pos );

    pos[0] += 0.5;
    pos[1] -= 0.5; // -y
    offset = createPoint( pos );

    pos[1] += 0.5;
    pos[2] -= 0.5; // -z
    offset = createPoint( pos );

    }
}


////////////////////////////////////////

void
VDBTopologyVisualizer::createPolyBox( NEVolumeNode& geo,
                      const openvdb::math::Transform& xform,
                      const openvdb::CoordBBox& bbox,
                      const vec3f& color,
                      bool solid  )
{
    struct Local
    {
    static inline vec3d Vec3dToNEV3( const openvdb::Vec3d& v )
    {
        return vec3d( float(v.x()), float(v.y()), float(v.z()));
    }
    };

    openvdb::math::Vec3d min = bbox.min().asVec3d() - openvdb::math::Vec3d( 0.5, 0.5, 0.5 );
    openvdb::math::Vec3d max = bbox.max().asVec3d() + openvdb::math::Vec3d( 0.5, 0.5, 0.5 );

    vec3f corners[8];
    int   cornerIndices[8];

    openvdb::Vec3d ptn = xform.indexToWorld( min );
    corners[0] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[0] = createPoint( corners[0], color  );

    ptn = openvdb::Vec3d( min.x(), min.y(), max.z() );
    ptn = xform.indexToWorld( ptn );
    corners[1] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[1] = createPoint( corners[1], color );

    ptn = openvdb::Vec3d( max.x(), min.y(), max.z() );
    ptn = xform.indexToWorld( ptn );
    corners[2] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[2] = createPoint( corners[2], color );

    ptn = openvdb::Vec3d( max.x(), min.y(), min.z() );
    ptn = xform.indexToWorld( ptn );
    corners[3] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[3] = createPoint( corners[3], color );


    ptn = openvdb::Vec3d( min.x(), max.y(), min.z() );
    ptn = xform.indexToWorld( ptn );
    corners[4] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[4] = createPoint( corners[4], color );

    ptn = openvdb::Vec3d( min.x(), max.y(), max.z() );
    ptn = xform.indexToWorld( ptn );
    corners[5] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[5] = createPoint( corners[5], color );


    ptn = xform.indexToWorld( max );
    corners[6] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[6] = createPoint( corners[6], color );

    ptn = openvdb::Vec3d( max.x(), max.y(), min.z() );
    ptn = xform.indexToWorld( ptn );
    corners[7] = vec3f( ptn.x(), ptn.y(), ptn.z() );
    cornerIndices[7] = createPoint( corners[7], color );

    // bottom
    createIndex( cornerIndices[0] );
    createIndex( cornerIndices[1] );
    createIndex( cornerIndices[2] );
    createIndex( cornerIndices[3] );

    // top
    createIndex( cornerIndices[4] );
    createIndex( cornerIndices[5] );
    createIndex( cornerIndices[6] );
    createIndex( cornerIndices[7] );

    // front
    createIndex( cornerIndices[0] );
    createIndex( cornerIndices[4] );
    createIndex( cornerIndices[7] );
    createIndex( cornerIndices[3] );

    // back
    createIndex( cornerIndices[1] );
    createIndex( cornerIndices[5] );
    createIndex( cornerIndices[6] );
    createIndex( cornerIndices[2] );

    /*
    // left
    createIndex( cornerIndices[0] );
    createIndex( cornerIndices[1] );
    createIndex( cornerIndices[5] );
    createIndex( cornerIndices[4] );

    // right
    createIndex( cornerIndices[3] );
    createIndex( cornerIndices[2] );
    createIndex( cornerIndices[6] );
    createIndex( cornerIndices[7] );
    */
}


////////////////////////////////////////

void
VDBTopologyVisualizer::createBox( NEVolumeNode& geo,
                  const openvdb::math::Transform& xform,
                  const openvdb::CoordBBox& bbox,
                  const vec3f& color,
                  bool solid  )
{
    struct Local
    {
    static inline vec3d Vec3dToNEV3( const openvdb::Vec3d& v )
    {
        return vec3d( float(v.x()), float(v.y()), float(v.z()));
    }
    };

    openvdb::math::Vec3d min = bbox.min().asVec3d() - openvdb::math::Vec3d( 0.5, 0.5, 0.5 );
    openvdb::math::Vec3d max = bbox.max().asVec3d() + openvdb::math::Vec3d( 0.5, 0.5, 0.5 );

    min = xform.indexToWorld( min );
    max = xform.indexToWorld( max );

    vec3f minPos = vec3f( min.x(), min.y(), min.z() );
    vec3f maxPos = vec3f( max.x(), max.y(), max.z() );

    //
    // Bottom face lines
    //
    vec3f v1 = vec3f( minPos.x(), minPos.y(), minPos.z() );
    createPoint( v1, color );
    vec3f v2 = vec3f( maxPos.x(), minPos.y(), minPos.z() );
    createPoint( v2, color );

    vec3f v3 = vec3f( maxPos.x(), minPos.y(), minPos.z() );
    createPoint( v3, color );
    vec3f v4 = vec3f( maxPos.x(), minPos.y(), maxPos.z() );
    createPoint( v4, color );

    vec3f v5 = vec3f( maxPos.x(), minPos.y(), maxPos.z() );
    createPoint( v5, color );
    vec3f v6 = vec3f( minPos.x(), minPos.y(), maxPos.z() );
    createPoint( v6, color );

    vec3f v7 = vec3f( minPos.x(), minPos.y(), maxPos.z() );
    createPoint( v7, color );
    vec3f v8 = vec3f( minPos.x(), minPos.y(), minPos.z() );
    createPoint( v8, color );

    //
    // Vertical wall column lines
    //
    vec3f v9  = vec3f( minPos.x(), minPos.y(), minPos.z() );
    createPoint( v9, color );
    vec3f v10 = vec3f( minPos.x(), maxPos.y(), minPos.z() );
    createPoint( v10, color );

    vec3f v11 = vec3f( maxPos.x(), minPos.y(), minPos.z() );
    createPoint( v11, color );
    vec3f v12 = vec3f( maxPos.x(), maxPos.y(), minPos.z() );
    createPoint( v12, color );

    vec3f v13 = vec3f( minPos.x(), minPos.y(), maxPos.z() );
    createPoint( v13, color );
    vec3f v14 = vec3f( minPos.x(), maxPos.y(), maxPos.z() );
    createPoint( v14, color );

    vec3f v15 = vec3f( maxPos.x(), minPos.y(), maxPos.z() );
    createPoint( v15, color );
    vec3f v16 = vec3f( maxPos.x(), maxPos.y(), maxPos.z() );
    createPoint( v16, color );

    /*
    //
    // Top
    //
    vec3f v17 = vec3f( minPos.x(), maxPos.y(), minPos.z() );
    createPoint( v17, color );
    vec3f v18 = vec3f( maxPos.x(), maxPos.y(), minPos.z() );
    createPoint( v18, color );

    vec3f v19 = vec3f( maxPos.x(), maxPos.y(), minPos.z() );
    createPoint( v19, color );
    vec3f v20 = vec3f( maxPos.x(), maxPos.y(), maxPos.z() );
    createPoint( v20, color );

    vec3f v21 = vec3f( maxPos.x(), maxPos.y(), maxPos.z() );
    createPoint( v21, color );
    vec3f v22 = vec3f( minPos.x(), maxPos.y(), maxPos.z() );
    createPoint( v22, color );

    vec3f v23 = vec3f( minPos.x(), maxPos.y(), maxPos.z() );
    createPoint( v23, color );
    vec3f v24 = vec3f( minPos.x(), maxPos.y(), minPos.z() );
    createPoint( v24, color );
    */

}


void
VDBTopologyVisualizer::addBox( const openvdb::CoordBBox& bbox,
                   const vec3f& color,
                   bool solid )
{
    //createBox( *mGeo, *mXform, bbox, color, solid );
    createPolyBox( *mGeo, *mXform, bbox, color, solid );
}

class NEVolumeRendererPrivate
{

public:

    NEVolumeRendererPrivate( NEVolumeRenderer* V )
    {
        S = V;
        m_Volume = NULL;
    }

    double m_EvalStart ;
    double m_EvalEnd  ;
    NEVolumeNode::DisplayMode m_DispMode;

    NEVolumeNode* m_Volume;
    std::shared_ptr< VDBTopologyVisualizer >  m_VolumeVis;

    static vec3f m_colors[];
    const vec3f& colorSign( bool negative );
    vec3f* m_vertices;
    vec3f* m_indices;

    NEVolumeRenderer* S;
};


vec3f NEVolumeRendererPrivate::m_colors[] =
{
    vec3f( 0.045f, 0.045f, 0.045f ),         // 0. Root
    vec3f( 0.0432f, 0.33f, 0.0411023f ),     // 1. First internal node level
    vec3f( 0.871f, 0.394f, 0.01916f ),       // 2. Intermediate internal node levels
    vec3f( 0.00608299f, 0.279541f, 0.625f ), // 3. Leaf level
    vec3f( 0.523f, 0.0325175f, 0.0325175f ), // 4. Value >= ZeroVal (for voxels or tiles)
    vec3f( 0.92f, 0.92f, 0.02f )             // 5. Value < ZeroVal (for voxels or tiles)
};


const vec3f& NEVolumeRendererPrivate::colorSign( bool negative )
{
    return m_colors[ negative ? 5 : 4 ];
}

bool NEVolumeRenderer::createVisualizer( NESpatial *spatial )
{
    if( !P->m_Volume )
        return false;

    if( P->m_Volume->DispMode() == NEVolumeNode::Points )
        P->m_VolumeVis = std::shared_ptr< VDBTopologyVisualizer > ( new VDBTopologyVisualizer( *(P->m_Volume), 0, 0, VDBTopologyVisualizer::POINTS, true ) );
    else if( P->m_Volume->DispMode()  == NEVolumeNode::Box )
        P->m_VolumeVis = std::shared_ptr< VDBTopologyVisualizer > ( new VDBTopologyVisualizer( *(P->m_Volume), 0, 0, VDBTopologyVisualizer::WIRE_BOX, true ) );

    openvdb::FloatGrid::Ptr grid;
    if( P->m_Volume )
    {
        grid = P->m_Volume->Volume()->Volume();
        if( grid == NULL )
            return false;

        if( P->m_VolumeVis )
            (*(P->m_VolumeVis))( *grid );
    }

    return true;
}


NEVolumeRenderer::NEVolumeRenderer()
{
    P =  new NEVolumeRendererPrivate( this );
}

NEVolumeRenderer::~NEVolumeRenderer()
{
    delete P;
}

void NEVolumeRenderer::render( NESpatial* spatial, IGLSurface* surface )
{
    NEVolumeNode *vn = qobject_cast< NEVolumeNode *>( spatial );

    if( !vn )
	    return;

    if( vn->RenderMode() == NESpatial::NERenderMode::Invisible )
	    return;

    P->m_Volume = vn;

    if( vn->Volume() == NULL)
        return;

    if( P->m_Volume->Volume() == NULL )
        return;

    if( !P->m_VolumeVis )
        return;


    openvdb::FloatGrid::Ptr grid = P->m_Volume->Volume()->Volume();

    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    NERENDERMAN.GLManager()->beginUnLitDraw(surface);
    NERENDERMAN.GLManager()->updatePositionUniforms(surface);
    GL->glPointSize( 2.0f );


    if( P->m_VolumeVis )
    {
        if( vn->DispMode() == NEVolumeNode::Points )
        {
            P->m_VolumeVis->drawPoints();
        }
        else if( vn->DispMode() == NEVolumeNode::Box )
        {
            if( vn->RenderMode() == NESpatial::NERenderMode::Wireframe )
                GL->glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;

            P->m_VolumeVis->drawBox();
        }
    }

    NERENDERMAN.GLManager()->endUnLitDraw(surface);
    GL->glPopAttrib();
}

void NEVolumeRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render( spatial, surface );
}

void NEVolumeRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render( spatial, surface );
}



NEVolumeRendererFactory::NEVolumeRendererFactory(NEManager* m) : NERendererFactory( m )
{
}

QString NEVolumeRendererFactory::nodeName() { return "Volume Node Renderer"; }
NEObject *NEVolumeRendererFactory::createInstance() { return new NEVolumeRenderer(); }


#if 0
//
// NEVolumeSource
//

void NEVolumeSourceRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEVolumeSource* volSource = qobject_cast<NEVolumeSource*>(spatial);

    if( !volSource )
        return;

    if( volSource->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    if( NEBASE.guiMode() == false )
        return;


    if( !volSource->Volume() || !volSource->Volume()->Volume() )
        return;

    openvdb::CoordBBox bbox;
    if( volSource->Volume() )
    {
        if( volSource->Volume()->Volume() )
        {
            bbox = volSource->Volume()->Volume()->evalActiveVoxelBoundingBox();
        }
    }

    //
    // Render block extent
    //
    glLineWidth( 2.0f );
    //vec3f minPos = position() + (scale()/2.0f);
    //vec3f maxPos = position() - (scale()/2.0f);
    openvdb::Vec3d vMinPos = bbox.min().asVec3d();
    openvdb::Vec3d vMaxPos = bbox.max().asVec3d();

    vMinPos = volSource->Volume()->Volume()->indexToWorld( vMinPos );
    vMaxPos = volSource->Volume()->Volume()->indexToWorld( vMaxPos );

    vec3f minPos =  vec3f( vMinPos.x(), vMinPos.y(), vMinPos.z() );
    vec3f maxPos =  vec3f( vMaxPos.x(), vMaxPos.y(), vMaxPos.z() );


    glBegin(GL_LINES);
    //bottom
    glVertex3f(minPos.x(), minPos.y(), minPos.z());
    glVertex3f(maxPos.x(), minPos.y(), minPos.z());

    glVertex3f(maxPos.x(), minPos.y(), minPos.z());
    glVertex3f(maxPos.x(), minPos.y(), maxPos.z());

    glVertex3f(maxPos.x(), minPos.y(), maxPos.z());
    glVertex3f(minPos.x(), minPos.y(), maxPos.z());

    glVertex3f(minPos.x(), minPos.y(), maxPos.z());
    glVertex3f(minPos.x(), minPos.y(), minPos.z());

    //vertical
    glVertex3f(minPos.x(), minPos.y(), minPos.z());
    glVertex3f(minPos.x(), maxPos.y(), minPos.z());

    glVertex3f(maxPos.x(), minPos.y(), minPos.z());
    glVertex3f(maxPos.x(), maxPos.y(), minPos.z());

    glVertex3f(maxPos.x(), minPos.y(), maxPos.z());
    glVertex3f(maxPos.x(), maxPos.y(), maxPos.z());

    glVertex3f(minPos.x(), minPos.y(), maxPos.z());
    glVertex3f(minPos.x(), maxPos.y(), maxPos.z());
    //top
    glVertex3f(minPos.x(), maxPos.y(), minPos.z());
    glVertex3f(maxPos.x(), maxPos.y(), minPos.z());

    glVertex3f(maxPos.x(), maxPos.y(), minPos.z());
    glVertex3f(maxPos.x(), maxPos.y(), maxPos.z());

    glVertex3f(maxPos.x(), maxPos.y(), maxPos.z());
    glVertex3f(minPos.x(), maxPos.y(), maxPos.z());

    glVertex3f(minPos.x(), maxPos.y(), maxPos.z());
    glVertex3f(minPos.x(), maxPos.y(), minPos.z());

    glEnd();

    glLineWidth(1.0f);

    /*
    if( volSource->drawParticlePositions() && m_tmpParticles.size() )
    {
        NERENDERMAN.GLManager()->beginParticleDraw( volSource->Radius(), true, false, false);
        NERENDERMAN.GLManager()->drawParticles(GL_V3F, sizeof(NEParticle),
                           &((NEParticle*)&m_tmpParticles[0])->position(), 0, (uint) m_tmpParticles.size());
        NERENDERMAN.GLManager()->endParticleDraw();
    }
    */

}


void NEVolumeSourceRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, widget);
}

void NEVolumeSourceRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    render(spatial, widget);
}

NEVolumeSourceRendererFactory::NEVolumeSourceRendererFactory(NEManager* m) : NERendererFactory( m )
{
}

QString NEVolumeSourceRendererFactory::nodeName() { return "Volume Particle Emitter Renderer"; }
NESpatialRenderer* NEVolumeSourceRendererFactory::createInstance() { return new NEVolumeSourceRenderer(); }

#endif

#if NE_VOLUME

//
// various Volume class renderer factory
//

NEMeshToVolumeRendererFactory::NEMeshToVolumeRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEMeshToVolumeRendererFactory::nodeName() { return "Mesh to Volume Renderer"; }
NEObject *NEMeshToVolumeRendererFactory::createInstance() { return new NEVolumeRenderer(); }

NEPartToVolumeRendererFactory::NEPartToVolumeRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEPartToVolumeRendererFactory::nodeName() { return "Part to Volume Renderer"; }
NEObject *NEPartToVolumeRendererFactory::createInstance() { return new NEVolumeRenderer(); }

NEVolumeCombineRendererFactory::NEVolumeCombineRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEVolumeCombineRendererFactory::nodeName() { return "Combine Volumes Renderer"; }
NEObject *NEVolumeCombineRendererFactory::createInstance() { return new NEVolumeRenderer(); }

NEVolumeFileRendererFactory::NEVolumeFileRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEVolumeFileRendererFactory::nodeName() { return "Volume File Renderer"; }
NEObject *NEVolumeFileRendererFactory::createInstance() { return new NEVolumeRenderer(); }

NEVolumeFilterRendererFactory::NEVolumeFilterRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEVolumeFilterRendererFactory::nodeName() { return "Filter Volume Renderer"; }
NEObject *NEVolumeFilterRendererFactory::createInstance() { return new NEVolumeRenderer(); }


//
// OSDVolumeRenderer
//
void NEOSDVolumeRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDVolume* osdVolume = qobject_cast<NEOSDVolume*>(spatial);
    if (!osdVolume) return;
    NESpatial* pVolumeAndCombine = osdVolume->Volume();
    NEVolumeNode* pVolume = qobject_cast<NEVolumeNode*>(pVolumeAndCombine);
    if (!pVolume) return;

    if(osdVolume->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    float minVal = 0, maxVal = 1;
    if (pVolume)
    {
        NEVolumeNodeRenderer* pVolumeRenderer = (NEVolumeNodeRenderer*)NERENDERMAN.getRenderer(pVolume);
        if (pVolumeRenderer)
        {
            minVal = pVolumeRenderer->MinColoringRange();
            maxVal = pVolumeRenderer->MaxColoringRange();
        }
    }
    if (!pVolume->getColorRampForVolume())
        return;

    {
        QVector<QPair<qreal, QColor>> colorList;
        if (pVolume)
            colorList = pVolume->getColorRampForVolume()->getColorRamp();
        int nSize = std::min(colorList.size(), MAX_COLORRAMP_SIZE);

        // Render Color Bar
        surface->setShaderProgram(osdVolume->getColorBarProgram());
        GL->glUniform1i(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), nSize );
        for(int i = 0; i < nSize; i++)
        {
            float pos = colorList.at(i).first;
            QColor color = colorList.at(i).second;
            vec3f colorv = vec3f(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
            GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[" + QString::number(i) + "]").toUtf8() ), 1, &colorv[0] );
            GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[" + QString::number(i) + "]").toUtf8() ), pos );
        }
    }

    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdVolume->Positionx();
    int yRes = height * osdVolume->Positiony();
    int wRes = OSD_PARTICLE_COLORBAR_WIDTH * osdVolume->Scalex();      //!< Width of color bar
    int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * osdVolume->Scaley();     //!< Height of color bar
    int hTitle = osdVolume->fontHeight() * 2;
    int wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdVolume->Scalex();   //!< Width of margin between color bar and measure
    unsigned int widthU = 0;   //!< Width of measurement text
    NEOSDVolume::NEOSDOrientation enumOrientation = osdVolume->OSDOrientation();
    if (enumOrientation == NEOSDVolume::NEOSDOrientation::Top ||
        enumOrientation == NEOSDVolume::NEOSDOrientation::Bottom)
    {
        if (osdVolume->FontSize() == 20)
            osdVolume->SetFontSize(12);
    } else
    {
        if (osdVolume->FontSize() == 12)
            osdVolume->SetFontSize(20);
    }
    vec2f OsdSize = osdVolume->osdSize();
    if (enumOrientation == NEOSDVolume::NEOSDOrientation::Top ||
        enumOrientation == NEOSDVolume::NEOSDOrientation::Bottom)
    {
        wRes = OsdSize.x() - (int)(osdVolume->fontWidth());
        hRes = OSD_PARTICLE_COLORBAR_WIDTH * osdVolume->Scaley();
        wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdVolume->Scaley();
    }

    int xPos = 0, yPos = 0;
    vec3f colorBarRect[4];
    vec3f colorBarRect_2D[4];
    vec2f widgetSize = vec2f(width, height);
    vec3f offsetOfOrientation;
    switch(enumOrientation)
    {
    case NEOSDVolume::NEOSDOrientation::Top:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes - hTitle, 0);
        colorBarRect[1] = vec3f(xRes, yRes - hTitle - hRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes - hTitle - hRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes - hTitle, 3);
        break;
    case NEOSDVolume::NEOSDOrientation::Bottom:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes + hRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes + hRes, 3);
        break;
    case NEOSDVolume::NEOSDOrientation::Right:
        offsetOfOrientation = vec3f(OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes - wRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes - wRes, yRes + hRes, 3);
        break;
    case NEOSDVolume::NEOSDOrientation::Left:
    default:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes + wRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes, yRes + hRes, 3);
        break;
    }
    for (int nIndex = 0; nIndex < 4; nIndex++)
    {
        colorBarRect[nIndex] = (colorBarRect[nIndex] + offsetOfOrientation); // widgetSize;
        colorBarRect_2D[nIndex] = vec3f(colorBarRect[nIndex].x(), colorBarRect[nIndex].y(), 0);
        colorBarRect[nIndex].setX(colorBarRect[nIndex].x() / widgetSize.x());
        colorBarRect[nIndex].setY(colorBarRect[nIndex].y() / widgetSize.y());
    }

    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect);
    GL->glDrawArrays(GL_TRIANGLE_FAN,0,4);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

    // Render black colorbar border
    NERENDERMAN.GLManager()->bindPositionShader(surface);
    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glColor4f(0,0,0,1);
    NERENDERMAN.GLManager()->updatePositionUniforms(surface);
    GL->glLineWidth(1.5f);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect_2D);
    GL->glDrawArrays(GL_LINE_LOOP,0,4);
    GL->glLineWidth(1.0f);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

#if USE_TICK_LARGE_MIDDLE_SHORT
    // Render Ruler
    int rulerLeft   = colorBarRect_2D[0].x(); // point of index 0 is bottomleft(or topleft)
    int rulerTop    = colorBarRect_2D[0].y();
    int rulerRight  = colorBarRect_2D[1].x(); // point of index 1 is bottomright(or bottomleft)
    int rulerbottom = colorBarRect_2D[1].y();
    int tickWidthL  = 5; // wRes / 10 + 1;
    int tickWidthM  = 3; // wRes / 20 + 1;
    int tickWidthS  = 3; // wRes / 25 + 1;
    int tickHeightL = 0;
    int tickHeightM = 0;
    int tickHeightS = 0;
    if (enumOrientation == NEOSDVolume::NEOSDOrientation::Top ||
        enumOrientation == NEOSDVolume::NEOSDOrientation::Bottom)
    {
        tickWidthL = 0;
        tickWidthM = 0;
        tickWidthS = 0;
        tickHeightL = 5; // hRes / 10 + 1;
        tickHeightM = 3; // hRes / 20 + 1;
        tickHeightS = 3; // hRes / 25 + 1;
    }
//    glLineWidth(2.0f);
    vec3f q3_1[2], q3_2[2];
    float offsetX, offsetY;

    glEnableClientState(GL_VERTEX_ARRAY);

    for( int i = 0; i <= 10 * osdVolume->TickCount(); i++)
    {
        float s = i / 10.0 / osdVolume->TickCount();
        if (enumOrientation == NEOSDVolume::NEOSDOrientation::Top ||
            enumOrientation == NEOSDVolume::NEOSDOrientation::Bottom)
        {
            offsetX = s * wRes; offsetY = 0;
        } else
        {
            offsetX = 0; offsetY = s * hRes;
        }

        q3_1[0] = vec2f(rulerLeft + offsetX, rulerTop + offsetY);
        q3_2[0] = vec2f(rulerRight + offsetX, rulerbottom + offsetY);
        if (i % 10 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthL, rulerTop + offsetY - tickHeightL);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthL, rulerbottom + offsetY + tickHeightL);
        }
        else if (i % 5 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthM, rulerTop + offsetY - tickHeightM);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthM, rulerbottom + offsetY + tickHeightM);
        }
        else
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthS, rulerTop + offsetY - tickHeightS);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthS, rulerbottom + offsetY + tickHeightS);
        }
        glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_1);
        glDrawArrays(GL_LINE_LOOP,0,2);
        glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_2);
        glDrawArrays(GL_LINE_LOOP,0,2);
    }
//    glLineWidth(1.0f);
    glDisableClientState(GL_VERTEX_ARRAY);
#endif

    // Draw Title Text
    surface->setShaderProgram(0);
    GL->glDisable(GL_BLEND);
    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );
    float titleTextWidth = osdVolume->getTitleText().length() * osdVolume->fontWidth();
    if (enumOrientation != NEOSDVolume::NEOSDOrientation::Top &&
        enumOrientation != NEOSDVolume::NEOSDOrientation::Bottom)
    {
        glPrint(osdVolume->getTitleText(),
                xRes - titleTextWidth / 2,
                yRes + hRes + hTitle - OsdSize.y() / 2,
                osdVolume->fontColor(),
                osdVolume->getTextureFont(), osdVolume->OutlineColor(), osdVolume->getTextureOutlineFont());
    } else {
        glPrint(osdVolume->getTitleText(),
                xRes - titleTextWidth / 2,
                yRes - osdVolume->fontHeight() + OsdSize.y() / 2,
                osdVolume->fontColor(),
                osdVolume->getTextureFont(), osdVolume->OutlineColor(), osdVolume->getTextureOutlineFont());
    }

    // Draw measurement values (1..step)
    for (int i = 0; i <= osdVolume->TickCount(); i++)
    {
        float s = i / (float)osdVolume->TickCount();
        float val = minVal * (1-s) + maxVal * s;
        QString text;
        if( osdVolume->DisplayNotation() == NEOSDBase::Exponent )
        {
            text = QString("%1").arg( (double)val, 2, 'E', 2 );
        }
        else if( osdVolume->DisplayNotation() == NEOSDBase::FloatingPoint )
        {
            text = QString("%1").arg( (double)val, osdVolume->TotalNumberLength(), 'f', osdVolume->FloatTypePrecision() );
        }
        widthU = (float)osdVolume->TotalNumberLength() * osdVolume->fontWidth();
        switch(enumOrientation)
        {
        case NEOSDVolume::NEOSDOrientation::Top:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - OsdSize.y()/2;
            break;
        case NEOSDVolume::NEOSDOrientation::Bottom:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - hTitle - (int)(osdVolume->fontHeight()) + OsdSize.y()/2;
            break;
        case NEOSDVolume::NEOSDOrientation::Right:
            xPos = xRes + (int)(osdVolume->fontWidth()) -  OsdSize.x()/2;
            yPos = yRes + s * hRes - OsdSize.y()/2;
            break;
        case NEOSDVolume::NEOSDOrientation::Left:
        default:
            xPos = xRes + wRes + wMargin -  OsdSize.x() / 2;
            yPos = yRes + s * hRes - OsdSize.y() / 2;
            break;
        }
        glPrint(text, xPos, yPos,
                osdVolume->fontColor(),
                osdVolume->getTextureFont(), osdVolume->OutlineColor(), osdVolume->getTextureOutlineFont());
    }

    GL->glPopAttrib();
}



NEOSDVolumeRendererFactory::NEOSDVolumeRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEOSDVolumeRendererFactory::nodeName() { return "Volume OSD Renderer"; }
NEObject*NEOSDVolumeRendererFactory::createInstance() { return new NEOSDVolumeRenderer(); }

#endif
