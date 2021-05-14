#pragma once

#include "Utility/CellIndex3D.h"

class NECEXP_NEPOSTPROCESSOR NEMCVertex
{
public:

    NEMCVertex()
    {
        m_Normal.set(0.0, 0.0, 0.0);
    }

    NEMCVertex(bool surface, const CellIndex3D& cI) :
        m_Isovalue(0.f),  /*m_SurfaceVertex(surface),*/
        m_Coordinates(cI)
    {
        m_IntersectionPointId[0] = m_IntersectionPointId[1] = m_IntersectionPointId[2] = -1;
    }

    ~NEMCVertex()
    {
    }

    float& isovalue()
    {
        return m_Isovalue;
    }

    void setIsovalue(const float& value)
    {
        m_Isovalue = value;
    }

    int intersectionPointId(const uint i)
    {
        return m_IntersectionPointId[i];
    }

    void setIntersectionPointId(const uint i, const int value)
    {
        m_IntersectionPointId[i] = value;
    }

    const CellIndex3D& coordinates()
    {
        return m_Coordinates;
    }

    void setCoordinates(const CellIndex3D& c)
    {
        m_Coordinates = c;
    }

    /*bool isSurfaceVertex() {return m_SurfaceVertex;}
    void setStateOfSurfaceVertex(bool value) {m_SurfaceVertex = value;} */

    vec3f& normal()
    {
        return m_Normal;
    }

    void setNormal(const vec3f& value)
    {
        m_Normal = value;
    }

    vec3f& velocity() {return m_Velocity;}
    void setVelocity(const vec3f& value) {m_Velocity = value;}

    //int flag() {return m_Flag;}
    //void setFlag(int value) {m_Flag = value;}

    virtual void clear()
    {
        for(int i = 0; i < 3; i++)
            m_IntersectionPointId[i] = -1;
        m_Isovalue = 0.0f;

        m_Normal.set(0.0, 0.0, 0.0);
        m_Velocity.set(0.0,0.0,0.0);
        //m_Flag = 0;
    }
    vec4f& Color() ;
    void setColor(const vec4f &Color);

protected:

    //! The isovalue of the vertex
    float m_Isovalue;

    //! Ids of the three possible intersection points going out of the vertex
    int m_IntersectionPointId[3];

    //! x,y,z coordinates of the vertex
    CellIndex3D m_Coordinates;

    //! Normal of the vertex
    vec3f m_Normal;

    //! Color of the vertex
    vec4f m_Color;

    //! State of the vertex
    //bool m_SurfaceVertex;
    //! Motion of the vertex
    vec3f m_Velocity;
    //! Flag of the vertex
    //int m_Flag;
};
/*!
  The Marching Cubes (MC) grid is defined by its vertices that reside on the
  eight corners of any MC voxel. Each vertex stores its corresponding isovalue,
  coordinates and the three possible intersection points which are on the edges
  going out of the vertex.
*/
class NECEXP_NEPOSTPROCESSOR NEMCGridVertex :
    public NEMCVertex
{
public:

    NEMCGridVertex() :
        NEMCVertex()
    {
    }

    ~NEMCGridVertex()
    {
    }

    vec3f& nominator()
    {
        return m_Nominator;
    }

    void setNominator(const vec3f& value)
    {
        m_Nominator = value;
    }

    void clear()
    {
        NEMCVertex::clear();
        m_Nominator.setX(0.0);
        m_Nominator.setY(0.0);
        m_Nominator.setZ(0.0);
    }
protected:
    //! The nominator computed for the vertex
    vec3f m_Nominator;
};

//Used for adaptive grids project
class NECEXP_NEPOSTPROCESSOR NEMCAdaptiveGridVertex
{
public:

    NEMCAdaptiveGridVertex()
    {
    }

    NEMCAdaptiveGridVertex(bool surface, const CellIndex3D& cI) :
        m_Coordinates(cI)
    {
    }

    ~NEMCAdaptiveGridVertex()
    {
    }

    int flag()
    {
        return m_Flag;
    }

    void setFlag(int value)
    {
        m_Flag = value;
    }

    const CellIndex3D& coordinates()
    {
        return m_Coordinates;
    }

    void setCoordinates(const CellIndex3D& c)
    {
        m_Coordinates = c;
    }

    void clear()
    {
        m_Flag = -1;
    }
protected:
    int m_Flag;
    //! x,y,z coordinates of the vertex
    CellIndex3D m_Coordinates;
};

/*!
  Grid structure is the base structure for the Marching Cubes method. Isosurface
  parameterization is done by computing the isovalues for the grid vertices. This
  parameterization is then used for the final triangulation.
*/
class NECEXP_NEPOSTPROCESSOR NEMCGrid
{
public:

