#include "Base/NEBase.h"

#include "NEPovrayExport.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/NELight.h"
//#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"

#if NE_DEFORMABLE
#include "Core/Physics/Spatial/Physical/Deformable/ParticleDeformable/NESphDeformable.h"
#endif

#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"

#include "PostProcessor/NEMarchingCubes.h"
#include "PostProcessor/NEMeshPostProcessor.h"

#include "Base/NESceneManager.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/Light/NEAreaLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NEImageBasedLight.h"

#include "Rendering/Resource/Material/NEClearWaterMaterial.h"
//#include "Resource/Material/NEGlassMaterial.h"
#include "Rendering/Resource/Material/NEOceanMaterial.h"
#include "Rendering/Resource/Material/NEPBRMaterial.h"
#include "Rendering/Resource/Material/NEPhongMaterial.h"


QString NEPovRayExport::exportCamera( const NECamera* cam, const QString path )
{
    return "";
}

QString NEPovRayExport::exportRigid( bool firstInSeq, const NESpatial* rigid, const QString path )
{
    QString frameNumber;
    frameNumber.sprintf("%06d", NESCENE.CurrentFrame());

    QString name = rigid->objectName();
    QColor color1 = rigid->Color();

    QString fileName = name + ".inc";

    if(firstInSeq)
    {
        QFile fout(path + fileName);
        if(!fout.open(QFile::WriteOnly | QFile::Truncate))
            return "";

        QTextStream out(&fout);
        out.setCodec("UTF-8");

        recordMesh(&rigid->triangleMesh(), out, 1, name, color1, true);
    }

    return fileName;
}

#if NE_DEFORMABLE
QString NEPovRayExport::exportDeformable(const NESphDeformable* deformable, const QString path )
{
    QString frameNumber;
    frameNumber.sprintf("%06d", NESCENE.CurrentFrame());

    QString name = deformable->objectName();
    QColor color1 = deformable->Color();

    QString fileName = frameNumber +"_" + name + ".inc";

    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
        return "";

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    recordMesh(((NEParticleDeformable*)deformable)->triangleMesh(), out, 1, name, color1, true);


    return fileName;
}
#endif

QString NEPovRayExport::exportParticleFluid( const NEParticleFluidSolver* fluidSolver, const QString path )
{
    QString fileName = fluidSolver->objectName() + ".inc";
    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
        return "";
    QTextStream out(&fout);
    out.setCodec("UTF-8");

    out <<"#declare "<<fluidSolver->objectName()<<"_material=\n";
    out <<"material\n";
    out <<"{\n";
    out <<"	texture\n";
    out <<"	{\n";
    out <<"		pigment\n";
    out <<"		{\n";
    out <<"			color rgbt<1,1,1,0.98>\n";
    out <<"		}\n";
    out <<"		finish\n";
    out <<"		{\n";
    out <<"			ambient 0.5\n";
    out <<"			diffuse 0.0\n";
    out <<"			reflection\n";
    out <<"			{\n";
    out <<"				0.0 1.0\n";
    out <<"				fresnel on\n";
    out <<"			}\n";
    out <<"			specular 0.05\n";
    out <<"		}\n";
    out <<"	}\n";
    out <<"	interior\n";
    out <<"	{\n";
    out <<"		ior 1.1\n";
    //out <<"		caustics 0.05\n";
    out <<"	}\n";
    out <<"}\n\n";

    const NEVectorMetaData& particles = *(fluidSolver->particlesVector());

    switch(fluidSolver->ExportMethod())
    {
    case NEParticleFluidSolver::Isolated:
    {
        for_all(NEFluidParticle, particle, particles)
        {
            if(particle->nrNeighborsFromType() < (uint) fluidSolver->MinNeighbors())
            {
                switch(fluidSolver->ExportMaterial())
                {
                case NEParticleFluidSolver::UniqueMaterial:
                    out<<"sphere{<"<<particle->position().x()<<", "<<particle->position().y()<<", "<<particle->position().z()<<">,"<<fluidSolver->volumeRadius()<<" material {"<<fluidSolver->objectName()<<"_material} photons {target reflection on refraction on}}"<<"\n";
                    break;
                case NEParticleFluidSolver::TypeBasedMaterial:
                    out<<"sphere{<"<<particle->position().x()<<", "<<particle->position().y()<<", "<<particle->position().z()<<">,"<<fluidSolver->volumeRadius()<<" material{texture{pigment{color rgb<" <<particle->color().red/255.0<<","<<particle->color().green/255.0<<","<<particle->color().blue/255.0<<">}}} photons {target reflection on refraction on}}"<<"\n";
                    break;
                default:
                    break;
                }
            }
        }
    }
        break;
    case NEParticleFluidSolver::All:
    {
        for_all(NEFluidParticle, particle, particles)
        {
            switch(fluidSolver->ExportMaterial())
            {
            case NEParticleFluidSolver::UniqueMaterial:
                out<<"sphere{<"<<particle->position().x()<<", "<<particle->position().y()<<", "<<particle->position().z()<<">,"<<fluidSolver->volumeRadius()<<" material {"<<fluidSolver->objectName()<<"_material} photons {target reflection on refraction on}}"<<"\n";
                break;
            case NEParticleFluidSolver::TypeBasedMaterial:
                out<<"sphere{<"<<particle->position().x()<<", "<<particle->position().y()<<", "<<particle->position().z()<<">,"<<fluidSolver->volumeRadius()<<" material{texture{pigment{color rgb<" <<particle->color().red/255.0<<","<<particle->color().green/255.0<<","<<particle->color().blue/255.0<<">}}} photons {target reflection on refraction on}}"<<"\n";
                break;
            default:
                break;
            }
        }
    }
        break;
    }
    return fileName;
}



