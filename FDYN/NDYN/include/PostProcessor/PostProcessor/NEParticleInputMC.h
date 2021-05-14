#pragma once

#include "NEMarchingCubes.h"

class NEParticleFluidSolver;

//!  Implementation of the marching cubes for SPH.
/*!

*/
class NECEXP_NEPOSTPROCESSOR NEParticleInputMC : public NEMarchingCubes
{
    Q_OBJECT
    NE_PROP_R(double, SupportRadius, "Support Radius", "Support radius of the marching cubes", true)
    NE_PROP_R(double, MCRadius, "Marching Cubes radius", "The marching cubes radius", true)
    NE_PROP_RW(double, InfluenceRatio, "Influence Ratio", "Influence ratio", true,)
    NE_PROP_RW(int, RadiusMultiplier, "Radius Multiplier", "Radius Multiplier", true,)

public:
    // Parallelization related structure
    struct IdPair
    {
        long CellID; //cellId of the particle
        int pId;     //Id of the particle

        bool operator < (const IdPair& k1) const
        {
            return (CellID < k1.CellID);
        }
    };

    NEParticleInputMC(NENodeFactory* val);
    virtual uint init(uint initMode = NE::kInitOnCreate);
    virtual void doPreprocess();
    virtual void computeGrid(bool splashExtract = false);
    virtual void update() = 0;
    virtual void computeScalarField() = 0;
    float computeSignedDist(vec3f& x0, vec3f& x1, float r);
    virtual void render();
    virtual void resetProperties();
    virtual void setParticleRadius();

protected:
    //! This method should be called by any input that connects this class as a target.
    virtual  bool addSource(NENode* node, NEConn &conn);
    //!
    virtual  void removeSource(NENode* node, uint type, uint sIndex, uint tIndex);
    //! Connects signals and slots. Called by addSource method.
    virtual  void setUpFluidConnections();
    //! The input source is an neparticle, the particles are supported by this source, but also other values.
    NEParticleFluidSolver* m_pParticleFluidSolver;
    //! Copies the values between different isosurface extraction methods
    virtual void copyValues(NEParticleInputMC& extractionMethod);
    //! Checks if the surface reconstruction has been initialized
    bool m_Initialized;
    //! Counts the number of steps run along the surface reconstruction
    int m_ReconstructionStep;
    //! Splash particles should be handled separately for split/merge operations
    uint m_SplashThreshold;
    //! m_H in the older version
    double m_SupportRadius;
    //! m_R in the older version
    double m_MCRadius;
    //!
    double m_InverseMCRadius;
    //!
    double m_MCRadiusSquare;
    //!
    double m_SupportRadiusSquare;
    //!
    double m_InverseMCRadiusSquare;
    //! m_MCRadius / m_SupportRadius
    double m_InneuenceRatio;
    //! Radius of the particle obtained from the simulation itself
    double m_ParticleRadius;
    //! The multiplier of the particle radius which effects the smoothness
    int m_RadiusMultiplier;

    //!
    void tesselateSplashParticle(NEIndexedTriMesh& mesh, float radius, unsigned int subdivision, SolidBase sbase);
    //!
    void createTessellatedRecursive(float* a, float* b, float* c, unsigned int div, float r, NEIndexedTriMesh& mesh);
    double particleRadius() const;
    uint SetResolution(const double& val) override;
    uint splashThreshold() const;
    void SetSplashThreshold(uint val);
};


Q_DECLARE_INTERFACE(NEParticleInputMC, "NEParticleInputMC")
