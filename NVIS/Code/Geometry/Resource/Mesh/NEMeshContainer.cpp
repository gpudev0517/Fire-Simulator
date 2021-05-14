#include "NEMeshContainer.h"


NEMeshContainer::NEMeshContainer(): NEMesh()
{
}


NEMeshContainer::~NEMeshContainer()
{
    m_TriMesh.clearDrawingBuffers();
}



uint NEMeshContainer::SetFlipMesh(const bool& val)
{
    m_TriMesh.flip();

    return NE::kReturnSuccess;
}


NEIndexedTriMesh& NEMeshContainer::triangleMesh()
{
    return m_TriMesh;
}

const NEIndexedTriMesh& NEMeshContainer::triangleMesh() const
{
    return m_TriMesh;
}

void NEMeshContainer::setTriMesh(const NEIndexedTriMesh& val)
{
    m_TriMesh = val;
}

std::vector<NEPrimitivePolygon>& NEMeshContainer::quadFaces()
{
    return m_QuadFaces;
}



void NEMeshContainer::createBoxVertices(const vec3f &halfSpacing, vec3f os, float normalInvert)
{
    NECTNPVertex v;

    float sign = 1.0f;


    // Left Face
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(),
                       -0.5f - halfSpacing.y() - os.y(),
                       -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(-1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), -0.5f - halfSpacing.y() - os.y(), 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(-1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(-1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(-1.0f, 0.0f, 0.0) * normalInvert;
    v.tCoord = vec2f(1.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);

    // Right
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), -0.5f - halfSpacing.y() - os.y(), -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), -0.5f - halfSpacing.y() - os.y(), 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(1.0f, 0.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);

    // Back
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), -0.5f - halfSpacing.y() - os.y(), -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(0.0f, 0.0f, -1.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(0.0f, 0.0f, -1.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(0.0f, 0.0f, -1.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), -0.5f - halfSpacing.y() - os.y(), -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(0.0f, 0.0f, -1.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);

    // Front
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), -0.5f - halfSpacing.y() - os.y(), 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(0.0f, 0.0f, 1.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(0.0f, 0.0f, 1.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5 + halfSpacing.x() + os.x(), 0.5f + abs(halfSpacing.y()) + abs(os.y()) * sign, 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(0.0f, 0.0f, 1.0f) * normalInvert;
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    v.tCoord = vec2f(1.0f, 1.0f);
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5 + halfSpacing.x() + os.x(), -0.5f - halfSpacing.y() - os.y(), 0.5 + halfSpacing.z() + os.z());
    v.normal = vec3f(0.0f, 0.0f, 1.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF());
    m_TriMesh.appendVertex(v);

    // Bottom
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), -0.5f - halfSpacing.y() - os.y(), -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(0.0f, -1.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f - halfSpacing.x() - os.x(), -0.5f - halfSpacing.y() - os.y(), 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(0.0f, -1.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(0.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5 + halfSpacing.x() + os.x(), -0.5f - halfSpacing.y() - os.y(), 0.5f + halfSpacing.z() + os.z());
    v.normal = vec3f(0.0f, -1.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 1.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f + halfSpacing.x() + os.x(), -0.5f - halfSpacing.y() - os.y(), -0.5f - halfSpacing.z() - os.z());
    v.normal = vec3f(0.0f, -1.0f, 0.0f) * normalInvert;
    v.tCoord = vec2f(1.0f, 0.0f);
    //v.color  = vec4f( Color().redF(), Color().greenF(), Color().blueF(), Color.alphaF() );
    m_TriMesh.appendVertex(v);

    return;
}

void NEMeshContainer::unorderedEdgesFromTriangles(const std::vector<NETriangleIndexed> &triangles,
                                                  std::vector<NEPrimitiveEdge> &edges_out)
{
    typedef std::set< std::pair< NEVertexID, NEVertexID > > edge_set_t;
    edge_set_t edges;
    for( unsigned int t = 0; t < triangles.size(); ++t )
    {
        edges.insert( std::make_pair( std::min( triangles[t].p1, triangles[t].p2 ),
                                      std::max( triangles[t].p1, triangles[t].p2 ) ) );
        edges.insert( std::make_pair( std::min( triangles[t].p2, triangles[t].p3),
                                      std::max( triangles[t].p2, triangles[t].p3 ) ) );
        edges.insert( std::make_pair( std::min( triangles[t].p3, triangles[t].p1 ),
                                      std::max( triangles[t].p3, triangles[t].p1 ) ) );
    }

    edges_out.resize( edges.size() );
    int e = 0;
    for( edge_set_t::const_iterator it = edges.begin(); it != edges.end(); ++it, ++e )
    {
        edges_out.at(e).start() = it->first;
        edges_out.at(e).end() = it->second;
    }
}

void NEMeshContainer::unorderedEdgesFromQuads(const std::vector<NEPrimitivePolygon> &quads,
                                              std::vector<NEPrimitiveEdge> &edges_out)
{
    typedef std::set< std::pair< NEVertexID, NEVertexID > > edge_set_t;
    edge_set_t edges;
    for( unsigned int t = 0; t < quads.size(); ++t )
    {
        if( quads[t].vertices.size() != 4 )
            continue;
        edges.insert( std::make_pair( std::min( quads[t].vertices[0], quads[t].vertices[1] ),
                                      std::max( quads[t].vertices[0], quads[t].vertices[1] ) ) );
        edges.insert( std::make_pair( std::min( quads[t].vertices[1], quads[t].vertices[2] ),
                                      std::max( quads[t].vertices[1], quads[t].vertices[2] ) ) );
        edges.insert( std::make_pair( std::min( quads[t].vertices[2], quads[t].vertices[3] ),
                                      std::max( quads[t].vertices[2], quads[t].vertices[3]) ) );
        edges.insert( std::make_pair( std::min( quads[t].vertices[3], quads[t].vertices[0] ),
                                      std::max( quads[t].vertices[3], quads[t].vertices[0]) ) );
    }

    edges_out.resize( edges.size() );
    int e = 0;
    for( edge_set_t::const_iterator it = edges.begin(); it != edges.end(); ++it, ++e )
    {
        edges_out.at(e).start() = it->first;
        edges_out.at(e).end() = it->second;
    }
}


uint NEMeshContainer::createBoxMesh( vec3f scale, float spacing, float surfaceOffsetValue )
{
    m_TriMesh.resize();

    vec3f halfSpacing(spacing / scale.x() * 0.5f, spacing / scale.y() * 0.5f, spacing / scale.z() * 0.5f);

    vec3f os(-surfaceOffsetValue * spacing / scale.x(), -surfaceOffsetValue * spacing / scale.y(),
             -surfaceOffsetValue * spacing / scale.z());

    os -= halfSpacing;

    // create vertices for outer border
    createBoxVertices(halfSpacing, os, 1.0);
    // create vertices for inner border
    createBoxVertices(-halfSpacing, -os, -1.0);

    NETriangleIndexed t1(0, 1, 3), t2(0, 3, 2), t3(5, 6, 7), t4(5, 4, 6); // left-right
    NETriangleIndexed t5(11, 9, 10), t6(11, 8, 9), t7(12, 14, 13), t8(12, 15, 14); // near-far
    NETriangleIndexed t9(18, 16, 19), t10(18, 17, 16); // bottom

    NETriangleIndexed t11(20, 23, 21), t12(20, 22, 23), t13(25, 27, 26), t14(25, 26, 24); // left-right
    NETriangleIndexed t15(31, 30, 29), t16(31, 29, 28), t17(32, 33, 34), t18(32, 34, 35); // near-far
    NETriangleIndexed t19(38, 39, 36), t20(38, 36, 37); // bottom

    // create vertices for top margin
    uint topLookup[] = {2, 3, 6, 7, 9, 10, 13, 14, 22, 23, 26, 27, 29, 30, 33, 34};

    NECTNPVertex v;
    for(int count = 0; count < 16; ++count)
    {
        v.position = m_TriMesh.vertices()[topLookup[count]].position;
        v.normal   = vec3f( 0.0, 1.0, 0.0 );
        m_TriMesh.appendVertex(v);
    }

    NETriangleIndexed t21(40, 41, 48);
    NETriangleIndexed t22(41, 49, 48);
    NETriangleIndexed t23(42, 50, 43);
    NETriangleIndexed t24(50, 51, 43);
    NETriangleIndexed t25(44, 52, 45);
    NETriangleIndexed t26(52, 53, 45);
    NETriangleIndexed t27(46, 47, 54);
    NETriangleIndexed t28(55, 54, 47);

    m_TriMesh.appendTriangle(t1);
    m_TriMesh.appendTriangle(t2);
    m_TriMesh.appendTriangle(t3);
    m_TriMesh.appendTriangle(t4);
    m_TriMesh.appendTriangle(t5);
    m_TriMesh.appendTriangle(t6);
    m_TriMesh.appendTriangle(t7);
    m_TriMesh.appendTriangle(t8);
    m_TriMesh.appendTriangle(t9);
    m_TriMesh.appendTriangle(t10);
    m_TriMesh.appendTriangle(t11);
    m_TriMesh.appendTriangle(t12);
    m_TriMesh.appendTriangle(t13);
    m_TriMesh.appendTriangle(t14);
    m_TriMesh.appendTriangle(t15);
    m_TriMesh.appendTriangle(t16);
    m_TriMesh.appendTriangle(t17);
    m_TriMesh.appendTriangle(t18);
    m_TriMesh.appendTriangle(t19);
    m_TriMesh.appendTriangle(t20);
    m_TriMesh.appendTriangle(t21);
    m_TriMesh.appendTriangle(t22);
    m_TriMesh.appendTriangle(t23);
    m_TriMesh.appendTriangle(t24);
    m_TriMesh.appendTriangle(t25);
    m_TriMesh.appendTriangle(t26);
    m_TriMesh.appendTriangle(t27);
    m_TriMesh.appendTriangle(t28);

    //create quad faces for extruding
    m_QuadFaces.clear();
    NEPrimitivePolygon face;

    //outside left right
    face.clear();
    face.size = 4;
    face.vertices.push_back(0);
    face.vertices.push_back(1);
    face.vertices.push_back(2);
    face.vertices.push_back(3);
    face.triangles.push_back(t1);
    face.triangles.push_back(t2);

    m_QuadFaces.push_back(face);

    face.clear();
    face.size = 4;
    face.vertices.push_back(4);
    face.vertices.push_back(5);
    face.vertices.push_back(6);
    face.vertices.push_back(7);
    face.triangles.push_back(t3);
    face.triangles.push_back(t4);

    m_QuadFaces.push_back(face);

    //outside near far
    face.clear();
    face.size = 4;
    face.vertices.push_back(8);
    face.vertices.push_back(9);
    face.vertices.push_back(10);
    face.vertices.push_back(11);
    face.triangles.push_back(t5);
    face.triangles.push_back(t6);

    m_QuadFaces.push_back(face);

    face.clear();
    face.size = 4;
    face.vertices.push_back(12);
    face.vertices.push_back(13);
    face.vertices.push_back(14);
    face.vertices.push_back(15);
    face.triangles.push_back(t7);
    face.triangles.push_back(t8);

    m_QuadFaces.push_back(face);

    //outside bottom
    face.clear();
    face.size = 4;
    face.vertices.push_back(16);
    face.vertices.push_back(17);
    face.vertices.push_back(18);
    face.vertices.push_back(19);
    face.triangles.push_back(t9);
    face.triangles.push_back(t10);

    m_QuadFaces.push_back(face);

    //inside left right
    face.clear();
    face.size = 4;
    face.vertices.push_back(20);
    face.vertices.push_back(21);
    face.vertices.push_back(22);
    face.vertices.push_back(23);
    face.triangles.push_back(t11);
    face.triangles.push_back(t12);

    m_QuadFaces.push_back(face);

    face.clear();
    face.size = 4;
    face.vertices.push_back(24);
    face.vertices.push_back(25);
    face.vertices.push_back(26);
    face.vertices.push_back(27);
    face.triangles.push_back(t13);
    face.triangles.push_back(t14);

    m_QuadFaces.push_back(face);

    //inside near far
    face.clear();
    face.size = 4;
    face.vertices.push_back(28);
    face.vertices.push_back(29);
    face.vertices.push_back(30);
    face.vertices.push_back(31);
    face.triangles.push_back(t15);
    face.triangles.push_back(t16);

    m_QuadFaces.push_back(face);

    face.clear();
    face.size = 4;
    face.vertices.push_back(32);
    face.vertices.push_back(33);
    face.vertices.push_back(34);
    face.vertices.push_back(35);
    face.triangles.push_back(t17);
    face.triangles.push_back(t18);

    m_QuadFaces.push_back(face);

    //inside bottom
    face.clear();
    face.size = 4;
    face.vertices.push_back(36);
    face.vertices.push_back(37);
    face.vertices.push_back(38);
    face.vertices.push_back(39);
    face.triangles.push_back(t19);
    face.triangles.push_back(t20);

    m_QuadFaces.push_back(face);

    //
    face.clear();
    face.size = 4;
    face.vertices.push_back(40);
    face.vertices.push_back(41);
    face.vertices.push_back(48);
    face.vertices.push_back(49);
    face.triangles.push_back(t21);
    face.triangles.push_back(t22);

    m_QuadFaces.push_back(face);

    face.clear();
    face.size = 4;
    face.vertices.push_back(42);
    face.vertices.push_back(43);
    face.vertices.push_back(50);
    face.vertices.push_back(51);
    face.triangles.push_back(t23);
    face.triangles.push_back(t24);

    m_QuadFaces.push_back(face);

    //
    face.clear();
    face.size = 4;
    face.vertices.push_back(44);
    face.vertices.push_back(45);
    face.vertices.push_back(52);
    face.vertices.push_back(53);
    face.triangles.push_back(t25);
    face.triangles.push_back(t26);

    m_QuadFaces.push_back(face);

    face.clear();
    face.size = 4;
    face.vertices.push_back(46);
    face.vertices.push_back(47);
    face.vertices.push_back(54);
    face.vertices.push_back(55);
    face.triangles.push_back(t27);
    face.triangles.push_back(t28);

    m_QuadFaces.push_back(face);



    unorderedEdgesFromQuads( m_QuadFaces, m_QuadEdges );

    return NE::kReturnSuccess;
}


uint NEMeshContainer::createCylinderMesh( vec3f scale, QColor color, float spacing,
                                          float surfaceOffsetValue, bool topcap, bool bottomcap,
                                          vec3f cylAxis
                                          )
{
    createCylinderMesh( scale, color, spacing,
                        surfaceOffsetValue, topcap, bottomcap );

    std::vector<NECTNPVertex>& vertices = m_TriMesh.vertices();
    matrix44d rm;
    rm.setToIdentity();
    vec3d yaxis = vec3d( 0.0, 1.0, 0.0 );
    vec3d caxis = vec3d( cylAxis.x(), cylAxis.y(), cylAxis.z() ).normalized();

    if( yaxis == caxis )
        return NE::kReturnSuccess;

    vec3d axis  = vec3d::crossProduct( yaxis, caxis );
    double angle  = acos( vec3d::dotProduct( yaxis, axis ) );
    //std::cout << "axis " << axis.x() << " " << axis.y() << " " << axis.z() << std::endl;
    rm.rotate( RADIAN_TO_DEGREE(angle), axis );
    //std::cout << "angle " << angle << std::endl;
    qDebug() << rm ;
    for( unsigned i = 0; i < m_TriMesh.numVertices(); ++i )
    {
        vec3f& p = vertices[i].position;
        vec3f& n = vertices[i].normal;

        vec3d pd( p.x(), p.y(), p.z() );
        vec3d pn( n.x(), n.y(), n.z() );

        vec3d rd = pd * rm;
        vec3d rn = pn * rm;

        p = vec3f( rd.x(), rd.y(), rd.z() );
        n = vec3d( rn.x(), rn.y(), rn.z() );
    }

    return NE::kReturnSuccess;
}


uint NEMeshContainer::createCylinderMesh( vec3f scale, QColor color, float spacing,
                                          float surfaceOffsetValue, bool topcap, bool bottomcap
                                          )
{
    m_TriMesh.clearDrawingBuffers();
    m_TriMesh.resize();

    float convert = float(M_PI / 180.0);
    vec3f pos = vec3f(0.0f, 0.0f, 0.0f);

    vec3f os(0.0, 0.0, 0.0);

    NECTNPVertex v;
    int index = -1;

    // top cap
    if( topcap )
    {
        // center vertex
        v.position = vec3f(0.0, 0.5 + os.y(), 0.0);
        v.tCoord = vec2f(0.5, 0.5);
        m_TriMesh.appendVertex(v);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );

        // first vertex
        v.position = vec3f(pos.x() + sin(0.0f) * (0.5f + os.x()), 0.5 + os.y(), pos.z() + cos(0.0f) * (0.5f + os.z()));
        v.tCoord = vec2f(0.0, 0.5);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        index += 2;

        for(int i = 5; i <= 355; i += 5)
        {
            float angleA = i * convert;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f + os.x()), 0.5 + os.y(), pos.z() + cos(angleA) * (0.5f + os.z()));
            v.tCoord = vec2f((sin(angleA) + 1.0) * 0.5, (cos(angleA) + 1.0) * 0.5);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);
            index++;

            NETriangleIndexed t(0, index - 1, index);
            m_TriMesh.appendTriangle(t);
        }

        // add last missing piece
        NETriangleIndexed t(0, index, 1);
        m_TriMesh.appendTriangle(t);

        // mesh of inner cap if the bottom cap is removed and there is a surface offset
        if(!bottomcap && os.length() != 0.0)
        {
            // center vertex
            v.position = vec3f(0.0, 0.5 - os.y(), 0.0);
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(),color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);

            // first vertex
            v.position = vec3f(pos.x() + sin(0.0f) * (0.5f - os.x()), 0.5 - os.y(), pos.z() + cos(0.0f) * (0.5f - os.z()));
            v.normal = vec3f(0.0, -1.0, 0.0);
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);

            index += 2;
            int center_index = index - 1;

            for(int i = 5; i <= 355; i += 5)
            {
                float angleA = i * convert;

                v.position = vec3f(pos.x() + sin(angleA) * (0.5f - os.x()), 0.5 - os.y(), pos.z() + cos(angleA) * (0.5f - os.z()));
                v.normal = vec3f(0.0, -1.0, 0.0);
                v.tCoord = vec2f(0.0, 0.0);
                v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_TriMesh.appendVertex(v);
                index++;

                NETriangleIndexed t(center_index, index - 1, index);
                m_TriMesh.appendTriangle(t);
            }

            // add last missing piece
            NETriangleIndexed t(center_index, index, center_index + 1);
            m_TriMesh.appendTriangle(t);
        }
    }

    // bottom cap
    if(bottomcap)
    {
        // center vertex
        v.position = vec3f(0.0, -0.5 - os.y(), 0.0);
        v.tCoord = vec2f(0.5, 0.5);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        // first vertex
        v.position = vec3f(pos.x() + sin(0.0f) * (0.5f + os.x()), -0.5 - os.y(), pos.z() + cos(0.0f) * (0.5f + os.z()));
        v.tCoord = vec2f(0.0, 0.5);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        index += 2;
        int center_index = index - 1;

        for(int i = 5; i <= 355; i += 5)
        {
            float angleA = i * convert;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f + os.x()), -0.5 - os.y(), pos.z() + cos(angleA) * (0.5f + os.z()));
            v.normal = vec3f(0.0, -1.0, 0.0);
            v.tCoord = vec2f((sin(angleA) + 1) * 0.5, (cos(angleA) + 1) * 0.5);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);
            index++;

            NETriangleIndexed t(center_index, index, index - 1);
            m_TriMesh.appendTriangle(t);
        }

        // add last missing piece
        NETriangleIndexed t(center_index, center_index + 1, index);
        m_TriMesh.appendTriangle(t);

        // mesh of inner cap if the top cap is removed and there is a surface offset
        if(!topcap && os.length() != 0.0)
        {
            // center vertex
            v.position = vec3f(0.0, -0.5 + os.y(), 0.0);
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);

            // first vertex
            v.position = vec3f(pos.x() + sin(0.0f) * (0.5f - os.x()), -0.5 + os.y(), pos.z() + cos(0.0f) * (0.5f - os.z()));
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);

            index += 2;
            center_index = index - 1;

            for(int i = 5; i <= 355; i += 5)
            {
                float angleA = i * convert;

                v.position = vec3f(pos.x() + sin(angleA) * (0.5f - os.x()), -0.5 + os.y(), pos.z() + cos(angleA) * (0.5f - os.z()));
                v.tCoord = vec2f(0.0, 0.0);
                v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_TriMesh.appendVertex(v);
                index++;

                NETriangleIndexed t(center_index, index - 1, index);
                m_TriMesh.appendTriangle(t);
            }

            // add last missing piece
            NETriangleIndexed t(center_index, index, center_index + 1);
            m_TriMesh.appendTriangle(t);
        }
    }

    //outer tube
    int outertube_begin = index + 1;
    for(int i = 0; i <= 355; i += 5)
    {
        float angleA = i * convert;

        v.position = vec3f(pos.x() + sin(angleA) * (0.5f + os.x()), -0.5f - os.y(), pos.z() + cos(angleA) * (0.5f + os.z()));
        v.tCoord = vec2f(angleA / 360.0, 0.0);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        v.position = vec3f(pos.x() + sin(angleA) * (0.5f + os.x()), 0.5f + os.y(), pos.z() + cos(angleA) * (0.5f + os.z()));
        v.tCoord = vec2f(angleA / 360.0f, 1.0f);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        index += 2;
    }

    for(int i = outertube_begin; i < (int)m_TriMesh.numVertices() - 2; i++)
    {
        if(i % 2 == 0)
        {
            NETriangleIndexed t(i, i + 2, i + 1);
            m_TriMesh.appendTriangle(t);
        }
        else
        {
            NETriangleIndexed t2(i, i + 1 , i + 2);
            m_TriMesh.appendTriangle(t2);
        }
    }

    //add last triangles
    NETriangleIndexed l1(outertube_begin, m_TriMesh.numVertices() - 1, m_TriMesh.numVertices() - 2);
    m_TriMesh.appendTriangle(l1);
    NETriangleIndexed l2(outertube_begin, outertube_begin + 1, m_TriMesh.numVertices() - 1);
    m_TriMesh.appendTriangle(l2);

    //inner tube
    if(!bottomcap || !topcap)
    {
        int innertube_begin = index + 1;
        double bottom_correct = 0;
        double top_correct = 0;
        if(!bottomcap)
            bottom_correct = 2.0 * os.y();
        if(!topcap)
            top_correct = 2.0 * os.y();

        for(int i = 0; i <= 355; i += 5)
        {
            float angleA = i * convert;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f - os.x()), -0.5 + os.y() - bottom_correct, pos.z() + cos(angleA) * (0.5f - os.z()));
            v.normal = -vec3f(pos.x() + sin(angleA) * 0.5f, 0.0, pos.z() + cos(angleA) * 0.5f).normalized();
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f - os.x()), 0.5 - os.y() + top_correct, pos.z() + cos(angleA) * (0.5f - os.z()));
            v.normal = -vec3f(pos.x() + sin(angleA) * 0.5f, 0.0, pos.z() + cos(angleA) * 0.5f).normalized();
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);

            index += 2;
        }

        for(int i = innertube_begin; i < (int) m_TriMesh.numVertices() - 2; i++)
        {
            if(i % 2 == 0)
            {
                NETriangleIndexed t(i, i + 1, i + 2);
                m_TriMesh.appendTriangle(t);
            }
            else
            {
                NETriangleIndexed t2(i, i + 2, i + 1);
                m_TriMesh.appendTriangle(t2);
            }
        }

        //add last triangles
        NETriangleIndexed l3(innertube_begin, m_TriMesh.numVertices() - 2, m_TriMesh.numVertices() - 1);
        m_TriMesh.appendTriangle(l3);
        NETriangleIndexed l4(innertube_begin, m_TriMesh.numVertices() - 1, innertube_begin + 1);
        m_TriMesh.appendTriangle(l4);
    }

    //if top cap is removed and offset is larger zero, we have to render a border
    if(!topcap && os.length() != 0.0)
    {
        // first outer vertex
        v.position = vec3f(pos.x() + sin(0.0f) * (0.5f + os.x()), 0.5 + os.y(), pos.z() + cos(0.0f) * (0.5f + os.z()));
        v.tCoord = vec2f(0.0, 0.0);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);
        index++;
        int first_outer = index;

        // first inner vertex
        v.position = vec3f(pos.x() + sin(0.0f) * (0.5f - os.x()), 0.5 + os.y(), pos.z() + cos(0.0f) * (0.5f - os.z()));
        v.tCoord = vec2f(0.0, 0.0);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);
        index++;
        int first_inner = index;

        for(int i = 5; i <= 355; i += 5)
        {
            float angleA = i * convert;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f + os.x()), 0.5 + os.y(), pos.z() + cos(angleA) * (0.5f + os.z()));
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);
            index++;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f - os.x()), 0.5 + os.y(), pos.z() + cos(angleA) * (0.5f - os.z()));
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);
            index++;

            NETriangleIndexed t(index - 2, index - 3, index - 1);
            m_TriMesh.appendTriangle(t);

            NETriangleIndexed t2(index - 1, index, index - 2);
            m_TriMesh.appendTriangle(t2);
        }

        // add last triangles
        NETriangleIndexed t3(index, index - 1, first_outer);
        m_TriMesh.appendTriangle(t3);

        NETriangleIndexed t4(first_outer, first_inner, index);
        m_TriMesh.appendTriangle(t4);

    }

    //if bottom cap is removed and offset is larger zero, we have to render a border
    if(!bottomcap && os.length() != 0.0)
    {
        // first outer vertex
        v.position = vec3f(pos.x() + sin(0.0f) * (0.5f + os.x()), -0.5 - os.y(), pos.z() + cos(0.0f) * (0.5f + os.z()));
        v.tCoord = vec2f(0.0, 0.0);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);
        index++;
        int first_outer = index;

        // first inner vertex
        v.position = vec3f(pos.x() + sin(0.0f) * (0.5f - os.x()), -0.5 - os.y(), pos.z() + cos(0.0f) * (0.5f - os.z()));
        v.tCoord = vec2f(0.0, 0.0);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);
        index++;
        int first_inner = index;

        for(int i = 5; i <= 355; i += 5)
        {
            float angleA = i * convert;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f + os.x()), -0.5 - os.y(), pos.z() + cos(angleA) * (0.5f + os.z()));
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);
            index++;

            v.position = vec3f(pos.x() + sin(angleA) * (0.5f - os.x()), -0.5 - os.y(), pos.z() + cos(angleA) * (0.5f - os.z()));
            v.tCoord = vec2f(0.0, 0.0);
            v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_TriMesh.appendVertex(v);
            index++;

            NETriangleIndexed t(index - 2, index - 1, index - 3);
            m_TriMesh.appendTriangle(t);

            NETriangleIndexed t2(index - 1, index - 2, index);
            m_TriMesh.appendTriangle(t2);
        }

        // add last triangles
        NETriangleIndexed t3(index - 1, first_outer, index);
        m_TriMesh.appendTriangle(t3);

        NETriangleIndexed t4(first_outer, index, first_inner);
        m_TriMesh.appendTriangle(t4);

    }

    for(uint i = 0; i < m_TriMesh.numVertices(); ++i)
    {
        m_TriMesh.vertices()[i].normal = vec3f(0, 0, 0);
    }


    for(uint i = 0; i < m_TriMesh.numTriangles(); ++i)
    {
        NECTNPVertex& p1 = m_TriMesh.vertices()[m_TriMesh.triangles()[i].p1];
        NECTNPVertex& p2 = m_TriMesh.vertices()[m_TriMesh.triangles()[i].p2];
        NECTNPVertex& p3 = m_TriMesh.vertices()[m_TriMesh.triangles()[i].p3];

        vec3f A = p2.position - p1.position;
        vec3f B = p3.position - p1.position;
        vec3f AB = vec3f::crossProduct(A, B);
        AB.normalize();

        p1.normal += AB;
        p2.normal += AB;
        p3.normal += AB;

        p1.normal.normalize();
        p2.normal.normalize();
        p3.normal.normalize();
    }

    //Add triangles now
    if(m_TriMesh.numTriangles() > 0)
    {
        m_TriMesh.recomputeNormals();
    }



    return NE::kReturnSuccess;
}

