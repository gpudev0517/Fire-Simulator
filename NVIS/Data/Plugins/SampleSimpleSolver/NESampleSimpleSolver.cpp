#include "NESampleSimpleSolver.h"
#include "Base/NESceneManager.h"
#include "Utility/NEOsd.h"
#include "Manager/NECollisionManager.h"
#include "Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h"
#include "Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"
#include "Utility/NETimer.h"
#include <iostream>

using namespace std;

NESampleSimpleSolver::NESampleSimpleSolver(NENodeFactory* factory) : NESphLiquidSolver(factory)
{
    declareAutoVector(NESampleSimpleParticle, m_FluidParticles, m_ParticleNEVector, m_Particles)
}

uint NESampleSimpleSolver::init(uint initMode)
{
	NESphLiquidSolver::init(initMode);

	SetTimestep(NESCENE.Timestep());


    std::cout << "Sample SPH Solver Init" << std::endl;
	return NE::kReturnSuccess;
}

NESampleSimpleSolver::~NESampleSimpleSolver()
{

}

/*!
  Particles can be added by sources or when loaded from disk (?).
*/


void NESampleSimpleSolver::addParticles(NEParticleSource* source, std::vector<NEParticle>& newParticles, const vec3f vel)
{
    NEParticleFluid::addParticles(newParticles, vel, m_ParticleNEVector);
}


/*!
    Update method for single phase fluids.
    Note that instead of boundary reference density, fluid rest density have been used.
*/
void NESampleSimpleSolver::update()
{
    NETimer::start("SampleSimpleAdvection");

    NETimer::start("SimpleForce");

	#pragma omp parallel for schedule(guided)
	for(int i = 0; i < (int)m_Particles.size(); i++)
	{
        NESampleSimpleParticle& p = m_Particles[i];
        p.setAccel(vec3f(0,0,0));
        p.accel() += totalExternalAcceleration(&(p.position()), &(p.velocity()));
	}


    NEOSD.add(31, NEOsdMsg("Advection:", QString::number(advectionTime)), this);

}

float NESampleSimpleSolver::getRestVolume()
{
	return (mass() * m_Particles.size()) / RestDensity();
}



void NESampleSimpleSolver::deleteParticles()
{
	NEParticleFluid::deleteParticles(m_ParticleNEVector);
}

void NESampleSimpleSolver::deleteParticle(NEParticle* particle)
{
	NEParticleFluid::deleteParticle(particle, m_ParticleNEVector);
}


uint NESampleSimpleSolver::saveFrameData(QString path, const int frameNumber)
{
	QString fileName;
	fileName.sprintf("/%06d.Timestep", frameNumber);

	QFile fout(path + fileName);
	if(!fout.open(QFile::WriteOnly | QFile::Truncate))
		return NE::kReturnFail;

	QDataStream out(&fout);
	out.setFloatingPointPrecision(QDataStream::DoublePrecision);
	out << Timestep() << m_OldDelta << m_TotalNrOfIterations << m_TotalPressureTime << NESCENE.SimulationStep();

	return NESphLiquidSolver::saveFrameData(path, frameNumber);
}

uint NESampleSimpleSolver::loadFrameData(QString path, const int frameNumber)
{
	uint returnValue = NESphLiquidSolver::loadFrameData(path, frameNumber);

	//if(m_Particles.size() == 0)
	//	return returnValue;

	QString fileName;
	fileName.sprintf("/%06d.Timestep", frameNumber);

	// Open the frame in read-only mode
	QString filePath = path + fileName;
	QFile fin(filePath);
    if( !fin.open(QFile::ReadOnly) )
	{
		Log() << this->objectName() << ": Cannot load " << filePath;
		EndLog(NE::kLogError);
		//resizeParticlesVector(0);
		return returnValue;
	}

	double tmpTimestep;

	QDataStream sIn(&fin);
	sIn.setFloatingPointPrecision(QDataStream::DoublePrecision);
	qreal simStep;
	sIn >> tmpTimestep >> m_OldDelta >> m_TotalNrOfIterations >> m_TotalPressureTime >> simStep;

	NESCENE.SetSimulationStep(simStep);

	if(m_ShockOccured)
	{
		m_ShockOccured = false;
		m_FrameChangedByUser = false;
	}
	else
	{
		SetTimestep(tmpTimestep);
		m_FrameChangedByUser = true;
	}
	if(ShowStatistics())
	{
		NEOSD.add(15, NEOsdMsg("Timestep:", QString::number(Timestep())), this);
		NEOSD.add(13, NEOsdMsg("OldDelta:", QString::number(m_OldDelta)), this);
		NEOSD.add(41, NEOsdMsg("TotalIterations:", QString::number(m_TotalNrOfIterations)), this);
		NEOSD.add(42, NEOsdMsg("Total Pressure Solve:", QString::number(m_TotalPressureTime)), this);
	}

	return returnValue;
}

void NESampleSimpleSolver::setRadius(qreal r)
{
	NEParticleFluid::SetRadius(r);
	SetTimestep(0.25f * (Radius() / totalExternalAcceleration().length()));

}

void NESampleSimpleSolver::setRestDensity(qreal density)
{
	NEParticleFluid::SetRestDensity(density);

}



NESampleSimpleFactory::NESampleSimpleFactory(NEManager* m) : NENodeFactory(m) {}

NESampleSimpleFactory::~NESampleSimpleFactory() {}

QString NESampleSimpleFactory::nodeName()
{
    return "Sample Simple solver";
}

uint NESampleSimpleFactory::nodeType()
{
	return NE::kObjectParticleFluid;
}

QString NESampleSimpleFactory::nodeInherits()
{
    return "NESphLiquidSolver";
}

QString NESampleSimpleFactory::objectName()
{
    return "SampleSphSolver";
}

QString NESampleSimpleFactory::nodeVersion()
{
	return "0.1";
}

QString NESampleSimpleFactory::nodeVendor()
{
	return "Neutrino Dynamics";
}

QString NESampleSimpleFactory::nodeHelp()
{
    return "Sample Solver for Neutrino";
}

NEObject* NESampleSimpleFactory::createInstance()
{
    return new NESampleSimpleSolver(this);
}