    NEMCGrid()
    {
        m_VMin = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
        m_VMax = vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    ~NEMCGrid()
    {
    }

    //! Returns the total number of vertices in the grid.
    int totalNrOfVertices() const
    {
        return (m_NrOfVertices.x() * m_NrOfVertices.y() * m_NrOfVertices.z());
    }

    //! Returns the total number of cells in the grid.
    int totalNrOfCells() const
    {
        int cells;
        cells = (m_NrOfVertices.x() - 1) * (m_NrOfVertices.y() - 1) * (m_NrOfVertices.z() - 1);

        if(cells < 0)
            cells = 0;
        return cells;
    }

    //! Returns the number of vertices in x,y and z directions.
    const vec3i& nrOfVertices() const
    {
        return m_NrOfVertices;
    }

    //! Returns the number of cells in x,y and z directions.
    vec3i nrOfCells() const
    {
        return vec3i(m_NrOfVertices.x() - 1, m_NrOfVertices.y() - 1, m_NrOfVertices.z() - 1);
    }

    //! Returns the number of cells in x,y directions.
    int nrOfCellsXY() const
    {
        return ((m_NrOfVertices.x() - 1) * (m_NrOfVertices.y() - 1));
    }

    //! Returns the total number of vertices in x,y directions.
    int nrOfVerticesXY() const
    {
        return (m_NrOfVertices.x() * m_NrOfVertices.y());
    }

    //! Returns the minimum corner
    vec3f& vmin()
    {
        return m_VMin;
    }

    //! Returns the maximum corner
    vec3f& vmax()
    {
        return m_VMax;
    }

    //! Returns the grid corner
    vec3f vcorner() const
    {
        return m_VMin;
    }

    const double& voxelSize() const
    {
        return m_VoxelSize;
    }

    float inverseVoxelSize() const
    {
        return m_InverseVoxelSize;
    }

    void setVoxelSize(const float& size)
    {
        m_VoxelSize = size;
    }

    void setInverseVoxelSize()
    {
        m_InverseVoxelSize = 1.0f / m_VoxelSize;
    }

    void setNrVerticesX(int numberX)
    {
        m_NrOfVertices.setX(numberX);
    }

    void setNrVerticesY(int numberY)
    {
        m_NrOfVertices.setY(numberY);
    }

    void setNrVerticesZ(int numberZ)
    {
        m_NrOfVertices.setZ(numberZ);
    }


    //! Returns the index of the vertex in the grid.
    int vertexGridId(const int i, const int j, const int k) const
    {
        return i + j * m_NrOfVertices.x() + k * nrOfVerticesXY();
    }

    //! Returns the position of the vertex in the grid.
    vec3f vertexPosition(const CellIndex3D& cellId)
    {
        return (vec3f(m_VMin.x(), m_VMin.y(), m_VMin.z())
                + m_VoxelSize * vec3f((float)cellId.x(), (float)cellId.y(), (float)cellId.z()));
    }

    //! Returns the isovalue stored on the vertex with index.
    float isovalue(const uint& index)
    {
        return m_UsedVertices[index].isovalue();
    }

    //!Returns the corresponding vertex.
    NEMCGridVertex& vertex(const int index)
    {
        return m_UsedVertices[index];
    }

    //!Resizes the used vertex std::vector.
    void resizeUsedVertices(const uint size)
    {
        m_UsedVertices.clear();
        m_UsedVertices.resize(size);
    }

    //! Checks if the given point is inside the grid
    inline bool isInside(const vec3f& p)
    {
        return (p.x() <= m_VMax.x() && p.x() >= m_VMin.x())
               && (p.y() <= m_VMax.y() && p.y() >= m_VMin.y()) && (p.z() <= m_VMax.z() && p.z() >= m_VMin.z());
    }

    inline void insertPoint(const vec3f& p)
    {
        if(p.x() < m_VMin.x())
            m_VMin.setX(p.x());

        if(p.x() > m_VMax.x())
            m_VMax.setX(p.x());

        if(p.y() < m_VMin.y())
            m_VMin.setY(p.y());

        if(p.y() > m_VMax.y())
            m_VMax.setY(p.y());

        if(p.z() < m_VMin.z())
            m_VMin.setZ(p.z());

        if(p.z() > m_VMax.z())
            m_VMax.setZ(p.z());
    }

    inline void reset()
    {
        m_VMin.set(FLT_MAX, FLT_MAX, FLT_MAX);
        m_VMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        m_NrOfVertices.set(0, 0, 0);
    }

    void drawOutlines();
    void draw();

    float volume() const
    {
        float l1 = m_VMax.x() - m_VMin.x();
        float l2 = m_VMax.y() - m_VMin.y();
        float l3 = m_VMax.z() - m_VMin.z();

        float m_Volume = l1 * l2 * l3;

        return m_Volume;
    }
protected:
    //! Stores the marching cubes voxel grid using base mc vertex class. Can be smaller than the total nr of vertices.
    std::vector<NEMCGridVertex> m_UsedVertices;
    //! Stores indices to m_UsedVertices for all grid vertices. Has to have the size=totalNrOfVertices.
    std::vector<int> m_AllVertices;
    //! The number of vertices in x, y and z directions.
    vec3i m_NrOfVertices;
    //! The size of a voxel.
    double m_VoxelSize;
    //! Inverse size of a voxel.
    float m_InverseVoxelSize;
    //! Minimum and maximum vertex positions of the grid
    vec3f m_VMin, m_VMax;
};


/*!
  Voxel -> a grid cell of the marching cube
*/
class NECEXP_NEPOSTPROCESSOR  NEMCVoxel
{
public:

    NEMCVoxel()
    {
    };

    uint vertex(const int& index)
    {
        return m_Vertices[index];
    }

    void setVertex(const int& index, const uint& value)
    {
        m_Vertices[index] = value;
    }

    void setPosition(const vec3f& pos)
    {
        m_Position = pos;
    }

    const vec3f& position()
    {
        return m_Position;
    }
protected:
    //! Position of lower left vertex
    vec3f m_Position;
    //! Id of the eight corner vertices of this cube in the vertex std::vector
    uint m_Vertices[8];
};
