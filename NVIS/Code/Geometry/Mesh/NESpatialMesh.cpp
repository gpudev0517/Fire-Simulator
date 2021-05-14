#include <QCoreApplication>

#include "NESpatialMesh.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Resource/Mesh/NEMeshContainer.h"

NE_PROP_DEF(NESpatialMesh::NESpatialMeshType, NESpatialMesh, SpatialMeshType, NESpatialMesh::Cuboid)

NE_PROP_DEF(FilePath, NESpatialMesh, CADFile, FilePath(FilePath::kRefeFileType))
NE_PROP_DEF(double, NESpatialMesh, UnitScale, 1.0f)
NE_PROP_DEF(double, NESpatialMesh, Thickness, 1.0f)
NE_PROP_DEF(bool, NESpatialMesh, FlipMesh, false)
NE_PROP_DEF(bool, NESpatialMesh, TwoSidedLighting, true)

NE_PROP_DEF(bool, NESpatialMesh, SwapUpAxes, false)
NE_PROP_DEF(bool, NESpatialMesh, FlipXAxis, false)
NE_PROP_DEF(bool, NESpatialMesh, FlipYAxis, false)
NE_PROP_DEF(bool, NESpatialMesh, FlipZAxis, false)
NE_PROP_DEF(bool, NESpatialMesh, TopCap, true)
NE_PROP_DEF(bool, NESpatialMesh, BottomCap, true)
NE_PROP_DEF(vec3f, NESpatialMesh, CylAxis, vec3f( 0, 1.0, 0.0))


class NESpatialMeshPrivate
{
public:
    NESpatialMeshPrivate(NESpatialMesh* s) : S(s)
    {
    }

    /* Functions */
    // Compute the centroid and set it as the center
    void computeCentroid();
    // Compute the center of mass and set is as the center
    void computeCenterOfPolyhedralMass();
    QString OBJexpand_environment_variables(QString s);

    /* Properties */
    NESpatialMesh::NESpatialMeshType m_SpatialMeshType;

    FilePath m_SpatialMeshFile;

    bool m_FlipMesh;

    //! Two sided lighting flag
    bool m_TwoSidedLighting;

    /* Other variables */
    matrix33d   m_InertiaTensor;
    vec3f       m_InvInertiaDiagLocal;
    vec3f       m_LocalInertia;
    double      m_InvMass;

    vec3f       m_CustomOrigSize;

    NEMeshContainer m_pMeshContainer;

    uint createBoxMesh();
    uint createCuboidMesh();
    uint createCylinderMesh();
    uint createSphereMesh();
    uint createCustomMesh();
    uint createPlaneMesh();

    void meshCreated();

    FilePath m_CADFile;
    double m_UnitScale;    
    double m_Thickness;
    double m_oldThickness;
    double m_UnitOrigScale;

    bool m_SwapUpAxes;
    bool m_FlipXAxis;
    bool m_FlipYAxis;
    bool m_FlipZAxis;

    bool m_TopCap;
    bool m_BottomCap;
    vec3f m_CylAxis;

    std::vector<int> m_measureLists;

    FilePath m_savedObjPath;

    /* Functions */
    // Reload the 3D model file
    void reloadCADFile();
    // Clear the 3D model file
    void clearCADFile();
    // Load the 3D model file
    void loadCADFile(const QString& filePath);

private:
    NESpatialMesh* S;
};

static void initProperties()
{
    NE_INIT_PROP(NESpatialMesh, SpatialMeshType);

    NE_INIT_PROP(NESpatialMesh, FlipMesh);
    NE_INIT_PROP(NESpatialMesh, UnitScale);
    NE_INIT_PROP(NESpatialMesh, Thickness);
    NE_INIT_PROP(NESpatialMesh, TwoSidedLighting);

    NE_INIT_PROP(NESpatialMesh, CADFile);
    NE_INIT_PROP(NESpatialMesh, SwapUpAxes);
    NE_INIT_PROP(NESpatialMesh, FlipXAxis);
    NE_INIT_PROP(NESpatialMesh, FlipYAxis);
    NE_INIT_PROP(NESpatialMesh, FlipZAxis);

    NE_INIT_PROP(NESpatialMesh, BottomCap);
    NE_INIT_PROP(NESpatialMesh, TopCap);
    NE_INIT_PROP(NESpatialMesh, CylAxis);

}

NESpatialMesh::NESpatialMesh(NENodeFactory* factory) : NESpatialGroup(factory)
{
    addNodeTypeToNodeGraphSceneType("Geometry", NENode::SceneType::Modeling );
    addNodeTypeToNodeGraphSceneType("Geometry", NENode::SceneType::Simulation );
    addNodeTypeToNodeGraphSceneType("Geometry", NENode::SceneType::Visualization );

    P = new NESpatialMeshPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);
    P->m_FlipMesh = NE_DEFAULT(FlipMesh);
    P->m_TwoSidedLighting = NE_DEFAULT(TwoSidedLighting);
    P->m_SpatialMeshType = NE_DEFAULT(SpatialMeshType);
    P->m_CADFile = NE_DEFAULT(CADFile);
    P->m_SwapUpAxes = NE_DEFAULT(SwapUpAxes);
    P->m_FlipXAxis  = NE_DEFAULT(FlipXAxis);
    P->m_FlipYAxis  = NE_DEFAULT(FlipYAxis);
    P->m_FlipZAxis  = NE_DEFAULT(FlipZAxis);
    P->m_UnitScale = NE_DEFAULT(UnitScale);
    P->m_Thickness = NE_DEFAULT(Thickness);
