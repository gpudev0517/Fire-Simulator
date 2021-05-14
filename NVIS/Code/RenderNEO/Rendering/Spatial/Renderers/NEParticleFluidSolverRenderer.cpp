#include "NEParticleFluidSolverRenderer.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/NEGLRenderer.h"

#include "Utility/NETimer.h"
#include "Utility/geometry.h"
#include "Utility/Sorting/NESort.h"
#include "Utility/mathUtil.h"

#include "Base/NEBase.h"
#include "Physics/Spatial/Physical/NEParticle.h"
#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"

#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/NEOSDParticleFluid.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Rendering/Manager/NERenderManager.h"

#include "Rendering/Resource/Material/NEParticleFluidMaterial.h"

// Fluid
NEFluidSolverRenderer::NEFluidSolverRenderer()
{
    m_ParticleVerts = NERENDERMAN.GLManager()->createVertexBuffer(kCPFormat);
    //m_ParticleVerts = NERENDERMAN.GLManager()->createVertexBuffer(kCPAFormat);
}

NEFluidSolverRenderer::~NEFluidSolverRenderer()
{
    if (m_ParticleVerts != 0)
        NERENDERMAN.GLManager()->destroy(m_ParticleVerts);
}

void NEFluidSolverRenderer::connectToSpatial()
{
    NEFluidSolver* fluidSolver = qobject_cast<NEFluidSolver*>(m_spatial);
    if (fluidSolver == NULL) return;
    //QObject::connect( fluidSolver, SIGNAL(RenderModeChanged()), this, SLOT(updateDrawingBuffers()), Qt::ConnectionType::DirectConnection );
    QObject::connect( fluidSolver, SIGNAL(updateDrawingBuffers()), this, SLOT(updateDrawingBuffers()), Qt::ConnectionType::DirectConnection );
    QObject::connect( fluidSolver, SIGNAL(initObject()), this, SLOT(initObject()), Qt::ConnectionType::DirectConnection );
}

NEVertexBuffer* NEFluidSolverRenderer::particleVerts()
{
    return m_ParticleVerts;
}


static inline float getanisoweight( const vec3f& p0, const vec3f& p1, float radiusSqr, float radiusInv, float& weightAccu )
{
    //
    float rSqr = (p0 - p1).lengthSquared();
    if(rSqr > radiusSqr)
        return 0.f;
    float r = sqrt(rSqr) * radiusInv;

    // now compute weight
    float  wij  = 1.f;
    wij = 1.f - (r*r*r);

//    std::cout << "  weight: " << wij << std::endl;
    // cut off small weights, and set negative ones to zero
    if(wij < 0.00001)
        wij = 0.f;

    weightAccu += wij;
    return wij;
}

static inline float getweight( const vec3f& p0, const vec3f& p1, float radiusSqr, float radiusInv, float& weightAccu )
{
    // now compute weight
    float  wij  = 1.f;

    float rSqr = (p0 - p1).lengthSquared();
    if(rSqr > radiusSqr)
        return 0.f;

    float r = sqrt(rSqr) * radiusInv;
    wij = 1.f - r*r;

    weightAccu += wij;
    return wij;
}

#include "svd3.h"
extern "C"
{
void computesvdnumrec(double **a, int m, int n, double w[], double **v);
}
//
// Numerical Recipes SVD function
//
void computesvd( matrix33f& G, matrix33f& R, vec3f& D )
{
    double w[4];
    double aVals[16];
    double vVals[16];
    double *a[4] = { &aVals[0],  &aVals[4],  &aVals[8],  &aVals[12] };
    double *v[4] = { &vVals[0],  &vVals[4],  &vVals[8],  &vVals[12] };

    a[0][0] = 1; a[1][0] = 1; a[2][0] = 1; a[3][0] = 1;
    a[1][1] = G(0, 0); a[1][2] = G( 0, 1 ); a[ 1 ][ 3 ] = G( 0, 2 );
    a[2][1] = G(1, 0); a[2][2] = G( 1, 1 ); a[ 2 ][ 3 ] = G( 1, 2 );
    a[3][1] = G(2, 0); a[3][2] = G( 2, 1 ); a[ 3 ][ 3 ] = G( 2, 2 );


//    double u[3][3], s[3][3], v[3][3];
//    svd( a[1][1], a[1][2], a[1][3], a[2][1], a[2][2], a[2][3], a[3][1], a[3][2], a[3][3],
//            a[1][1], a[1][2], a[1][3], a[2][1], a[2][2], a[2][3], a[3][1], a[3][2], a[3][3],
//            s[1][1], a[1][2], a[1][3], a[2][1], a[2][2], a[2][3], a[3][1], a[3][2], a[3][3],
//            a[1][1], a[1][2], a[1][3], a[2][1], a[2][2], a[2][3], a[3][1], a[3][2], a[3][3] );


    computesvdnumrec( a, 3, 3, w, v );
    //4x4 , check? -> do 3x3 ?

    D[0] = (float)w[1];
    D[1] = (float)w[2];
    D[2] = (float)w[3];

    matrix33f Ru;
    Ru( 0, 0 ) = (float)a[1][1]; Ru( 0, 1 ) = (float)a[1][2]; Ru( 0, 2 ) = (float)a[1][3];
    Ru( 1, 0 ) = (float)a[2][1]; Ru( 1, 1 ) = (float)a[2][2]; Ru( 1, 2 ) = (float)a[2][3];
    Ru( 2, 0 ) = (float)a[3][1]; Ru( 2, 1 ) = (float)a[3][2]; Ru( 2, 2 ) = (float)a[3][3];

    //std::cout << "RU " << Ru.toUtf8().toStdString() << std::endl;
    R = Ru;
    return;
}

