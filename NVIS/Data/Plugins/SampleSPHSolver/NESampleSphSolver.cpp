#include "NESampleSphSolver.h"
#include "Base/NESceneManager.h"
#include "Utility/NEOsd.h"
#include "Manager/NECollisionManager.h"
#include "Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h"
#include "Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"
#include "Utility/NETimer.h"
#include <iostream>

using namespace std;

NESampleSphSolver::NESampleSphSolver(NENodeFactory* factory) : NESphLiquidSolver(factory)
{
	m_OldDeltaInitialized = false;
	m_OldDelta = 0.0f;
	m_ShockOccured = false;
	m_ShockInstant = -1.0f;
	m_LastTimestep = 0.0f;
	m_MinNumIters = 3;	// The number of minimum and maximum PCISPH iterations
	m_MaxNumIters = 3;
	m_FrameChangedByUser = false;
    m_PcisphModel = NESampleSphSolver::pciModelAkinci;
	m_IsolatedDamping = false;
	m_IsoDampingFactor = 1.0f;
	m_OldAveDensity = -1.0f;
	//maximum density error allowed by user
	m_MaxDensErrUser = 10.0f;
	m_AvgDensErrUser = 0.1f;
	m_StopCriterion = scAvgDensity;
	m_TotalPressureTime = 0.0;
	m_TotalNrOfIterations = 0;
    std::cout << "Sample SPH Solver Constructor" << std::endl;
    declareAutoVector(NESampleSphParticle, m_FluidParticles, m_ParticleNEVector, m_Particles)
}

uint NESampleSphSolver::init(uint initMode)
{
	NESphLiquidSolver::init(initMode);

	SetTimestep(NESCENE.Timestep());
	// Depending on the total external forces, adjust the Timestep
	if(totalExternalAcceleration().length() > FEps)
		SetTimestep(0.25f * (Radius() / totalExternalAcceleration().length()));
	// Compute the PCISPH delta value for a prototype particle
	computePrototypeDelta();

	m_TotalPressureTime = 0.0;
	m_TotalNrOfIterations = 0;

    std::cout << "Sample SPH Solver Init" << std::endl;
	return NE::kReturnSuccess;
}

NESampleSphSolver::~NESampleSphSolver()
{

}

/*!
  Particles can be added by sources or when loaded from disk (?).
*/


void NESampleSphSolver::addParticles(NEParticleSource* source, std::vector<NEParticle>& newParticles, const vec3f vel)
{
    NEParticleFluid::addParticles(newParticles, vel, m_ParticleNEVector);
}


/*!
    Update method for single phase fluids.
    Note that instead of boundary reference density, fluid rest density have been used.
*/
void NESampleSphSolver::update()
{
    NETimer::start("SampleSphAdvection");

	//update delta wrt the current time step
	updateDelta();
	// NEW: Compute densities of particles
	computeFluidDensitiesWithRemoval();
	// Compute all forces except pressure force and store it in a_aux
	computeForcesWithoutPressure();
    double advectionTime = NETimer::stop("SampleSphAdvection");

	// Compute pressure value using jacobi iterations
    NETimer::start("SampleSphIterativeUpdate");
	// Compute pressure force in predictive-corrective manner
	switch(m_PcisphModel)
	{
    case NESampleSphSolver::pciModelAkinci:
		iterativePressureSolveAkinci();
		break;
    case NESampleSphSolver::pciModelIhmsen:
		iterativePressureSolveIhmsen();
		break;
    case NESampleSphSolver::pciModelSolenthaler:
		iterativePressureSolveSolenthaler();
		break;
	default :
		iterativePressureSolveAkinci();
	}
    m_PressureIterationTime = NETimer::stop("SampleSphIterativeUpdate");



	NETimer::start("PCISphForce");
	//PreciseTimer::start("PCISphIntegration");
	#pragma omp parallel for schedule(guided)
	for(int i = 0; i < (int)m_Particles.size(); i++)
	{
        NESampleSphParticle& p = m_Particles[i];
		p.accel() += p.a_aux;
		p.predPos = p.position();
		p.a_aux = p.velocity();
	}

    /*
	vector<NERigidSphParticle*>* boundaryParticles = NECOLMGR.activeRigidParticles();
	#pragma omp parallel for schedule(guided)
	for(int i = 0; i < (int)boundaryParticles->size(); i++)
	{
		NERigidSphParticle& bp = *(*boundaryParticles)[i];
		computeRigidAccel(bp);
	}
	double forceTime = NETimer::stop("PCISphForce");
*/
	m_TotalPressureTime += m_PressureIterationTime;
	NEOSD.add(31, NEOsdMsg("Advection:", QString::number(advectionTime)), this);
	NEOSD.add(32, NEOsdMsg("Pressure Solve:", QString::number(m_PressureIterationTime)), this);
    //NEOSD.add(33, NEOsdMsg("Pressure Force:", QString::number(forceTime)), this);
	NEOSD.add(42, NEOsdMsg("Total Pressure Solve:", QString::number(m_TotalPressureTime)), this);
	NEOSD.add(41, NEOsdMsg("TotalIterations:", QString::number(m_TotalNrOfIterations)), this);

}

float NESampleSphSolver::getRestVolume()
{
	return (mass() * m_Particles.size()) / RestDensity();
}

