#pragma once

//! Representation of a spatial cell index, for 3D grids.
/*!
	At the moment this class
*/
struct CellIndex3D
{
private:
	//! cell indices in x,y,z dimension
	int m_x, m_y, m_z;

public:
	//! default constructor
	CellIndex3D(){}
	CellIndex3D(int x, int y, int z):m_x(x), m_y(y), m_z(z){}
	
	CellIndex3D(const vec3f& pos)
	{
        m_x = (int) (pos.x());
        m_y = (int) (pos.y());
        m_z = (int) (pos.z());
	}

    //member functions
	
	//operator overloading
    bool operator==(const CellIndex3D &cc)const{return (m_x == cc.x()) && (m_y == cc.y()) && (m_z == cc.z());}
    bool operator!=(const CellIndex3D &cc)const{return !(operator == (cc));}
	CellIndex3D operator+(const CellIndex3D &cc1) const {return CellIndex3D(m_x+cc1.x(), m_y+cc1.y(), m_z+cc1.z());}
	CellIndex3D operator-(const CellIndex3D &cc1) const {return CellIndex3D(m_x-cc1.x(), m_y-cc1.y(), m_z-cc1.z());}
	CellIndex3D operator-() const {return CellIndex3D(-m_x, -m_y, -m_z);}
	CellIndex3D operator*(uint a)const {return CellIndex3D(m_x*a,m_y*a,m_z*a);}
	inline int& operator [] (uint i) { return (&m_x)[i]; }
	inline int operator [] (uint i) const { return (&m_x)[i]; }
	
	//get functions
	int x() const {return m_x;}
	int y() const {return m_y;}
	int z() const {return m_z;}
};