//    P->m_Thickness = Scaley();
//    SetThickness(Scaley());
    P->m_TopCap = NE_DEFAULT(TopCap);
    P->m_BottomCap = NE_DEFAULT(BottomCap);
    P->m_CylAxis   = NE_DEFAULT(CylAxis);

    P->m_CADFile.setFilter(ReferenceFilter());
    P->m_CADFile.setPath("");
    P->m_CADFile.setDialogType(FilePath::kRefeFileType);

    hideProperty("CenterMethod", false);
    hideProperty("LocalizedTransforms", false);
    hideProperty("GroupBox", true);

    P->m_CustomOrigSize = vec3f(1, 1, 1);
    P->m_UnitOrigScale = 1.0f;
    P->m_measureLists.clear();

    setLocalizedTransforms(true);

    P->m_InvInertiaDiagLocal = vec3f(0.0, 0.0, 0.0);
    P->m_InvMass = 0.0;
    P->m_InertiaTensor = matrix33d(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    P->m_oldThickness = P->m_Thickness;

    setOutputs( outputs() | NE::kIOTriangleMesh );

#if 0 // Disable for warnings to have no signals.
    connect(this, SIGNAL(ScaleyChanged(float)), this, SLOT(setThickness()));
#endif
    connect(this, SIGNAL(ScaleChanged(vec3f)), this, SLOT(setThickness()));

}

NESpatialMesh::~NESpatialMesh()
{
//    for(NESpatial* object: NESCENE.spatials() )
//    {
//        QString m = object->factory().objectName();
//        if ( object->factory().objectName() == "")
//             NESCENE.deleteObject(object, true);
//    }
    delete P;
}

uint NESpatialMesh::init(uint initMode)
{
    int ret = NESpatial::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    emit initObject();

    P->m_CADFile.setFilter(ReferenceFilter());
    P->m_CADFile.setDialogType(FilePath::kRefeFileType);

    if( P->m_SpatialMeshType == NESpatialMesh::Box)
        P->createBoxMesh();
    else if( P->m_SpatialMeshType == NESpatialMesh::Cuboid)
        P->createCuboidMesh();
    else if( P->m_SpatialMeshType == NESpatialMesh::Sphere)
        P->createSphereMesh();
    else if( P->m_SpatialMeshType == NESpatialMesh::Cylinder)
        P->createCylinderMesh();
    else if( P->m_SpatialMeshType == NESpatialMesh::Custom )
    {

    }
    else if( P->m_SpatialMeshType == NESpatialMesh::CADMesh )
    {
        if(P->m_CADFile.path() != "")
            P->loadCADFile(P->m_CADFile);
    }
    else if( P->m_SpatialMeshType == NESpatialMesh::Plane)
        P->createPlaneMesh();

    return NE::kReturnSuccess;
}

void NESpatialMesh::addMeasureObject(int num)
{
    P->m_measureLists.push_back(num);
}

std::vector<int> NESpatialMesh::getMeasureObjects()
{
    return P->m_measureLists;
}

const bool& NESpatialMesh::FlipMesh() const
{
    return P->m_FlipMesh;
}

const bool& NESpatialMesh::TwoSidedLighting() const
{
    return P->m_TwoSidedLighting;
}

uint NESpatialMesh::SetTwoSidedLighting(const bool& val)
{
    NE_PROP_SET_EVAL(TwoSidedLighting, P, val);

    return NE::kReturnSuccess;
}

const FilePath& NESpatialMesh::CADFile() const
{
    return P->m_CADFile;
}

const bool& NESpatialMesh::SwapUpAxes() const
{
    return P->m_SwapUpAxes;
}

const bool& NESpatialMesh::FlipXAxis() const
{
    return P->m_FlipXAxis;
}

const bool& NESpatialMesh::FlipYAxis() const
{
    return P->m_FlipYAxis;
}

const bool& NESpatialMesh::FlipZAxis() const
{
    return P->m_FlipZAxis;
}

uint NESpatialMesh::SetCADFile(const FilePath& val)
{
    NE_PROP_SET_EVAL(CADFile,P,val);
    if (P->m_CADFile.dialogType() != FilePath::kRefeFileType
     && P->m_CADFile.dialogType() != FilePath::kImpoFileType)
        P->m_CADFile.setDialogType(FilePath::kRefeFileType);

    if (!(QFile(P->m_CADFile.path()).exists()))
        return NE::kReturnFileNotFound;

    P->loadCADFile(P->m_CADFile);

    return NE::kReturnSuccess;
}

uint NESpatialMesh::SetSwapUpAxes(const bool& val)
{
    NE_PROP_SET_EVAL(SwapUpAxes, P, val);

    P->reloadCADFile();
    NESpatial::updateTransform();

    return NE::kReturnSuccess;
}

const double& NESpatialMesh::UnitScale() const
{
    return P->m_UnitScale;
}

const double& NESpatialMesh::Thickness() const
{
    return P->m_Thickness;
}

double NESpatialMesh::oldThickness()
{
    return P->m_oldThickness;
}