NESphLiquidSolver::NESphUpdatePart NESampleSphSolver::updateMultiPhase(NESphLiquidSolver::NESphUpdatePart currentStep)
{
    std::cout << "multi update" << std::endl;
	NESphUpdatePart nextStep;
	switch(currentStep)
	{
	// compute densities and pressure
	case NESphLiquidSolver::kOne:
	{
		computeInverseMass();
		m_CurrentIter = 0;
        m_IterativePart = NESampleSphSolver::kOne;
		//update delta wrt the current time step
		updateDelta();
		//compute densities
		#		pragma omp parallel for schedule(guided)
		for(int i = 0; i < (int)m_Particles.size(); i ++)
		{
			computeAdaptedDensity(&m_Particles[i], i);
		}
		//return next step
		nextStep = NESphLiquidSolver::kTwo;
	}
	break;

	// compute interface normals
	case NESphLiquidSolver::kTwo:
		#		pragma omp parallel for schedule(guided)
		for(int i = 0; i < (int)m_Particles.size(); i ++)
		{
			computeInterfaceNormals(&m_Particles[i], i);
		}
		nextStep = NESphLiquidSolver::kThree;
		break;

	case NESphLiquidSolver::kThree:
		// Compute all forces except pressure force and store it in a_aux
		#pragma omp parallel for  schedule(guided)
		for(int i = 0; i < (int)m_Particles.size(); i++)
		{
            NESampleSphParticle& p = m_Particles[i];
			p.setAccel(vec3f(0, 0, 0));
			computeMultiAcceleration_NoPressure(i);
			p.a_aux += totalExternalAcceleration(&(p.position()), &(p.velocity()));
			p.setPressure(0.0f);
		}
		nextStep = NESphLiquidSolver::kFour;
		break;

	case NESphLiquidSolver::kFour:
		// compute forces
		iterativePressureSolveMultiAkinci();
		if(m_CurrentIter < m_MinNumIters)
			nextStep = NESphLiquidSolver::kFour;
		else
			nextStep = NESphLiquidSolver::kFive;
		break;

	// compute forces
    case NESampleSphSolver::kFive:
		#	pragma omp parallel for schedule(guided)
		for(int i = 0; i < (int)m_Particles.size(); i++)
		{
            NESampleSphParticle& p = m_Particles[i];
			p.accel() += p.a_aux;
			p.predPos = p.position();
			p.a_aux = p.velocity();
		}
		computeStatistics();
        nextStep = NESampleSphSolver::kFinish;
		break;
	}
	return nextStep;

}

/*!
    Implements the PCISPH loop as explained in SP09
*/
void NESampleSphSolver::iterativePressureSolveSolenthaler()
{
    vector<NESampleSphParticle>& particles = m_Particles;
	int i;
	int nrOfFluidParticles = (int) particles.size();
	double InvNrOfFluidParticle = 1.0 / (double) nrOfFluidParticles;
	int iter = 0;	// Holds the number of pcisph iterations
	bool densitiesCorrected = false;	// Have the densities been corrected for each individual particle?
	double absoluteAvgYieldError = m_AvgDensErrUser * 0.01 * RestDensity() + RestDensity();

	float maxDensErrUserPercentage = m_MaxDensErrUser * 0.01f * RestDensity() + RestDensity();
	float maxDensErr = FLT_MIN;
	float maxDensErrLastIteration = FLT_MAX;
	double aveDensity(0.0);
	// Compute pressure force in predictive-corrective manner
	while(iter < maxIter())
	{
		aveDensity = 0.0;
		// predict velocity and position
		#pragma omp parallel private(i)
		{
			float t_densError(0.0);
			#pragma omp for schedule(guided)
			for(i = 0; i < nrOfFluidParticles; i++)
			{
                NESampleSphParticle& particle = m_Particles[i];
				// predict velocity and position
				vec3f predVel = particle.velocity() + ((particle.accel() + particle.a_aux) * Timestep());
				particle.predPos = particle.position() + (predVel * Timestep());

			}
			#pragma omp for schedule(guided) reduction(+:aveDensity)
			for(i = 0; i < nrOfFluidParticles; i++)
			{
				// Predict density
				float density = recomputeDensityWithUpdatedNeighbors(i);
				// Predict density variation
				float partDensityError = density - RestDensity();

				if(partDensityError > t_densError)
					t_densError = partDensityError;
				// Update pressure according to the density error
				m_Particles[i].pressure() += m_NewDelta * partDensityError;

				aveDensity += (double) density * InvNrOfFluidParticle;
			}

			#pragma omp critical
			{
				if(t_densError > maxDensErr)
					maxDensErr = t_densError;
			}
		}
		maxDensErr = maxDensErr * 100.f / RestDensity();
		//Logging info
		if(Verbose())
		{
			Log() << "Iteration " << iter << ", Density Error, " << maxDensErr;
			EndLog(NE::kLogInfo);
		}

		if(maxDensErr > maxDensErrLastIteration)
		{
			Log() << "Convergence warning. Density error last iteration: " << maxDensErrLastIteration << ", current iteration: " << maxDensErr;
			EndLog(NE::kLogWarning);
		}
		maxDensErrLastIteration = maxDensErr;

		//check escape criterion
		bool breakIterations = false;

		if(stopCriterion() == scAvgDensity)
		{
			breakIterations = (aveDensity <= absoluteAvgYieldError && iter >= iterations());
		}
		else if(stopCriterion() == scMaxDensity)
		{
			breakIterations = (maxDensErr <= m_MaxDensErrUser && iter >= iterations());
		}

		if(breakIterations)
		{
			break;
		}
		else
		{
			#			pragma omp parallel for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
			{
				//computeAcceleration_P(i);
				computeAcceleration_P_oldPosition(i);
			}
		}
		maxDensErr = 0.0f;
		iter++;
	}
	//accumulate the total nr of iterations
	m_TotalNrOfIterations += iter;
	if(ShowStatistics())
	{
		NEOSD.add(16, NEOsdMsg("Iterations:", QString::number(iter)), this);
		NEOSD.add(17, NEOsdMsg("DensError %:", QString::number(maxDensErr * 100.f / RestDensity())), this);
		NEOSD.add(18, NEOsdMsg("RestVolume:", QString::number(getRestVolume())), this);
	}
}

/*!
    Implements the PCISPH loop as explained in IAGT10
*/
void NESampleSphSolver::iterativePressureSolveIhmsen()
{
	int nrOfFluidParticles = (int) m_Particles.size();
	uint iter = 0;	// Holds the number of pcisph iterations
	vector<NERigidSphParticle*>* boundaryParticles = NECOLMGR.activeRigidParticles();

	// Compute pressure force in predictive-corrective manner
	while(iter < m_MinNumIters)
	{
		// predict velocity and position
		#pragma omp parallel
		{
			#pragma omp for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
			{
                NESampleSphParticle& particle = m_Particles[i];

				// predict velocity and position
				vec3f predVel = particle.velocity() + ((particle.accel() + particle.a_aux) * Timestep());
				particle.predPos = particle.position() + (predVel * Timestep());

				// predict and correct world collision
				vector<Neighbor>& rigidNeighbor = m_RigidParticleNeighbors[i];
				if(rigidNeighbor.size() > 0)
					boundaryCollision(&particle, particle.predPos, false);
			}
			#pragma omp for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
			{
				// Check distances with the neighbors again
				recomputeDistancesWithNeighbors(i);
				// Predict density
				computeFluidParticleDensity(&m_Particles[i], i);
				// Predict density variation
				float partDensityError = m_Particles[i].density() - RestDensity();
				// Update pressure according to the density errpr
				m_Particles[i].pressure() += m_NewDelta * partDensityError;
			}

			//loop over the boundary particles and
			#pragma omp for schedule(guided)
			for(int i = 0; i < (int)boundaryParticles->size(); i ++)
			{
				NERigidSphParticle& bp = *(*boundaryParticles)[i];
				// Check distances with the neighbors again
				recomputeDistancesWithNeighbors(bp);
				// Predict density
				computeRigidParticleDensity(bp);
				//correct rigid density
				if(bp.density() < this->RestDensity() && CorrectDensity())
					bp.density() = this->RestDensity();

				// Predict density variation
				double partDensityError = bp.density() - this->RestDensity();
				//double partDensityError = bp.density() - bp.initialDensity();
				// Update pressure according to the density error
				bp.pressure() += m_NewDelta * partDensityError;
			}

			// Compute acceleration caused by the pressure force
			#pragma omp for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
				computeAcceleration_P(i);
		} //end of parallel region
		iter++;
	}
}

