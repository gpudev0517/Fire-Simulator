
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"


#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Rendering/Resource/NEIndexBuffer.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "NEGLRenderableMesh.h"


bool NEGLRenderableMesh::isBuffered() const
{
	return m_Buffered;
}

uint NEGLRenderableMesh::drawingMode()
{
	return m_DrawingMode;
}

void NEGLRenderableMesh::setMaterial(NEMaterial* material )
{
	m_pMaterial = material;
}

NEMaterial* NEGLRenderableMesh::material() const
{
	return m_pMaterial;
}


NEGLRenderableMesh::NEGLRenderableMesh()
{
	m_DrawingMode = GL_STATIC_DRAW;
	m_Buffered = false;
	m_pIBuffer = 0;
	m_pIBufferWithAdjacency = 0;
	m_pVBuffer = 0;
	m_pUnindexedVBuffer = 0;
    m_pBoundingBoxBuffer = 0;
    m_pUnindexedBoundingBoxBuffer = 0;
	m_pMaterial = 0;
}

void NEGLRenderableMesh::clearDrawingBuffers( NEIndexedTriMesh *mesh)
{
    if( !m_Buffered )
		return;

	NERENDERMAN.GLManager()->destroy( m_pVBuffer );
	NERENDERMAN.GLManager()->destroy( m_pIBuffer );
	NERENDERMAN.GLManager()->destroy( m_pIBufferWithAdjacency );
	m_pVBuffer = 0;
	m_pIBuffer = 0;
	m_pIBufferWithAdjacency = 0;
	m_Buffered = false;

    NERENDERMAN.GLManager()->destroy(m_pUnindexedVBuffer);
    m_pUnindexedVBuffer = 0;

    NERENDERMAN.GLManager()->destroy(m_pBoundingBoxBuffer);
    m_pBoundingBoxBuffer = 0;

    NERENDERMAN.GLManager()->destroy(m_pUnindexedBoundingBoxBuffer);
    m_pUnindexedBoundingBoxBuffer = 0;
}

void NEGLRenderableMesh::createDrawingBuffers(NEIndexedTriMesh *mesh, uint mode )
{
    m_DrawingMode = mode;
    Q_ASSERT_X( m_Buffered == false, "NEGLRenderable::createDrawingBuffers", "Buffers need to be cleared first" );
    m_pVBuffer = NERENDERMAN.GLManager()->createVertexBuffer( kCTNPFormat, (uint)mesh->vertices().size(), &mesh->vertices()[0], mode );
    m_pIBuffer = NERENDERMAN.GLManager()->createIndexBuffer( (uint) mesh->triangles().size() * 3,
                         &mesh->triangles()[0], mode );
	m_pIBufferWithAdjacency = NERENDERMAN.GLManager()->generateAdjacencyIndexBuffer((uint) mesh->triangles().size() * 3, &mesh->triangles()[0], mode );

    m_pUnindexedVBuffer = NERENDERMAN.GLManager()->generateUnindexedVBuffer(kPBFormat, (uint) mesh->triangles().size() / 2, &mesh->triangles()[0], &mesh->vertices()[0], mode);

    m_pBoundingBoxBuffer = NERENDERMAN.GLManager()->generateBoundingBoxBuffer(kCTNPFormat, kPBFormat, (uint)mesh->vertices().size(), &mesh->vertices()[0], mode, m_pUnindexedBoundingBoxBuffer);

	m_Buffered = true;
}

void NEGLRenderableMesh::rehintDrawingBuffers( NEIndexedTriMesh *mesh,  uint mode )
{
	m_DrawingMode = mode;
    Q_ASSERT_X( m_Buffered == true, "NEGLRenderable::rehintDrawingBuffers", "Buffers need to be created first" );
    m_pVBuffer->setData( mesh->vertices().size()*sizeof( NECTNPVertex ), &mesh->vertices()[0], mode );
    m_pIBuffer->setData( mesh->triangles().size()*sizeof( NETriangleIndexed ), &mesh->triangles()[0], mode );
    m_pIBufferWithAdjacency->setData( mesh->triangles().size()*sizeof( NETriangleIndexedAdjacency ), &mesh->triangles()[0], mode );

	//m_pUnindexedVBuffer->setData(mesh->unIndex)
}

