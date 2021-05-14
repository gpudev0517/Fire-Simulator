#pragma once

#include "Physics/Spatial/Physical/NERigid.h"
#include "Physics/Spatial/Physical/NEFluid.h"
#include "Physics/Spatial/Physical/Fluid/GridFluid/NEMarkerParticle.h"
#include "Utility/Grid/NEUniformGrid.h"

struct CellIndex3D;
class NEIndexedTriMesh;
class NEUniformGrid;

class ExportDensityData
{
private:
	int resolutionX, resolutionY, resolutionZ;
	int resolutionX_old, resolutionY_old, resolutionZ_old;
	float*** data;
	float*** dataSmoothed;
	float sigma;
	int range;
	float absorptionCoefficient, scatteringCoefficient;


public:
	vec3f m_Boundary_p1, m_Boundary_p2;

	//! Class ExportDensityData constructor
	ExportDensityData(void)
	{
		resolutionX = 0, resolutionY = 0, resolutionZ = 0;
		resolutionX_old = 0, resolutionY_old = 0, resolutionZ_old = 0;
		m_Boundary_p1 = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
		m_Boundary_p2 = vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		data = NULL;
		dataSmoothed = NULL;
		sigma = 1.f;
		range = 1;
		absorptionCoefficient = 1.f;
		scatteringCoefficient = 4.f;
	}

	void reallocate(void)
	{
		// Delete old data
		if(data != NULL)
		{
			for(int i = 0; i < resolutionX_old; ++i)
			{
				if(data[i] != NULL)
				{
					for(int j = 0; j < resolutionY_old; ++j)
					{
						if(data[i][j] != NULL)
						{
							delete[] data[i][j];
							delete[] dataSmoothed[i][j];
						}
					}
					delete[] data[i];
					delete[] dataSmoothed[i];
				}
			}
			delete[] data;
			delete[] dataSmoothed;
		}
		resolutionX_old = resolutionX;
		resolutionY_old = resolutionY;
		resolutionZ_old = resolutionZ;

		// Allocate new
		data = new float**[resolutionX];
		dataSmoothed = new float**[resolutionX];
		for(int i = 0; i < resolutionX; ++i)
		{
			data[i] = new float*[resolutionY];
			dataSmoothed[i] = new float*[resolutionY];
			for(int j = 0; j < resolutionY; ++j)
			{
				data[i][j] = new float[resolutionZ];
				dataSmoothed[i][j] = new float[resolutionZ];
				for(int k = 0; k < resolutionZ; ++k)
				{
					data[i][j][k] = 0.f;
					dataSmoothed[i][j][k] = 0.f;
				}
			}
		}
	}

	void clear(void)
	{		
		// Delete old data
		if(data != NULL)
		{
			for(int i = 0; i < resolutionX; ++i)
			{
				if(data[i] != NULL)
				{
					for(int j = 0; j < resolutionY; ++j)
					{
						if(data[i][j] != NULL)
						{
							delete[] data[i][j];
							delete[] dataSmoothed[i][j];
						}
					}
					delete[] data[i];
					delete[] dataSmoothed[i];
				}
			}
			delete[] data;
			delete[] dataSmoothed;
		}
		data = NULL;
		dataSmoothed = NULL;

		resolutionX = 0, resolutionY = 0, resolutionZ = 0;
		resolutionX_old = 0, resolutionY_old = 0, resolutionZ_old = 0;
		m_Boundary_p1 = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
		m_Boundary_p2 = vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		return;
	}

	void clearData(void)
	{
		for(int i = 0; i < resolutionX; ++i)
		{
			for(int j = 0; j < resolutionY; ++j)
			{
				for(int k = 0; k < resolutionZ; ++k)
				{
					data[i][j][k] = 0.f;
					dataSmoothed[i][j][k] = 0.f;
				}
			}
		}
	}

