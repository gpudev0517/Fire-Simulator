#include "NESampleEmitter.h"
#include "Spatial/Physical/Fluid/NEParticleFluid.h"
#include "Base/NESceneManager.h"
#include "Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"

#include "Base/NEGLManager.h"

NESampleEmitter::NESampleEmitter( NENodeFactory* factory ) : NEParticleSource(factory)
{
	SetPosition(vec3f(0,1,0));
	NESpatial::SetScale(vec3f(1.0, 1.0, 1.0));
	m_Velocity.set(0,-2,0);
	m_Verbose = false;
	m_UseSolidPlane = false;
	m_BackPlane = NULL;
}

NESampleEmitter::~NESampleEmitter()
{
    QObject* bPlane = (NESphRigid*)NESCENE.findChildNode( this->objectName() + "_BackPlane");
    if(bPlane)
        NESCENE.deleteObject("SPH rigid plane", this->objectName() + "_BackPlane", true);

}

uint NESampleEmitter::init(uint initMode)
{
	NEParticleSource::init(initMode);

	return NE::kReturnSuccess;
}

void NESampleEmitter::deleteSolidPlane()
{
	if((NESphRigid*)NESCENE.findChildNode( this->objectName() + "_BackPlane"))
	{
		NESCENE.deleteObject("SPH rigid plane", this->objectName() + "_BackPlane", true);
	}
}

bool NESampleEmitter::updateAABB()
{
    AABB val;

    val.insertPoint(m_MinPos);
    val.insertPoint(m_MaxPos);

    setAABB(val);

    return aabb().valid();
}

/*!
    Detailed description.
*/
void NESampleEmitter::generateParticles()
{
    if( !Active() )
        return;

    NEParticleSource::generateParticles();
    if( m_DeltaX.length() >= m_YSpacing )
    {
        m_tmpParticles.resize(0);

        if(spherePacking())
        {
            generateParticlesSpherePacking();
        }
        else
        {
            generateParticlesOnGrid();
        }

        //transform particles position and velocities according to orientation and position of source
        transformParticles();
    }
}

/*!
    Detailed description.
*/
void NESampleEmitter::generateParticlesOnGrid()
{
    // Compute the total number of particles in x and z directions, and the total
    int nx = (int) (floor( (Scale().x()/ m_Spacing ) + FEps )+FEps);
    int nz = (int) (floor( (Scale().z()/ m_Spacing ) + FEps )+FEps);

    double xSpacing = m_Spacing/Scale().x();
    double zSpacing = m_Spacing/Scale().z();
    vec3f deltaOffset = vec3f(0.0f,m_DeltaX.length() - m_YSpacing ,0.0f);

    // Compute the minimum and maximum positions. Make particles inside square extent with half spacing offset to the border.
    double x_Offset = (double)nx * 0.5 * xSpacing - 0.5*xSpacing;
    double z_Offset = (double)nz * 0.5 * zSpacing - 0.5*zSpacing;
    const vec3d min = vec3d(-x_Offset, 0.0, -z_Offset) ;
//	const vec3d max = vec3d(x_Offset, 0.0, z_Offset);

//	int cx=0, cz= 0;
    //NEW: change slightly position
    for(int cx=0; cx<nx ; cx++)
    {
        for(int cz= 0; cz<nz ; cz++)
        {
            NEParticle particle;
            vec3f pos(min.x() + cx * xSpacing, 0.f, min.z() + cz * zSpacing);
            particle.setPosition(pos - deltaOffset);
            m_tmpParticles.push_back(particle);
        }
    }

    setDelta(deltaOffset);
    if(m_DeltaX.length() > m_YSpacing)
    {
        m_LayerCount++;
        generateParticlesOnGrid();
    }

}

/*!
	Detailed description.
*/
void NESampleEmitter::generateParticlesSpherePacking()
{
	// Compute the total number of particles in x and z directions, and the total
	int nx = ceil((Scale().x() / m_Spacing)-FEps);
	int nz = ceil((Scale().z() / m_Spacing)-FEps);

//	int cx=0, cz= 0;
	const vec3d min = vec3d(-0.5,0.0,-0.5);
//	const vec3d max = vec3d(0.5,0.0,0.5);

	double xSpacing = m_Spacing/Scale().x();
	double zSpacing = m_Spacing/Scale().z();
	vec3f deltaOffset = vec3f(0.0f,m_Spacing - m_DeltaX.length(),0.0f);

	//NEW: change slightly position
	for(int cx=0; cx<nx ; cx++)
	{
		for(int cz= 0; cz<nz ; cz++)
		{
			NEParticle particle;
			vec3f pos(min.x() + cx * xSpacing, 0.f, min.z() + cz * zSpacing);
			particle.setPosition(pos + deltaOffset);
			m_tmpParticles.push_back(particle);
		}
	}

	setDelta(deltaOffset);
	if(m_DeltaX.length() > m_Spacing)
	{
		m_LayerCount++;
		generateParticlesSpherePacking();
	}

}


/*!
	Generates the solid backplane for the source
*/
void NESampleEmitter::transformSolidPlane()
{
	m_BackPlane = (NESphRigid*)NESCENE.findChildNode( this->objectName() + "_BackPlane");
	if(m_UseSolidPlane && m_BackPlane != NULL)
	{
		float trH = sqrt(3.f) * 0.5f * m_Spacing;
		vec3f pos(0.f,trH,0.f);

		m_BackPlane->SetScale(NESpatial::Scale());
		m_BackPlane->SetOrientation(NESpatial::Orientation());
		m_BackPlane->SetPosition(transform()*pos);
	}
}