QString NEPovRayExport::exportPostprocessedSurface( const NEMeshPostProcessor* pp, const QString path )
{
    QString name = pp->objectName();
    QColor color1 = pp->Color();

    QString fileName = name + ".inc";

    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
        return "";

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    recordMesh(pp->outputMesh(), out, 1, name,color1, false);

    return fileName;
}

void NEPovRayExport::recordMesh( const NEIndexedTriMesh* triMesh, QTextStream& ostr, uint mesh, QString& name, QColor& color, bool isRigid )
{
    if (triMesh->vertices().size() == 0)
        return;

    NEMaterial* mat = NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh *>( triMesh ) );
    exportMaterial(mat,color,ostr,name);

    ostr <<"#declare mesh_" <<name <<"=mesh2{\n";

    // Write out vertices
    ostr <<"  vertex_vectors {\n" <<triMesh->numVertices();
    for (uint vId = 0; vId < triMesh->numVertices(); vId++)
    {
        const vec3f& v = triMesh->vertices()[vId].position;
        if (vId % 3 == 0)
            ostr << "\n";
        ostr <<",<" <<v.x() <<"," <<v.y() <<"," <<v.z() <<">";
    }
    ostr <<"\n  }\n";

    // Write out normals
    ostr <<"normal_vectors {\n" <<triMesh->numVertices();
    for (uint nId = 0; nId < triMesh->numVertices(); nId++)
    {
        const vec3f& n = triMesh->vertices()[nId].normal;
        if (nId % 3 == 0)
            ostr << "\n";
        ostr <<",<" <<n.x() <<"," <<n.y() <<"," <<n.z() <<">";
    }
    ostr <<"\n  }\n";

    // Write out face indices
    ostr <<"face_indices {\n" <<triMesh->numTriangles();
    for (uint tId = 0; tId < triMesh->numTriangles(); tId++)
    {
        const NETriangleIndexed& t = triMesh->triangles()[tId];
        if (tId % 3 == 0)
            ostr << "\n";
        ostr <<",<" <<t.p1 <<"," <<t.p2 <<"," <<t.p3 <<">";
    }
    ostr <<"}\n  }\n";
    ostr << "\n";

    if(isRigid == false)
    {
        ostr <<"object {mesh_" <<name <<" material {"<<name<<"_material}";
        ostr <<"photons {target reflection on refraction on}";
        ostr <<"}\n";
    }
}

QString NEPovRayExport::exportLight( const NELight* light, const QString path )
{
    if( light->factory().objectName() =="AreaLight" )
        return exportAreaLight(light, path);
    else if( light->factory().objectName() =="PointLight" )
        return exportPointLight(light, path);
    else if( light->factory().objectName() =="SpotLight" )
        return exportSpotLight(light, path);
    else if( light->factory().objectName() =="DirectionalLight" )
        return exportDirectionalLight(light, path);
    else if( light->factory().objectName() =="IBLight" )
        return exportImageBasedLight(light, path);
    else
        return exportDirectionalLight(light, path);
}