	void smoothData(void)
	{
		// sqrt(2*PI) = 2.506628274631000502415765284811045253006986740609938316629923...
		float weighting = 0.f, sum = 0.f, vectorLength;
		float factor0 = 1.f/((2.5066282746310005024f*sigma)*(2.5066282746310005024f*sigma)*(2.5066282746310005024f*sigma));
		for(int i = 0; i < resolutionX; ++i)
		{
			for(int j = 0; j < resolutionY; ++j)
			{
				for(int k = 0; k < resolutionZ; ++k)
				{
					if(range <= 0)
					{
						dataSmoothed[i][j][k] = data[i][j][k];
						continue;
					}

					sum = 0.f;
					// For every cell loop over the neighbouring cells
					for(int x = -range; x <= range; ++x)
					{
						if(i + x >= 0 && i + x < resolutionX)
						{
							for(int y = -range; y <= range; ++y)
							{
								if(j + y >= 0 && j + y < resolutionY)
								{
									for(int z = -range; z <= range; ++z)
									{
										if(k + z >= 0 && k + z < resolutionZ)
										{
											vectorLength = sqrt((float)(x*x + y*y + z*z))/range;
											if(vectorLength > 1.f)
												continue;
											weighting = factor0*exp(-vectorLength*vectorLength/(2.f*sigma*sigma));
											dataSmoothed[i][j][k] += weighting*data[i + x][j + y][k + z];
											sum += weighting;
										}
									}
								}
							}
						}
					}
					// Save the smoothed value
					if(sum > 0.f)
					{
						dataSmoothed[i][j][k] /= sum;
						if(dataSmoothed[i][j][k] < 0.00000001)
							dataSmoothed[i][j][k] = 0.f;
					}
					else
						dataSmoothed[i][j][k] = data[i][j][k];
				}
			}
		}
	}

	inline void setData(int i, int j, int k, float d) { data[i][j][k] = d; }
	inline float getData(int i, int j, int k) { return data[i][j][k]; }
	inline float getDataSmoothed(int i, int j, int k) { return dataSmoothed[i][j][k]; }
	
	inline void setResolutionX(int x) {resolutionX_old = resolutionX; resolutionX = x; }
	inline int getResolutionX(void) const { return resolutionX; }
	inline void setResolutionY(int y) { resolutionY_old = resolutionY; resolutionY = y; }
	inline int getResolutionY(void) const { return resolutionY; }
	inline void setResolutionZ(int z) { resolutionZ_old = resolutionZ; resolutionZ = z; }
	inline int getResolutionZ(void) const { return resolutionZ; }

	inline void setAbsorptionCoefficient(float absorp) { absorptionCoefficient = absorp; }
	inline float getAbsorptionCoefficient(void) const { return absorptionCoefficient; }
	inline void setScatteringCoefficient(float sc) { scatteringCoefficient = sc; }
	inline float getScatteringCoefficient(void) const { return scatteringCoefficient; }

	inline void setSigma(float sig) { sigma = sig; }
	inline float getSigma(void) const { return sigma; }
	inline void setFilterRange(int r) { range = r; }
	inline int getFilterRange(void) const { return range; }

};

class NEGridFluid : public NEFluid
{
	Q_OBJECT

	Q_PROPERTY(double cellSize READ cellSize WRITE setCellSize)	
	Q_PROPERTY(GridBoundaryMethod boundaryHandling READ boundaryMethod WRITE setBoundaryMethod)
	Q_PROPERTY(double boundarySpacing READ boundaryParticleSpacing)

	// TODO: copied from NEParticleFluid, maybe move it to NEFluid?
	Q_PROPERTY(PropertyGroupStart Coloring READ groupStart)
	Q_PROPERTY(NEParticleColoring coloring READ coloring WRITE setColoring)
	Q_PROPERTY(bool automaticRange READ automaticRange WRITE setAutomaticRange)
	Q_PROPERTY(double coloringExponent READ coloringExponent WRITE setColoringExponent)
	Q_PROPERTY(double minColoringRange READ minColoringRange WRITE setMinColoringRange)
	Q_PROPERTY(double maxColoringRange READ maxColoringRange WRITE setMaxColoringRange)
	Q_PROPERTY(QColor minRangeColor READ minRangeColor WRITE setMinRangeColor)
	Q_PROPERTY(QColor maxRangeColor READ maxRangeColor WRITE setMaxRangeColor)
	Q_PROPERTY(PropertyGroupEnd Coloring_End READ groupEnd)
		

	friend class NERenderer;
	
	Q_ENUMS(GridBoundaryMethod)
	Q_ENUMS(NEParticleDrawing);
	Q_ENUMS(NEParticleColoring)

public:
	//different boundary handling methods
	enum GridBoundaryMethod
	{
		kVelocityZero = 1,
		kImpulse = 2,
		kMeshFraction =3, // implements [BBB07] with the fractional amount of fluid volume in a cell, fraction is computed based on the rigid mesh
		kParticleFraction =4
	};