#if 1
#include "Utility/Math/vec234f.h"
#include "Utility/Math/matrix33f.h"

void computeAnisotropicKernel( NEParticle& p, float interRadius, matrix33f& matfinal, matrix33f& matrot, vec3f& matscale )
{
    //std::cout << "Particle " << p.id() << " at position " << p.position().x() << ", " << p.position().y() << ", " << p.position().z() << std::endl;

    const float kernelWidth = interRadius;
    const float weightradsqr = kernelWidth * kernelWidth;
    const float weightradinv = 1.f / kernelWidth;

    //const int MIN_NEIGHBORS = 12;

    const vec3f srcPosition = p.position();
    vec3f meanPosition = srcPosition;

    // compute averaged center (mean)
    int numNeighbors  = 0;
    meanPosition      = vec3f( 0, 0, 0 );
    float totalweight = 0.f;

    std::vector<Neighbor>* neighbors = p.neighborVector(p.index());
    int nEnd = (int) neighbors->size() ;

    for( int i = 0; i < nEnd; i++ )
    {

        vec3f pPos = (*neighbors)[i].neighbor()->position();
        float wij  = getanisoweight( srcPosition, pPos, weightradsqr, weightradinv, totalweight );


        if( wij <= 0.f )
            continue;

        numNeighbors++;
        meanPosition += pPos * wij;
    }

//    std::cout << "  Mean position: " << meanPosition.x() << ", " << meanPosition.y() << ", " << meanPosition.z();

    //
    // Normalize
    //
    if( totalweight > 0.f )
        meanPosition *= 1.f/totalweight;
    else
        meanPosition  = srcPosition;

//    std::cout << " -> " << meanPosition.x() << ", " << meanPosition.y() << ", " << meanPosition.z() << std::endl;

//    std::cout << "Total W "
    //
    // If less than N neighbors, default to identity scaling...
    //


    totalweight = 0.f;
    matrix33f C;
    C.setToZero();


    nEnd = (int) neighbors->size() - 1 ;
    for( int i = 0; i <= nEnd; i++ )
    {

        vec3f srcPosition = p.position();
        vec3f pPos        = (*neighbors)[i].neighbor()->position();

        //std::cout << "PPos " << srcPosition.toUtf8().toStdString() << " " << pPos.toUtf8().toStdString() << std::endl;

        float wij = getanisoweight( srcPosition, pPos, weightradsqr, weightradinv, totalweight );

        if( wij <= 0.f )
            continue;

        // setup covariance matrix
        vec3f nb = (pPos - meanPosition);

        //nb *= 1.f/kernelWidth;

        C(0,0) += nb.x() * nb.x() * wij;
        C(0,1) += nb.y() * nb.x() * wij;
        C(0,2) += nb.z() * nb.x() * wij;

        C(1,0) += nb.x() * nb.y() * wij;
        C(1,1) += nb.y() * nb.y() * wij;
        C(1,2) += nb.z() * nb.y() * wij;

        C(2,0) += nb.x() * nb.z() * wij;
        C(2,1) += nb.y() * nb.z() * wij;
        C(2,2) += nb.z() * nb.z() * wij;

        //C(3,3) = 1.f;
    }

    if(totalweight > 0.f)
        C *= 1.f/totalweight;

    float  volumescaling = 1.f;

    //float fnorm = C.norm();

    float fnorm = sqrtf( // frobenius norm
                         (C(0,0)*C(0,0)) +  (C(0,1)*C(0,1)) +  (C(0,2)*C(0,2)) +
                         (C(1,0)*C(1,0)) +  (C(1,1)*C(1,1)) +  (C(1,2)*C(1,2)) +
                         (C(2,0)*C(2,0)) +  (C(2,1)*C(2,1)) +  (C(2,2)*C(2,2)) );

//    std::cout << "Fnorm " << fnorm << std::endl;

    if( nEnd > 0 && fnorm > 0.0001 )
    {
//        std::cout << "Cov " << C.toUtf8().toStdString() << std::endl;

        computesvd( C, matrot, matscale );

//        std::cout << "Cov returned " << matrot.toUtf8().toStdString() << std::endl;
        // restrict stretching

        float maxeigval = matscale[0];
        maxeigval = std::max( (float)matscale[1] , maxeigval );
        maxeigval = std::max( (float)matscale[2] , maxeigval );

        float thresh = 0.25f;

        // Clamp ranges
        float volumescaling = 1.0f;

        for( int i = 0; i < 3; ++i )
        {
            if(matscale[i] / maxeigval < thresh)
            {
                matscale[i] = thresh * maxeigval;
            }
        }

        for( int i = 0; i < 3; ++i )
            volumescaling += matscale[ i ];
        volumescaling /= 1.2f;
        //matscale[0] = 2000000;

        matrix33f St;
        St.setToIdentity ();
        St(0,0) = matscale.x()/volumescaling;
        St(1,1) = matscale.y()/volumescaling;
        St(2,2) = matscale.z()/volumescaling;
        matrix33f S = St.inverse();

        //std::cout << "Rot " << matrot.toUtf8().toStdString() << std::endl;
        //std::cout << "Scale " << matscale.toUtf8().toStdString() << std::endl;

        matfinal =  matrot * S * matrot.transposed() ;
        matfinal /= interRadius;

    }
    else {
        // no variance, identity
        volumescaling = 1.f;
        matfinal.setToIdentity();
    }

}
#endif


