#pragma once

#include "Resource/NEMesh.h"
#include "Utility/geometry.h"


NE_FWD_DECL_1(class, NE_PRIVATE(NEIndexedTriMesh))



//! An indexed triangle mesh class that is more memory friendly.

/*!
	In this design, the m_VBuffer vertex array is traversed by using
	the triangle indices that are defined in m_Triangles array.
*/

class NECEXP_CORE NEIndexedTriMesh : public NEMesh
{

    //! Used to find the non-manifold triangles
    struct VertexTriCouple;

public:
    friend class NERenderer;
    NEIndexedTriMesh();
    NEIndexedTriMesh(const NEIndexedTriMesh& mesh);
    ~NEIndexedTriMesh();

    //! Returns the number of triangles in the triangle mesh
    uint numTriangles() const ;

    //! Returns the number of vertices in the triangle mesh.h
    //! Duplicate vertices are included for separate normals in sharp regions (only if there is more than one normal for a vertex)
    uint numVertices() const;

    //! Appends a vertex to the list of vertices.
    NEVertexID appendVertex(const NECTNPVertex& v);

    //! Appends a vertex (if it is not already available in the mesh) and returns the index of the vertex. Consider, if this is needed, because
    //! checking for duplicates is expensive.
    NEVertexID appendVertex(const NECTNPVertex &v, bool checkDuplicate);

    //! Appends an edge (convention first index < second index)
    NEEdgeID appendEdge(uint a, uint b);

    //! Appends a motion std::vector to the list of motions.
    void appendVelocityVector(const vec3f& v);

    //! Appends a triangle to the list of triangles.
    NETriangleID appendTriangle(const NETriangleIndexed& t);

    ///
    /// \brief removeTriangle
    /// \param t
    ///
    ///  Remove the triangle with triangle ID t
    ///
    void removeTriangle(NETriangleIndexed &t);

    //! Appends another mesh to itself.
    void appendMesh(const NEIndexedTriMesh& mesh);


    void recomputeNormals();
    void recomputeVertexNormals();
    void calcNormalsMWA();
    void calcNormalsMWASEL();
    void calcNormalsMWE();

    //! Flips the order of points of triangles and the direction of normals such that
    //! inside-pointing triangles point outside, and vice versa.
    void flip();

    //! Returns the triangles
    const std::vector<NETriangleIndexed>& triangles() const;

    //! Returns the triangles
    std::vector<NETriangleIndexed>& triangles();

    //! Returns the vertices
    const std::vector<NECTNPVertex>& vertices() const;

    //! Returns the vertices
    std::vector<NECTNPVertex>& vertices();

    //! Returns the vertex indices
    const std::vector<NEVertexID>& vids();

    //! Returns the velocities
    std::vector<vec3f>& velocities();

    //! Returns the velocities
    const std::vector<vec3f>& velocities() const;

    //! Returns the edges
    const std::multimap<uint, uint>& edges() const;




    //! Loads the mesh from a file (format is selected depending on the file extension).
    uint loadFromFile(const QString& filename, bool saveBinary = false,
                      bool suppressAssimp = false,
                      float scalex = 1.0f, float scaley = 1.0f, float scalez = 1.0f, bool swapUpAxis = false );

    //! Saves the mesh to a file (format is selected depending on the file extension).
    uint saveToFile(const QString& filename, bool saveBinary = false, int saveVersion = 1);

    //! Loads the velocities from a file (format is selected depending on the file extension).
    uint loadVelocitiesFromFile(const QString& filename, bool saveBinary = false);

    //! Saves the velocities to a file (format is selected depending on the file extension).
    uint saveVelocitiesToFile(const QString& filename, bool saveBinary = false);

    //    //! Allocate GPU buffers for the object
    //    void createDrawingBuffers(uint mode);
    //! Refresh GPU buffer contents for the object (the mode can also be changed)
    void rehintDrawingBuffers(uint mode);

    //! Clear drawing buffers for the object
    void clearDrawingBuffers();

    //!
    void updateDrawingBuffers(uint mode);

    //! Resizes the whole mesh.
    void resize(const int sizeV=0, const int sizeT=0);
    int splashTriStartIndex() const;
    void setSplashTriStartIndex(int val);
    int splashVertexStartIndex() const;
    void setSplashVertexStartIndex(int val);

    bool swapUpAxes() const;

    //! Returns a copy of this mesh which is transformed by the provided transformation matrix transM
    NEIndexedTriMesh transformedMesh(const matrix44f& transM) const;
    uint loadFromBgeoTopo(const QString &file);
    uint loadBgeoPoints(const QString &bgeo0);
    //uint loadBgeoInterpolatedPoints(double curTime, const QString &bgeo0, const QString &bgeo1);


private:
    NE_DECL_PRIVATE(NEIndexedTriMesh)
};