uint NESampleEmitter::SetPosition(const vec3f& position)
{
	uint retVal = NEParticleSource::SetPosition(position);
	if(retVal == NE::kReturnSuccess)
		transformSolidPlane();
	return retVal;
}

uint NESampleEmitter::SetScale(const vec3f& scale)
{
	if(scale.x() < 0 ||
	   scale.z() < 0 )
	   return NE::kReturnFail;

	vec3f fixedScale(scale);
	//fix y scale to 1
	fixedScale.setY(1.0f);
	//adjust x scale
	//if(scale.x() != NESpatial::scale().x())
	//{
	//	fixedScale.setX(clampScale(scale.x(), m_Spacing));
	//}
	////adjust z scale
	//if(scale.z() != NESpatial::scale().z())
	//{
	//	fixedScale.setZ(clampScale(scale.z(), m_Spacing));
	//}

	if(NESpatial::SetScale(fixedScale) != NE::kReturnSuccess)
		return NE::kReturnFail;
	NESpatial::updateWorldTransform();

	transformSolidPlane();

	return NE::kReturnSuccess;
}

uint NESampleEmitter::SetOrientation(const eulerf &rotation)
{
	uint retVal = NEParticleSource::SetOrientation(rotation);
	if(retVal == NE::kReturnSuccess)
		transformSolidPlane();
	return retVal;
}

void NESampleEmitter::setUseSolidPlane(bool val)
{
	m_UseSolidPlane = val;
	if(m_UseSolidPlane && m_BackPlane == NULL)
	{
		m_BackPlane = (NESphRigid*)NESCENE.findChildNode( this->objectName() + "_BackPlane");
		if(m_BackPlane == NULL)
		{
			createBackPlane();
		}
		transformSolidPlane();
	}
	else if(!m_UseSolidPlane)
	{
		deleteSolidPlane();
		m_BackPlane = NULL;
	}
}

uint NESampleEmitter::createBackPlane()
{
	m_BackPlane = (NESphRigid*)NESCENE.createObject("SPH rigid plane", this->objectName() + "_BackPlane", this);
	m_BackPlane->SetColor(QColor(255,228,181,30));
	m_BackPlane->SetRenderMode(NERenderMode::Wireframe);
	m_BackPlane->SetCacheData(false);
	if(m_BackPlane)
	{
		return NE::kReturnSuccess;
	}
	return NE::kReturnFail;
}

void NESampleEmitter::recomputeMinMax()
{
	//m_MinPos = position() - vec3f(delta,0,delta);
	//m_MaxPos = position() + vec3f(delta,0,delta);

	//estimate nr of particles generated in one step
	//computeNrOfGeneratedParticlesPerStep();
}


/*!
*/
void NESampleEmitter::estimateSpacing(double val)
{

	if(/*NEParticleFluid* fluid = */qobject_cast<NEParticleFluid*>(m_pFluid))
	{
		if(SceneInitialized())
		{
			//float minSpacing = fluid->restDistance() + fluid->restDistance()* 0.1f;
			//if(val < minSpacing)
			////for particle fluids add 10% safety distance to ensure numerical stability
			//	val = minSpacing;
		}

		setSpacing( val );
	}
	else
		NEParticleSource::setSpacing(val);
}

void NESampleEmitter::render()
{
	if( RenderMode() == NERenderMode::Invisible )
		return;

	NEGL.bindColorShader();

	vec3f minPos =  vec3f(-0.5f,0.f,-0.5f);
	vec3f maxPos =  vec3f(0.5f,0.0f,0.5f);

	glLineWidth(1.0f);
	//draw source field
	glBegin(GL_QUADS);
	glVertex3f(minPos.x(), minPos.y(), minPos.z());
	glVertex3f(maxPos.x(), minPos.y(), minPos.z());
	glVertex3f(maxPos.x(), minPos.y(), maxPos.z());
	glVertex3f(minPos.x(), minPos.y(), maxPos.z());
	glEnd();
	glLineWidth(3.0f);
	//draw velocity direction
	vec3f velNorm =  vec3f(0.f, -0.3f, 0.f);//m_Velocity.normalized()*0.4f;
	glBegin(GL_LINES);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(velNorm.x(), velNorm.y(), velNorm.z());
	glEnd();

	glLineWidth(1.0f);

}

void NESampleEmitter::renderPickable()
{
	render();
}

void NESampleEmitter::renderPicked()
{
	render();
}

NESampleEmitterFactory::NESampleEmitterFactory(NEManager* m) : NENodeFactory(m) {}

NESampleEmitterFactory::~NESampleEmitterFactory(){}

QString NESampleEmitterFactory::nodeName() { return "Sample Particle Emitter"; }

uint NESampleEmitterFactory::nodeType(){ return NE::kObjectParticleSource;}

QString NESampleEmitterFactory::nodeInherits() { return "NEParticleSource";}

QString NESampleEmitterFactory::objectName() { return "SampleEmitter"; }

QString NESampleEmitterFactory::nodeVersion() { return "0.1"; }

QString NESampleEmitterFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NESampleEmitterFactory::nodeHelp() { return "Square particle emitter"; }

NEObject*NESampleEmitterFactory::createInstance() { return new NESampleEmitter(this);	}