void NEFluidSolverRenderer::updateDrawingBuffers()
{
    NEFluidSolver* fluidSolver = qobject_cast<NEFluidSolver*>(m_spatial);
    if (fluidSolver == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if(fluidSolver->RenderMode() != NESpatial::Invisible)
    {
        NETimer::start("GPUBufferUpdate");
        const NEVectorMetaData& particles = *(fluidSolver->particlesVector());

        NEParticleFluidMaterial* waterMaterial = qobject_cast<NEParticleFluidMaterial*> (FluidMaterial() ? FluidMaterial() :
                                                                                                           NERENDERMAN.GLManager()->getDefaultFluidMaterial());
        double reduction = 1.0 / (waterMaterial->RenderOptLevel() + 1.0);
        // Get the number of particles
        uint nParticles = particles.objectCount * reduction;
        // Allocate space for buffer particles
        NEVisParticle* visParticles = new NEVisParticle[nParticles * sizeof(NEVisParticle)];

        matrix33f matrot, matfinal;
        vec3f matscale;
        float interactionRadius = fluidSolver->particleRadius() * 2;
        matfinal.setToIdentity();

        // Copy the particles
        if (!waterMaterial->WaterRendering())
        {
            #pragma omp parallel
            {
                omp_for_all(NEParticle, p, particles,)
                {
                    omp_next_item(NEParticle, p, particles)

                    visParticles[__i__].col = p->color();
                    visParticles[__i__].pos = p->position();
                }
            }
        }
        else
        {
            int j = 0;
            for_all(NEParticle, p, particles)
            {
                visParticles[j].col = p->color();
                visParticles[j].pos = p->position();

                // Compute the anisotropic matrix here...and copy it to the Vis Particle
                if( waterMaterial->WaterRendering() )
                    computeAnisotropicKernel( *p, interactionRadius*2, matfinal, matrot, matscale );
                //visParticles[j].aniso = matfinal;

                if(__i__ % (waterMaterial->RenderOptLevel()+ 1) == 0)
                    ++j;
            }
        }

        particleVerts()->setData(nParticles * sizeof(NEVisParticle), visParticles, GL_DYNAMIC_DRAW);

        delete [] visParticles;

        double GPUBufferUpdateTime = NETimer::stop("GPUBufferUpdate");
    }
}


// ParticleFluid

void NEParticleFluidSolverRenderer::zOrderParticles(NEParticleFluidSolver* fluidSolver)
{
    NECamera* cam = NERENDERMAN.defaultCamera();
    vec3f camPos  = cam->Position();
    vec3f camDir  = camPos - cam->LookAt();

    camDir.normalize();
    float spacing = 1.f / (2 * fluidSolver->particleRadius());

    const NEVectorMetaData& fParticles = fluidSolver->particlesMetaVector();
    const int pEnd = fParticles.objectCount;

    if( pEnd != (int)m_IdPairs.size() )
        m_IdPairs.resize(pEnd);

    //Compute the cellId/particle pairs for grid
    omp_parallel_for_all( NEParticle, particle, fParticles, schedule(guided) )
    {
        omp_next_item( NEParticle, particle, fParticles )

                float dist = ( particle->position() - camPos ) * camDir;
        int cId = dist * spacing;

        m_IdPairs[__i__].CellID = cId;
        m_IdPairs[__i__].pointer  = particle;
    }

    uint nrT = nesort::secondPower( NEBASE.Threads() );
    nesort::sort( m_IdPairs.begin(), m_IdPairs.end(), nrT );
}

void NEParticleFluidSolverRenderer::initObject()
{
    NEParticleFluidSolver* fluidSolver = qobject_cast<NEParticleFluidSolver*>(m_spatial);
    if (fluidSolver == NULL) return;

    if( FluidMaterial() == NULL)
    {
        NEMaterial* defaultFluidMaterial = NERENDERMAN.materials()["DefaultFluidMaterial"];
        fluidSolver->connectTo(reinterpret_cast<NENode*>(defaultFluidMaterial), NE::kIOParticles, 0, 0);
    }
}

//
// Particle Fluid
//
void NEParticleFluidSolverRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleFluidSolver* fluidSolver = qobject_cast<NEParticleFluidSolver*>(spatial);
    if (fluidSolver == NULL) return;

    if( fluidSolver->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    const NEVectorMetaData& particles = *(fluidSolver->particlesVector());


    if(fluidSolver->DrawVelocity())
    {
        NERENDERMAN.GLManager()->beginUnLitDraw(surface);
        GL->glColor3f( 1.0f, 0.0f, 0.0f );

        NERENDERMAN.GLManager()->updatePositionUniforms(surface);
        NEPVertex *q3 = new NEPVertex[particles.objectCount * 2];
        int i = 0;
        for_all( NEFluidParticle, particle, particles )
        {
            vec3f pos1 = particle->position();
            vec3f pos2 = pos1 + (float)fluidSolver->Timestep() * particle->velocity();
            q3[i*2 + 0].position = pos1;
            q3[i*2 + 1].position = pos2;
            i++;
        }
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
        GL->glDrawArrays(GL_LINES,0,particles.objectCount * 2);
        GL->glDisableClientState(GL_VERTEX_ARRAY);
        delete [] q3;

        NERENDERMAN.GLManager()->endUnLitDraw(surface);
        return;
    }

    //
    // Draw regular fluid only if drawVelocity is false
    //
    {
        if( NEBASE.guiMode() == false )
            return;

        NEParticleFluidMaterial* waterMaterial = qobject_cast<NEParticleFluidMaterial*> (FluidMaterial() ? FluidMaterial() :
                                                                                                           NERENDERMAN.GLManager()->getDefaultFluidMaterial());

        if(waterMaterial->WaterRendering())
            return;

        bool shadeFlat = (fluidSolver->RenderMode() == NESpatial::NERenderMode::FlatShaded);

        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

        NERENDERMAN.GLManager()->beginParticleDraw(surface, fluidSolver->particleRadius(), !shadeFlat, false, false);
        surface->shaderProgram()->setUniformValue(QString("meshColor").toUtf8().constData(), spatial->Color());
        NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), particles.objectCount * (1.0 / (waterMaterial->RenderOptLevel() + 1)));
        NERENDERMAN.GLManager()->endParticleDraw(surface);

        GL->glPopAttrib();

    }


    if(fluidSolver->DrawParticleNormals())
    {
        NERENDERMAN.GLManager()->beginUnLitDraw(surface);
        GL->glColor3f(1.0f,0.0f,0.0f);

        NERENDERMAN.GLManager()->updatePositionUniforms(surface);
        NEPVertex *q3 = new NEPVertex[particles.objectCount * 2];
        int i = 0;
        for_all( NEFluidParticle, particle, particles )
        {
            particle->normal().normalize();
            particle->setNormal(particle->normal() * -1);

            vec3f pos1 = particle->position();
            vec3f pos2 = pos1 + 0.02f*particle->normal();

            q3[i*2 + 0].position = pos1;
            q3[i*2 + 1].position = pos2;
            i++;
        }
        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
        GL->glDrawArrays(GL_LINES,0,particles.objectCount * 2);
        GL->glDisableClientState(GL_VERTEX_ARRAY);
        delete [] q3;

        NERENDERMAN.GLManager()->endUnLitDraw(surface);
    }

    if(fluidSolver->DrawFluidNeighbors())
    {
        NERENDERMAN.GLManager()->beginUnLitDraw(surface);
        GL->glColor3f(0.0f,1.0f,1.0f);

        NERENDERMAN.GLManager()->updatePositionUniforms(surface);
        for_all(NEFluidParticle, particle, particles)
        {
            std::vector<Neighbor>& neighbors = fluidSolver->liquidParticleNeighbors()[__i__];
            NEPVertex *q3 = new NEPVertex[neighbors.size() * 2];
            for(uint i=0; i<neighbors.size(); i++)
            {
                const NEParticle& particle2 = static_cast<const NEParticle&> (*neighbors[i].neighbor());
                vec3f pos1 = particle->position();
                vec3f pos2 = particle2.position();

                q3[i*2 + 0].position = pos1;
                q3[i*2 + 1].position = pos2;
            }
            GL->glEnableClientState(GL_VERTEX_ARRAY);
            GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
            GL->glDrawArrays(GL_LINES,0,neighbors.size() * 2);
            GL->glDisableClientState(GL_VERTEX_ARRAY);
            delete [] q3;
        }

        NERENDERMAN.GLManager()->endUnLitDraw(surface);
    }

    if(fluidSolver->DrawRigidNeighbors())
    {
        NERENDERMAN.GLManager()->beginUnLitDraw(surface);
        GL->glColor3f(1.0f,1.0f,0.0f);
        GL->glBegin(GL_LINES);
        for_all(NEFluidParticle, particle, particles)
        {
            std::vector<Neighbor>& rigidNeighbor = fluidSolver->rigidParticleNeighbors()[__i__];
            vec3f *q3 = new vec3f[rigidNeighbor.size() * 2];
            for(uint i=0; i<rigidNeighbor.size(); i++)
            {
                const NEParticle& particle2 = static_cast<const NEParticle&> (*rigidNeighbor[i].neighbor());
                vec3f pos1 = particle->position();
                vec3f pos2 = particle2.position();

                q3[i*2 + 0] = pos1;
                q3[i*2 + 1] = pos2;
            }
            GL->glEnableClientState(GL_VERTEX_ARRAY);
            GL->glVertexPointer(2, GL_FLOAT, sizeof(vec3f) * rigidNeighbor.size() * 2, (float*)&q3);
            GL->glDrawArrays(GL_LINES,0,rigidNeighbor.size() * 2);
            GL->glDisableClientState(GL_VERTEX_ARRAY);
            delete [] q3;
        }
        GL->glEnd();
        NERENDERMAN.GLManager()->endUnLitDraw(surface);
    }
}

