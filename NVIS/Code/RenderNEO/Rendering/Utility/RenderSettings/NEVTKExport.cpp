#include "NEVTKExport.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/NELight.h"

#if NE_VOLUME
#include "Geometry/Volume/Core/NEVolumeNode.h"
#include "Geometry/Volume/Core/NEVolume.h"
#endif

#include "Physics/Spatial/NEMeasurementField.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h"
#if NE_DEFORMABLE
#include "Core/Physics/Spatial/Physical/Deformable/ParticleDeformable/NESphDeformable.h"
#endif

#include "Rendering/MeshConstructor/NEMarchingCubes.h"

#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"

#if NE_POST
#include "PostProcessor/Rendering/PostProcessor/NEMeshPostProcessor.h"
#endif

#if NE_SPRAY
#include "Spray/Physics/Spatial/Physical/Fluid/ParticleFluid/NEFoamSolver.h"
#include "Spray/Physics/Spatial/Physical/Fluid/ParticleFluid/NEFoamParticle.h"
#endif

#include "Base/NESceneManager.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h"
#include "Rendering/Manager/NERenderManager.h"


QString NEVTKExport::exportCamera( const NECamera* cam, const QString path )
{
    return "";
}

QString NEVTKExport::exportRigid( bool firstInSeq, const NESphRigid* rigid, const QString path )
{
    QString name  = rigid->objectName();
    QColor color1 = rigid->Color();

    QString fileName;

    fileName = name + ".vtp";
    QFile fout( path + fileName );

    if( !fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";

    recordMesh( rigid, &rigid->triangleMesh(), fout, 1, name, color1, false, true, false, true );

    return fileName;
}

#if NE_DEFORMABLE
QString NEVTKExport::exportDeformable( const NESphDeformable* deformable, const QString path )
{
    QString frameNumber;
    frameNumber.sprintf( "%06d", NESCENE.CurrentFrame() );

    QString name = deformable->objectName();
    QColor color1 = deformable->Color();

    QString fileName;

    fileName = frameNumber + "_" + name + ".vtp";

    QFile fout( path + fileName );
    if( !fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";

    recordMesh(  deformable, ((NEParticleDeformable*)deformable)->triangleMesh(), fout, 1, name,color1, false, true, false, true );

    return fileName;
}
#endif

//! Exports either general particles set or the foam particles set
QString NEVTKExport::exportParticleFluid( NEParticleFluidSolver* fluidSolver, const QString path )
{
    return exportGeneralParticleFluid(fluidSolver, path);
}

QString NEVTKExport::exportGeneralParticleFluid( const NEParticleFluidSolver* fluidSolver, const QString path )
{
    QString frameNumber;
    frameNumber.sprintf( "%06d", NESCENE.CurrentFrame() );

    QString fileName = fluidSolver->objectName() + "." + frameNumber + ".vtp";
    QFile fout( path + fileName );

    if( !fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";


    bool seq;

    if( NERENDERMAN.renderSequence() == false )
        seq = false;
    else
        seq = true;

    fout.close();

    vtkSmartPointer< vtkPolyData > partPolyData  = vtkSmartPointer< vtkPolyData >::New();
    vtkSmartPointer< vtkPoints >   points        = vtkSmartPointer< vtkPoints >::New();
    vtkSmartPointer< vtkCellArray > cellArray    = vtkSmartPointer< vtkCellArray >::New();

    const NEVectorMetaData& particles = *(fluidSolver->particlesVector());
    points->Resize( particles.objectCount );

    vtkIdType pid[1];

    //
    // Add point data to each point
    //
    //vtkSmartPointer< vtkFloatArray > vmag = vtkSmartPointer< vtkFloatArray >::New();
    //vmag->SetNumberOfComponents( 1 );
    //vmag->SetName( "vmag" );

    //
    // Add point data to each point
    //
    vtkSmartPointer< vtkFloatArray > pressures = vtkSmartPointer< vtkFloatArray >::New();
    pressures->SetNumberOfComponents( 1 );
    pressures->SetName( "p" );


    //
    // Add point data to each point
    //
    vtkSmartPointer< vtkFloatArray > densities = vtkSmartPointer< vtkFloatArray >::New();
    densities->SetNumberOfComponents( 1 );
    densities->SetName( "d" );

    //
    // Velocities
    //

    vtkSmartPointer< vtkFloatArray > velocities = vtkSmartPointer< vtkFloatArray >::New();
    velocities->SetNumberOfComponents( 3 );
    velocities->SetName( "v" );

    for_all( NEFluidParticle, particle, particles )
    {
        vec3f p = particle->position();
        float pressure = particle->pressure();
        float density = particle->density();
        pid[0] = points->InsertNextPoint( p.x(), p.y(), p.z() );
        cellArray->InsertNextCell( 1, pid );

        //vmag->InsertNextValue( particle->velocity().length() );
        pressures->InsertNextValue( pressure );
        densities->InsertNextValue( density );

        velocities->InsertNextTuple3( particle->velocity().x(), particle->velocity().y(), particle->velocity().z() );
    }

    partPolyData->SetPoints( points );
    partPolyData->SetVerts( cellArray );

    //partPolyData->GetPointData()->AddArray( vmag );
    partPolyData->GetPointData()->AddArray( pressures );
    partPolyData->GetPointData()->AddArray( densities );
    partPolyData->GetPointData()->AddArray( velocities );

    vtkSmartPointer< vtkXMLPolyDataWriter > writer = vtkSmartPointer< vtkXMLPolyDataWriter >::New();
    writer->SetFileName( fout.fileName().toStdString().c_str() );
    writer->SetDataModeToAscii();
    writer->SetInputData( partPolyData );
    writer->Write();


    /*
    //
    //
    vtkSmartPointer< vtkUnstructuredGrid > unstructuredGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    unstructuredGrid->SetPoints( points );
    unstructuredGrid->SetCells( VTK_VERTEX, cellArray );

    //
    // Write file
    //
    vtkSmartPointer< vtkXMLUnstructuredGridWriter > writer = vtkSmartPointer< vtkXMLUnstructuredGridWriter >::New();
    writer->SetFileName( fileName.toStdString().c_str() );
    writer->SetInputData( unstructuredGrid );
    writer->Write();
    */

    return fileName;
}


void NEVTKExport::deleteFile( const NEParticleFluidSolver* fluidSolver, int exitCode, QProcess::ExitStatus exitStatus )
{
    fluidSolver->disconnect( m_pConvertProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                       fluidSolver, SLOT(deleteFile(int, QProcess::ExitStatus)));
    QFile::remove( externalAsciiFile() );
}


QString NEVTKExport::exportFluidSurface( const NEMarchingCubes* mc, const QString path )
{
    QString frameNumber;
    frameNumber.sprintf("%06d", NESCENE.CurrentFrame());

    QString name = mc->objectName();
    QColor color1 = mc->Color();

    QString fileName = frameNumber +"_"+name + ".vtp";

    QFile fout(path + fileName);
    if(!fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";

    fout.close();

    recordMesh( mc, &mc->triangleMesh(), fout, 1, name,color1, false, mc->ExportTextureCoord(),mc->ExportVelocities(),mc->ExportSplashes());

    return fileName;
}

#if NE_POST
QString NEVTKExport::exportPostprocessedSurface( const NEMeshPostProcessor* pp, const QString path )
{
    QString frameNumber;
    frameNumber.sprintf( "%06d", NESCENE.CurrentFrame() );

    QString name = pp->objectName();
    QColor color1 = pp->Color();

    QString fileName = frameNumber +"_"+name + ".vtp";

    QFile fout(path + fileName);
    if( !fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";

    fout.close();

    recordMesh( pp, pp->outputMesh(), fout, 1, name,color1, false, pp->ExportTextureCoord(),pp->ExportVelocities(),pp->ExportSplashes());

    return fileName;
}
#endif

#if NE_VOLUME
QString NEVTKExport::exportVolume(const NEVolumeNode *vol, const QString path)
{
    QString frameNumber;
    frameNumber.sprintf( "%06d", NESCENE.CurrentFrame() );

    QString name  = vol->objectName();
    QColor color1 = vol->Color();

    QString fileName = frameNumber +"_"+name + ".vts";

    QFile fout( path + fileName );
    if( !fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";

    fout.close();

    std::shared_ptr< NEVolume > volume = const_cast< NEVolumeNode *>( vol)->Volume();
    openvdb::FloatGrid::Ptr grid = volume->Volume();
    openvdb::math::Transform& transform = grid->transform();

    //const openvdb::CoordBBox& bbox = grid->evalActiveVoxelBoundingBox();
    const openvdb::Coord subdiv = grid->evalActiveVoxelDim();


    vtkSmartPointer<vtkStructuredGrid> structuredGrid = vtkSmartPointer<vtkStructuredGrid>::New();

    vtkSmartPointer< vtkPoints > points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( subdiv.x() * subdiv.y() * subdiv.z() );


    openvdb::FloatGrid::Accessor accessor = grid->getAccessor();

    //
    // Add point data to each point
    //
    vtkSmartPointer< vtkFloatArray > densities = vtkSmartPointer< vtkFloatArray >::New();
    densities->SetNumberOfComponents( 1 );
    densities->SetName( "d" );
    densities->SetNumberOfTuples( subdiv.x()* subdiv.y()*subdiv.z() );

   // for( openvdb::FloatGrid::ValueOnCIter iter = grid->cbeginValueOn(); iter.test(); ++iter)
    {
        for( int k = 0; k < subdiv.z(); ++k )
        {
            int kOffset = k * subdiv.x() * subdiv.y();
            for( int j = 0; j < subdiv.y(); ++j )
            {
                int jOffset = j * subdiv.x();
                for( int i = 0; i < subdiv.x(); ++i )
                {
                    //const float value = *iter;
                    openvdb::Coord localCoord = openvdb::Coord( i-subdiv.x()/2, j-subdiv.y()/2, k-subdiv.z()/2);
                    const float value = accessor.getValue( localCoord ) * -10;
                    int offset = i + jOffset + kOffset;
                    //if( value > 0 )
                    {
                       // openvdb::Coord c = iter.getCoord();
                       // vec3f pos( c.x() +0.5, c.y() + 0.5, c.z() + 0.5 );

                        vec3f pos( localCoord.x(), localCoord.y(), localCoord.z() );
                        openvdb::Vec3s ipos = transform.indexToWorld( openvdb::Vec3d( pos.x() ,
                                                                                      pos.y() ,
                                                                                      pos.z() ) );
                        //if( iter.isVoxelValue() )
                        {
                            //points->InsertNextPoint( ipos.x(), ipos.y(), ipos.z() );
                            points->InsertPoint( offset, ipos.x(), ipos.y(), ipos.z() );
                            //densities->InsertTuple( offset, (const double*) &value );
                            //densities->InsertNextValue( value );
                            densities->InsertValue( offset, value );
                            //std::cout << "value " << value << std::endl;
                            //std::cout << ipos.x() << " " << ipos.y() << " " << ipos.z() << " " << value << std::endl;
                        }
                    }
                }
            }
        }
    }

    //std::cout << "Size " << bbox.extents().x() << " " << bbox.extents().y() << " " << bbox.extents().z() << std::endl;
    structuredGrid->SetDimensions( subdiv.x(), subdiv.y(), subdiv.z() );
    structuredGrid->SetPoints( points );
    structuredGrid->GetPointData()->AddArray( densities );


    //std::cout << "Size of points " << points->GetNumberOfPoints() << std::endl;
    //std::cout << "Den " << densities->GetNumberOfTuples() << std::endl;


     // Write file
     vtkSmartPointer< vtkXMLStructuredGridWriter > writer = vtkSmartPointer< vtkXMLStructuredGridWriter >::New();
     writer->SetDataModeToAscii();
     writer->SetFileName( fout.fileName().toStdString().c_str() );

     writer->SetInputData( structuredGrid );

     writer->Write();

    return "";
}
#endif

QString NEVTKExport::exportMeasurement(const NEMeasurementField *field, const QString path)
{

    if( field->subdivideMeasurementField() == false )
        return "";

    QString frameNumber;
    frameNumber.sprintf( "%06d", NESCENE.CurrentFrame() );


    QString name  = field->objectName();
    QColor color1 = field->Color();

    QString fileName = name + "." + frameNumber + ".vts";

    QFile fout( path + fileName );
    if( !fout.open( QFile::WriteOnly | QFile::Truncate ) )
        return "";

    fout.close();


    vtkSmartPointer< vtkStructuredGrid > structuredGrid = vtkSmartPointer< vtkStructuredGrid >::New();

    vtkSmartPointer< vtkPoints > points = vtkSmartPointer<vtkPoints>::New();

    const vec3i& subdiv  = field->subdiv();
    const vec3f& subdivh = field->subdivh();
    const vec3f& minPos  = field->aabb().minPos();


    const boost::multi_array< vec3f, 3 >& avgVelField = field->avgVelField();
    const boost::multi_array< float, 3 >& avgPressureField = field->avgPressureField();
    const boost::multi_array< float, 3 >& avgDensityField = field->avgDensityField();

    //
    // Add point data to each point
    //
    vtkSmartPointer< vtkFloatArray > pressures = vtkSmartPointer< vtkFloatArray >::New();
    pressures->SetNumberOfComponents( 1 );
    pressures->SetName( "p" );
    pressures->SetNumberOfTuples( subdiv.x()* subdiv.y()*subdiv.z() );


    //
    // Add point data to each point
    //
    vtkSmartPointer< vtkFloatArray > densities = vtkSmartPointer< vtkFloatArray >::New();
    densities->SetNumberOfComponents( 1 );
    densities->SetName( "d" );
    densities->SetNumberOfTuples( subdiv.x()* subdiv.y()*subdiv.z() );

    //
    // Velocities
    //
    vtkSmartPointer< vtkFloatArray > velocities = vtkSmartPointer< vtkFloatArray >::New();
    velocities->SetNumberOfComponents( 3 );
    velocities->SetName( "v" );
    velocities->SetNumberOfTuples( subdiv.x()* subdiv.y()*subdiv.z() );

    for( int k = 0; k < subdiv.z(); ++k )
    {
        int kOffset = k * subdiv.x() * subdiv.y();
        for( int j = 0; j < subdiv.y(); ++j )
        {
            int jOffset = j * subdiv.x();
            for( int i = 0; i < subdiv.x(); ++i )
            {
                vec3f pos( i+0.5f, j+0.5f, k+0.5f );
                pos *= subdivh;
                pos += minPos;

                int offset = i + jOffset + kOffset;
                points->InsertNextPoint( pos.x(), pos.y(), pos.z() );

                const vec3f vel = avgVelField[i][j][k];
                float velVal[3];
                velVal[0] = vel[0];
                velVal[1] = vel[1];
                velVal[2] = vel[2];
                velocities->InsertTuple( offset, velVal );
                pressures->InsertTuple( offset, (const double*) &avgPressureField[i][j][k]  );
                densities->InsertTuple( offset, (const double*) &avgDensityField[i][j][k] );
            }
        }
    }

    //
    // Specify the dimensions of the grid
    //
    structuredGrid->SetDimensions( subdiv.x(), subdiv.y(), subdiv.z() );
    structuredGrid->SetPoints( points );
    structuredGrid->GetPointData()->AddArray( velocities );
    structuredGrid->GetPointData()->AddArray( pressures );
    structuredGrid->GetPointData()->AddArray( densities );


    // Write file
    vtkSmartPointer< vtkXMLStructuredGridWriter > writer = vtkSmartPointer< vtkXMLStructuredGridWriter >::New();
    writer->SetFileName( fout.fileName().toStdString().c_str() );
    writer->SetInputData( structuredGrid );

    writer->Write();

    return "";
}

void NEVTKExport::recordMesh( const NESpatial* obj, const NEIndexedTriMesh* triMesh, QFile& ofile, uint mesh, QString& name, QColor& color, bool isRigid, bool exportTextures, bool exportVelocities, bool exportSplashes )
{
    NERigid* rigid = qobject_cast< NERigid *>( obj );
    if( !rigid )
        return;


    vtkSmartPointer< vtkPolyData > polyData  = vtkSmartPointer< vtkPolyData >::New();
    polyData->DeepCopy( getVTKPolyData(triMesh) );

    vtkSmartPointer< vtkTransform > ttransform = vtkSmartPointer< vtkTransform >::New();
    ttransform->Translate( obj->Positionx(), obj->Positiony(), obj->Positionz() );
    ttransform->RotateX( obj->Orientation().toVector3D().x() );
    ttransform->RotateY( obj->Orientation().toVector3D().y() );
    ttransform->RotateZ( obj->Orientation().toVector3D().z() );
    ttransform->Scale( obj->Scalex(), obj->Scaley(), obj->Scalez() );

    vtkSmartPointer< vtkTransformPolyDataFilter > tfilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    tfilter->SetTransform( ttransform );
    tfilter->SetInputData( polyData );
    tfilter->Update();


    vtkSmartPointer< vtkXMLPolyDataWriter > writer = vtkSmartPointer< vtkXMLPolyDataWriter >::New();
    writer->SetFileName( ofile.fileName().toStdString().c_str() );
    writer->SetInputData( tfilter->GetOutput() );

    writer->Write();
}

#ifdef NE_VTK
vtkPolyData* NEVTKExport::getVTKPolyData(const NEIndexedTriMesh* triMesh) const
{
    vtkPolyData* polyData = vtkPolyData::New();

    vtkPoints* points = vtkPoints::New();
    vtkFloatArray* tcoords = vtkFloatArray::New();
    tcoords->SetNumberOfComponents(2);
    vtkFloatArray* normals = vtkFloatArray::New();
    normals->SetNumberOfComponents(3);
    vtkCellArray* polys = vtkCellArray::New();

    points->SetNumberOfPoints(triMesh->vertices().size());
    tcoords->SetNumberOfTuples(triMesh->vertices().size());
    normals->SetNumberOfTuples(triMesh->vertices().size());
    //polys->SetNumberOfCells(inputMesh->triangles().size());


    for(uint i = 0; i < triMesh->vertices().size(); i++)
    {
        NECTNPVertex v = triMesh->vertices()[i];
        points->SetPoint(i, (float*) & (v.position));
        tcoords->SetTuple(i, (float*) & (v.tCoord));
        normals->SetTuple(i, (float*) & (v.normal));
    }

    for(uint i = 0; i < triMesh->triangles().size(); i++)
    {
        NETriangleIndexed t = triMesh->triangles()[i];
        polys->InsertNextCell(0);
        polys->InsertCellPoint(t.p1);
        polys->InsertCellPoint(t.p2);
        polys->InsertCellPoint(t.p3);
        polys->UpdateCellCount(3);
    }

    polyData->SetPoints(points);
    polyData->SetPolys(polys);
    polyData->GetPointData()->SetTCoords(tcoords);
    polyData->GetPointData()->SetNormals(normals);

    points->Delete();
    tcoords->Delete();
    normals->Delete();
    polys->Delete();

    return polyData;
}
#endif

QString NEVTKExport::exportLight( const NELight* light, const QString path )
{
    return QString("");
}



void NEVTKExport::exportMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{
}

void NEVTKExport::exportClearWaterMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{

}

void NEVTKExport::exportOceanMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{

}

void NEVTKExport::exportGlassMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{

}

void NEVTKExport::exportVolumeMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{

}


void NEVTKExport::exportPhongMaterial( NEMaterial* material, const QColor objColor, QTextStream& ostr, QString name, QString path/*=""*/ ) const
{
}