QList<NETriangle> NEMeshContainer::createPolygons(int iterations)
{
    QMap<uint, NETriangle> f;
    int i, it;
    double a;
    vec3f p[6] = {vec3f(0, 0, 0.5),  vec3f(0, 0, -0.5),  vec3f(-0.5, -0.5, 0),  vec3f(0.5, -0.5, 0),  vec3f(0.5, 0.5, 0), vec3f(-0.5, 0.5, 0)};
    vec3f pa, pb, pc;
    int nt = 0, ntold;

    /* Create the level 0 object */
    a = 1 / sqrt(2.0);
    for(i = 0; i < 6; i++)
    {
        p[i].setX(p[i].x() * a);
        p[i].setY(p[i].y() * a);
    }
    f[0].v1.position = p[0];
    f[0].v2.position = p[3];
    f[0].v3.position = p[4];
    f[1].v1.position = p[0];
    f[1].v2.position = p[4];
    f[1].v3.position = p[5];
    f[2].v1.position = p[0];
    f[2].v2.position = p[5];
    f[2].v3.position = p[2];
    f[3].v1.position = p[0];
    f[3].v2.position = p[2];
    f[3].v3.position = p[3];
    f[4].v1.position = p[1];
    f[4].v2.position = p[4];
    f[4].v3.position = p[3];
    f[5].v1.position = p[1];
    f[5].v2.position = p[5];
    f[5].v3.position = p[4];
    f[6].v1.position = p[1];
    f[6].v2.position = p[2];
    f[6].v3.position = p[5];
    f[7].v1.position = p[1];
    f[7].v2.position = p[3];
    f[7].v3.position = p[2];
    nt = 8;

    if(iterations < 1)
        return f.values();

    /* Bisect each edge and move to the surface of a unit sphere */
    for(it = 0; it < iterations; it++)
    {
        ntold = nt;
        for(i = 0; i < ntold; i++)
        {
            pa = (f[i].v1.position + f[i].v2.position) / 2;
            pb = (f[i].v2.position + f[i].v3.position) / 2;
            pc = (f[i].v3.position + f[i].v1.position) / 2;
            pa.normalize();
            pb.normalize();
            pc.normalize();
            pa = pa * 0.5;
            pb = pb * 0.5;
            pc = pc * 0.5;
            f[nt].v1.position = f[i].v1.position;
            f[nt].v2.position = pa;
            f[nt].v3.position = pc;
            nt++;
            f[nt].v1.position = pa;
            f[nt].v2.position = f[i].v2.position;
            f[nt].v3.position = pb;
            nt++;
            f[nt].v1.position = pb;
            f[nt].v2.position = f[i].v3.position;
            f[nt].v3.position = pc;
            nt++;
            f[i].v1.position = pa;
            f[i].v2.position = pb;
            f[i].v3.position = pc;
        }
    }

    return f.values();
}