void NEParticleFluidSolverRenderer::renderPickable(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleFluidSolver* fluidSolver = qobject_cast<NEParticleFluidSolver*>(spatial);
    if (fluidSolver == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if( fluidSolver->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    NEParticleFluidMaterial* waterMaterial = qobject_cast<NEParticleFluidMaterial*> (FluidMaterial() ? FluidMaterial() :
                                                                                                       NERENDERMAN.GLManager()->getDefaultFluidMaterial());

    NERENDERMAN.GLManager()->bindParticleShader(surface);
    float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    fluidSolver->getColorID4fv(color);
    GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), 1, color );

    const NEVectorMetaData& particles = *(fluidSolver->particlesVector());


    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    NERENDERMAN.GLManager()->beginParticleDraw(surface, fluidSolver->particleRadius(), false, false, false, true);
    NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), particles.objectCount * (1.0 / (waterMaterial->RenderOptLevel() + 1)));
    NERENDERMAN.GLManager()->endParticleDraw(surface);

    GL->glPopAttrib();
}

void NEParticleFluidSolverRenderer::renderPicked(NESpatial* spatial, IGLSurface* surface)
{
    NEParticleFluidSolver* fluidSolver = qobject_cast<NEParticleFluidSolver*>(spatial);
    if (fluidSolver == NULL) return;

    if( NEBASE.guiMode() == false )
        return;

    if( fluidSolver->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    NEParticleFluidMaterial* waterMaterial = qobject_cast<NEParticleFluidMaterial*> (FluidMaterial() ? FluidMaterial() :
                                                                                                       NERENDERMAN.GLManager()->getDefaultFluidMaterial());

    bool shadeFlat = (fluidSolver->RenderMode() == NESpatial::NERenderMode::FlatShaded);

    const NEVectorMetaData& particles = *(fluidSolver->particlesVector());

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    NERENDERMAN.GLManager()->beginParticleDraw(surface, fluidSolver->particleRadius(), !shadeFlat, true, 0);
    NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), particles.objectCount * (1.0 / (waterMaterial->RenderOptLevel() + 1)));
    NERENDERMAN.GLManager()->endParticleDraw(surface);

    GL->glPopAttrib();
}