/*!
    This method implements the iterative pressure solve loop with adaptive time stepping
    and robust two-way coupling of rigid bodies as explained in Akinci et al. 2012
*/
void NESampleSphSolver::iterativePressureSolveAkinci()
{
	int nrOfFluidParticles = (int) m_Particles.size();
	uint iter = 0;	// Holds the number of pcisph iterations
	float maxDensityError = 0.0f;
	float densErrLastIteration = FLT_MAX;

	// Compute pressure force in predictive-corrective manner
	while(iter < m_MinNumIters)
	{
		// predict velocity and position
		#pragma omp parallel
		{
			float t_densError = 0.0f;

			#pragma omp for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
			{
                NESampleSphParticle& particle = m_Particles[i];
				// predict velocity and position
				vec3f predVel = particle.velocity() + ((particle.accel() + particle.a_aux) * Timestep());
				particle.predPos = particle.position() + (predVel * Timestep());
			}
			#pragma omp for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
			{
				// Check distances with the neighbors again
				recomputeDistancesWithNeighbors(i);
				// Predict density
				float predDens = getFluidParticleDensity(m_Particles[i], i);
				// Predict density variation
				float partDensityError = predDens - RestDensity();
				t_densError = max(t_densError, partDensityError);
				// Update pressure according to the density errpr
				m_Particles[i].pressure() += m_NewDelta * partDensityError;
			}

			// Compute acceleration caused by the pressure force
			#pragma omp for schedule(guided)
			for(int i = 0; i < nrOfFluidParticles; i++)
				computeAcceleration_P(i);

			#pragma omp critical
			{
				maxDensityError = max(t_densError, maxDensityError);

			}
		} //end of parallel region

		if(maxDensityError > densErrLastIteration)
			Log() << "Convergence warning. Density error last iteration: " << densErrLastIteration << ", current iteration: " << maxDensityError;
		EndLog(NE::kLogWarning);

		densErrLastIteration = maxDensityError;
		iter++;
	}

}
/*!
    Computes all internal and external forces except the pressure force.
    Initializes pressure values
*/
void NESampleSphSolver::computeForcesWithoutPressure()
{
	#pragma omp parallel for  schedule(guided)
	for(int i = 0; i < (int)m_Particles.size(); i++)
	{
        NESampleSphParticle& p = m_Particles[i];
		//p.setDensity(RestDensity());
		p.setAccel(vec3f(0, 0, 0));
		computeAcceleration_V_G_EXT(i);
		p.a_aux += totalExternalAcceleration(&(p.position()), &(p.velocity()));

		// Damp isolated particles' velocities
		if(isolatedDamping())
		{
			if(p.neighborVector(p.index())->size() < 2 && p.velocity()*p.velocity() > 1.0)
			{
				vec3f v = -p.velocity();
				p.a_aux += m_IsoDampingFactor * v;
			}
		}

		p.setPressure(0.0f);
	}
}

/*!
*/
void NESampleSphSolver::updateDelta()
{
	m_NewDelta = (double)m_PrototypeDeltaWOTimestep * (1.0 / ((double)Timestep() * Timestep()));

	if(m_OldDeltaInitialized == false || m_OldDelta == 0.0f) // If delta is not initialized yet or the first frame is loaded
	{
		m_OldDelta = m_NewDelta;
		m_OldDeltaInitialized = true;
	}
	if(m_OldDelta / m_NewDelta < 0.975)
		m_NewDelta = m_OldDelta * 1.025f;

	m_OldDelta = m_NewDelta;

	if(ShowStatistics())
	{
		//NEOSD.add(12, NEOsdMsg("NewDelta:", QString::number(m_NewDelta), osdMsgPos() ));
		NEOSD.add(13, NEOsdMsg("OldDelta:", QString::number(m_OldDelta)), this);
	}
}

void NESampleSphSolver::computePressure(const int particleVecId)
{

}



void NESampleSphSolver::resizeParticlesVector(uint nItems)
{
	m_ParticleNEVector.resize(nItems);
	m_LiquidParticleNeighbors.resize(nItems);
	m_RigidParticleNeighbors.resize(nItems);
}

void NESampleSphSolver::reserveSpaceForParticles(uint nItems)
{
	m_ParticleNEVector.reserve(nItems);
	m_LiquidParticleNeighbors.reserve(nItems);
	m_RigidParticleNeighbors.reserve(nItems);
}

// Computes the delta value for a prototype particle