void NESpatialMesh::setOldThickness(double t)
{
    P->m_oldThickness = t;
}

uint NESpatialMesh::SetUnitScale(const double& val)
{
    P->m_UnitOrigScale = P->m_UnitScale;
    NE_PROP_SET_EVAL(UnitScale, P, val);
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );
    settings.setValue("DefaultUnitScale", val );
    settings.sync();
    applyUnitScale( P->m_UnitScale, LocalizedTransforms() ? true : false );

    return NE::kReturnSuccess;
}

uint NESpatialMesh::setUnitScale(const double& val)
{
    P->m_UnitOrigScale = P->m_UnitScale;
    NE_PROP_SET_EVAL(UnitScale, P, val);
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );
    settings.setValue("DefaultUnitScale", val );
    settings.sync();

    return NE::kReturnSuccess;
}

uint NESpatialMesh::SetThickness(const double& val)
{
    P->m_oldThickness = Thickness();
    NE_PROP_SET_EVAL(Thickness, P, val);

    NESpatial::SetScaley(val);
    NESpatial::updateTransform();
    emit updateDrawingBuffers();

    return NE::kReturnSuccess;
}

void NESpatialMesh::setThickness()
{
    SetThickness(Scaley());
}

uint NESpatialMesh::SetFlipXAxis(const bool& val)
{
    NE_PROP_SET_EVAL(FlipXAxis, P, val);
    P->reloadCADFile();
    NESpatial::updateTransform();

    return NE::kReturnSuccess;
}

uint NESpatialMesh::SetFlipYAxis(const bool& val)
{
    NE_PROP_SET_EVAL(FlipYAxis, P, val);
    P->reloadCADFile();
    NESpatial::updateTransform();

    return NE::kReturnSuccess;
}

uint NESpatialMesh::SetFlipZAxis(const bool& val)
{
    NE_PROP_SET_EVAL(FlipZAxis, P, val);
    P->reloadCADFile();
    NESpatial::updateTransform();

    return NE::kReturnSuccess;
}

const bool& NESpatialMesh::TopCap() const
{
    return P->m_TopCap;
}

uint NESpatialMesh::SetTopCap(const bool& val)
{
    NE_PROP_SET_EVAL(TopCap, P, val);
    P->createCylinderMesh();

    return NE::kReturnSuccess;
}

const bool& NESpatialMesh::BottomCap() const
{
    return P->m_BottomCap;
}

uint NESpatialMesh::SetBottomCap(const bool& val)
{
    NE_PROP_SET_EVAL(BottomCap, P, val);
    P->createCylinderMesh();
    return NE::kReturnSuccess;
}


const vec3f& NESpatialMesh::CylAxis() const
{
    return P->m_CylAxis;
}

uint NESpatialMesh::SetCylAxis(const vec3f& val)
{
    NE_PROP_SET_EVAL(CylAxis, P, val);
    P->createCylinderMesh();

    return NE::kReturnSuccess;
}


void NESpatialMesh::initializeGUIActions()
{
    NESpatial::initializeGUIActions();

    QObject* actionReloadObjFile = NULL;
    QObject* actionClearObjFile = NULL;

    QString stActionReloadObjFile("Reload 3D-Model File");
    emit registerAction(stActionReloadObjFile, actionReloadObjFile);
    connect(actionReloadObjFile, SIGNAL(triggered()), this, SLOT(reloadCADFile()));
    addAction(actionReloadObjFile);

    QString strActionClearObjFile("Clear 3D-Model File");
    emit registerAction( stActionReloadObjFile, actionClearObjFile );
    connect(actionClearObjFile, SIGNAL(triggered()), this, SLOT(clearCADFile()));
    addAction(actionClearObjFile);
}

void NESpatialMesh::resetProperties()
{
    NESpatial::resetProperties();

    ResetFlipMesh();
    ResetTwoSidedLighting();
    ResetSpatialMeshType();
    ResetCADFile();
    ResetSwapUpAxes();
    ResetFlipXAxis();
    ResetFlipYAxis();
    ResetFlipZAxis();
    ResetUnitScale();
    ResetThickness();
    ResetTopCap();
    ResetBottomCap();
}

const NESpatialMesh::NESpatialMeshType& NESpatialMesh::SpatialMeshType() const
{
    return P->m_SpatialMeshType;
}

uint NESpatialMesh::SetSpatialMeshType(const NESpatialMesh::NESpatialMeshType& val)
{
    NESpatialMeshType oldType = P->m_SpatialMeshType;

    NE_PROP_SET_EVAL(SpatialMeshType, P, val);

    if( val == NESpatialMesh::Box)
        P->createBoxMesh();
    else if (val == NESpatialMesh::Sphere)
        P->createSphereMesh();
    else if( val == NESpatialMesh::Cuboid)
        P->createCuboidMesh();
    else if(val == NESpatialMesh::Cylinder)
        P->createCylinderMesh();
    else if( val == NESpatialMesh::Custom )
    {
        if(P->m_CADFile.path() != "")
            P->loadCADFile(P->m_CADFile);
        
    }
    else if(val == NESpatialMesh::Plane){
        P->createPlaneMesh();
    }
    else if(val == NESpatialMesh::None)
         triangleMesh().resize();

    if( oldType == NESpatialMesh::Custom)
        SetScale(Scale().elementWiseMultiplication(P->m_CustomOrigSize));
    else if( oldType != NESpatialMesh::Custom && val == NESpatialMesh::Custom)
        SetScale(Scale().elementWiseDivision(P->m_CustomOrigSize));

    return NE::kReturnSuccess;
}