NEGLRenderableMesh::~NEGLRenderableMesh()
{
}

// Calculate the normals for a given mesh using the
// "Mean Weighted by Angle" (MWA) algorithm as seen in the paper
// "A Comparison of Algorithms for Vertex Normal Computation"
void NEGLRenderableMesh::calcNormalsMWA(NEIndexedTriMesh *mesh)
{
    // keep track of which triangles a given vertex is part of
    std::vector<std::vector<size_t>> vtmap;
    vtmap.resize(mesh->vertices().size());

    // calculate triangle normals
    std::vector<glm::vec3> tN;
    for(size_t i = 0; i < mesh->triangles().size(); i++)
    {
        const NETriangleIndexed& t = mesh->triangles()[i];

        const NECTNPVertex& va = mesh->vertices()[t.p1];
        const NECTNPVertex& vb = mesh->vertices()[t.p2];
        const NECTNPVertex& vc = mesh->vertices()[t.p3];


        const glm::vec3 a = glm::vec3(va.position.x(), va.position.y(), va.position.z());
        const glm::vec3 b = glm::vec3(vb.position.x(), vb.position.y(), vb.position.z());
        const glm::vec3 c = glm::vec3(vc.position.x(), vc.position.y(), vc.position.z());
        tN.push_back(glm::cross(b - a, c - a));

        vtmap[t.p1].push_back(i);
        vtmap[t.p2].push_back(i);
        vtmap[t.p3].push_back(i);
    }

    // Now calculate the vertex normals
    for(size_t i = 0; i < mesh->vertices().size(); i++)
    {
        const std::vector<size_t>& tList = vtmap[i];

        glm::vec3 normal(0.0f, 0.0f, 0.0f);

        for(size_t k = 0; k < tList.size(); k++)
        {
            const NETriangleIndexed& t = mesh->triangles()[tList[k]];

            const NECTNPVertex& va = mesh->vertices()[t.p1];
            const NECTNPVertex& vb = mesh->vertices()[t.p2];
            const NECTNPVertex& vc = mesh->vertices()[t.p3];

            const glm::vec3 p1 = glm::vec3(va.position.x(), va.position.y(), va.position.z());
            const glm::vec3 p2 = glm::vec3(vb.position.x(), vb.position.y(), vb.position.z());
            const glm::vec3 p3 = glm::vec3(vc.position.x(), vc.position.y(), vc.position.z());


            glm::vec3 edge1 = p2 - p1;
            glm::vec3 edge2 = p3 - p1;
            glm::vec3 edgecross = glm::cross(edge2, edge1);
            float sinalpha = glm::length(edgecross) / (glm::length(edge1) * glm::length(edge2));
            sinalpha = MIN(1, MAX(-1, sinalpha));
            float alpha = asin(sinalpha);

            normal += alpha * tN[tList[k]];
        }

        glm::vec3 vn = glm::normalize(normal);
        NECTNPVertex& nv = mesh->vertices()[i];
        nv.normal = vec3f(vn.x, vn.y, vn.z);
    }
}

