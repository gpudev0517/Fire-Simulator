#pragma once

//!  Axis aligned bounding box.
/*!
  This is used for fast collision detection.
*/
struct AABB
{
private:
	vec3f m_MinPos;
	vec3f m_MaxPos;
	//bool m_Empty;

public:
	AABB()
	{
		m_MinPos = vec3f(FLT_MAX,FLT_MAX,FLT_MAX);
		m_MaxPos = vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}

	AABB(const vec3f& min, const vec3f& max)
	{
		m_MinPos = min;
		m_MaxPos = max;
	}
	//we should be able to draw the bounding box
	//void draw() const;
	void clear();

	//! Tests if point p is inside the bounding box.
    inline bool isInside(const vec3f& p) const;
    inline bool isInside(const vec3f& p, double eps) const;

	//! Inserts point p into the bounding box.
	inline void insertPoint(const vec3f& p);

    //! Inserts the bounding box into the given bounding box
    inline void insert( const AABB& b );

	//inline void setMinR(double &acc, double const rhs){ if(acc > rhs) acc = rhs;};
	//inline void setMaxR(double &acc, double const rhs){ if(acc < rhs) acc = rhs;};	

	inline void setMin(const vec3f& min){ m_MinPos = min; }
	inline void setMax(const vec3f& max){ m_MaxPos = max; }
	
	const vec3f& minPos() const { return m_MinPos; }
	const vec3f& maxPos() const { return m_MaxPos; }

	//! For not computing something for empty AABBs. Where should we change this (?)
	inline bool valid() const 
	{ 
		return ( (m_MinPos.x() <= m_MaxPos.x()) &&
				 (m_MinPos.y() <= m_MaxPos.y()) &&
				 (m_MinPos.z() <= m_MaxPos.z())); 
	}

	//! Computes if aabb2 intersects with this aabb.
	inline bool intersects(const AABB& aabb2);

	//! Returns the overlap of this bounding boxes with the given AABB
	inline AABB overlapAABB(const AABB& aabb2) const;
	//inline void setEmpty(bool b) {m_Empty = b;}

    inline void translate(const vec3f& v) { m_MaxPos += v; m_MinPos += v; }
};


//bool AABBOverlapTest(const AABB& a, const AABB& b);
//bool AABBContainmentTest(const AABB& mother, const AABB& child);

struct VoxelGridExtent
{
	AABB aabb;
	uint xNumCells, yNumCells, zNumCells;

	void computeNrOfCells(double cellSize)
	{
		// If the bounding box is empty, then 
		if(aabb.valid())
		{
			const double offset = 0.5;		
			xNumCells = (uint)floor(((aabb.maxPos().x() - aabb.minPos().x()) / cellSize) + offset)+1;
			yNumCells = (uint)floor(((aabb.maxPos().y() - aabb.minPos().y()) / cellSize) + offset)+1;
			zNumCells = (uint)floor(((aabb.maxPos().z() - aabb.minPos().z()) / cellSize) + offset)+1;
		}
	}

	void clear()
	{
		aabb.clear();
		xNumCells = yNumCells = zNumCells = 0;
	}
};



class OBB
{
    public:
        OBB();
        OBB(const vec3f &position, const quatf &orientation, const vec3f &extents);
        virtual ~OBB();

        void setOrientationQuat( const quatf& orientation );
        void setCenter( const vec3f& position );
        void setExtents(const vec3f& extents);

        const quatf& getOrientationQuat() const;

        const vec3f& getCenter() const;
        const vec3f& getExtents() const;
        const vec3f& getAxis(unsigned int axis) const;  //0 = X, 1 = Y, 2 = Z

        void sync(const vec3f &position, const quatf &orientation);

        bool intersects(const OBB &obb2);

        bool isInside( vec3f& p );

    private:
        quatf m_orientationQuat;

        vec3f m_center;
        vec3f m_extents;
        vec3f m_axes[3];       //axes, left, up and forward, calculate from orientation

        void updateAxes(const quatf &orientation);
};

#include "Utility/NEAABB.inl"