uint NESpatialMesh::SetFlipMesh(const bool& val)
{
    NE_PROP_SET_EVAL(FlipMesh, P, val);
    triangleMesh().flip();

    return NE::kReturnSuccess;
}

bool NESpatialMesh::isCADMesh()
{
    return (P->m_SpatialMeshType == CADMesh );
}

bool NESpatialMesh::isCylinder()
{
    return( P->m_SpatialMeshType == Cylinder );
}

uint NESpatialMesh::SetCenterMethod(const NESpatial::NECenterCalculation& val)
{
    uint ret = NESpatial::SetCenterMethod(val);
    if (ret != NE::kReturnSuccess)
        return ret;

    emit updateDrawingBuffers();

    return NE::kReturnSuccess;
}

uint NESpatialMesh::SetLocalizedTransforms(const bool& val)
{
    uint ret = NESpatial::SetLocalizedTransforms(val);
    if (ret != NE::kReturnSuccess)
        return ret;

    emit updateDrawingBuffers();

    return NE::kReturnSuccess;
}

void NESpatialMesh::reloadCADFile()
{
    P->reloadCADFile();
}
void NESpatialMesh::clearCADFile()
{
    P->clearCADFile();
}

NEIndexedTriMesh& NESpatialMesh::triangleMesh()
{
    return P->m_pMeshContainer.triangleMesh();
}

const NEIndexedTriMesh& NESpatialMesh::triangleMesh() const
{
    return P->m_pMeshContainer.triangleMesh();
}

void NESpatialMesh::setTriMesh(const NEIndexedTriMesh& val)
{
    P->m_pMeshContainer.setTriMesh( val );
}

std::vector<NEPrimitivePolygon>& NESpatialMesh::quadFaces()
{
    return P->m_pMeshContainer.quadFaces();
}

void NESpatialMeshPrivate::reloadCADFile()
{
    loadCADFile(m_CADFile);
}

void NESpatialMeshPrivate::clearCADFile()
{
    m_CADFile = FilePath("");
    m_CADFile.setFilter(ReferenceFilter());
    m_CADFile.setDialogType(FilePath::kRefeFileType);
    S->triangleMesh().resize();
    S->updateAABB();
}

void NESpatialMeshPrivate::loadCADFile(const QString& filePath)
{
    m_pMeshContainer.triangleMesh().resize();

    QString expandedFilePath = OBJexpand_environment_variables( filePath );
    QFile rigidFile( expandedFilePath );
    //QFile rigidFile( filePath );

    //
    // If the rigid file doesn't exist then look for it in the scene geometry directory
    //
    if( !rigidFile.exists() )
    {
        QString projectPath = NESCENE.sceneGeometryDirectory();
        expandedFilePath = filePath;
        expandedFilePath.prepend( projectPath + "/" );
        rigidFile.setFileName( expandedFilePath );
    }

    //
    // If the rigid file doesn't exist then look for it relative to the current save directory
    //
    if( !rigidFile.exists() )
    {
        QString projectPath = NEBASE.SaveDir();
        expandedFilePath = filePath;
        expandedFilePath.prepend( projectPath + "/" );
    }

    rigidFile.setFileName( expandedFilePath );

    //
    // If the rigid file still doesn't exist then its probably loaded from an archive with
    // Save and scene directory set to a different directory. In that case set them to the current and check
    //
    if( !rigidFile.exists() )
    {
        QString scenePath = NEBASE.CurrentFilePath();
        QFileInfo fprefixpath( scenePath );
        QString scenePrefixPath = fprefixpath.path() + "/" + fprefixpath.baseName();
        QString projectPath = NESCENE.sceneGeometryDirectory( scenePrefixPath );
        expandedFilePath = filePath;
        expandedFilePath.prepend( projectPath + "/" );
    }

    float scalex = ( m_FlipXAxis ? -1.0f : 1.0f );
    float scaley = ( m_FlipYAxis ? -1.0f : 1.0f );
    float scalez = ( m_FlipZAxis ? -1.0f : 1.0f );

    //std::cout << "custom file path : " << expandedFilePath.toStdString() << std::endl;

    uint ret = m_pMeshContainer.createCustomMesh( expandedFilePath, scalex, scaley, scalez, m_SwapUpAxes );
    if (ret != NE::kReturnSuccess)
        return;

    S->updateAABB();
    m_CustomOrigSize = S->aabb().maxPos() - S->aabb().minPos();

    if( S->LocalizedTransforms() )
        S->setCenterMethod( S->LocalizedTransforms()==true ? NESpatial::NECenterCalculation::BoundingBoxCenter : S->CenterMethod());

    S->applyUnitScale( m_UnitScale, false );

    if( S->LocalizedTransforms() )
    {
        S->localizeTransforms();
        S->SetPositionx( S->Positionx() );
        S->SetPositiony( S->Positiony() );
        S->SetPositionz( S->Positionz() );
    }
    else
    {
        S->SetPosition( vec3f( 0, 0, 0 ) );
    }

    emit S->updateDrawingBuffers();
}