// Calculate the normals for a given mesh using the
// "Mean Weighted by Angle" (MWASEL) algorithm as seen in the paper
// "A Comparison of Algorithms for Vertex Normal Computation"
void NEGLRenderableMesh::calcNormalsMWASEL(NEIndexedTriMesh *mesh)
{
    // keep track of which triangles a given vertex is part of
    std::vector<std::vector<size_t>> vtmap;
    vtmap.resize(mesh->vertices().size());

    // calculate triangle normals
    std::vector<glm::vec3> tN;
    for(size_t i = 0; i < mesh->triangles().size(); i++)
    {
        const NETriangleIndexed& t = mesh->triangles()[i];

        const NECTNPVertex& va = mesh->vertices()[t.p1];
        const NECTNPVertex& vb = mesh->vertices()[t.p2];
        const NECTNPVertex& vc = mesh->vertices()[t.p3];

        const glm::vec3 a = glm::vec3(va.position.x(), va.position.y(), va.position.z());
        const glm::vec3 b = glm::vec3(vb.position.x(), vb.position.y(), vb.position.z());
        const glm::vec3 c = glm::vec3(vc.position.x(), vc.position.y(), vc.position.z());

        tN.push_back(glm::cross(b - a, c - a));

        vtmap[t.p1].push_back(i);
        vtmap[t.p2].push_back(i);
        vtmap[t.p3].push_back(i);
    }

    //
    // Now calculate the vertex normals
    //
    for(size_t i = 0; i < mesh->vertices().size(); i++)
    {
        const std::vector< size_t >& tList = vtmap[i];

        glm::vec3 normal(0.0f, 0.0f, 0.0f);

        for(size_t k = 0; k < tList.size(); k++)
        {
            const NETriangleIndexed& t = mesh->triangles()[tList[k]];

            const NECTNPVertex& va = mesh->vertices()[t.p1];
            const NECTNPVertex& vb = mesh->vertices()[t.p2];
            const NECTNPVertex& vc = mesh->vertices()[t.p3];

            const glm::vec3 p1 = glm::vec3(va.position.x(), va.position.y(), va.position.z());
            const glm::vec3 p2 = glm::vec3(vb.position.x(), vb.position.y(), vb.position.z());
            const glm::vec3 p3 = glm::vec3(vc.position.x(), vc.position.y(), vc.position.z());


            glm::vec3 edge1 = p2 - p1;
            glm::vec3 edge2 = p3 - p1;
            glm::vec3 edgecross = glm::cross(edge1, edge2);
            float edge1len = glm::length(edge1);
            float edge2len = glm::length(edge2);
            float sinalpha = glm::length(edgecross) / (edge1len * edge2len);

            normal += sinalpha * tN[tList[k]] / (edge1len * edge2len);
        }

        glm::vec3 vn = glm::normalize(normal);
        mesh->vertices()[i].normal = vec3f(vn.x, vn.y, vn.z);
    }
}


// Calculate the normals for a given mesh using the
// "Mean Weighted Equally" (MWE) algorithm (Naive approach)
void NEGLRenderableMesh::calcNormalsMWE(NEIndexedTriMesh *mesh)
{
    // keep track of which triangles a given vertex is part of
    std::vector<std::vector<size_t>> vtmap;
    vtmap.resize(mesh->vertices().size());

    // calculate triangle normals
    std::vector<glm::vec3> tN;
    for(size_t i = 0; i < mesh->triangles().size(); i++)
    {
        const NETriangleIndexed& t = mesh->triangles()[i];

        const NECTNPVertex& va = mesh->vertices()[t.p1];
        const NECTNPVertex& vb = mesh->vertices()[t.p2];
        const NECTNPVertex& vc = mesh->vertices()[t.p3];

        const glm::vec3 a = glm::vec3(va.position.x(), va.position.y(), va.position.z());
        const glm::vec3 b = glm::vec3(vb.position.x(), vb.position.y(), vb.position.z());
        const glm::vec3 c = glm::vec3(vc.position.x(), vc.position.y(), vc.position.z());


        tN.push_back(glm::cross(b - a, c - a));

        vtmap[t.p1].push_back(i);
        vtmap[t.p2].push_back(i);
        vtmap[t.p3].push_back(i);
    }

    std::vector< vec3f > normals;
    normals.clear();

    // Now calculate the vertex normals
    for(size_t i = 0; i < mesh->vertices().size(); i++)
    {
        const std::vector<size_t>& tList = vtmap[i];

        glm::vec3 normal(0.0f, 0.0f, 0.0f);

        for(size_t k = 0; k < tList.size(); k++)
            normal += tN[tList[k]];

        glm::vec3 vn = glm::normalize(normal);
        mesh->vertices()[i].normal = vec3f(vn.x, vn.y, vn.z);
    }
}