void NEParticleFluidSolverRenderer::renderWater(NESpatial *spatial, IGLSurface* surface)
{
    NEParticleFluidSolver* fluidSolver = qobject_cast<NEParticleFluidSolver*>(spatial);
    if (fluidSolver == NULL) return;

    if( NEBASE.guiMode() == false )
        return;


    if( fluidSolver->RenderMode() == NESpatial::NERenderMode::Invisible )
        return;

    // render fluid surface
    NEParticleFluidMaterial* waterMaterial = qobject_cast<NEParticleFluidMaterial*> (FluidMaterial() ? FluidMaterial() :
                                                                                                       NERENDERMAN.GLManager()->getDefaultFluidMaterial());

    if(!waterMaterial->WaterRendering())
        return;

    const NEVectorMetaData& particles = *(fluidSolver->particlesVector());

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    float radius = fluidSolver->particleRadius();


    if( waterMaterial )
        NERENDERMAN.GLManager()->beginParticleDraw_Thickness(surface, radius, waterMaterial->ThicknessParticleScale() );
    NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), particles.objectCount * (1.0 / (waterMaterial->RenderOptLevel() + 1)));
    NERENDERMAN.GLManager()->endParticleDraw(surface);
    if (NERENDERMAN.GLManager()->OpenGLVersion() >= 410) {
        if( waterMaterial )
            NERENDERMAN.GLManager()->beginParticleDraw_Depth(surface, waterMaterial->DepthParticleScale() );
        NERENDERMAN.GLManager()->drawParticles(surface, particleVerts(), particles.objectCount * (1.0 / (waterMaterial->RenderOptLevel() + 1)));
        NERENDERMAN.GLManager()->endParticleDraw(surface);
    }
    
    if( waterMaterial)
        NERENDERMAN.GLManager()->drawParticleBlurDepth(surface, radius, waterMaterial->SmoothingRadius(), waterMaterial->MaxRadialSamples(),
                                                   waterMaterial->DepthEdgeFalloff(), waterMaterial->DepthBlurFalloff() );
    NERENDERMAN.GLManager()->drawParticleDisplay(surface, FluidMaterial(), spatial->Color().alpha()/255.0f);

    GL->glPopAttrib();
}

