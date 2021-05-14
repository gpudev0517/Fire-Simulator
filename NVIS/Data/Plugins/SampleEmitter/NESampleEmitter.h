#pragma once

#include "Spatial/Source/NEParticleSource.h"
#include "Base/NENodeFactory.h"

class NESphRigid;

class NESampleEmitter : public NEParticleSource
{
	Q_INTERFACES(NEParticleSource)
	Q_OBJECT
    Q_CLASSINFO("ClassName", "Sample Particle Source")

	//Q_PROPERTY(double length READ length WRITE setLength)
	Q_PROPERTY(bool useSolidPlane READ useSolidPlane WRITE setUseSolidPlane)
	Q_CLASSINFO("name-useSolidPlane", "Solid Back-Plane")
	Q_CLASSINFO("help-useSolidPlane", "Use a solid back-plane behing the source to reduce the chance of source explosions.")


public:
	NESampleEmitter(NENodeFactory* factory);
	virtual ~NESampleEmitter();

	//! Set default values for the particle source.
	uint init(uint initMode = NE::kInitOnCreate);
	//! Generates the particles, see update method in NEParticleSource.
	void generateParticles();

	//void render();
	virtual void renderPickable();
	virtual void renderPicked();
	bool updateAABB();
	//! Deletes the solid backplane
	virtual void deleteSolidPlane();
public slots:

	virtual uint SetPosition(const vec3f& Position);
	virtual uint SetScale(const vec3f& Scale);
	virtual uint SetOrientation(const eulerf &rotation);
	void recomputeMinMax();
	//! Overwrite estimate spacing, for square source the spacing should be 10%larger than the rest distance of the solver, in order to ensure stability
	virtual void estimateSpacing(double val);

	bool useSolidPlane() const {return m_UseSolidPlane;}
	void setUseSolidPlane(bool val);


protected:
	//! Generates the particles grid aligned on the specified rectangle.
	void generateParticlesOnGrid();
	//! Generates the particles using sphere packing on the specified rectangle.
	void generateParticlesSpherePacking();
	//! This should render an object as it should be seen to the viewer
	virtual void render();
	//! Generates the solid backplane for the source
	void transformSolidPlane();
	uint createBackPlane();

	vec3f m_MinPos;
	vec3f m_MaxPos;
	double m_Length;

	bool m_UseSolidPlane;
	NESphRigid* m_BackPlane;
};

class NESampleEmitterFactory : public NENodeFactory
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "NENodeFactory")
	Q_INTERFACES(NENodeFactory)

public:
	NESampleEmitterFactory(NEManager* m = 0);
	~NESampleEmitterFactory();
	NE_DERIVED(NENodeFactory, Public)
};
