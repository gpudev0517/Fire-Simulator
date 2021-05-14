#include "NEParticleInputMCRenderer.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "PostProcessor/NEMarchingCubes.h"
#include "PostProcessor/NEParticleInputMC.h"

#include "Rendering/Resource/NEVertexBuffer.h"

//
// ParticleInputMC
//
void NEParticleInputMCRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleInputMC* particleInputMC = qobject_cast<NEParticleInputMC*>(spatial);

    NEMarchingCubesRenderer::render(spatial, surface);

    if(particleInputMC->DrawGrid())
        particleInputMC->grid().draw();

    if(particleInputMC->DrawGridOutline())
        particleInputMC->grid().drawOutlines();

    if(particleInputMC->DrawSurfaceNormals())
    {
        NERENDERMAN.GLManager()->beginUnLitDraw(surface);
        GL->glColor3f(1.0f, 0.0f, 1.0f);

        NERENDERMAN.GLManager()->updatePositionUniforms(surface);
        NEPVertex *q3 = new NEPVertex[particleInputMC->mesh().vertices().size() * 2];
        for(uint i = 0; i < particleInputMC->mesh().vertices().size(); i++)
        {
            NECTNPVertex v = particleInputMC->mesh().vertices()[i];
            q3[i*2 + 0].position = v.position;
            q3[i*2 + 1].position = v.position - 0.1f * v.normal;
        }
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
        GL->glDrawArrays(GL_LINES,0,particleInputMC->mesh().vertices().size() * 2);
        GL->glDisableClientState(GL_VERTEX_ARRAY);
        delete [] q3;

        NERENDERMAN.GLManager()->endUnLitDraw(surface);
    }

    if(particleInputMC->DrawVertices() == true)
    {
        GL->glPointSize(2.0);
        NERENDERMAN.GLManager()->bindCPShader(surface);
        NERENDERMAN.GLManager()->updateCPUniforms(surface);

        int nx = particleInputMC->grid().nrOfVertices().x();
        int ny = particleInputMC->grid().nrOfVertices().y();
        int nz = particleInputMC->grid().nrOfVertices().z();
        int vertexCount = nx * ny * nz;
        NEVertexBuffer *vertexBuffer = NERENDERMAN.GLManager()->createVertexBuffer(kCPFormat, vertexCount, 0, GL_STATIC_DRAW);
        NECPVertex* q3 = (NECPVertex*) vertexBuffer->beginLoadData();
        int gIndex = 0;
        for(int i = 0; i < particleInputMC->grid().nrOfVertices().x(); i++)
            for(int j = 0; j < particleInputMC->grid().nrOfVertices().y(); j++)
                for(int k = 0; k < particleInputMC->grid().nrOfVertices().z(); k++)
                {
                    vec3f vpos = particleInputMC->grid().vcorner() + particleInputMC->grid().voxelSize() * vec3f(float (i), float (j), float (k));
                    int id = particleInputMC->grid().vertexGridId(i, j, k);
                    NEMCGridVertex& v = particleInputMC->grid().vertex(id);

                    if(v.isovalue() < particleInputMC->IsoThreshold())
                        q3[gIndex].color = Color32(255,0,255,255);
                    else
                        q3[gIndex].color = Color32(255,255,0,255);

                    q3[gIndex].position = vpos;
                    gIndex++;
                }
        vertexBuffer->endLoadData();
        NERENDERMAN.GLManager()->draw(kPointListPrim, vertexBuffer);
        NERENDERMAN.GLManager()->destroy(vertexBuffer);

        NERENDERMAN.GLManager()->endUnLitDraw(surface);
        GL->glPointSize(1);
    }
}