NEParticleFluidSolverRendererFactory::NEParticleFluidSolverRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEParticleFluidSolverRendererFactory::nodeName() { return "IISPH Solver Renderer"; }
NEObject *NEParticleFluidSolverRendererFactory::createInstance()
{
    return new NEParticleFluidSolverRenderer();
}

#define USE_TICK_LARGE_MIDDLE_SHORT 1


//
// OSDParticleFluidRenderer
//
void NEOSDParticleFluidRenderer::render(NESpatial* spatial, IGLSurface* surface)
{
    NEOSDParticleFluid* osdParticleFluid = qobject_cast<NEOSDParticleFluid*>(spatial);
    if (osdParticleFluid == NULL) return;
    const NEParticleFluidSolver* fluid = osdParticleFluid->particleFluidSolver();
    if (!fluid) return;
    if(osdParticleFluid->RenderMode() == NESpatial::NERenderMode::Invisible)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadIdentity();

    float minVal = fluid->MinColoringRange();
    float maxVal = fluid->MaxColoringRange();

    if (fluid->ColoringOptions() == NEParticleFluidSolver::Fixed)
    {
        QColor minColor = fluid->MinRangeColor();
        QColor maxColor = fluid->MaxRangeColor();
        bool interpolateMode = fluid->InterpolateHues();
        vec3f minColorv = vec3f(minColor.red() / 255.0, minColor.green() / 255.0, minColor.blue() / 255.0);
        vec3f maxColorv = vec3f(maxColor.red() / 255.0, maxColor.green() / 255.0, maxColor.blue() / 255.0);

        // Render Color Bar
        surface->setShaderProgram(osdParticleFluid->getColorBarProgram());
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), interpolateMode );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), 2 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[0]").toUtf8() ), 1, &minColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[0]").toUtf8() ), 0.0 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[1]").toUtf8() ), 1, &maxColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[1]").toUtf8() ), 1.0 );
    }
    else if (fluid->ColoringOptions() == NEParticleFluidSolver::Ramp)
    {
        QVector<QPair<qreal, QColor>> colorList = fluid->getColorList();
        int nSize = std::min(colorList.size(), MAX_COLORRAMP_SIZE);

        // Render Color Bar
        surface->setShaderProgram(osdParticleFluid->getColorBarProgram());
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), 0 );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), nSize );
        for(int i = 0; i < nSize; i++)
        {
            float pos = colorList.at(i).first;
            QColor color = colorList.at(i).second;
            vec3f colorv = vec3f(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
            GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[" + QString::number(i) + "]").toUtf8() ), 1, &colorv[0] );
            GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[" + QString::number(i) + "]").toUtf8() ), pos );
        }
    }

    int width = surface->getWidth();
    int height = surface->getHeight();

    int xRes = width * osdParticleFluid->Positionx();
    int yRes = height * osdParticleFluid->Positiony();
    int wRes = OSD_PARTICLE_COLORBAR_WIDTH * osdParticleFluid->Scalex();      //!< Width of color bar
    int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * osdParticleFluid->Scaley();     //!< Height of color bar
    int hTitle = osdParticleFluid->fontHeight() * 2;
    int wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdParticleFluid->Scalex();   //!< Width of margin between color bar and measure
    unsigned int widthU = 0;   //!< Width of measurement text
    NEOSDParticleFluid::NEOSDOrientation enumOrientation = osdParticleFluid->OSDOrientation();
    if (enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Top ||
        enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Bottom)
    {
        if (osdParticleFluid->FontSize() == 20)
            osdParticleFluid->SetFontSize(12);
    } else
    {
        if (osdParticleFluid->FontSize() == 12)
            osdParticleFluid->SetFontSize(20);
    }
    vec2f OsdSize = osdParticleFluid->osdSize();
    if (enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Top ||
        enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Bottom)
    {
        wRes = OsdSize.x() - (int)(osdParticleFluid->fontWidth());
        hRes = OSD_PARTICLE_COLORBAR_WIDTH * osdParticleFluid->Scaley();
        wMargin = OSD_PARTICLE_MEASURE_MARGIN * osdParticleFluid->Scaley();
    }

    int xPos = 0, yPos = 0;
    vec3f colorBarRect[4];
    vec3f colorBarRect_2D[4];
    vec2f widgetSize = vec2f(width, height);
    vec3f offsetOfOrientation;
    switch(enumOrientation)
    {
    case NEOSDParticleFluid::NEOSDOrientation::Top:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes - hTitle, 0);
        colorBarRect[1] = vec3f(xRes, yRes - hTitle - hRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes - hTitle - hRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes - hTitle, 3);
        break;
    case NEOSDParticleFluid::NEOSDOrientation::Bottom:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes + hRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes, 2);
        colorBarRect[3] = vec3f(xRes + wRes, yRes + hRes, 3);
        break;
    case NEOSDParticleFluid::NEOSDOrientation::Right:
        offsetOfOrientation = vec3f(OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes - wRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes - wRes, yRes + hRes, 3);
        break;
    case NEOSDParticleFluid::NEOSDOrientation::Left:
    default:
        offsetOfOrientation = vec3f(-OsdSize.x()/2, -OsdSize.y()/2, 0);
        colorBarRect[0] = vec3f(xRes, yRes, 0);
        colorBarRect[1] = vec3f(xRes + wRes, yRes, 1);
        colorBarRect[2] = vec3f(xRes + wRes, yRes + hRes, 2);
        colorBarRect[3] = vec3f(xRes, yRes + hRes, 3);
        break;
    }
    for (int nIndex = 0; nIndex < 4; nIndex++)
    {
        colorBarRect[nIndex] = (colorBarRect[nIndex] + offsetOfOrientation); // widgetSize;
        colorBarRect_2D[nIndex] = vec3f(colorBarRect[nIndex].x(), colorBarRect[nIndex].y(), 0);
        colorBarRect[nIndex].setX(colorBarRect[nIndex].x() / widgetSize.x());
        colorBarRect[nIndex].setY(colorBarRect[nIndex].y() / widgetSize.y());
        //std::cout << "NEOSDParticleFluidRenderer::render colorBarRect x=" << colorBarRect[nIndex].x() << ", y=" << colorBarRect[nIndex].y() << ", z=" << colorBarRect[nIndex].z() << ", index=" << nIndex << std::endl;
    }

    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect);
    GL->glDrawArrays(GL_TRIANGLE_FAN,0,4);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

    // Render black colorbar border
    NERENDERMAN.GLManager()->bindPositionShader(surface);
    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glColor4f(0,0,0,1);
    NERENDERMAN.GLManager()->updatePositionUniforms(surface);
    GL->glLineWidth(1.5f);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&colorBarRect_2D);
    GL->glDrawArrays(GL_LINE_LOOP,0,4);
    GL->glLineWidth(1.0f);
    GL->glDisableClientState(GL_VERTEX_ARRAY);