uint NESpatialMesh::localizeTransforms()
{
    if(SpatialMeshType() != NESpatialMesh::Custom)
        return NE::kReturnSuccess;

    std::vector<NECTNPVertex>& vertices =  triangleMesh().vertices();
    computeCenter();

    if (vertices.size() == 0)
        return NE::kReturnSuccess;

    for (unsigned i = 0; i != vertices.size(); ++i)
    {
        vec3f& position = vertices[i].position;
        position -= center();
    }
    updateAABB();
    computeBoundingBox();
    //SetPosition(center());

    return NE::kReturnSuccess;
}

uint NESpatialMesh::relocalizeTransforms()
{
    if( SpatialMeshType() != NESpatialMesh::CADMesh)
        return NE::kReturnSuccess;

    std::vector<NECTNPVertex>& vertices = triangleMesh().vertices();

    if( vertices.size() == 0 )
        return NE::kReturnSuccess;

    for( unsigned i = 0; i != vertices.size(); ++i )
    {
        vec3f& position = vertices[i].position;
        position += center();
    }
    updateAABB();

    computeCenter();

    for (unsigned i = 0; i != vertices.size(); ++i)
    {
        vec3f& position = vertices[i].position;
        position -= center();
    }
    updateAABB();
    computeBoundingBox();
    SetPosition(center());

    return NE::kReturnSuccess;
}

uint NESpatialMesh::freezeTransforms()
{
    if( SpatialMeshType() != NESpatialMesh::CADMesh )
        return NE::kReturnSuccess;
    std::vector<NECTNPVertex>& vertices = triangleMesh().vertices();
    for( unsigned i = 0; i != vertices.size(); ++i )
    {
        vec3f& position = vertices[i].position;
        position += center();
    }
    updateAABB();
    computeBoundingBox();
    SetPosition(vec3f(0.0f, 0.0f, 0.0f));

    return NE::kReturnSuccess;
}

uint NESpatialMesh::computeCenter()
{
    if (CenterMethod() == PolyhedralMass)
        P->computeCenterOfPolyhedralMass();
    else if (CenterMethod() == Centroid)
        P->computeCentroid();
    else
        NESpatial::computeCenter();

    return NE::kReturnSuccess;
}

uint NESpatialMesh::applyUnitScale(double unitScale, bool bApplyAfterLocalizing)
{

    std::vector<NECTNPVertex>& vertices =  triangleMesh().vertices();
    if (vertices.size() == 0)
        return NE::kReturnSuccess;

    if( bApplyAfterLocalizing )
    {
        for (unsigned i = 0; i != vertices.size(); ++i)
        {
            vec3f& position = vertices[i].position;
            position += center();
            position *= unitScale / P->m_UnitOrigScale;
        }

        updateAABB();
        computeCenter();
        for (unsigned i = 0; i != vertices.size(); ++i)
        {
            vec3f& position = vertices[i].position;
            position -= center();
        }
        //SetPosition(center());
    }
    else
    {
        for (unsigned i = 0; i != vertices.size(); ++i)
        {
            vec3f& position = vertices[i].position;
            position *= unitScale / P->m_UnitOrigScale;
        }
    }

    updateAABB();
    computeBoundingBox();

    emit updateDrawingBuffers();
    return NE::kReturnSuccess;
}

const vec3f& NESpatialMesh::centerOfMass() const
{
    return center();
}

const matrix33d NESpatialMesh::InertiaTensor() const
{
    return P->m_InertiaTensor;
}

bool NESpatialMesh::updateAABB()
{
    std::vector<NECTNPVertex>& v = triangleMesh().vertices();

    if( v.size() <= 0 )
        return false;

    vec3f minPos = v[0].position;
    vec3f maxPos = v[0].position;

    for( unsigned i = 0; i < v.size(); ++i )
    {
        vec3f po = v[i].position;
        if ( po.x() < minPos.x() ) minPos.x() = po.x();
        if ( po.y() < minPos.y() ) minPos.y() = po.y();
        if ( po.z() < minPos.z() ) minPos.z() = po.z();
        if ( po.x() > maxPos.x() ) maxPos.x() = po.x();
        if ( po.y() > maxPos.y() ) maxPos.y() = po.y();
        if ( po.z() > maxPos.z() ) maxPos.z() = po.z();
    }
    AABB val;

    val.insertPoint( minPos );
    val.insertPoint( maxPos );
    setAABB( val );
    return aabb().valid();
}

NEMeshContainer &NESpatialMesh::meshContainer()
{
    return P->m_pMeshContainer;
}

bool isSameSide(vec3f p1, vec3f p2, vec3f a, vec3f b)
{
    vec3f t1 = vec3f::crossProduct(b-a, p1-a);
    vec3f t2 = vec3f::crossProduct(b-a, p2-a);

    if (vec3f::dotProduct(t1, t2) >= -FEps)
        return true;
    else
        return false;
}

bool pointInsideTriangle(vec3f p, vec3f x1, vec3f x2, vec3f x3)
{
    if (isSameSide(p, x1, x2, x3) && isSameSide(p, x2, x1, x3) && isSameSide(p, x3, x1, x2))
        return true;
    else
        return false;
}

