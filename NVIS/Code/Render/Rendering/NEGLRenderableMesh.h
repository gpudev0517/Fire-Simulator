#pragma once

class NEVertexBuffer;
class NEIndexBuffer;
class NEIndexedTriMesh;
class NEMaterial;


//! All classes that need gpu cached geometry rendering should inherit from this abstract base class and implement its methods.
/*!
	For an example, check NEIndexedTriMesh class for creation and NEMarchingCubes class for usage.
*/
class NECEXP_NERENDERING NEGLRenderableMesh
{
	friend class NEGLRenderer;

public:
	NEGLRenderableMesh();
	virtual ~NEGLRenderableMesh();

	//! Returns whether the object is buffered or not
	bool isBuffered() const;
	//! For different modes, see OpenGL reference pages
	uint drawingMode();

	void setMaterial(NEMaterial* material);
	NEMaterial* material() const;

    NEIndexBuffer* indexBuffer()
    {
        return m_pIBuffer;
    }
    NEIndexBuffer* indexBufferWithAdjacency()
    {
        return m_pIBufferWithAdjacency;
    }
    NEVertexBuffer* vertexBuffer()
    {
        return m_pVBuffer;
    }

	NEVertexBuffer* unIndexedVBuffer()
	{
		return m_pUnindexedVBuffer;
	}

    NEVertexBuffer* boundingBoxBuffer()
    {
        return m_pBoundingBoxBuffer;
    }

    NEVertexBuffer *unIndexedBoundingBoxBuffer()
    {
        return m_pUnindexedBoundingBoxBuffer;
    }

    virtual void createDrawingBuffers(NEIndexedTriMesh* mesh, uint mode);
    virtual void clearDrawingBuffers(NEIndexedTriMesh* mesh );
    virtual void rehintDrawingBuffers(NEIndexedTriMesh* mesh, uint mode);

    void calcNormalsMWA(NEIndexedTriMesh *mesh);
    void calcNormalsMWASEL(NEIndexedTriMesh *mesh);
    void calcNormalsMWE(NEIndexedTriMesh *mesh);

protected:
	uint m_DrawingMode;
	bool m_Buffered;

	// Pointers to the GPU buffers, allocated by calling createDrawingBuffers,
	// cleared by calling clearDrawingBuffers
	NEIndexBuffer* m_pIBuffer;
    NEIndexBuffer* m_pIBufferWithAdjacency; //for solid wireframe rendering.
	NEVertexBuffer* m_pVBuffer;
    NEVertexBuffer* m_pUnindexedVBuffer; // for quad wireframe rendering.
    NEVertexBuffer* m_pBoundingBoxBuffer; //This is buffer for bounding box shaded rendering.
    NEVertexBuffer* m_pUnindexedBoundingBoxBuffer; // this is unindexed buffer for bounding box quadwireframe rendering.

	// The material for the renderable primitive
	NEMaterial* m_pMaterial;
};