#if USE_TICK_LARGE_MIDDLE_SHORT
    // Render Ruler
    int rulerLeft   = colorBarRect_2D[0].x(); // point of index 0 is bottomleft(or topleft)
    int rulerTop    = colorBarRect_2D[0].y();
    int rulerRight  = colorBarRect_2D[1].x(); // point of index 1 is bottomright(or bottomleft)
    int rulerbottom = colorBarRect_2D[1].y();
    int tickWidthL  = 5; // wRes / 10 + 1;
    int tickWidthM  = 3; // wRes / 20 + 1;
    int tickWidthS  = 3; // wRes / 25 + 1;
    int tickHeightL = 0;
    int tickHeightM = 0;
    int tickHeightS = 0;
    if (enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Top ||
        enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Bottom)
    {
        tickWidthL = 0;
        tickWidthM = 0;
        tickWidthS = 0;
        tickHeightL = 5; // hRes / 10 + 1;
        tickHeightM = 3; // hRes / 20 + 1;
        tickHeightS = 3; // hRes / 25 + 1;
    }
//    glLineWidth(2.0f);
    vec3f q3_1[2], q3_2[2];
    float offsetX, offsetY;

    GL->glEnableClientState(GL_VERTEX_ARRAY);

    for( int i = 0; i <= 10 * osdParticleFluid->TickCount(); i++)
    {
        float s = i / 10.0 / osdParticleFluid->TickCount();
        if (enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Top ||
            enumOrientation == NEOSDParticleFluid::NEOSDOrientation::Bottom)
        {
            offsetX = s * wRes; offsetY = 0;
        } else
        {
            offsetX = 0; offsetY = s * hRes;
        }

        q3_1[0] = vec2f(rulerLeft + offsetX, rulerTop + offsetY);
        q3_2[0] = vec2f(rulerRight + offsetX, rulerbottom + offsetY);
        if (i % 10 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthL, rulerTop + offsetY - tickHeightL);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthL, rulerbottom + offsetY + tickHeightL);
        }
        else if (i % 5 == 0)
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthM, rulerTop + offsetY - tickHeightM);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthM, rulerbottom + offsetY + tickHeightM);
        }
        else
        {
            q3_1[1] = vec2f(rulerLeft + offsetX + tickWidthS, rulerTop + offsetY - tickHeightS);
            q3_2[1] = vec2f(rulerRight + offsetX - tickWidthS, rulerbottom + offsetY + tickHeightS);
        }
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_1);
        GL->glDrawArrays(GL_LINE_LOOP,0,2);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3_2);
        GL->glDrawArrays(GL_LINE_LOOP,0,2);