vec2f NEMeshContainer::computetCoord(vec3f position)
{
    vec2f retval(0, 0);

    float sin_phi = ((position.y() / 0.5) + 1) * 0.5;
    retval.setX(sin_phi);

    float atan_omega = (atan2(position.x(), position.z()) + M_PI) / (M_PI * 2.0);
    retval.setY(atan_omega);

    return retval;
}

void NEMeshContainer::extrudeFace(uint faceNum, vec3f normal, double distance)
{

}

void NEMeshContainer::duplicateFace(uint faceNum)
{

}

void NEMeshContainer::deleteFace(uint faceNum)
{

}



uint NEMeshContainer::createSphereMesh(QColor color)
{
    m_TriMesh.resize();

    //	float convert = float(M_PI/180.0);
    //	vec3f pos = vec3f(0.0f,0.0f,0.0f);

    NECTNPVertex v;

    QList<NETriangle> f = createPolygons(5);

    uint count = 0;
    for(NETriangle tri: f)
    {
        v.position = tri.v1.position;
        v.normal = tri.v1.position.normalized();
        v.tCoord = computetCoord(v.position);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        v.position = tri.v2.position;
        v.normal =  tri.v2.position.normalized();
        v.tCoord = computetCoord(v.position);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        v.position = tri.v3.position;
        v.normal =  tri.v3.position.normalized();
        v.tCoord = computetCoord(v.position);
        v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
        m_TriMesh.appendVertex(v);

        NETriangleIndexed t(count, count + 1, count + 2);
        m_TriMesh.appendTriangle(t);
        count += 3;
    }

    return NE::kReturnSuccess;
}



