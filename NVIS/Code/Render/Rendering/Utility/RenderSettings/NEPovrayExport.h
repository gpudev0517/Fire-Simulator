#include "Utility/Singleton.h"

class NECamera;
class NELight;
//class NESphRigid;
#if NE_DEFORMABLE
class NESphDeformable;
#endif
class NEMarchingCubes;
class NEParticleFluidSolver;
class NEIndexedTriMesh;
class NEMeshPostProcessor;
class NEMaterial;
class NESpatial;

class  NEPovRayExport : public Singleton<NEPovRayExport>
{
public:
	QString exportCamera(const NECamera* cam, const QString path);
	QString exportLight(const NELight* light, const QString path);
    QString exportRigid(bool firstInSeq, const NESpatial* rigid, const QString path);

#if NE_DEFORMABLE
	QString exportDeformable(const NESphDeformable* deformable, const QString path);
#endif

    QString exportParticleFluid(const NEParticleFluidSolver* fluidSolver, const QString path);
	QString exportFluidSurface(const NEMarchingCubes* mc, const QString path);
	QString exportPostprocessedSurface(const NEMeshPostProcessor* pp, const QString path);
	void recordMesh(const NEIndexedTriMesh* triMesh, QTextStream& ostr, uint mesh, QString& name, QColor& color, bool isRigid);

private:
	QString exportAreaLight(const NELight* light, const QString path);	
	QString exportSpotLight(const NELight* light, const QString path);
	QString exportDirectionalLight(const NELight* light, const QString path);
	QString exportPointLight(const NELight* light, const QString path);
	QString exportImageBasedLight(const NELight* light, const QString path);

	//! Material export related functions
	void exportMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
	void exportClearWaterMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
	void exportOceanMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
//	void exportGlassMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
	void exportPhongMaterial(NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path="") const;
	QString stateFromBool(const bool val) const{if(val) return "on"; else return "off";}
};

#define NEPOVEXP NEPovRayExport::Instance()
