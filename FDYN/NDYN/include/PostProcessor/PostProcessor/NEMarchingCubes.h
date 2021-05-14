#pragma once

#include "Mesh/NEMeshConstructor.h"
#include "NEMCGrid.h"

#include "Core/Utility/CellIndex3D.h"


NE_FWD_DECL_4(class, NEParticleFluidSolver, NEFluidParticle, NEIndexedTriMesh, NE_PRIVATE(NEMarchingCubes))

//!  Implementation of general marching cubes.
/*!
  The main surface reconstruction method used in the framework is the Marching Cubes
  method. In the method, world is divided into equidistant voxels, for whose vertices,
  scalar values are computed wrt. any isosurface parameterization approach. Those
  isovalues are used for the triangulation in the final step.
*/
class NECEXP_NEPOSTPROCESSOR NEMarchingCubes : public NEMeshConstructor
{
public:
    enum SolidBase
    {
        octahedron,
        icosahedron
    };

private:

    Q_OBJECT
    NE_PROP_RW(double, IsoThreshold, "Isosurface Threshold", "The threshold for the isovalue", true,)
    NE_PROP_RW(double, Resolution, "Grid Resolution", "Marching Cubes grid resolution.", true, virtual)
    NE_PROP_R(double, VoxelSize, "Voxel Size", "Marching Cubes voxel size", true)
    NE_PROP_RW(bool, SaveBinaryMesh, "SaveBinary Mesh", "Save the mesh in binary format", true,)
    NE_PROP_RW(bool, UseVertexColor, "Use Vertex Color", "Use vertex color instead of mesh color", true,)
    NE_PROP_RW(bool, DrawGrid, "Draw Grid", "Draws the Marching Cubes grid", true,)
    NE_PROP_RW(bool, DrawGridOutline, "Draw Grid Outline", "Draws the grid outline of the Marching Cubes grid", true,)
    NE_PROP_RW(bool, DrawSurfaceNormals, "Draw Surface Normals", "Draws the surface normals of the generated triangle mesh", true,)
    NE_PROP_RW(bool, DrawVertices, "Draw Vertices", "Draws the vertices of the generated triabngle mesh", true,)
    NE_PROP_RW(double, DegTriThreshold, "Triangle Degree Threshold", "The threshold used for eliminating triangles", true,)
    NE_PROP_GROUP_START(SplashTesselation, "Splash Tessellation", "", true)
    NE_PROP_RW(bool, TesselateSplash, "Tesselate Splash", "Flag that controls splash tesselation", true,)
    NE_PROP_RW(uint, MinNrNeighbors, "Min. Number of Neighbors", "The minimum umber of neighbors when determining splash particles", true,)
    NE_PROP_RW(uint, TesselSubdiv, "Tesselation Subdivision", "The amount of subdivision applied to the splash particles", true,)
    NE_PROP_RW(double, SplashRadiusMultiplier, "Splash Rsdius Multiplier", "Used for scaling the splash particles", true,)
    NE_PROP_RW(SolidBase, TesselSolidBase, "Tesselation Base", "The solid basis used for tesselation", true,)
    NE_PROP_GROUP_END(SplashTesselation)
    NE_PROP_GROUP_START(ExportRelated, "Export Related", "", true)
    NE_PROP_RW(bool, ExportTextureCoord, "Export Texture Coord", "Export texture coordinates", true,)
    NE_PROP_RW(bool, ExportVelocities, "Export Velocities", "Used for exporting per-vertex velocities", true,)
    NE_PROP_RW(bool, ExportSplashes, "Export Splashes", "Used for exporting splash particles", true,)
    NE_PROP_GROUP_END(ExportRelated)

    Q_ENUMS(SolidBase)

public:
    NEMarchingCubes(NENodeFactory* val);
    virtual ~NEMarchingCubes();

    //// LOOKUP TABLES ARE DEFINED AS D METHODS ////
    //! Surface kernel function
     float surfaceKernel(float s);
    //! Method is used for forming the triangles that cut a voxel.
     int trianglesForVoxel(const int& cubeIndex, const int& i);
    //! Method is used for forming the triangles that cut a voxel.
     int cuttedEdges(const int& cubeIndex);
    //!
     int edgeVoxelVertexId(const int&, const int&);
    //! Returns the index of the endpoint for a vertex with index vertexIndex.
     int edgeEndPoint(const int& edgeIndex);
    //! Returns the std::vector position, relative to vertex 0 for a vertex with vertexIndex.
     const vec3f& vertexOffset(const int& vertexIndex);
    //! Returns the CellIndex offset, relative to vertex 0 for a vertex with vertexIndex.
     const CellIndex3D& vertexCellIndexOffset(const int& vertexIndex);

    //// GENERAL INITIALIZATION AND UPDATE METHODS ////
    //! Init function performs necessary initializations
    virtual uint init(uint initMode = NE::kInitOnCreate);
    //! Preprocesses data.
    virtual void doPreprocess() = 0;
    //! Update method called from post processor manager.
    virtual void update() = 0;
    //! Compute the scalar field out of position data.
    virtual void computeScalarField() = 0;
    //! Compute the MC grid
    virtual void computeGrid(bool splashExtract = false) = 0;
    //! Triangulates the computed scalar field.
    virtual void triangulate();
    uint SetPosition(const vec3f& val);
    virtual void resetProperties();
    void setPreprocessTime(double PreprocessTime);
    void setScalarFieldComputationTime(double ScalarFieldComputationTime);
    void setTriangulationTime(double TriangulationTime);
    void setTotalTime(double TotalTime);
	double preprocessTime() const;
	double scalarFieldComputationTime() const;
	double triangulationTime() const;
    double totalTime() const;

    // For rendering
    NEIndexedTriMesh& mesh();
    NEMCGrid& grid();


public slots:
    //! Saves data for this simulation/animation frame.
    uint saveFrameData(QString path);
    //! Loads data for this simulation/animation frame.
    uint loadFrameData(QString path);

protected:
    NE_DERIVED(NESpatial, Protected)
    //! Creates a temporary voxel for the vertex with vertexIndex.
    NEMCVoxel createVoxel(NEMCGrid& grid, const CellIndex3D& vertexIndex3D);
    void setEmptyVertexIsovalue(double val);
    double emptyVertexIsovalue();
    void renderInMotion();
    uint setVoxelSize(double val);

private:

    //! Colorize the surface according to the chosen method
    enum SurfaceColor
    {
        basic = 0,
        velocity,
        acceleration,
        density,
        pressure
    };

    NE_DECL_PRIVATE(NEMarchingCubes)
};


Q_DECLARE_INTERFACE(NEMarchingCubes, "NEMarchingCubes")