void NESampleSphSolver::computePrototypeDelta()
{
	double beta = (double)Timestep() * (double)Timestep() * (double)m_ParticleMass * (double)m_ParticleMass *
				  (2.0 / ((double)RestDensity() * (double)RestDensity()));

	double betaWoTimestep = (double)m_ParticleMass * (double)m_ParticleMass * (2.0 / ((double)RestDensity() * (double)RestDensity()));

	vec3f k1(0, 0, 0);
	double k2 = 0.0f;

	float distInv = 1.0 / m_RestDistance;
	float distInvSq2 = 1.0 / (m_RestDistance, sqrt(2.0));
	float distInvSq3 = 1.0 / (m_RestDistance, sqrt(3.0));

	vec3f kvalue = KernelGrad(m_RestDistance, distInv) * vec3f(m_RestDistance, 0, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInv) * vec3f(-m_RestDistance, 0, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInv) * vec3f(0, m_RestDistance, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInv) * vec3f(0, -m_RestDistance, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(m_RestDistance, m_RestDistance, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(-m_RestDistance, m_RestDistance, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(m_RestDistance, -m_RestDistance, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(-m_RestDistance, -m_RestDistance, 0);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);


	kvalue = KernelGrad(m_RestDistance, distInv) * vec3f(0, 0, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInv) * vec3f(0, 0, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(0 , m_RestDistance, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(0 , m_RestDistance, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(0 , -m_RestDistance, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(0, -m_RestDistance, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);

	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(m_RestDistance, 0, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(m_RestDistance, 0, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(-m_RestDistance, 0, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq2) * vec3f(-m_RestDistance, 0, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);


	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(m_RestDistance, m_RestDistance, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(m_RestDistance, m_RestDistance, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(m_RestDistance, -m_RestDistance, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(-m_RestDistance, m_RestDistance, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(m_RestDistance, -m_RestDistance, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(-m_RestDistance, m_RestDistance, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(-m_RestDistance, -m_RestDistance, m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);
	kvalue = KernelGrad(m_RestDistance, distInvSq3) * vec3f(-m_RestDistance, -m_RestDistance, -m_RestDistance);
	k1 += kvalue;
	k2 += vec3f::dotProduct(kvalue, kvalue);

	m_PrototypeDelta = float(-1.0 / (beta * (-(double)(vec3f::dotProduct(k1, k1)) - k2)));
	m_PrototypeDeltaWOTimestep = float(-1.0 / (betaWoTimestep * (-(double)(vec3f::dotProduct(k1, k1)) - k2)));
	Log() << "m_PciSphPrototypeDeltaWOTimestep" << m_PrototypeDeltaWOTimestep;
	EndLog(NE::kLogInfo);
}

void NESampleSphSolver::recomputeDistancesWithNeighbors(int pIndex)
{
    NESampleSphParticle& particle = m_Particles[pIndex];
	vector<Neighbor>& fNeighbors = m_LiquidParticleNeighbors[pIndex];
	for(int i = 0; i < fNeighbors.size(); i++)
	{
        NESampleSphParticle* neighbor = (NESampleSphParticle*)fNeighbors[i].neighbor();
		fNeighbors[i].setDistance((particle.predPos - neighbor->predPos).length());
	}

	vector<Neighbor>& rNeighbors = m_RigidParticleNeighbors[pIndex];
	for(int i = 0; i < rNeighbors.size(); i++)
	{
		NERigidParticle* neighbor = (NERigidSphParticle*)rNeighbors[i].neighbor();
		rNeighbors[i].setDistance((particle.predPos - neighbor->position()).length());
	}
}

/*!
*/
float NESampleSphSolver::recomputeDensityWithUpdatedNeighbors(int pIndex)
{
	float density(0.0f);
    NESampleSphParticle& particle = m_Particles[pIndex];
	vector<Neighbor>& fNeighbors = m_LiquidParticleNeighbors[pIndex];
	for(int i = 0; i < fNeighbors.size(); i++)
	{
        NESampleSphParticle* neighbor = (NESampleSphParticle*)fNeighbors[i].neighbor();
		float dist = (particle.predPos - neighbor->predPos).length();
		density += m_ParticleMass * Kernel(dist);

	}

	vector<Neighbor>& rNeighbors = m_RigidParticleNeighbors[pIndex];
	for(int i = 0; i < rNeighbors.size(); i++)
	{
		NERigidSphParticle* neighbor = (NERigidSphParticle*)rNeighbors[i].neighbor();
		float dist = (particle.predPos - neighbor->position()).length();
		density += neighbor->mass(RestDensity()) * Kernel(dist);
	}

	// The particle itself contributes to the density.
	density += m_ParticleMass * m_WCubicSplineZero;
	// Correct the density, if it is lower than the rest density, in order to not g et negative pressures.
	if(density < RestDensity() && CorrectDensity())
		density = RestDensity();

	// Finally, set the density.
	return density;
}

void NESampleSphSolver::recomputeDistancesWithNeighbors(NERigidSphParticle& bParticle)
{
	vector<Neighbor>& neighbors = *bParticle.neighborVector(bParticle.index());

	for(int i = 0; i < neighbors.size(); i++)
	{
        NESampleSphParticle* p2 = (NESampleSphParticle*)neighbors[i].neighbor();
		neighbors[i].setDistance((bParticle.position() - p2->predPos).length());
	}
}

void NESampleSphSolver::computeAcceleration_V_G_EXT(const int particleVecId)
{
    NESampleSphParticle& particle1 = m_Particles[particleVecId];
	vector<Neighbor>& neighbors = m_LiquidParticleNeighbors[particleVecId];

	vec3f AccelCohesion(0, 0, 0);
	vec3f AccelViscosity(0, 0, 0);

	for(int i = 0; i < neighbors.size(); i++)
	{
        const NESampleSphParticle& particle2 = static_cast<const NESampleSphParticle&>(*neighbors[i].neighbor());

		NESphLiquidSolver* fluid2 = static_cast<NESphLiquidSolver*>(particle2.solver());

		float W = FLT_MAX;
		float WGrad = FLT_MAX;
		float WLapl = FLT_MAX;

		const float r = neighbors[i].distance();
		const vec3f dx = particle1.position() - particle2.position();
		const vec3f dv = particle1.velocity() - particle2.velocity();

		// Compute acceleration caused by surface tension
		AccelCohesion += surfaceTensionAccelContribution(r, mass(), dx, particle1.density(), &W, &WGrad, &WLapl, Cohesion());
		AccelViscosity += viscosityAccelContribution(r, fluid2->mass(), particle1.density(), particle2.density(), dv, dx, &WGrad, &WLapl, Viscosity());
	}

	//compute friction influence of rigid neighbors

	vector<Neighbor>& rigidNeighbor = m_RigidParticleNeighbors[particleVecId];
	for(int i = 0; i < (int) rigidNeighbor.size(); i++)
	{
		float W = FLT_MAX;
		float WGrad = FLT_MAX;
		float WLapl = FLT_MAX;

		const NERigidSphParticle* rigidParticle = static_cast<const NERigidSphParticle*>(rigidNeighbor[i].neighbor());

		// Compute the velocity of the rigid particle
		if(rigidParticle->solver()->physicalType() == NEPhysical::kSolid)
		{
			NESphRigid* rigid = static_cast<NESphRigid*>(rigidParticle->solver());
			vec3f angVel = rigid->AngularVelocity();
			vec3f relPos = rigidParticle->position() - rigid->Position();
			vec3f pVel = vec3f::crossProduct(angVel, relPos) + rigid->LinearVelocity();

			// Compute the velocity difference between two particles
			vec3f dv = particle1.velocity() - pVel;
			const float r = rigidNeighbor[i].distance();
			const vec3f dx = particle1.position() - rigidParticle->position();
			AccelCohesion += surfaceTensionAccelContribution(r, rigidParticle->mass(RestDensity()), dx, particle1.density(), &W, &WGrad, &WLapl, rigid->Adhesion());
			AccelViscosity += viscosityAccelContribution(r, rigidParticle->mass(RestDensity()), particle1.density(), particle1.density(), dv, dx, &WGrad, &WLapl, rigid->Viscosity());
		}
		else if(rigidParticle->solver()->physicalType() == NEPhysical::kDeformable)
		{

		}
	}

	particle1.a_aux = (AccelViscosity + AccelCohesion);
}

/*!
    Computes the acceleration for the particle at vector position particleVecId.
*/
void NESampleSphSolver::computeMultiAcceleration_NoPressure(const int particleVecId)
{
	// Get Particle & Neighbor vector
    NESampleSphParticle& particle1 = m_Particles[particleVecId];
	vector<Neighbor>& neighbors = m_LiquidParticleNeighbors[particleVecId];

	const double density1 = particle1.density() * mass();
	//const double p1d2 = density1 * density1;
	const double p1d2 = particle1.density() * particle1.density();
	const double presOverD2 = particle1.pressure() / p1d2;

	vec3f AccelViscosity(0, 0, 0), AccelCohesion(0, 0, 0), AccelInterface(0, 0, 0);
	qreal curvature(0.f);
	qreal curvatureDenom = Kernel(0.f) / particle1.density();

	for(int i = 0; i < (int) neighbors.size(); i++)
	{
		// Get Neighbor, this works only for single-phase fluids like this!!!
        const NESampleSphParticle& particle2 = static_cast<const NESampleSphParticle&>(*neighbors[i].neighbor());

		float W = FLT_MAX;
		float WGrad = FLT_MAX;
		float WLapl = FLT_MAX;

		const float r = neighbors[i].distance();
		const float rInv = 1.0f / r;
		const vec3f dx = particle1.position() - particle2.position();
		const vec3f dv = particle1.velocity() - particle2.velocity();
		WGrad = KernelGrad(r, rInv);
		W = Kernel(r);
		const vec3f kernelGrad = WGrad * dx;

		NESphLiquidSolver* fluid2 = static_cast<NESphLiquidSolver*>(particle2.solver());
		float density2 = particle2.density() * mass();
		float p2d2 = particle2.density() * particle2.density();

		// compute acceleration due to surface tension
		AccelCohesion += surfaceTensionAccelContribution(r, mass(), dx, density1, &W, &WGrad, &WLapl, Cohesion());

		// compute acceleration due to Viscosity
		float avgViscConst = 0.5f * (Viscosity() + fluid2->Viscosity());
		AccelViscosity += viscosityAccelContribution(r, fluid2->mass(), density1, density2, dv, dx, &WGrad, &WLapl, avgViscConst);

		//compute curvature
		if(useInterfaceTension())
		{
			curvatureDenom += -kernelGrad * (particle2.normal() - particle1.normal()) / particle2.density();
			curvature += W / particle2.density();
		}
	}
	//see Eq. 24 in Solenthaler & Pajarola 2008, colorValue() is here the normal Length
	if(useInterfaceTension())
	{
		curvature /= curvatureDenom;
		qreal normalLength = 1.f;//particle1.accel().x();
		AccelInterface = (curvature * InterfaceTension() * particle1.normal() * normalLength) / density1;
	}

	//compute influence of rigid newParticles
	vector<Neighbor>& rigidNeighbor = m_RigidParticleNeighbors[particleVecId];
	for(int i = 0; i < (int) rigidNeighbor.size(); i++)
	{
		float W = FLT_MAX;
		float WGrad = FLT_MAX;
		float WLapl = FLT_MAX;

		const NERigidSphParticle* rigidParticle = static_cast<const NERigidSphParticle*>(rigidNeighbor[i].neighbor());
		if(rigidParticle->solver()->physicalType() == NEPhysical::kSolid)
		{
			NESphRigid* rigid = static_cast<NESphRigid*>(rigidParticle->solver());
			// Compute the velocity of the rigid particle
			vec3f angVel = rigid->AngularVelocity();
			vec3f relPos = rigidParticle->position() - rigid->Position();
			vec3f pVel = vec3f::crossProduct(angVel, relPos) + rigid->LinearVelocity();

			vec3f dv = particle1.velocity() - pVel;

			const float r = rigidNeighbor[i].distance();
			const float rInv = 1.0 / r;
			const vec3f dx = particle1.position() - rigidParticle->position();
			WGrad = KernelGrad(r, rInv);
			const vec3f kernelGrad = WGrad * dx;
			float rigidParticleMass = rigidParticle->mass(RestDensity());

			AccelCohesion += surfaceTensionAccelContribution(r, rigidParticleMass, dx, particle1.density(), &W, &WGrad, &WLapl, rigid->Adhesion());
			AccelViscosity += viscosityAccelContribution(r, rigidParticleMass, density1, density1, dv, dx, &WGrad, &WLapl, rigid->Viscosity());
		}
		else if(rigidParticle->solver()->physicalType() == NEPhysical::kDeformable)
		{

		}
	}
	particle1.a_aux = (AccelViscosity + AccelCohesion + AccelInterface);
}

/*!
*/
void NESampleSphSolver::computeAcceleration_P(int pIndex)
{
	// Get Particle & Neighbor vector
    NESampleSphParticle& particle1 = m_Particles[pIndex];
	vector<Neighbor>& fNeighbors = m_LiquidParticleNeighbors[pIndex];

	const float p1d2 = particle1.density() * particle1.density();
	const float presOverD2 = particle1.pressure() / p1d2;

	vec3f AccelPressure(0, 0, 0);

	for(int i = 0; i < fNeighbors.size(); i++)
	{
		// Get Neighbor, this works only for single-phase fluids like this!!!
        const NESampleSphParticle& particle2 = static_cast<const NESampleSphParticle&>(*fNeighbors[i].neighbor());
		const float dist = fNeighbors[i].distance();
		const float distInv = 1.0f / dist;
		float p2d2 = particle2.density() * particle2.density();
		vec3f dx = particle1.predPos - particle2.predPos;
		vec3f kernelGrad = KernelGrad(dist, distInv) * dx;
		// compute acceleration due to pressure
		AccelPressure += (-m_ParticleMass) * kernelGrad * (particle2.pressure() / p2d2 + presOverD2);
	}

	//compute influence of rigid newParticles
	bool useBoundaryPressure = explicitBoundaryHandling();
	vector<Neighbor>& rigidNeighbor = m_RigidParticleNeighbors[pIndex];
	for(int i = 0; i < (int) rigidNeighbor.size(); i++)
	{
		const NERigidSphParticle* rigidParticle = static_cast<const NERigidSphParticle*>(rigidNeighbor[i].neighbor());

		const float dist = rigidNeighbor[i].distance();
		const float distInv = 1.0 / dist;
		const vec3f diffPos = particle1.position() - rigidParticle->position();
		const vec3f kernelGrad = KernelGrad(dist, distInv) * diffPos;
		if(useBoundaryPressure)
			AccelPressure += (-rigidParticle->mass(RestDensity())) * kernelGrad * (rigidParticle->pressure() / (rigidParticle->density() * rigidParticle->density()) + presOverD2);
		else
			AccelPressure += (-rigidParticle->mass(RestDensity())) * kernelGrad * (BoundaryCoefficient() * presOverD2);
	}

	//AccelPressure *= -m_ParticleMass;

	particle1.accel() = AccelPressure;

	// TODO: Add boundary particle pressure contribution
}

/*!
*/
void NESampleSphSolver::computeAcceleration_P_oldPosition(int pIndex)
{
	// Get Particle & Neighbor vector
    NESampleSphParticle& particle1 = m_Particles[pIndex];
	vector<Neighbor>& fNeighbors = m_LiquidParticleNeighbors[pIndex];

	const float p1d2 = particle1.density() * particle1.density();
	const float presOverD2 = particle1.pressure() / p1d2;

	vec3f AccelPressure(0, 0, 0);

	for(int i = 0; i < fNeighbors.size(); i++)
	{
		// Get Neighbor, this works only for single-phase fluids like this!!!
        const NESampleSphParticle& particle2 = static_cast<const NESampleSphParticle&>(*fNeighbors[i].neighbor());
		const float dist = fNeighbors[i].distance();
		const float distInv = 1.0f / dist;
		float p2d2 = particle2.density() * particle2.density();
		vec3f dx = particle1.position() - particle2.position();
		vec3f kernelGrad = KernelGrad(dist, distInv) * dx;
		// compute acceleration due to pressure
		AccelPressure += (-m_ParticleMass) * kernelGrad * (particle2.pressure() / p2d2 + presOverD2);
	}

	//compute influence of rigid newParticles
	bool useBoundaryPressure = explicitBoundaryHandling();
	vector<Neighbor>& rigidNeighbor = m_RigidParticleNeighbors[pIndex];
	for(int i = 0; i < (int) rigidNeighbor.size(); i++)
	{
		const NERigidSphParticle* rigidParticle = static_cast<const NERigidSphParticle*>(rigidNeighbor[i].neighbor());

		const float dist = rigidNeighbor[i].distance();
		const float distInv = 1.0 / dist;
		const vec3f diffPos = particle1.position() - rigidParticle->position();
		const vec3f kernelGrad = KernelGrad(dist, distInv) * diffPos;
		if(useBoundaryPressure)
			AccelPressure += (-rigidParticle->mass(RestDensity())) * kernelGrad * (rigidParticle->pressure() / (rigidParticle->density() * rigidParticle->density()) + presOverD2);
		else
			AccelPressure += (-rigidParticle->mass(RestDensity())) * kernelGrad * (BoundaryCoefficient() * presOverD2);
	}

	//AccelPressure *= -m_ParticleMass;

	particle1.accel() = AccelPressure;

	// TODO: Add boundary particle pressure contribution
}

void NESampleSphSolver::computeMultiAcceleration_Pressure(int pIndex)
{
	// Get Particle & Neighbor vector
    NESampleSphParticle& particle1 = m_Particles[pIndex];
	vector<Neighbor>& fNeighbors = m_LiquidParticleNeighbors[pIndex];

	const float p1d2 = particle1.density() * particle1.density();
	const float presOverD2 = particle1.pressure() / p1d2;

	vec3f AccelPressure(0, 0, 0);

	int ownNeighbors = 0;
	int otherNeighbors = 0;

	for(int i = 0; i < fNeighbors.size(); i++)
	{
		// Get Neighbor, this works only for single-phase fluids like this!!!
        const NESampleSphParticle& particle2 = static_cast<const NESampleSphParticle&>(*fNeighbors[i].neighbor());
		const float dist = fNeighbors[i].distance();
		const float distInv = 1.0 / dist;
		float p2d2 = particle2.density() * particle2.density();
		vec3f dx = particle1.predPos - particle2.predPos;
		vec3f kernelGrad = KernelGrad(dist, distInv) * dx;
		// compute acceleration due to pressure
		AccelPressure += -kernelGrad * (particle2.pressure() / p2d2 + presOverD2);

		if(((NESphLiquidSolver*)particle2.solver())->RestDensity() > this->RestDensity())
			otherNeighbors++;
		else
			ownNeighbors++;
	}
	bool extraForce = otherNeighbors > ownNeighbors;

	//compute influence of rigid newParticles
	bool useBoundaryPressure = explicitBoundaryHandling();
	vector<Neighbor>& rigidNeighbor = m_RigidParticleNeighbors[pIndex];
	for(int i = 0; i < (int) rigidNeighbor.size(); i++)
	{
		const NERigidSphParticle* rigidParticle = static_cast<const NERigidSphParticle*>(rigidNeighbor[i].neighbor());

		const float dist = rigidNeighbor[i].distance();
		const float distInv = 1.0 / dist;
		const vec3f diffPos = particle1.position() - rigidParticle->position();
		const vec3f kernelGrad = KernelGrad(dist, distInv) * diffPos;
		float rigidParticleMass = rigidParticle->mass(RestDensity());
		//if(useBoundaryPressure)
		//	AccelPressure += (-rigidParticle->mass(RestDensity())) * kernelGrad*(rigidParticle->pressure() / (rigidParticle->density() * rigidParticle->density()) + presOverD2);
		//else

		if(extraForce)
			AccelPressure += 2.0f * (-rigidParticleMass * inverseMass()) * kernelGrad * (BoundaryCoefficient() * presOverD2);
		else
			AccelPressure += (-rigidParticleMass * inverseMass()) * kernelGrad * (BoundaryCoefficient() * presOverD2);

	}
	AccelPressure *= inverseMass();
	particle1.setAccel(AccelPressure);
}
/*!
*/
bool NESampleSphSolver::explicitBoundaryHandling() const
{
	switch(m_PcisphModel)
	{
    case NESampleSphSolver::pciModelAkinci:
		return false;
    case NESampleSphSolver::pciModelIhmsen:
		return true;
    case NESampleSphSolver::pciModelSolenthaler:
		return false;
	default :
		return false;
	}

}

void NESampleSphSolver::deleteParticles()
{
	NEParticleFluid::deleteParticles(m_ParticleNEVector);
}

void NESampleSphSolver::deleteParticle(NEParticle* particle)
{
	NEParticleFluid::deleteParticle(particle, m_ParticleNEVector);
}

/*
Computes the required time step and sets it accordingly
Returns 0 if there is no shock otherwise it should return
a negative value which determines how many frames backwards to go in time.
*/
qreal NESampleSphSolver::updateTimeStep()
{
	int framesToGoBack(0);
	qreal newTimestep = computeOptimalTimestep(framesToGoBack);
	SetTimestep(newTimestep);
	return (qreal)framesToGoBack;
}


/*!
*/
qreal NESampleSphSolver::computeOptimalTimestep(int& framesToGoBack)
{
	qreal estimatedTS = Timestep();
	framesToGoBack = 0;
	// Initialize the old maximum density
	if(m_OldAveDensity < 0.0f)
		m_OldAveDensity = m_AveDensity;

	bool compressing = m_OldAveDensity < m_AveDensity;
	double maxAccel = sqrt(m_MaxAccel2);
	double maxVel = sqrt(m_MaxVel2);
	double requiredTimestep;

	if(maxAccel < FEps)
		requiredTimestep = Timestep();
	else
		requiredTimestep = 0.3 * sqrt(Radius() / (maxAccel));

	maxVel *= Timestep();

	if(maxVel < FEps)
		return estimatedTS;

	double HOverMaxVel = Radius() / maxVel;

	if(ShowStatistics())
	{
		NEOSD.add(9, NEOsdMsg("Req-Timestep:", QString::number(requiredTimestep)), this);
		NEOSD.add(10, NEOsdMsg("HOverMaxVel:", QString::number(HOverMaxVel)), this);
		//NEOSD.add(15, NEOsdMsg("FrameNr",QString::number(NESCENE.currentFrame()), osdMsgPos() ));
	}

//	double TimestepRatio = requiredTimestep/m_Timestep;

	//NEW
	qreal maxDensityRatio = m_MaxDensity / RestDensity();
	qreal aveDensityRato = m_AveDensity / RestDensity();
	//if(HOverMaxVel < 2.0f || (m_MaxDensity > 1400.0))
	if(HOverMaxVel < 2.0f || (maxDensityRatio > 1.4))
	{
		double oldTimestep = Timestep();
		// Introduced a hack here, since maxAccel doesn't always result in a sensible requiredTimestep
		// m_Timestep*0.5 is also added here to create a strict lowerbound
		estimatedTS = MIN(MIN(requiredTimestep, (Timestep() * (0.45 * HOverMaxVel))), Timestep() * 0.5);

		double m_NewDelta = (double)m_PrototypeDeltaWOTimestep * (1.0 / ((double)Timestep() * Timestep()));
		double newOverOldDelta = m_NewDelta / m_OldDelta;

		double simStepsToGoBack = log(newOverOldDelta) / log(1.025);

		// Compute the number of frames to seek back
		framesToGoBack = -ceil(simStepsToGoBack * Timestep() * NESCENE.RecordFrameRate());

		m_ShockOccured = true;
		m_ShockInstant = NESCENE.ElapsedTime();

		if(AdaptiveTimestep())
		{
			Log() << "PCISPH Shock occured with max density: " << m_MaxDensity << ", velratio: " << HOverMaxVel << /*", required dt ratio: "<< TimestepRatio <<*/ ", frames to go back: " << framesToGoBack;
			EndLog(NE::kLogWarning);
			Log() << " Old Timestep: " << oldTimestep << " New Timestep:" << Timestep();
			EndLog(NE::kLogInfo);
		}
		return estimatedTS;
	}

	//else if(TimestepRatio > 1.05f && m_MaxDensity < 1095.0f && m_AveDensity < 1010.0f && HOverMaxVel > 2.4f && m_EnergyKinetic > 1.0f && ((NESCENE.elapsedTime() >= m_ShockInstant) || m_FrameChangedByUser))
	else if(/*TimestepRatio > 1.05f &&*/ maxDensityRatio < 1.055f && aveDensityRato < 1.002f && HOverMaxVel > 2.4f && m_EnergyKinetic > 1.0f && ((NESCENE.ElapsedTime() >= m_ShockInstant) || m_FrameChangedByUser))
	{
		if(aveDensityRato < 1.002f && compressing);	// Started to compress
		else
			estimatedTS = Timestep() * 1.001956947;
	}
	//else if(TimestepRatio < 1.0f || m_MaxDensity > 1105.0f || m_AveDensity >= 1100.0f || HOverMaxVel <= 2.3f)
	else if(/*TimestepRatio < 1.0f ||*/ maxDensityRatio > 1.105f || aveDensityRato >= 1.010f || HOverMaxVel <= 2.3f)
	{
		if(aveDensityRato >= 1.010f && !compressing);	//	Started to decompress
		else
			estimatedTS = Timestep() * 0.998046875;
	}

	if(ShowStatistics())
	{
		NEOSD.add(15, NEOsdMsg("Timestep:", QString::number(Timestep())), this);
	}


	m_OldAveDensity = m_AveDensity;

	return estimatedTS;
}

uint NESampleSphSolver::saveFrameData(QString path, const int frameNumber)
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

uint NESampleSphSolver::loadFrameData(QString path, const int frameNumber)
{
	uint returnValue = NESphLiquidSolver::loadFrameData(path, frameNumber);

	//if(m_Particles.size() == 0)
	//	return returnValue;

	QString fileName;
	fileName.sprintf("/%06d.Timestep", frameNumber);

	// Open the frame in read-only mode
	QString filePath = path + fileName;
	QFile fin(filePath);
	if(!fin.open(QFile::ReadOnly))
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

void NESampleSphSolver::setRadius(qreal r)
{
	NEParticleFluid::SetRadius(r);
	SetTimestep(0.25f * (Radius() / totalExternalAcceleration().length()));
	computePrototypeDelta();
}

void NESampleSphSolver::setRestDensity(qreal density)
{
	NEParticleFluid::SetRestDensity(density);
	computePrototypeDelta();
}

void NESampleSphSolver::setIterations(int count)
{
	if(count < 1 || m_MaxNumIters == count)
		return;

	m_MaxNumIters = m_MinNumIters = count;
}

/*!
    This method implements the iterative pressure solve loop with adaptive time stepping
    and robust two-way coupling of rigid bodies as explained in Akinci et al. 2012
*/
void NESampleSphSolver::iterativePressureSolveMultiAkinci()
{
	int nrOfFluidParticles = (int) m_Particles.size();

	switch(m_IterativePart)
	{
    case NESampleSphSolver::kOne:
	{
		// Predict positions and velocities
		#			pragma omp for schedule(guided)
		for(int i = 0; i < nrOfFluidParticles; i++)
		{
            NESampleSphParticle& particle = m_Particles[i];
			// predict velocity and position
			vec3f predVel = particle.velocity() + ((particle.accel() + particle.a_aux) * Timestep());
			particle.predPos = particle.position() + (predVel * Timestep());
		}
        m_IterativePart = NESampleSphSolver::kTwo;
		break;
	}
    case NESampleSphSolver::kTwo:
	{
		#			pragma omp for schedule(guided)
		for(int i = 0; i < nrOfFluidParticles; i++)
		{
			// Check distances with the neighbors again
			recomputeDistancesWithNeighbors(i);
			// Predict density
			computeAdaptedDensity(&m_Particles[i], i);
			// Predict density variation
			float partDensityError = (m_Particles[i].density() * mass()) - RestDensity();
			// Update pressure according to the density errpr
			m_Particles[i].pressure() += m_NewDelta * partDensityError;
		}
        m_IterativePart = NESampleSphSolver::kThree;
		break;
	}
    case NESampleSphSolver::kThree:
	{
		// Compute acceleration caused by the pressure force
		#			pragma omp for schedule(guided)
		for(int i = 0; i < nrOfFluidParticles; i++)
		{
			computeMultiAcceleration_Pressure(i);
		}
        m_IterativePart = NESampleSphSolver::kOne;
		m_CurrentIter++;
		break;
	}
	}
}

uint NESampleSphSolver::memoryConsumption()
{
	uint totalConsumption = 0;
    totalConsumption += sizeof(NESampleSphParticle) * (int) m_Particles.size();
	return totalConsumption;
}

/*!
    Boundary collision method for FlFluidparticles with Particle sampled rigids.
    Arguments are :
    the fluidparticle,
    the position that should be corrected for the particle, e.g. predPos for PCISPH
    and a boolean if the velocity should be corrected (true) or not
*/
void NESampleSphSolver::boundaryCollision(NEPhysicalParticle* particle, vec3f& pos, bool correctVel)
{
	//Log()<<"Explicit boundary handling applied.";EndLog(NE::kLogInfo);
	uint itemIndex = particle->index();
	vector<Neighbor>& bNeighbor = m_RigidParticleNeighbors[itemIndex];

	if(bNeighbor.size() == 0)
		return;

	float totalWeight = 0.0f;
	vec3f tmpPos(0, 0, 0);
	bool colFlag = false;

	for(int i = 0; i < (int) bNeighbor.size(); i++)
	{
		const NERigidPciSphParticle& bp = static_cast<const NERigidPciSphParticle&>(*(bNeighbor[i].neighbor()));
		vec3f distv;
		float dist;

		if(!colFlag)
		{
			distv = pos - bp.position();
			dist = distv * distv;
		}
		else
		{
			distv = pos /*+ tmpPos/totalWeight*/ - bp.position();	// This is changed since it makes the process order dependent
			dist = distv * distv;
		}

		if(dist < m_RestDistance * m_RestDistance)
		{
			dist = sqrt(dist);
			if(dist < FEps)
				dist = FEps;
			float weight = (1.0f / (dist));

			float dot = (pos -  bp.position()) * bp.normal();
			vec3f normal = bp.normal();
			if(dot < 0.0f)
				normal = -normal;

			totalWeight += weight;
			tmpPos += weight * normal * ((m_RestDistance - dist) * 1.0f);
			colFlag = true;
		}
	}

	if(colFlag)
	{
		tmpPos /= totalWeight;
		pos += tmpPos;

		if(correctVel)
		{
			tmpPos.normalize();
			float normalPart = abs(tmpPos * particle->velocity());

			particle->velocity() -= (tmpPos * particle->velocity()) * tmpPos;
			//tangential friction
			float tangentFriction =  normalPart / abs(particle->velocity().x()) + normalPart / abs(particle->velocity().y()) + normalPart / abs(particle->velocity().z());
			tangentFriction = max(0.f, 1.f - tangentFriction * 0.6f);
			//if(tangentFriction > 0.0f)
			{
				particle->velocity() *= tangentFriction;
			}
		}
	}
}


//NESampleSphFactory::NESampleSphFactory() {  }

NESampleSphFactory::NESampleSphFactory(NEManager* m) : NENodeFactory(m) {}

NESampleSphFactory::~NESampleSphFactory() {}

QString NESampleSphFactory::nodeName()
{
    return "Sample SPH solver";
}

uint NESampleSphFactory::nodeType()
{
	return NE::kObjectParticleFluid;
}

QString NESampleSphFactory::nodeInherits()
{
    return "NESphLiquidSolver";
}

QString NESampleSphFactory::objectName()
{
    return "SampleSphSolver";
}

QString NESampleSphFactory::nodeVersion()
{
	return "0.1";
}

QString NESampleSphFactory::nodeVendor()
{
	return "Neutrino Dynamics";
}

QString NESampleSphFactory::nodeHelp()
{
    return "Sample PCISPH Solver for Neutrino";
}

NEObject* NESampleSphFactory::createInstance()
{
    return new NESampleSphSolver(this);
}