vec3f NESpatialMesh::findClosetFaceAndIntersectionPoint(QList<NESpatialMesh *> boundaries, vec3f rayStart, vec3f rayEnd, QPair<NESpatialMesh *, int> &selected, QPair<NESpatialMesh *, int> &targeted)
{
    float minDistance = FLT_MAX;
    vec3f minInterPoint;
    vec3f dirLine = rayEnd - rayStart;
    dirLine.normalize();

    Q_FOREACH (NESpatialMesh* otherCompart, boundaries)
    {
        NEIndexedTriMesh &otherTriMesh = otherCompart->triangleMesh();
        std::vector<NEPrimitivePolygon> &otherFaces = otherCompart->quadFaces();

        for( size_t oi = 0; oi != otherFaces.size(); oi++ )
        {
            NEPrimitivePolygon otherFace = otherFaces[oi];

            bool bInsideFace = false;
            vec3f interPoint;

            for (size_t oj = 0; oj != otherFace.triangles.size(); ++oj)
            {
                vec3f p1 = otherCompart->transform() * otherTriMesh.vertices()[otherFace.triangles[oj].p1].position;
                vec3f p2 = otherCompart->transform() * otherTriMesh.vertices()[otherFace.triangles[oj].p2].position;
                vec3f p3 = otherCompart->transform() * otherTriMesh.vertices()[otherFace.triangles[oj].p3].position;

                vec3f otherNormal = vec3f::normal(p1, p2, p3);

                interPoint = linePlaneInterPoint(rayStart, dirLine, p1, otherNormal);

                if (!std::isnan(interPoint.x()) && !std::isnan(interPoint.y()) && !std::isnan(interPoint.z()))
                {
                    float sign1 = vec3f::dotProduct(otherNormal, (rayStart - p1));
                    float sign2 = vec3f::dotProduct(otherNormal, (rayEnd - p1));

                    if( sign1*sign2 < 0 || abs(sign1) > abs(sign2))
                        if( pointInsideTriangle(interPoint, p1, p2, p3))
                        {
                            bInsideFace = true;
                            break;
                        }
                }

                bInsideFace = false;
            }

            if(bInsideFace == true && (Position() - interPoint).length() <= minDistance )
            {
                minDistance = (Position() - interPoint).length();
                minInterPoint = interPoint;
                targeted.first = otherCompart;
                targeted.second = oi;
            }
        }
    }

    if( targeted.first != nullptr)
    {
        NESpatialMesh * otherCompart = targeted.first;
        NEIndexedTriMesh &otherTriMesh = otherCompart->triangleMesh();
        std::vector<NEPrimitivePolygon> &otherFaces = otherCompart->quadFaces();
        int faceID = targeted.second;

        NEPrimitivePolygon otherFace = otherFaces[faceID];

        vec3f p1 = otherCompart->transform() * otherTriMesh.vertices()[otherFace.triangles[0].p1].position;
        vec3f p2 = otherCompart->transform() * otherTriMesh.vertices()[otherFace.triangles[0].p2].position;
        vec3f p3 = otherCompart->transform() * otherTriMesh.vertices()[otherFace.triangles[0].p3].position;

        vec3f otherNormal = vec3f::normal(p1, p2, p3);

        NEIndexedTriMesh &selTriMesh = triangleMesh();
        std::vector<NEPrimitivePolygon> &selFaces = quadFaces();

        for (size_t i = 0; i != selFaces.size(); ++i)
        {
            NEPrimitivePolygon selFace = selFaces[i];
            vec3f p1 = transform() * selTriMesh.vertices()[selFace.triangles[0].p1].position;
            vec3f p2 = transform() * selTriMesh.vertices()[selFace.triangles[0].p2].position;
            vec3f p3 = transform() * selTriMesh.vertices()[selFace.triangles[0].p3].position;

            vec3f selNormal = vec3f::normal(p1, p2, p3);

            if( std::abs(vec3f::dotProduct(otherNormal, selNormal)+1) <= FEps )
            {
                selected.first = this;
                selected.second = i;
                break;
            }
        }
    }

    return minInterPoint;
}