QString NEPovRayExport::exportAreaLight( const NELight* light, const QString path )
{
    QString fileName = light->objectName() + ".inc";
    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
    {
        Log() << "Cannot create light file, for path::" << path <<fileName; EndLog(NE::kLogError);
        return "";
    }

    const NEAreaLight* al = static_cast<const NEAreaLight*> (light);

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    out << "light_source\n";
    out << "{\n";
    out << "	<" << al->Position().x() <<", " << al->Position().y() <<", " << al->Position().z() <<">\n";
    out << "	color rgb <" <<  al->Color().redF() <<", " << al->Color().greenF() <<", " << al->Color().blueF() <<">\n";
    out << "	area_light <" <<  al->Width() <<", 0, 0>, <0, 0, "<< al->Height() <<">, " << al->Samples1()<<", "<< al->Samples2();
    if( al->PovrayCircular())
        out << " circular\n";
    out << "	adaptive "<<  al->PovrayAdaptive() <<"\n";
    if( al->PovrayJitter())
        out << "	jitter\n";

    out << "	looks_like\n";
    out << "	{\n";
    out << "		sphere\n";
    out << "	    {\n";
    out << "			0, 6\n";
    out << "			hollow\n";
    out << "			pigment\n";
    out << "			{\n";
    out << "				rgbt 1\n";
    out << "			}\n";
    out << "			finish\n";
    out << "			{\n";
    out << "				diffuse 0 ambient 0\n";
    out << "			}\n";
    out << "		}\n";
    out << "	}\n";

    out << "	photons\n";
    out << "	{\n";
    out << "		reflection on\n";
    out << "		refraction on\n";
    out << "	}\n";

    out << "}\n";

    return fileName;

}

QString NEPovRayExport::exportSpotLight( const NELight* light, const QString path )
{
    const NESpotLight* sl = static_cast<const NESpotLight*> (light);

    QString fileName = sl->objectName() + ".inc";
    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
    {
        Log() << "Cannot create light file, for path::" << path <<fileName; EndLog(NE::kLogError);
        return "";
    }

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    out << "light_source\n";
    out << "{\n";
    out << "	<" << sl->Position().x() <<", " <<sl->Position().y() <<", " <<sl->Position().z() <<">\n";
    out << "	color rgb <" << sl->Color().redF() <<", " <<sl->Color().greenF() <<", " <<sl->Color().blueF() <<">\n";
    out << "	adaptive "<< sl->PovrayAdaptive() <<"\n";
    if(sl->PovrayJitter())
        out << "	jitter\n";
    out << "	spotlight\n";
    out << "	radius " << sl->PovrayRadius() <<"\n";
    out << "	falloff " << sl->Cutoff() <<"\n";
    out << "	point_at <" << sl->direction().x() <<", " <<sl->direction().y() <<", " <<sl->direction().z() <<">\n";

    out << "	photons\n";
    out << "	{\n";
    out << "		reflection on\n";
    out << "		refraction on\n";
    out << "	}\n";

    out << "}\n";

    return fileName;

}

QString NEPovRayExport::exportDirectionalLight( const NELight* light, const QString path )
{
    QString fileName = light->objectName() + ".inc";
    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
    {
        Log() << "Cannot create light file, for path::" << path <<fileName; EndLog(NE::kLogError);
        return "";
    }

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    // Since there is no directional light in Povray, it will be mimicked by an area light

    float y;
    if(light->Position().y() < 3)
        y = 3;
    else
        y = light->Position().y();
    out << "light_source\n";
    out << "{\n";
    out << "	<" << light->Position().x() <<", " << y <<", " <<light->Position().z() <<">\n";
    out << "	color rgb <" << light->Color().redF() <<", " <<light->Color().greenF() <<", " <<light->Color().blueF() <<">\n";
    out << "	area_light < 20, 0, 0>, <0, 0, 20>, 10, 10\n";
    out << "	adaptive 2\n";
    out << "	jitter\n";

    out << "	photons\n";
    out << "	{\n";
    out << "		reflection on\n";
    out << "		refraction on\n";
    out << "	}\n";

    out << "}\n";

    return fileName;
}

