#pragma once

#include "Resource/NEMesh.h"
#include "Utility/geometry.h"

//! A triangle mesh class that is optimized for speed.
/*!
	In this design, shared vertices are copied. Use NEIndexedTriMesh
	wherever possible.
*/
class NETriMesh : public NEMesh
{
	//Q_OBJECT

public:
	NETriMesh();
	~NETriMesh();

//public slots:

	inline uint numTriangles();
	inline const std::vector<NETriangle>& triangles() const;	
	inline std::vector<NETriangle>& triangles();

	uint loadFromFile(const QString& filename);

private:
	uint loadFromObj(const QString& filename);
	uint loadFromLwo(const QString& filename);

	std::vector<NETriangle> m_Triangles;
};

#include "Resource/Mesh/NETriMesh.inl"