vec3f NESpatialMesh::findClosetFaceAndIntersectionPointAndRotate(QList<NESpatialMesh *> boundaries, vec3f rayStart, vec3f rayEnd, QPair<NESpatialMesh *, int> &selected, QPair<NESpatialMesh *, int> &targeted, vec3f &otherNormal)
{
    float minDistance = FLT_MAX;
    vec3f minInterPoint;
    vec3f dirLine = rayEnd - rayStart;
    dirLine.normalize();

    Q_FOREACH (NESpatialMesh* otherCompart, boundaries)
    {
        NEIndexedTriMesh &otherTriMesh = otherCompart->triangleMesh();

        for( unsigned int oi = 0; oi < otherCompart->triangleMesh().numTriangles(); oi++ )
        {
            bool bInsideFace = false;
            vec3f interPoint;

            vec3f p1 = otherCompart->transform() * otherTriMesh.vertices()[otherCompart->triangleMesh().triangles().at(oi).p1].position;
            vec3f p2 = otherCompart->transform() * otherTriMesh.vertices()[otherCompart->triangleMesh().triangles().at(oi).p2].position;
            vec3f p3 = otherCompart->transform() * otherTriMesh.vertices()[otherCompart->triangleMesh().triangles().at(oi).p3].position;

            vec3f otherNormal = vec3f::normal(p1, p2, p3);

            interPoint = linePlaneInterPoint(rayStart, dirLine, p1, otherNormal);

            bInsideFace = false;
            if (!std::isnan(interPoint.x()) && !std::isnan(interPoint.y()) && !std::isnan(interPoint.z()))
            {
                float sign1 = vec3f::dotProduct(otherNormal, (rayStart - p1));
                float sign2 = vec3f::dotProduct(otherNormal, (rayEnd - p1));

                if( sign1*sign2 < 0 || abs(sign1) > abs(sign2))
                {
                    if( pointInsideTriangle(interPoint, p1, p2, p3))
                    {
                        bInsideFace = true;
                    }
                }
            }

            if(bInsideFace == true && (Position() - interPoint).length() < minDistance )
            {
                minDistance = (Position() - interPoint).length();
                minInterPoint = interPoint;
                targeted.first = otherCompart;
                targeted.second = oi;
            }
        }
    }

    if( targeted.first != nullptr)
    {
        NESpatialMesh * otherCompart = targeted.first;
        NEIndexedTriMesh &otherTriMesh = otherCompart->triangleMesh();
        int faceID = targeted.second;

        vec3f p1 = otherCompart->transform()*otherTriMesh.vertices()[otherCompart->triangleMesh().triangles().at(faceID).p1].position;
        vec3f p2 = otherCompart->transform()*otherTriMesh.vertices()[otherCompart->triangleMesh().triangles().at(faceID).p2].position;
        vec3f p3 = otherCompart->transform()*otherTriMesh.vertices()[otherCompart->triangleMesh().triangles().at(faceID).p3].position;
        vec3d otherNormalVec = vec3d::normal(vec3d(p1), vec3d(p2), vec3d(p3));
        otherNormal = vec3f(otherNormalVec);

        eulerf oldOrientation = Orientation();

        matrix44d transform = matrix44d(this->transform());
        vec3d ventDirVec = vec3d(transform*vec4d(0.0, 1.0, 0.0, 0.0)).normalized();
        vec3d rotAxis = vec3d::crossProduct(ventDirVec, otherNormalVec);
        if (std::abs(rotAxis.x()) <= FEps)
            rotAxis.x() = 0.0;
        if (std::abs(rotAxis.y()) <= FEps)
            rotAxis.y() = 0.0;
        if (std::abs(rotAxis.z()) <= FEps)
            rotAxis.z() = 0.0;

        // Rotate the vent if the vent thickness direction and plane normal are not colinear
        if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
        {
            rotAxis.normalize();
            double angle = std::acos(ventDirVec.normalized()*otherNormalVec);
            constexpr double radToDeg = 180.0/M_PI;
            SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(vec3f(rotAxis), radToDeg*angle)));

            // Test rotating along one of the basis axis if the rotational axis is somehow not right
            transform = matrix44d(this->transform());
            ventDirVec = vec3d(transform*vec4d(0.0, 1.0, 0.0, 0.0));
            rotAxis = vec3d::crossProduct(ventDirVec, otherNormalVec);
            if (std::abs(rotAxis.x()) <= FEps)
                rotAxis.x() = 0.0;
            if (std::abs(rotAxis.y()) <= FEps)
                rotAxis.y() = 0.0;
            if (std::abs(rotAxis.z()) <= FEps)
                rotAxis.z() = 0.0;

            // Test rotating along the x-axis
            if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
            {
                vec3f axisX(1.0, 0.0, 0.0);
                SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(axisX, radToDeg*angle)));
            }

            transform = matrix44d(this->transform());
            ventDirVec = vec3d(transform*vec4d(0.0, 1.0, 0.0, 0.0));
            rotAxis = vec3d::crossProduct(ventDirVec, otherNormalVec);
            if (std::abs(rotAxis.x()) <= FEps)
                rotAxis.x() = 0.0;
            if (std::abs(rotAxis.y()) <= FEps)
                rotAxis.y() = 0.0;
            if (std::abs(rotAxis.z()) <= FEps)
                rotAxis.z() = 0.0;

            // Test rotating along the y-axis
            if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
            {
                vec3f axisY(0.0, 1.0, 0.0);
                SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(axisY, radToDeg*angle)));
            }

            transform = matrix44d(this->transform());
            ventDirVec = vec3d(transform*vec4d(0.0, 1.0, 0.0, 0.0));
            rotAxis = vec3d::crossProduct(ventDirVec, otherNormalVec);
            if (std::abs(rotAxis.x()) <= FEps)
                rotAxis.x() = 0.0;
            if (std::abs(rotAxis.y()) <= FEps)
                rotAxis.y() = 0.0;
            if (std::abs(rotAxis.z()) <= FEps)
                rotAxis.z() = 0.0;

            // Test rotating along the z-axis
            if (std::abs(rotAxis.x()) > FEps || std::abs(rotAxis.y()) > FEps || std::abs(rotAxis.z()) > FEps)
            {
                vec3f axisZ(0.0, 0.0, 1.0);
                SetOrientation(oldOrientation+eulerAnglesFromQuaternion(quatf::fromAxisAndAngle(axisZ, radToDeg*angle)));
            }
        }

        updateAABB();
        computeCenter();

        NEIndexedTriMesh &selTriMesh = triangleMesh();

        for (unsigned int i = 0; i < triangleMesh().numTriangles(); ++i)
        {
            vec3f _p1 = this->transform() * selTriMesh.vertices()[triangleMesh().triangles().at(i).p1].position;
            vec3f _p2 = this->transform() * selTriMesh.vertices()[triangleMesh().triangles().at(i).p2].position;
            vec3f _p3 = this->transform() * selTriMesh.vertices()[triangleMesh().triangles().at(i).p3].position;

            vec3d selNormal = vec3d::normal(vec3d(_p1), vec3d(_p2), vec3d(_p3));
            if (std::abs(vec3d::dotProduct(selNormal, otherNormalVec)) <= FEps)
            {
                selected.first = this;
                selected.second = i;
                break;
            }
        }
    }

    return minInterPoint;
}

