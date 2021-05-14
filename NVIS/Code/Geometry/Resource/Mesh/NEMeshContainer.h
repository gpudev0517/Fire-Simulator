#pragma once
#include "Utility/FilePath.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"


NE_FWD_DECL_4(class, NETriangleIndexed, NEIndexedTriMesh, FilePath, NE_PRIVATE(NESpatialMesh))

typedef int NEVertexID;
typedef int NEFaceID;
typedef int NEEdgeID;

struct NEPrimitiveEdge
{
    NEVertexID v[2];
    std::vector<NEFaceID> sharedfaces;

    NEVertexID& start() { return v[0]; }
    const NEVertexID& start() const { return v[0]; }

    NEVertexID& end() { return v[1]; }
    const NEVertexID& end() const { return v[1]; }

    NEPrimitiveEdge()
    {
        v[0] = v[1] = -1;
    }
};

//
// Data structure to store quads
//

struct NEPrimitivePolygon
{
    int size;
    std::vector<NEVertexID> vertices;
    std::vector<NEEdgeID> edges;


    std::vector<NETriangleIndexed> triangles;

    void clear()
    {
        size = 0;
        vertices.clear();
        triangles.clear();
    }
};




class NECEXP_GEOMETRY NEMeshContainer : public NEMesh
{
public:

    NEMeshContainer();
    virtual ~NEMeshContainer();


    //
    //! Returns the associated triangle mesh
    //!
    NEIndexedTriMesh& triangleMesh();
    const NEIndexedTriMesh& triangleMesh() const;
    void setTriMesh(const NEIndexedTriMesh& val);

    uint createCuboidMesh(QColor color);
    uint createBoxMesh(vec3f scale, float spacing, float surfaceOffsetValue);
    uint createPlaneMesh(QColor color);

    uint createCylinderMesh(vec3f scale, QColor color, float spacing, float surfaceOffsetValue,
                            bool topcap, bool bottomcap);
    uint createCylinderMesh(vec3f scale, QColor color, float spacing, float surfaceOffsetValue,
                            bool topcap, bool bottomcap, vec3f cylAxis);


    uint createSphereMesh(QColor color);


    uint createCustomMesh(const QString &filename, float scalex, float scaley, float scalez, bool swapUpAxis);

    uint SetFlipMesh(const bool &val);
    vec2f computetCoord(vec3f position);


    //
    // Extrude the face along the normal direction along a certain distance
    //
    void extrudeFace( uint faceNum, vec3f normal, double distance );

    //
    // Duplicate the face with connections preserved to the existing face.
    // Vertices are duplicated and connected to the vertices of the face being duplicated
    //
    void duplicateFace( uint faceNum );

    //
    // Delete the face
    //
    void deleteFace( uint faceNum );

    ///
    /// \brief createPolygons
    /// \param iterations
    /// \return
    ///

    QList<NETriangle> createPolygons(int iterations);

    std::vector<NEPrimitivePolygon>& quadFaces();

protected:

    uint computeCenter() ;

    // Compute the centroid and set it as the center
    void computeCentroid();

    // Compute the center of mass and set is as the center
    void computeCenterOfMass();

    FilePath m_SpatialMeshFile;

    bool m_FlipMesh;

    // The render mesh representation of the rigid
    NEIndexedTriMesh m_TriMesh;
    vec3f m_Planenormal;

    std::vector<NEPrimitivePolygon> m_QuadFaces;
    std::vector<NEPrimitiveEdge>    m_QuadEdges;


    void createBoxVertices(const vec3f& halfSpacing, vec3f os, float normalInvert);

    void unorderedEdgesFromTriangles( const std::vector<NETriangleIndexed>& triangles,
                                       std::vector< NEPrimitiveEdge >& edges_out );

    void unorderedEdgesFromQuads( const std::vector<NEPrimitivePolygon>& quads,
                                     std::vector<NEPrimitiveEdge>& edges_out );


};

