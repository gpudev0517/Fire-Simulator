inline uint NETriMesh::numTriangles()
{
	return (uint)m_Triangles.size();
}

inline const std::vector<NETriangle>& NETriMesh::triangles() const
{
	return m_Triangles;
}

inline std::vector<NETriangle>& NETriMesh::triangles()
{
	return m_Triangles;
}