//        std::cout << "new index=" << i << std::endl;
//        std::cout << "q3_1[0] x=" << q3_1[0].x() << ", y=" << q3_1[0].y() << ", z=" << q3_1[0].z() << std::endl;
//        std::cout << "q3_1[1] x=" << q3_1[1].x() << ", y=" << q3_1[1].y() << ", z=" << q3_1[1].z() << std::endl << std::endl;
    }
//    glLineWidth(1.0f);
    GL->glDisableClientState(GL_VERTEX_ARRAY);
#endif

    // Draw Title Text
    surface->setShaderProgram(0);
    GL->glDisable(GL_BLEND);
    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );
    float titleTextWidth = osdParticleFluid->TitleText().length() * osdParticleFluid->fontWidth();
    if (enumOrientation != NEOSDParticleFluid::NEOSDOrientation::Top &&
        enumOrientation != NEOSDParticleFluid::NEOSDOrientation::Bottom)
    {
        glPrint(osdParticleFluid->TitleText(),
                xRes - titleTextWidth / 2,
                yRes + hRes + hTitle - OsdSize.y() / 2,
                osdParticleFluid->fontColor(),
                osdParticleFluid->getTextureFont(), osdParticleFluid->OutlineColor(), osdParticleFluid->getTextureOutlineFont());
    } else {
        glPrint(osdParticleFluid->TitleText(),
                xRes - titleTextWidth / 2,
                yRes - osdParticleFluid->fontHeight() + OsdSize.y() / 2,
                osdParticleFluid->fontColor(),
                osdParticleFluid->getTextureFont(), osdParticleFluid->OutlineColor(), osdParticleFluid->getTextureOutlineFont());
    }

    // Draw measurement values (1..step)
    for (int i = 0; i <= osdParticleFluid->TickCount(); i++)
    {
        float s = i / (float)osdParticleFluid->TickCount();
        float val = minVal * (1-s) + maxVal * s;
        QString text;
        if( osdParticleFluid->DisplayNotation() == NEOSDBase::Exponent )
        {
            text = QString("%1").arg( (double)val, 2, 'E', 2 );
        }
        else if( osdParticleFluid->DisplayNotation() == NEOSDBase::FloatingPoint )
        {
            text = QString("%1").arg( (double)val, osdParticleFluid->TotalNumberLength(), 'f', osdParticleFluid->FloatTypePrecision() );
        }
        widthU = (float)osdParticleFluid->TotalNumberLength() * osdParticleFluid->fontWidth();
        switch(enumOrientation)
        {
        case NEOSDParticleFluid::NEOSDOrientation::Top:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - OsdSize.y()/2;
            break;
        case NEOSDParticleFluid::NEOSDOrientation::Bottom:
            xPos = xRes + s * wRes - (widthU/2) -  OsdSize.x()/2;
            yPos = yRes - hTitle - (int)(osdParticleFluid->fontHeight()) + OsdSize.y()/2;
            break;
        case NEOSDParticleFluid::NEOSDOrientation::Right:
            xPos = xRes + (int)(osdParticleFluid->fontWidth()) -  OsdSize.x()/2;
            yPos = yRes + s * hRes - OsdSize.y()/2;
            break;
        case NEOSDParticleFluid::NEOSDOrientation::Left:
        default:
            xPos = xRes + wRes + wMargin -  OsdSize.x()/2;
            yPos = yRes + s * hRes - OsdSize.y()/2;
            break;
        }
        glPrint(text, xPos, yPos,
                osdParticleFluid->fontColor(),
                osdParticleFluid->getTextureFont(), osdParticleFluid->OutlineColor(), osdParticleFluid->getTextureOutlineFont());
    }

    GL->glPopAttrib();
}


NEOSDParticleFluidRendererFactory::NEOSDParticleFluidRendererFactory(NEManager* m) : NERendererFactory( m )
{
}
QString NEOSDParticleFluidRendererFactory::nodeName() { return "Particle Fluid OSD Renderer"; }
NEObject*NEOSDParticleFluidRendererFactory::createInstance() { return new NEOSDParticleFluidRenderer(); }