void NESpatialMeshPrivate::computeCentroid()
{
    vec3f centroid = vec3f(0.0f, 0.0f, 0.0f);
    std::vector<NECTNPVertex>& vertices = S->triangleMesh().vertices();
    for (unsigned i = 0; i != vertices.size(); ++i)
    {
        vec3f& position = vertices[i].position;
        centroid += position;
    }
    centroid /= vertices.size();
    S->setCenter(centroid);
}

void NESpatialMeshPrivate::computeCenterOfPolyhedralMass()
{
    matrix33d moi;

    vec3f c;
    double v;
    computePolyhedralProperties(S->triangleMesh(), 1.0, v, c, moi);
    S->setCenter(c);
}

QString NESpatialMeshPrivate::OBJexpand_environment_variables(QString s)
{
    QString r(s);
    QRegExp env_var("\\$([A-Za-z0-9_]+)");
    int i;

    while((i = env_var.indexIn(r)) != -1)
    {
        QByteArray value(qgetenv(env_var.cap(1).toLatin1().data()));

        if(value.size() > 0)
        {
            r.remove(i, env_var.matchedLength());
            r.insert(i, value);
        }
        else
            break;
    }
    return r;
}

uint NESpatialMeshPrivate::createBoxMesh()
{
    m_pMeshContainer.createBoxMesh( S->Scale(), 0.0f, 0.0f );

    m_UnitOrigScale = 1.0;
    S->applyUnitScale(m_UnitScale, false);

    // notify that the mesh has been created
    meshCreated();

    return NE::kReturnSuccess;
}

uint NESpatialMeshPrivate::createCuboidMesh()
{
    m_pMeshContainer.createCuboidMesh( S->Color() );

    m_UnitOrigScale = 1.0;
    S->applyUnitScale(m_UnitScale, false);

    // notify that the mesh has been created
    meshCreated();

    return NE::kReturnSuccess;
}

void NESpatialMeshPrivate::meshCreated()
{
    if( S->FlipMesh())
        S->triangleMesh().flip();
}

uint NESpatialMeshPrivate::createCylinderMesh()
{
    //std::cout << "Axis " << S->CylAxis().x() << " " << S->CylAxis().y() << " " << S->CylAxis().z() << std::endl;

    m_pMeshContainer.createCylinderMesh( S->Scale(), S->Color(), 0.0f, 0.0f, S->TopCap(), S->BottomCap(), S->CylAxis() );

    m_UnitOrigScale = 1.0;
    S->applyUnitScale( m_UnitScale, false );

    // notify that the mesh has been created
    meshCreated();

    return NE::kReturnSuccess;
}

uint NESpatialMeshPrivate::createSphereMesh()
{
    m_pMeshContainer.createSphereMesh(S->Color());

    m_UnitOrigScale = 1.0;
    S->applyUnitScale(m_UnitScale, false);

    // notify that the mesh has been created
    meshCreated();

    return NE::kReturnSuccess;
}

uint NESpatialMeshPrivate::createCustomMesh()
{
    loadCADFile( m_CADFile.path() );

    // notify that the mesh has been created
    meshCreated();

    return NE::kReturnSuccess;
}

uint NESpatialMeshPrivate::createPlaneMesh()
{
    m_pMeshContainer.createPlaneMesh(  S->Color() );

    m_UnitOrigScale = 1.0;
    S->applyUnitScale(m_UnitScale, false);

    // notify that the mesh has been created
    meshCreated();

    return NE::kReturnSuccess;
}

void NESpatialMesh::renderPickable()
{
}

void NESpatialMesh::render()
{

}

void NESpatialMesh::renderPicked()
{
}

NESpatialMeshFactory::NESpatialMeshFactory(NEManager* m) : NENodeFactory(m)
{

}

NESpatialMeshFactory::~NESpatialMeshFactory()
{

}

QString NESpatialMeshFactory::nodeName()
{
    return "Spatial Mesh";
}

QStringList NESpatialMeshFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("Spatial Mesh");

    return aNames;
}

uint NESpatialMeshFactory::nodeType()
{
    return NE::kObjectGroup;
}

QString NESpatialMeshFactory::nodeInherits()
{
    return "Geometry";
}

QString NESpatialMeshFactory::objectName()
{
    return "SpatialMesh";
}

QString NESpatialMeshFactory::nodeVersion()
{
    return "0.1";
}

QString NESpatialMeshFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NESpatialMeshFactory::nodeHelp()
{
    return "Spatial Mesh";
}

NEObject* NESpatialMeshFactory::createInstance()
{
    return new NESpatialMesh(this);
}