	enum NEParticleColoring
	{
		Default = 0,
		VelocityBased,
		PressureBased,
		SignBased
	};

    NEFEXP NEGridFluid(NENodeFactory *factory);
	NEFEXP virtual ~NEGridFluid();

	NEFEXP virtual uint init(uint initMode = NE::kInitOnCreate);
	
	virtual uint updatePredict() = 0;
	virtual uint updateCorrect() = 0;

	//! Provides the data needed for export
	virtual ExportDensityData* exportDensityData(void)
	{
		ExportDensityData* pt = NULL;
		return pt; 
	}

	//! For accessing the particles by other instances/plugins
	const NEVectorMetaData* particlesVector() const { return &m_MarkerParticles; }
	//! Resize the meta data std::vector of marker particles
	void resizeParticlesVector(uint nItems, NEParticle* firstParticle );
	//! Returns the radius of a marker particle
	double particleRadius() const { return m_MarkerParticleRadius; }

	virtual int markersPerDimension() = 0;
	//! Compute statistics of the solver and prints it to the OSD
	virtual NEFEXP void computeStatistics();

	//! Removes the respective solid from the grid
	virtual void removeSolid(NERigid& rigid)= 0;
	//! Sets the cell of position occupied by the particles to solid or inserts new solid cells
	virtual void insertSolid(const NEVectorMetaData& solidParticles)= 0;
	//! Provides a list of moving rigids for inserting into the grid cells
	virtual void insertSolids(QList<NERigid*>& rigids) = 0;
	//! For adding fluid volume to the solver, for example by a source that is connected to the fluid.
	virtual void addInflow(const std::vector<NEParticle>& particles, const vec3f& vel, bool permanentInflow=true) = 0;
	//! For rebuilding auxiliary data structures when the simulation is restarted
	virtual void rebuildDataStructures() = 0;
	//!
	NEFEXP virtual void colorizeParticles() = 0;

	virtual const NEUniformGrid& grid() const = 0;
	
	virtual bool updateTimestep(float maxSourceVel) = 0;
	//! Returns the memory consumption of the particles
	virtual uint memoryConsumption() { return -1; };

public slots:
	//void connectedExtentChanged();s
	//! Returns the boundary method that is currently employed
	GridBoundaryMethod boundaryMethod() const {return m_BoundaryMethod;}
	void setBoundaryMethod(GridBoundaryMethod val);

	virtual float cellSize() const { return grid().cellSize(); }
	virtual void setCellSize(float cellSize) = 0;

	double boundaryParticleSpacing() {return grid().cellSize() * 0.5;}
	// TODO: copied from NEParticleFluid, maybe move it to NEFlui
	//! Coloring scheme for the particle fluid
	NEParticleColoring coloring() const { return m_Coloring; }
	void setColoring(NEParticleColoring val);
	//! Coloring range method for the particle fluid
	bool automaticRange() const;
	void setAutomaticRange(bool val);
	float coloringExponent() const { return m_ColoringExponent; }
	void setColoringExponent(float val);
	float minColoringRange() const { return m_MinColoringRange; }
	void setMinColoringRange(float val);
	float maxColoringRange() const { return m_MaxColoringRange; }
	void setMaxColoringRange(float val);
	const QColor& minRangeColor() const { return m_MinRangeColor; }
	void setMinRangeColor(const QColor& val);
	const QColor& maxRangeColor() const { return m_MaxRangeColor; }
	void setMaxRangeColor(const QColor& val);

signals:
	void cellSizeChanged(double);
	void boundarySpacingChanged(double);

protected:

	NEFEXP bool markParticleForDeletion(NEParticle* p);
	// This is an intelligent reference to the underlying marker particles
	NEVectorMetaData m_MarkerParticles;
	//!
	double m_MarkerParticleRadius;
	//! The current boundary method 
	GridBoundaryMethod m_BoundaryMethod;

	// TODO: copied from NEParticleFluid, maybe move it to NEFluid?
	NEParticleColoring m_Coloring;
	bool m_AutomaticRange;
	float m_MinColoringRange;
	QColor m_MinRangeColor;
	float m_MaxColoringRange;
	QColor m_MaxRangeColor;
	float m_ColoringExponent;
};

Q_DECLARE_INTERFACE(NEGridFluid, "NEGridFluid")
