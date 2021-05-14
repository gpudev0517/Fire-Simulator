#pragma once
//#include "Core/NESpatial.h"
#include "Utility/Singleton.h"

class NESpatial;
class NECamera;
class NELight;
class NESphRigid;
#if NE_DEFORMABLE
class NESphDeformable;
#endif
class NEMarchingCubes;
class NEParticleFluidSolver;
class NEIndexedTriMesh;
#if NE_POST
class NEMeshPostProcessor;
#endif
class NEMaterial;
class NEVolumeMaterial;
class NEVolumeNode;
class NEMeasurementField;
#ifdef NE_VTK
class vtkPolyData;
#endif

class NEVTKExport : public Singleton<NEVTKExport>
{
public:
        QString exportCamera( const NECamera* cam, const QString path );
        QString exportLight( const NELight* light, const QString path );
        QString exportParticleFluid( NEParticleFluidSolver *fluidSolver, const QString path );
        QString exportFluidSurface( const NEMarchingCubes* mc, const QString path );
        QString exportRigid( bool firstInSeq, const NESphRigid* rigid, const QString path );

#if NE_DEFORMABLE
        QString exportDeformable( const NESphDeformable* deformable, const QString path );
#endif

#if NE_POST
        QString exportPostprocessedSurface( const NEMeshPostProcessor* pp, const QString path );
#endif
#if NE_VOLUME
        QString exportVolume(const NEVolumeNode *vol, const QString path );
#endif
#if NE_MEASURE
        QString exportMeasurement( const NEMeasurementField *field,const QString path );
#endif
	
private:

	//! Fluid particle / foam particle export related functions
	mutable QProcess* m_pConvertProcess;
	mutable QString m_ExternalAsciiFile;

	QString externalAsciiFile() const { return m_ExternalAsciiFile; }
	void setExternalAsciiFile(QString val) const { m_ExternalAsciiFile = val; }
    void deleteFile( const NEParticleFluidSolver* fluidSolver, int exitCode, QProcess::ExitStatus exitStatus );
    void writeVolumeCube( NEParticleFluidSolver *fluidSolver, const QString cubeName, QTextStream& ostr );
    void recordMesh( const NESpatial* spatial, const NEIndexedTriMesh* triMesh, QFile& ofile, uint mesh, QString& name, QColor& color, bool isRigid, bool exportTextures, bool exportVelocities, bool exportSplashes);
#ifdef NE_VTK
    vtkPolyData* getVTKPolyData(const NEIndexedTriMesh* triMesh) const;
#endif
    QString exportGeneralParticleFluid(const NEParticleFluidSolver* fluidSolver, const QString path);

    //! Material export related functions
    void exportMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
    void exportClearWaterMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
    void exportOceanMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
    void exportGlassMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
    void exportVolumeMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
    void exportPhongMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
    QString stateFromBool(const bool val) const{if(val) return "on"; else return "off";}
};

#define NEVTKEXP NEVTKExport::Instance()