uint NEMeshContainer::createCuboidMesh(QColor color)
{
    m_TriMesh.resize();
    NECTNPVertex v;

    // left wall
    //v0
    v.position = vec3f(-0.5f, -0.5f, -0.5f);
    v.normal = vec3f(-1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(),color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);

    //v1
    v.position = vec3f(-0.5f, -0.5f, 0.5f);
    v.normal = vec3f(-1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(),color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v2
    v.position = vec3f(-0.5f, 0.5f, -0.5f);
    v.normal = vec3f(-1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(),color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v3
    v.position = vec3f(-0.5f, 0.5f, 0.5f);
    v.normal = vec3f(-1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);


    //right wall
    //v4
    v.position = vec3f(0.5f, -0.5f, -0.5f);
    v.normal = vec3f(1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(),color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v5
    v.position = vec3f(0.5f, -0.5f, 0.5f);
    v.normal = vec3f(1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v6
    v.position = vec3f(0.5f, 0.5f, -0.5f);
    v.normal = vec3f(1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v7
    v.position = vec3f(0.5f, 0.5f, 0.5f);
    v.normal = vec3f(1.0f, 0.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);


    //far wall
    //v8
    v.position = vec3f(-0.5f, -0.5f, -0.5f);
    v.normal = vec3f(0.0f, 0.0f, -1.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v9
    v.position = vec3f(-0.5f, 0.5f, -0.5f);
    v.normal = vec3f(0.0f, 0.0f, -1.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v10
    v.position = vec3f(0.5f, 0.5f, -0.5f);
    v.normal = vec3f(0.0f, 0.0f, -1.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v11
    v.position = vec3f(0.5f, -0.5f, -0.5f);
    v.normal = vec3f(0.0f, 0.0f, -1.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f(color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);

    //near wall
    //v12
    v.position = vec3f(-0.5f, -0.5f, 0.5f);
    v.normal = vec3f(0.0f, 0.0f, 1.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(),color.greenF(),color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v13
    v.position = vec3f(-0.5f, 0.5f, 0.5f);
    v.normal = vec3f(0.0f, 0.0f, 1.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(),color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v14
    v.position = vec3f(0.5f, 0.5f, 0.5f);
    v.normal = vec3f(0.0f, 0.0f, 1.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v15
    v.position = vec3f(0.5f, -0.5f, 0.5f);
    v.normal = vec3f(0.0f, 0.0f, 1.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);

    //bottom wall
    //v16
    v.position = vec3f(-0.5f, -0.5f, -0.5f);
    v.normal = vec3f(0.0f, -1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v17
    v.position = vec3f(-0.5f, -0.5f, 0.5f);
    v.normal = vec3f(0.0f, -1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v18
    v.position = vec3f(0.5f, -0.5f, 0.5f);
    v.normal = vec3f(0.0f, -1.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v19
    v.position = vec3f(0.5f, -0.5f, -0.5f);
    v.normal = vec3f(0.0f, -1.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);

    //top wall
    //v20
    v.position = vec3f(-0.5f, 0.5f, -0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(),color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v21
    v.position = vec3f(-0.5f, 0.5f, 0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v22
    v.position = vec3f(0.5f, 0.5f, 0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    //v23
    v.position = vec3f(0.5f, 0.5f, -0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);

    NETriangleIndexed t1(0, 1, 3), t2(0, 3, 2), t3(5, 6, 7), t4(5, 4, 6); // left-right
    NETriangleIndexed t5(11, 9, 10), t6(11, 8, 9), t7(12, 14, 13), t8(12, 15, 14); // near-far
    NETriangleIndexed t9(18, 16, 19), t10(18, 17, 16), t11(21, 23, 20), t12(21, 22, 23); // bottom-top

    /*
    NETriangleIndexed t1(0, 1, 3), t2(0, 3, 2), t3(5, 6, 7), t4(5, 4, 6); // left-right
    NETriangleIndexed t5(1, 5, 7), t6(1, 7, 3), t7(0, 4, 2), t8(4, 6, 2); // near-far
    NETriangleIndexed t9(0, 1, 5), t10(0, 5, 4), t11(2, 3, 7), t12(2, 7, 6); // bottom-top
    */

    m_TriMesh.appendTriangle(t1);
    m_TriMesh.appendTriangle(t2);
    m_TriMesh.appendTriangle(t3);
    m_TriMesh.appendTriangle(t4);
    m_TriMesh.appendTriangle(t5);
    m_TriMesh.appendTriangle(t6);
    m_TriMesh.appendTriangle(t7);
    m_TriMesh.appendTriangle(t8);
    m_TriMesh.appendTriangle(t9);
    m_TriMesh.appendTriangle(t10);
    m_TriMesh.appendTriangle(t11);
    m_TriMesh.appendTriangle(t12);


    //6 sides on cube, 4 verts each (corners)

    //create quad faces for extruding
    m_QuadFaces.clear();

    NEPrimitivePolygon face;

    //left right
    face.clear();
    face.vertices.push_back(0);
    face.vertices.push_back(1);
    face.vertices.push_back(2);
    face.vertices.push_back(3);
    face.triangles.push_back(t1);
    face.triangles.push_back(t2);
    face.size = 4;
    m_QuadFaces.push_back(face);

    face.clear();
    face.vertices.push_back(4);
    face.vertices.push_back(5);
    face.vertices.push_back(6);
    face.vertices.push_back(7);
    face.triangles.push_back(t3);
    face.triangles.push_back(t4);
    face.size = 4;
    m_QuadFaces.push_back(face);

    //near far
    face.clear();
    face.vertices.push_back(8);
    face.vertices.push_back(9);
    face.vertices.push_back(10);
    face.vertices.push_back(11);
    face.triangles.push_back(t5);
    face.triangles.push_back(t6);
    face.size = 4;
    m_QuadFaces.push_back(face);

    face.clear();
    face.vertices.push_back(12);
    face.vertices.push_back(13);
    face.vertices.push_back(14);
    face.vertices.push_back(15);
    face.triangles.push_back(t7);
    face.triangles.push_back(t8);
    face.size = 4;
    m_QuadFaces.push_back(face);

    //bottom top
    face.clear();
    face.vertices.push_back(16);
    face.vertices.push_back(17);
    face.vertices.push_back(18);
    face.vertices.push_back(19);
    face.triangles.push_back(t9);
    face.triangles.push_back(t10);
    face.size = 4;
    m_QuadFaces.push_back(face);

    face.clear();
    face.vertices.push_back(20);
    face.vertices.push_back(21);
    face.vertices.push_back(22);
    face.vertices.push_back(23);
    face.triangles.push_back(t11);
    face.triangles.push_back(t12);
    face.size = 4;
    m_QuadFaces.push_back(face);




//    //left right
//    face.clear();
//    face.vertices.push_back(0);
//    face.vertices.push_back(1);
//    face.vertices.push_back(2);
//    face.vertices.push_back(3);
//    face.triangles.push_back(t1);
//    face.triangles.push_back(t2);
//    face.size = 4;
//    m_QuadFaces.push_back(face);

//    face.clear();
//    face.vertices.push_back(4);
//    face.vertices.push_back(5);
//    face.vertices.push_back(7);
//    face.vertices.push_back(6);
//    face.triangles.push_back(t3);
//    face.triangles.push_back(t4);
//    face.size = 4;
//    m_QuadFaces.push_back(face);

//    //near far
//    face.clear();
//    face.vertices.push_back(1);
//    face.vertices.push_back(5);
//    face.vertices.push_back(7);
//    face.vertices.push_back(3);
//    face.triangles.push_back(t5);
//    face.triangles.push_back(t6);
//    face.size = 4;
//    m_QuadFaces.push_back(face);

//    face.clear();
//    face.vertices.push_back(0);
//    face.vertices.push_back(4);
//    face.vertices.push_back(6);
//    face.vertices.push_back(2);
//    face.triangles.push_back(t7);
//    face.triangles.push_back(t8);
//    face.size = 4;
//    m_QuadFaces.push_back(face);

//    //bottom top
//    face.clear();
//    face.vertices.push_back(0);
//    face.vertices.push_back(1);
//    face.vertices.push_back(5);
//    face.vertices.push_back(4);
//    face.triangles.push_back(t9);
//    face.triangles.push_back(t10);
//    face.size = 4;
//    m_QuadFaces.push_back(face);

//    face.clear();
//    face.vertices.push_back(2);
//    face.vertices.push_back(3);
//    face.vertices.push_back(7);
//    face.vertices.push_back(6);
//    face.triangles.push_back(t11);
//    face.triangles.push_back(t12);
//    face.size = 4;
//    m_QuadFaces.push_back(face);

    unorderedEdgesFromQuads( m_QuadFaces, m_QuadEdges );

    return NE::kReturnSuccess;
}



uint NEMeshContainer::createCustomMesh( const QString& filename,
                                        float scalex, float scaley, float scalez, bool swapUpAxis )
{
    m_TriMesh.loadFromFile( filename, false, false, scalex, scaley, scalez, swapUpAxis );

    return NE::kReturnSuccess;
}

uint NEMeshContainer::createPlaneMesh( QColor color )
{
    m_TriMesh.resize();

    NECTNPVertex v;

    v.position = vec3f(-0.5f, 0.0f, -0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f, 0.0f, 0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f, 0.0f, 0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 1.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f, 0.0f, -0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(1.0f, 0.0f);
    v.color  = vec4f( color.redF(), color.greenF(), color.blueF(), 1.0 );
    m_TriMesh.appendVertex(v);

    m_Planenormal = v.normal;

    //Add triangles now
    NETriangleIndexed t1(1, 3, 0), t2(1, 2, 3);
    m_TriMesh.appendTriangle(t1);
    m_TriMesh.appendTriangle(t2);

    //create quad face for extruding
    m_QuadFaces.clear();

    NEPrimitivePolygon face;
    face.clear();

    face.size = 4;
    face.vertices.push_back(0);
    face.vertices.push_back(1);
    face.vertices.push_back(2);
    face.vertices.push_back(3);

    face.triangles.push_back(t1);
    face.triangles.push_back(t2);

    m_QuadFaces.push_back(face);

    unorderedEdgesFromQuads( m_QuadFaces, m_QuadEdges );

    return NE::kReturnSuccess;
}