QString NEPovRayExport::exportPointLight( const NELight* light, const QString path )
{
    QString fileName = light->objectName() + ".inc";
    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
    {
        Log() << "Cannot create light file, for path::" << path <<fileName; EndLog(NE::kLogError);
        return "";
    }

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    out << "light_source\n";
    out << "{\n";
    out << "	<" << light->Position().x() <<", " <<light->Position().y() <<", " <<light->Position().z() <<">\n";
    out << "	color rgb <" << light->Color().redF() <<", " <<light->Color().greenF() <<", " <<light->Color().blueF() <<">\n";

    out << "	photons\n";
    out << "	{\n";
    out << "		reflection on\n";
    out << "		refraction on\n";
    out << "	}\n";

    out << "}\n";

    return fileName;

}

QString NEPovRayExport::exportImageBasedLight( const NELight* light, const QString path )
{
    const NEImageBasedLight* il = static_cast<const NEImageBasedLight*> (light);

    QString fileName = light->objectName() + ".inc";
    QFile fout(path + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
    {
        Log() << "Cannot create light file, for path::" << path <<fileName; EndLog(NE::kLogError);
        return "";
    }

    QString img = il->ImgName();
    vec3f v = il->RotDirection();
    v *= il->Rotate();

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    out << "sphere\n";
    out << "{\n";

    out << "	<" << il->Position().x() <<", " <<il->Position().y() <<", " <<il->Position().z() <<">, "<<il->SphSize()<<"\n";
    out << "	texture\n";
    out << "	{\n";
    out << "		pigment\n";
    out << "		{\n";
    out << "			image_map\n";
    out << "			{\n";
    out << "				hdr \""<<img<<"\"\n";
    out << "				map_type "<<il->MapType()<<"\n";
    out << "				interpolate "<<il->Interpolate()<<"\n";
    out << "			}\n";
    out << "		}\n";
    out << "		finish\n";
    out << "		{\n";
    out << "			emission "<<il->Emission()<<"\n";
    out << "			diffuse "<<il->Diffuse()<<"\n";
    out << "		}\n";
    out << "		rotate <"<<v.x()<<","<<v.y()<<","<<v.z()<<">\n";
    out << "	}\n";

    out << "}\n";

    return fileName;
}

void NEPovRayExport::exportMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{
    if( material->factory().objectName() == "ClearWater" )
        exportClearWaterMaterial(material, objColor, ostr, name, path);
    else if( material->factory().objectName() == "Ocean" )
        exportOceanMaterial(material, objColor, ostr, name, path);
    //	else if( material->factory().objectName() == "Glass" )
    //		exportGlassMaterial(material, objColor, ostr, name, path);
    else if( material->factory().objectName() == "Phong" )
        exportPhongMaterial(material, objColor, ostr, name, path);
}

void NEPovRayExport::exportClearWaterMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{
    NEClearWaterMaterial* cw = qobject_cast<NEClearWaterMaterial*> (material);

    if(cw->clearWaterType() == cw->PovrayRadiosity)
    {
        ostr <<"#declare "<<name<<"_material=\n";
        ostr <<"material\n";
        ostr <<"{\n";
        ostr <<"	texture\n";
        ostr <<"	{\n";
        ostr <<"		pigment\n";
        ostr <<"		{\n";
        ostr <<"			color rgbt<0.1,0.5,1,0.98>\n";
        ostr <<"		}\n";
        ostr <<"		finish\n";
        ostr <<"		{\n";
        ostr <<"			emission 1.0\n";
        ostr <<"			diffuse 0.0\n";
        ostr <<"			reflection\n";
        ostr <<"			{\n";
        ostr <<"				0.02 1.0\n";
        ostr <<"				fresnel on\n";
        ostr <<"			}\n";
        ostr <<"			specular 0.05 \n";
        ostr <<"		}\n";
        ostr <<"	}\n";
        ostr <<"	interior\n";
        ostr <<"	{\n";
        ostr <<"		ior 1.33\n";
        ostr <<"	}\n";
        ostr <<"}\n\n";
    }
    else
    {
        float r = (float)(objColor.red())/255.0f;
        float g = (float)(objColor.green())/255.0f;
        float b = (float)(objColor.blue())/255.0f;
        float t = 1.0f - (float)(objColor.alpha())/255.0f;

        float r1 = (float)(cw->Color().red())/255.0f;
        float g1 = (float)(cw->Color().green())/255.0f;
        float b1 = (float)(cw->Color().blue())/255.0f;
        float t1 = 1.0f - (float)(cw->Color().alpha())/255.0f;

        ostr <<"#declare "<<name<<"_material=\n";
        ostr <<"material\n";
        ostr <<"{\n";
        ostr <<"	texture\n";
        ostr <<"	{\n";
        ostr <<"		pigment\n";
        ostr <<"		{\n";
        ostr <<"			color rgbt<"<<r*r1<<","<<g*g1<<","<<b*b1<<","<<t*t1<<">\n";
        ostr <<"		}\n";
        ostr <<"		finish\n";
        ostr <<"		{\n";
        ostr <<"			emission 1.0\n";
        ostr <<"			diffuse "<<cw->KDiffuse()<<"\n";
        ostr <<"			reflection\n";
        ostr <<"			{\n";
        ostr <<"				0.02 1.0\n";
        ostr <<"				fresnel on\n";
        ostr <<"			}\n";
        ostr <<"			specular "<<1.0f/cw->Shininess()<<"\n";
        ostr <<"		}\n";
        ostr <<"	}\n";
        ostr <<"	interior\n";
        ostr <<"	{\n";
        ostr <<"		ior "<<cw->IndexOfRefraction()<<"\n";
        ostr <<"	}\n";
        ostr <<"}\n\n";
    }
}

void NEPovRayExport::exportOceanMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{
    NEOceanMaterial* ow = qobject_cast<NEOceanMaterial*> (material);
    float r = (float)(objColor.red())/255.0f;
    float g = (float)(objColor.green())/255.0f;
    float b = (float)(objColor.blue())/255.0f;
    float t = 1.0f - (float)(objColor.alpha())/255.0f;

    float r1 = (float)(ow->Color().red())/255.0f;
    float g1 = (float)(ow->Color().green())/255.0f;
    float b1 = (float)(ow->Color().blue())/255.0f;
    float t1 = 1.0f - (float)(ow->Color().alpha())/255.0f;

    ostr <<"#declare "<<name<<"_material=\n";
    ostr <<"material\n";
    ostr <<"{\n";
    ostr <<"	texture\n";
    ostr <<"	{\n";
    ostr <<"		pigment\n";
    ostr <<"		{\n";
    ostr <<"			color rgbt<"<<r*r1<<","<<g*g1<<","<<b*b1<<","<<t*t1<<">\n";
    ostr <<"		}\n";
    ostr <<"		finish\n";
    ostr <<"		{\n";
    ostr <<"			emission 1.0\n";
    ostr <<"			diffuse "<<ow->KDiffuse()<<"\n";
    ostr <<"			reflection\n";
    ostr <<"			{\n";
    ostr <<"				0.02 1.0\n";
    ostr <<"				fresnel on\n";
    ostr <<"			}\n";
    ostr <<"			specular "<<1.0f/ow->Shininess()<<"\n";
    ostr <<"		}\n";
    ostr <<"	}\n";
    ostr <<"	interior\n";
    ostr <<"	{\n";
    ostr <<"		ior "<<ow->IndexOfRefraction()<<"\n";
    ostr <<"	}\n";
    ostr <<"}\n\n";
}

//void NEPovRayExport::exportGlassMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
//{

//	NEGlassMaterial* gm = qobject_cast<NEGlassMaterial*> (material);

//	float r = (float)(objColor.red())/255.0f;
//	float g = (float)(objColor.green())/255.0f;
//	float b = (float)(objColor.blue())/255.0f;
//	float t = 1.0f - (float)(objColor.alpha())/255.0f;

//	float r1 = (float)(gm->Color().red())/255.0f;
//	float g1 = (float)(gm->Color().green())/255.0f;
//	float b1 = (float)(gm->Color().blue())/255.0f;
//	float t1 = 1.0f - (float)(gm->Color().alpha())/255.0f;

//	ostr <<"#declare "<<name<<"_material=\n";
//	ostr <<"material\n";
//	ostr <<"{\n";
//	ostr <<"	texture\n";
//	ostr <<"	{\n";
//	ostr <<"		pigment\n";
//	ostr <<"		{\n";
//	ostr <<"			color rgbt<"<<r*r1<<","<<g*g1<<","<<b*b1<<","<<t*t1<<">\n";
//	ostr <<"		}\n";
//	ostr <<"		finish\n";
//	ostr <<"		{\n";
//	ostr <<"			emission 1.0\n";
//	ostr <<"			diffuse "<<gm->KDiffuse()<<"\n";
//	ostr <<"			reflection\n";
//	ostr <<"			{\n";
//	ostr <<"				0.02 1.0\n";
//	ostr <<"				fresnel on\n";
//	ostr <<"			}\n";
//	ostr <<"			specular "<<1.0f/gm->shininess()<<"\n";
//	ostr <<"		}\n";
//	ostr <<"	}\n";
//	ostr <<"	interior\n";
//	ostr <<"	{\n";
//	ostr <<"		ior "<<gm->indexOfRefraction()<<"\n";
//	ostr <<"	}\n";
//	ostr <<"}\n\n";
//}

void NEPovRayExport::exportPhongMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{
    NEPhongMaterial* pm = qobject_cast<NEPhongMaterial*> (material);

    if(pm->textured() == false)
    {
        if(pm->preset() == pm->PovrayRadiosity)
        {
            ostr <<"#declare "<<name<<"_material=\n";
            ostr <<"material\n";
            ostr <<"{\n";
            ostr <<"	texture\n";
            ostr <<"	{\n";
            ostr <<"		pigment\n";
            ostr <<"		{\n";
            ostr <<"			 color rgbt<0.1,0.3,1.0,0.0>\n";
            ostr <<"		}\n";
            ostr <<"		finish\n";
            ostr <<"		{\n";
            ostr <<"			ambient 0.0\n";
            ostr <<"			diffuse 1\n";
            ostr <<"			specular 0.05\n";
            ostr <<"			roughness 0.001\n";
            ostr <<"		}\n";
            ostr <<"	}\n";
            ostr <<"}\n\n";
        }
        else
        {
            float r = (float)(objColor.red())/255.0f;
            float g = (float)(objColor.green())/255.0f;
            float b = (float)(objColor.blue())/255.0f;

            float r1 = (float)(pm->Color().red())/255.0f;
            float g1 = (float)(pm->Color().green())/255.0f;
            float b1 = (float)(pm->Color().blue())/255.0f;

            ostr <<"#declare "<<name<<"_material=\n";
            ostr <<"material\n";
            ostr <<"{\n";
            ostr <<"	texture\n";
            ostr <<"	{\n";
            ostr <<"		pigment\n";
            ostr <<"		{\n";
            ostr <<"			color rgbt<"<<r*r1<<","<<g*g1<<","<<b*b1<<">\n";
            ostr <<"		}\n";
            ostr <<"		finish\n";
            ostr <<"		{\n";
            ostr <<"			ambient 0.0\n";
            ostr <<"			diffuse "<<pm->KDiffuse()<<"\n";
            ostr <<"			specular "<<pm->KSpecular()<<"\n";
            ostr <<"			roughness "<<pm->shininess()<<"\n";
            ostr <<"		}\n";
            ostr <<"	}\n";
            ostr <<"}\n\n";
        }
    }
    else
    {
        QString img = pm->imgNamePositiveX();

        ostr <<"#declare "<<name<<"_material=\n";
        ostr <<"material\n";
        ostr <<"{\n";
        ostr <<"	texture\n";
        ostr <<"	{\n";
        ostr <<"		pigment\n";
        ostr <<"		{\n";
        ostr <<"			image_map\n";
        ostr <<"			{\n";
        ostr <<"				jpeg "<<"\""<<img<<"\"\n";
        ostr <<"				interpolate 2 \n";
        ostr <<"			}\n";
        ostr <<"			scale 1\n";
        ostr <<"			rotate <0,0,0>\n";
        ostr <<"		}\n";
        ostr <<"		finish\n";
        ostr <<"		{\n";
        ostr <<"			ambient 0.0\n";
        ostr <<"			diffuse "<<pm->KDiffuse()<<"\n";
        ostr <<"			phong "<<pm->KSpecular()<<"\n";
        ostr <<"			phong_size "<<pm->shininess() <<"\n";
        ostr <<"		}\n";
        ostr <<"	}\n";
        ostr <<"}\n\n";
    }
}


