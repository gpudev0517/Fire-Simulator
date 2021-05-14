#include "NERenderManager.h"
#include "Base/NEGUICallInterface.h"

#if NE_VOLUME
#include "Volume/Core/NEVolumeNode.h"
#endif


#include "Base/NESceneManager.h"
#include "Base/NEMultiNodeFactory.h"

#include "Manager/NEGeometryManager.h"
#if NE_GRIDFLUID
#include "Grid/Physics/Spatial/Physical/Fluid/NEGridFluid.h"
#endif
#include "Spatial/NESpatialGroup.h"
#include "Base/NEBase.h"
#include "Utility/neutrino_config.h"
#if NE_DEFORMABLE
#include "Deformable/Physics/Spatial/Physical/Deformable/ParticleDeformable/NESphDeformable.h"
#endif

#include "PostProcessor/NEMeshPostProcessor.h"
#include "Rendering/Spatial/NECamera.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/Light/NEAreaLight.h"
#include "Rendering/Spatial/Light/NEImageBasedLight.h"
#include "Rendering/Spatial/Light/NESkyLight.h"
#include "Rendering/Resource/Material/NEPBRMaterial.h"
#include "Rendering/Resource/Material/NEPhongMaterial.h"
#include "Rendering/Resource/Material/NEStructureMaterial.h"
#include "Rendering/Resource/Material/NETerrainMaterial.h"
#include "Rendering/Resource/Material/NEToonMaterial.h"
#include "Rendering/Resource/Material/NEToneMaterial.h"
#include "Rendering/Resource/Material/NEOceanMaterial.h"
#include "Rendering/Resource/Material/NEClearWaterMaterial.h"
#include "Rendering/Resource/Material/NEVolumeMaterial.h"
#include "Rendering/Utility/RenderSettings/NEPovrayExport.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"

#include "Rendering/Spatial/NEOSDGlobalInformation.h"

#if NE_VOLUME
#include "Rendering/Spatial/NEOSDVolume.h"
#include "Rendering/Spatial/Renderers/NEVolumeRenderer.h"
#endif

#include "Rendering/Spatial/NEClipPlane.h"
#include "Rendering/Spatial/NENullGizmo.h"
#include "Rendering/Spatial/NEMeasurementItem.h"
#include "Rendering/Spatial/NEFloorPlane.h"
#include "Rendering/Spatial/NEInteractiveMeasurementGizmo.h"
#include "Rendering/Spatial/Renderers/NEObjectRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialGroupRenderer.h"
#include "Rendering/Spatial/Renderers/NECameraRenderer.h"
#include "Rendering/Spatial/Renderers/NELightsRenderer.h"
#include "Rendering/Spatial/Renderers/NESpatialMeshRenderer.h"
#include "Rendering/Spatial/Renderers/NEZoneRenderer.h"
#include "Rendering/Spatial/Renderers/NEZoneCompartmentRenderer.h"

#include "Rendering/Utility/RenderSettings/NERenderSetting.h"


using namespace std;

class NERenderManagerPrivate
{
public:
    NERenderManagerPrivate(NERenderManager* s) : S(s)
    {
        m_pGLManager = new NEGLManager;
    }

    QMap<QString, NECamera*> m_Cameras;         /**< Map of cameras */
    QMap<QString, NEMaterial*> m_Materials;     /**< Map of materials */
    QMap<QString, NELight*> m_Lights;           /**< Map of lights */
    QMap<QString, NEClipPlane*> m_ClipPlanes;           /**< Map of ClipPlanes */

    QMap<NESpatial*, NESpatialRenderer*> m_Renderers;          //!< Map from the generated objects to the renderer
    QMap<QString, NERendererFactory*> m_RendererFactories;  //!< Map from object type to the actual renderer factory

    void loadPlugins();
    void loadStatics();
    bool m_RenderSequence;                      /**< Are we rendering the whole sequence or a single frame */

    NELight* m_pDefaultLight;
    NEMaterial* m_pDefaultMaterial;     /**< Pointer to the defaut material */
    NECamera* m_pDefaultCamera;         /**< Pointer to the default perspective cam */
    NECamera* m_pTopCamera;             /**< Pointer to the default top cam */
    NECamera* m_pLeftCamera;            /**< Pointer to thr default left cam */
    NECamera* m_pFrontCamera;           /**< Pointer to the default front cam */

    NEGLManager* m_pGLManager;

    void initBasicSceneElements();
    uint initDefaultCameras();

    NERenderSetting m_pRenderSetting;

private:
    NERenderManager* S;
};



void NERenderManager::deInit()
{
    for(NENode* node : objects()){
        deleteChildNode(node);
        delete node;
    }
    objects().clear();
    for(NENodeFactory* factory : factories())
    {
        delete factory;
    }
    factories().clear();

    //for(int i = 0; i < )
    QMapIterator<NESpatial*, NESpatialRenderer*> i(P->m_Renderers);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
}

void NERenderManager::deleteChildNode(NENode* node)
{
    if(NESpatialGroup* gr = qobject_cast<NESpatialGroup*>(node)){
        for(QObject* obj : gr->children()){
            if(NENode* child = qobject_cast<NENode*>(obj))
            {
                deleteChildNode(child);
				objects().remove(obj->objectName());
                delete child;
            }
        }
    }
}

void NERenderManager::initRenderer()
{
    if(!NEBASE.guiMode()){
        m_offlineRenderSurface = new IGLSurface(true);
        m_offlineRenderSurface->initializeGLSurface();
        m_offlineRenderSurface->setGLSurfaceSize(1280, 720, 1);
    }
    P->m_pGLManager->init();
}

void NERenderManager::deInitRenderer()
{

    P->m_pGLManager->deInit();
    delete P->m_pGLManager;
}

NERenderManager& NERenderManager::Instance( std::shared_ptr< NERenderManager > renderManagerInstance )
{
    static std::shared_ptr< NERenderManager > instance;

    if( renderManagerInstance != 0 )
        instance = renderManagerInstance;

    return *instance;
}


unsigned int NERenderManager::update()
{
    unsigned int errorCode(0);
    return errorCode;
}

uint NERenderManager::init()
{
    setObjectName( "RENDERMAN" );

    emit managerCreated( this );

    P->loadStatics();
    P->loadPlugins();
    P->m_RenderSequence = false;
    m_bDrawGrid = true;
    AddPyObject( objectName(), this );

    QObject::connect( &NESCENE, SIGNAL( createRenderer( NESpatial* ) ), this, SLOT( createRenderer( NESpatial* ) ), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL( initRenderSceneElements() ), this, SLOT( initBasicSceneElements() ), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL( updateRenderManager() ), this, SLOT( update() ), Qt::DirectConnection );


    // Set the signal for initializing renderer
    QObject::connect( &NESCENE, &NESceneManager::initRenderer, this, &NERenderManager::initRenderer );
    QObject::connect( &NESCENE, &NESceneManager::deInitRenderer, this, &NERenderManager::deInitRenderer );
    QObject::connect( &NESCENE, &NESceneManager::updateOfflineRender, this, &NERenderManager::render );

    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( createDrawingBuffers_Signal(NEIndexedTriMesh*, uint) ),
                      GLManager(), SLOT( createDrawingBuffers(NEIndexedTriMesh*, uint) ) );
    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( rehintDrawingBuffers_Signal(NEIndexedTriMesh*, uint) ),
                      GLManager(), SLOT( rehintDrawingBuffers(NEIndexedTriMesh*, uint) ) );
    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( clearDrawingBuffers_Signal(NEIndexedTriMesh*) ),
                      GLManager(), SLOT( clearDrawingBuffers(NEIndexedTriMesh*) ) );
    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( updateDrawingBuffers_Signal(NEIndexedTriMesh*, uint) ),
                      GLManager(), SLOT( updateDrawingBuffers(NEIndexedTriMesh*, uint) ) );

    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( calcNormalsMWA_Signal(NEIndexedTriMesh*) ),
                      GLManager(), SLOT( calcNormalsMWA(NEIndexedTriMesh*) ) );
    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( calcNormalsMWASEL_Signal(NEIndexedTriMesh*) ),
                      GLManager(), SLOT( calcNormalsMWASEL(NEIndexedTriMesh*) ) );
    QObject::connect( NESCENE.geometryManager().get(), SIGNAL( calcNormalsMWE_Signal(NEIndexedTriMesh*) ),
                      GLManager(), SLOT( calcNormalsMWE(NEIndexedTriMesh*) ) );

    return NE::kReturnSuccess;
}

NERenderManager::NERenderManager(): NEManager(nullptr)
{
    P = new NERenderManagerPrivate(this);

    QObject::connect( &NESCENE, &NESceneManager::initRenderManager, this, &NERenderManager::init, Qt::DirectConnection );
    QObject::connect( &NESCENE, &NESceneManager::deInitRenderManager, this, &NERenderManager::deInit, Qt::DirectConnection );
}

NERenderManager::~NERenderManager()
{
    delete P;
}

NERenderSetting &NERenderManager::RenderSetting()
{
    return P->m_pRenderSetting;
}

void NERenderManagerPrivate::loadStatics()
{
    EXEC_IF(1, S->addStaticToMaps(new NECameraFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEPBRMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEPhongMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEStructureMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEToonMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEToneMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEOceanMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEClearWaterMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEVolumeMaterialFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NETerrainMaterialFactory(S)));
    //    EXEC_IF(NE_GLASS_MATERIAL, S->addStaticToMaps(new NEGlassMaterialFactory(S)));

    EXEC_IF(1, S->addStaticToMaps(new NEDirectionalLightFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEPointLightFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NESpotLightFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEAreaLightFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEImageBasedLightFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NESkyLightFactory(S)));
    EXEC_IF(1, S->addRendererToMaps(new NEDirectionalLightRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps(new NEPointLightRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps(new NEAreaLightRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps(new NESpotLightRendererFactory(S)) );


    //EXEC_IF(1, S->addStaticToMaps(new NESpatialLayerFactory(S)));

    EXEC_IF(1, S->addStaticToMaps( new NEOSDGlobalInformationFactory(S)) );

#if NE_VOLUME
    EXEC_IF(1, S->addRendererToMaps(new NEMeshToVolumeRendererFactory(S)));
    EXEC_IF(1, S->addRendererToMaps(new NEPartToVolumeRendererFactory(S)));
    EXEC_IF(1, S->addRendererToMaps(new NEVolumeCombineRendererFactory(S)));
    EXEC_IF(1, S->addRendererToMaps(new NEVolumeFileRendererFactory(S)));
    EXEC_IF(1, S->addRendererToMaps(new NEVolumeFilterRendererFactory(S)));
    EXEC_IF(1, S->addStaticToMaps( new NEOSDVolumeFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEOSDVolumeRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEVolumeRendererFactory(S)) );
#endif

    EXEC_IF(1, S->addStaticToMaps( new NEClipPlaneFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NEFloorPlaneFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NENullGizmoFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NEMeasurementItemFactory(S)) );
    EXEC_IF(1, S->addStaticToMaps( new NEInteractiveMeasurementGizmoFactory(S)) );

    EXEC_IF(1, S->addRendererToMaps( new NECameraRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NESpatialGroupRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEClipPlaneRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEFloorPlaneRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NENullGizmoRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEMeasurementItemRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEInteractiveMeasurementGizmoRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEOSDGlobalInformationRendererFactory(S)) );

    EXEC_IF(1, S->addRendererToMaps( new NESpatialMeshRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEZoneRendererFactory(S)) );
    EXEC_IF(1, S->addRendererToMaps( new NEZoneCompartmentRendererFactory(S)) );

}

void NERenderManagerPrivate::loadPlugins()
{
#ifdef _DEBUG
    S->addDynamicRendererToMaps("Plugins/Debug/Rendering/Spatial");
    S->addDynamicsToMaps("Plugins/Debug/Rendering/Material");
#else
    S->addDynamicRendererToMaps("Plugins/Release/Rendering/Spatial");
    S->addDynamicsToMaps("Plugins/Release/Rendering/Material");
#endif
}

void NERenderManagerPrivate::initBasicSceneElements()
{
    NESCENE.createObject("Directional Light", "DefaultLight");
    NESCENE.createObject("Phong Material", "DefaultMaterial");
    NESCENE.createObject("Toon Material", "DefaultInactiveMaterial");
    m_pDefaultLight     = S->lights()["DefaultLight"];
    m_pDefaultMaterial  = S->materials()["DefaultMaterial"];
    NEMaterial *pDefaultInactiveMaterial = S->materials()["DefaultInactiveMaterial"];
    m_pDefaultLight->setStatic(true);
    m_pDefaultMaterial->setStatic(true);
    pDefaultInactiveMaterial->setStatic(true);
    initDefaultCameras();
    //NERENDERMAN.GLManager()->updateAllShaders();
}



uint NERenderManagerPrivate::initDefaultCameras()
{
    NESCENE.createObjectOnLoad( "Camera", "CamPersp" );
    NESCENE.createObjectOnLoad( "Camera", "CamTop" );
    NESCENE.createObjectOnLoad( "Camera", "CamLeft" );
    NESCENE.createObjectOnLoad( "Camera", "CamFront" );

    m_pDefaultCamera = S->cameras()["CamPersp"];
    m_pTopCamera     = S->cameras()["CamTop"];
    m_pLeftCamera    = S->cameras()["CamLeft"];
    m_pFrontCamera   = S->cameras()["CamFront"];

    m_pDefaultCamera->setStatic( true );
    m_pTopCamera->setStatic( true );
    m_pLeftCamera->setStatic( true );
    m_pFrontCamera->setStatic( true );
    m_pDefaultCamera->SetFov( 45 );
    m_pDefaultCamera->init( NE::kInitOnLoad );

    m_pTopCamera->SetOrthoType(NECamera::OrthographicType::Top);
    m_pTopCamera->SetOrtho(true);
    m_pTopCamera->init(NE::kInitOnLoad );
    m_pTopCamera->resetFixedOrtho();

    m_pFrontCamera->SetOrthoType(NECamera::OrthographicType::Front);
    m_pFrontCamera->SetOrtho(true);
    m_pFrontCamera->init( NE::kInitOnLoad );
    m_pFrontCamera->resetFixedOrtho();

    m_pLeftCamera->SetOrthoType(NECamera::OrthographicType::Left);
    m_pLeftCamera->SetOrtho(true);
    m_pLeftCamera->init( NE::kInitOnLoad );
    m_pLeftCamera->resetFixedOrtho();

    return NE::kReturnSuccess;
}


void NERenderManager::initBasicSceneElements()
{
    P->initBasicSceneElements();
}



NECamera* NERenderManager::defaultCamera() const
{
    return P->m_pDefaultCamera;
}

NECamera* NERenderManager::topCamera() const
{
    return P->m_pTopCamera;
}

NECamera* NERenderManager::leftCamera() const
{
    return P->m_pLeftCamera;
}

NECamera* NERenderManager::frontCamera() const
{
    return P->m_pFrontCamera;
}

NEGLManager *NERenderManager::GLManager() const
{
    return P->m_pGLManager;
}

void NERenderManager::addDynamicRendererToMaps(QString pluginPath)
{
    QDir pluginsDir( pluginPath );
    QStringList nameFilters;
    nameFilters<<"*.dll"<<"*.so"<<"*.dylib";
    pluginsDir.setNameFilters( nameFilters );

    for( QString fileName : pluginsDir.entryList( QDir::Files ) )
    {
        QString absPath = pluginsDir.absoluteFilePath( fileName );
        QPluginLoader loader( absPath );
        NEMultiNodeFactory* mfactory = qobject_cast< NEMultiNodeFactory *>( loader.instance() );

        if( mfactory )
        {
            QList< NENodeFactory* >& factories = mfactory->factories();
            QList< NENodeFactory*>::iterator ii = factories.begin();
            for( ; ii != factories.end(); ++ii  )
            {
                NERendererFactory* factory = qobject_cast< NERendererFactory* >( *ii );
                if( factory )
                {
                    P->m_RendererFactories[(*ii)->nodeName()] = factory;
                }
            }
        }
        else
        {
            NERendererFactory* factory = qobject_cast< NERendererFactory* >( loader.instance() );
            if( factory )
            {
                QRegularExpression re("(\s)Rendering$");
                QString factorynodename = factory->nodeName().replace( re, "" );

                P->m_RendererFactories[factory->nodeName()] = factory;
            }
            else
                std::cerr << "Plugin loading error - Cannot load plugin " << absPath.toStdString() << std::endl;
        }

    }
}


void NERenderManager::addRendererToMaps(NERendererFactory* factory)
{
    P->m_RendererFactories[factory->nodeName()] = factory;
}

NENode* NERenderManager::createObject(QString type, QString name, const NEObject* parent)
{
    NENode* object = NEManager::createObject(type, name, parent);

    if(NECamera* tmpObject = qobject_cast<NECamera*> (object))
    {
        if( NEBASE.guiMode() )
            NEBASE.getGUICallInterface()->addCameraIntoContextMenus(object);
        P->m_Cameras[name] = tmpObject;
    }
    else if(NEMaterial* tmpObject = qobject_cast<NEMaterial*> (object))
    {
        P->m_Materials[name] = tmpObject;
        if(NEBASE.initialized())
            GLManager()->updateMaterialShaders(tmpObject);
    }
    else if(NELight* tmpObject = qobject_cast<NELight*> (object))
    {
        P->m_Lights[name] = tmpObject;
        if(NEBASE.initialized())
            GLManager()->updateAllShaders();
    }
    else if(NEClipPlane* tmpObject = qobject_cast<NEClipPlane*> (object))
    {
        P->m_ClipPlanes[name] = tmpObject;
        if(NEBASE.initialized())
            GLManager()->updateAllShaders();
    }
    else if( type == "Interactive Measurement Gizmo")
    {
        NENode* group = NESCENE.createObject("Group", QString(), parent);
        object->setParent(group);
        NENode* null1 = NESCENE.createObject( "Null Gizmo", "NullGizmo_Interactive1", group );
        NENode* null2 = NESCENE.createObject( "Null Gizmo", "NullGizmo_Interactive2", group );
        null1->connectTo( object, NE::kIOGizmo );
        null2->connectTo( object, NE::kIOGizmo );
    }

    return object;
}

//! Deletes an object and removes all dependencies.
NENode* NERenderManager::deleteObject(const QString& type , const QString& name, uint trashIndex)
{
    // remove the object from the objects() map and get the object
    NENode* object = NEManager::deleteObject(type, name, trashIndex);

    //
    if(NECamera* tmpObject = qobject_cast<NECamera*> (object))
    {
        NEBASE.getGUICallInterface()->removeCameraFromContextMenus(object);
        P->m_Cameras.remove(name);
    }
    else if(qobject_cast<NEMaterial*> (object))
    {
        P->m_Materials.remove(name);
    }
    else if(qobject_cast<NELight*> (object))
    {
        P->m_Lights.remove(name);
        GLManager()->updateAllShaders();
    }
    else if(qobject_cast<NEClipPlane*> (object))
    {
        P->m_ClipPlanes.remove(name);
        GLManager()->updateAllShaders();
    }
    return object;
}

NENode* NERenderManager::deleteObject(const QString& type , const QString& name)
{
    // remove the object from the objects() map and get the object
    NENode* object = NEManager::deleteObject(type, name);

    //
    if(NECamera* tmpObject = qobject_cast<NECamera*> (object))
    {
        NEBASE.getGUICallInterface()->removeCameraFromContextMenus(object);
        P->m_Cameras.remove(name);
    }
    else if(qobject_cast<NEMaterial*> (object))
    {
        P->m_Materials.remove(name);
    }
    else if(qobject_cast<NELight*> (object))
    {
        P->m_Lights.remove(name);
        GLManager()->updateAllShaders();
    }
    else if(qobject_cast<NEClipPlane*> (object))
    {
        P->m_ClipPlanes.remove(name);
        GLManager()->updateAllShaders();
    }
    return object;
}


NESpatialRenderer* NERenderManager::createRenderer(NESpatial* spatial)
{
    // Create a renderer
    NESpatialRenderer* newRenderer = NULL;
    QMap<NESpatial*, NESpatialRenderer*>::iterator i = P->m_Renderers.find(spatial);
    if(i != P->m_Renderers.end())
        return NULL;

    QString type = spatial->factory().nodeName();

    //QRegularExpression re("(\s)Renderer$");
   // QString factorynodename = type.replace( re, "" );
    QString factorynodename = type + " Renderer";


    if( P->m_RendererFactories.find( factorynodename ) == P->m_RendererFactories.end() )
        return NULL;


    newRenderer = qobject_cast<NESpatialRenderer*>(P->m_RendererFactories[factorynodename]->createInstance());
    newRenderer->setSpatial(spatial);

    if( NEBASE.guiMode() )
    {
        if( spatial->metaObject()->indexOfSignal("createVisualizer") != -1)
            QObject::connect( spatial,
                              SIGNAL( createVisualizer( NESpatial*)), newRenderer,
                              SLOT( createVisualizer(NESpatial*)), Qt::DirectConnection);
    }

    P->m_Renderers[spatial] = newRenderer;

    return newRenderer;
}

uint NERenderManager::exportFrameToExternalRenderer(long long glWidgets, bool startFrame, bool renderNow)
{
    if(RenderSetting().Renderer() == NERenderSetting::OpenGL){

        QList<IGLSurface *> *pGLWidgets = (QList<IGLSurface *> *)glWidgets;

        if(NEBASE.sceneName() == "untitled")
            return NE::kReturnFail;

        QString sceneDataPath = NEBASE.sceneDataPath();
        QString outDir = sceneDataPath + "/" + RenderSetting().OutputDir() + "GL/";
        QDir dir(outDir);
        if(!dir.exists())
        {
            dir.mkpath(outDir);
        }
        QString fileName = NEBASE.sceneName() + "-" + RenderSetting().Camera() + "-" + QString::number(NESCENE.CurrentFrame()) + ".png";
        QString path = outDir + fileName;
        renderOffline(pGLWidgets->at(0), RenderSetting().Camera(), RenderSetting().Resolution(), path, RenderSetting().FrameRate(), RenderSetting().SpeedUp());
        return NE::kReturnSuccess;
    }


    // get directory and file path

    QString outDir = RenderSetting().OutputDir();
    QString fileName = RenderSetting().outputFileName( NESCENE.CurrentFrame() );
    QString path = outDir+fileName;

    std::cout << "render path : " << path.toStdString() << std::endl;

    // create file
    QFile fout(path);

    if( RenderSetting().Renderer() != NERenderSetting::Vtk )
    {
        if(!fout.open(QFile::WriteOnly | QFile::Truncate))
            return NE::kReturnFail;
    }

    //
    // Create a data stream for the file
    //

    QTextStream out(&fout);
    out.setCodec("UTF-8");

    //
    // Gather all meshes
    //
    QMapIterator< uint, NESpatial* > i(NESCENE.spatials());
    QString frameNumber;
    frameNumber.sprintf("%06d", NESCENE.CurrentFrame());

    {
        bool ibl = false;

        QString objectFileName;

        //
        // Then, first output the light headers
        //
        for(NELight* light : lights())
        {
            if(light->factory().nodeName() == "Image Based Light")
                ibl = true;

            if(light->Behavior() == NENode::Active && light->ExternalRender())
            {
                if(RenderSetting().Renderer() == NERenderSetting::PovRay)
                    objectFileName = NEPOVEXP.exportLight(light, outDir);

                if(!objectFileName.isEmpty())
                    out << RenderSetting().includeFile(objectFileName);
            }

        }
        out << "\n";

        std::vector<QString> Meshes;
        std::vector<QString> Fluids;
        std::vector<QString> Rigids;
        Meshes.resize(0);
        Fluids.resize(0);
        Rigids.resize(0);
        int nrOfObjects;
        nrOfObjects = 0;

        //Secondly, output other meshes
        while(i.hasNext())
        {
            i.next();
            NESpatial* spatial = i.value();
            QString objName = spatial->objectName();

            if(qobject_cast<NESpatial*> (spatial) && spatial->ExternalRender())
            {

                NESpatial* rigid = qobject_cast<NESpatial*> (spatial);
                NEMaterial* material = GLManager()->material( const_cast< NEIndexedTriMesh* >( &rigid->triangleMesh() ));

                if(RenderSetting().Renderer() == NERenderSetting::PovRay)
                {
                    objectFileName = NEPOVEXP.exportRigid(startFrame, rigid, outDir);
                }
#if NE_VTK
                else if(RenderSetting().Renderer() == NERenderSetting::VTK)
                {
                    objectFileName = NEVTKEXP.exportRigid(startFrame, rigid, outDir);
                }
#endif
                if(!objectFileName.isEmpty())
                {
                    matrix44f t = spatial->transform();

                    out << RenderSetting().includeFile(objectFileName);

                    if( RenderSetting().Renderer() == NERenderSetting::PovRay )
                    {
                        out << "object {\n";
                        out << "	mesh_" << objName << "\n";
                        out << "	matrix <" << t.column(0).x() << "," << t.column(0).y() << "," << t.column(0).z() << ",\n";
                        out << "			" << t.column(1).x() << "," << t.column(1).y() << "," << t.column(1).z() << ",\n";
                        out << "			" << t.column(2).x() << "," << t.column(2).y() << "," << t.column(2).z() << ",\n";
                        out << "			" << t.column(3).x() << "," << t.column(3).y() << "," << t.column(3).z() << ">\n";
                        out << "		material {" << objName << "_material} photons {target reflection on refraction on}\n";
                        out << "}\n";
                    }

                }
            }
#if NE_DEFORMABLE
            else if(qobject_cast<NESphDeformable*> (spatial) && spatial->ExternalRender())
            {

                NESphDeformable* deformable = qobject_cast<NESphDeformable*> (spatial);
                NEMaterial* material = GLManager()->material( deformable->triangleMesh() );


                if(RenderSetting().Renderer() == NERenderSetting::PovRay)
                {
                    objectFileName = NEPOVEXP.exportDeformable(deformable, outDir);
                }
#if NE_VTK
                else if(RenderSetting().Renderer() == NERenderSetting::VTK)
                {
                    objectFileName = NEVTKEXP.exportDeformable(deformable, outDir);
                }
#endif


                if(!objectFileName.isEmpty())
                {
                    matrix44f t = spatial->transform();

                    out << RenderSetting().includeFile(objectFileName);

                    if(RenderSetting().Renderer() == NERenderSetting::PovRay)
                    {
                        out << "object {\n";
                        out << "	mesh_" << objName << "\n";
                        out << "	matrix <" << t.column(0).x() << "," << t.column(0).y() << "," << t.column(0).z() << ",\n";
                        out << "			" << t.column(1).x() << "," << t.column(1).y() << "," << t.column(1).z() << ",\n";
                        out << "			" << t.column(2).x() << "," << t.column(2).y() << "," << t.column(2).z() << ",\n";
                        out << "			" << t.column(3).x() << "," << t.column(3).y() << "," << t.column(3).z() << ">\n";
                        out << "		material {" << objName << "_material} photons {target reflection on refraction on}\n";
                        out << "}\n";
                    }

                }
            }
#endif
            else if(qobject_cast<NEMeshPostProcessor*> (spatial))
            {
                if(spatial->ExternalRender())
                {

                    if( RenderSetting().Renderer() == NERenderSetting::PovRay )
                        objectFileName = NEPOVEXP.exportPostprocessedSurface(qobject_cast<NEMeshPostProcessor*> (spatial), outDir);
#if NE_VTK
                    else if( RenderSetting().Renderer() == NERenderSetting::VTK )
                        objectFileName = NEVTKEXP.exportPostprocessedSurface(qobject_cast<NEMeshPostProcessor*> (spatial), outDir);
#endif
                    if(!objectFileName.isEmpty())
                        out << RenderSetting().includeFile(objectFileName);
                }
            }
#if 0
            else if(qobject_cast<NEParticleFluidSolver*> (spatial))
            {
                if(spatial->ExternalRender())
                {
                    if( RenderSetting().Renderer() == NERenderSetting::PovRay )
                        objectFileName = NEPOVEXP.exportParticleFluid(qobject_cast<NEParticleFluidSolver*> (spatial), outDir);
#if NE_VTK
                    else if( RenderSetting().Renderer() == NERenderSetting::VTK )
                    {
                        objectFileName = NEVTKEXP.exportParticleFluid(qobject_cast<NEParticleFluidSolver*> (spatial), outDir);
                    }
#endif

                    if(!objectFileName.isEmpty())
                        out << RenderSetting().includeFile(objectFileName);
                }
            }
#endif
#if NE_VTK
            /*else if(qobject_cast<NEVolumeNode*> (spatial) && spatial->ExternalRender())
            {
                if( RenderSetting().Renderer() == NERenderSetting::VTK )
                    objectFileName = NEVTKEXP.exportVolume( qobject_cast<NEVolumeNode*> (spatial), outDir );
            }*/
            if(qobject_cast<NEMeasurementField*> (spatial) && spatial->ExternalRender())
            {
                if( RenderSetting().Renderer() == NERenderSetting::VTK )
                    objectFileName = NEVTKEXP.exportMeasurement( qobject_cast<NEMeasurementField*> (spatial), outDir );
            }
#endif
        }
        out << "\n";



        //get camera
        if( RenderSetting().camera() )
        {
            NECamera& cam = *(RenderSetting().camera());

            if(RenderSetting().Renderer() == NERenderSetting::PovRay)
            {
                vec3f p = cam.Position();
                vec3f l = cam.LookAt();
                float fov = cam.Fov();

                out << "camera\n";
                out << "{\n";
                out << "	location <" << p.x() << ", " << p.y() << ", " << p.z() << ">\n";
                out << "	look_at <" << l.x() << ", " << l.y() << ", " << l.z() << ">\n";
                out << "	angle " << fov << "\n";
                out << "	right -image_width/image_height*x\n";
                out << "}\n";

            }
        }
        out << "\n";

        //create static header
        QString staticHeaderName = RenderSetting().staticIncludes(outDir);
        out << RenderSetting().includeFile(staticHeaderName);
        out << "\n";



        fout.close();

        //render frame
        if(renderNow)
        {
            //Render the frame directly and not store the command in a batch file
            QString renderCommand = RenderSetting().renderFrame(outDir + fileName);
            Log() << "Render cmd:" << renderCommand;
            EndLog(NE::kLogInfo);
        }
    }
    return NE::kReturnSuccess;
}

uint NERenderManager::exportSequenceToExternalRenderer()
{
    return NE::kReturnSuccess;
}

uint NERenderManager::exportSequenceToExternalRenderer(long long glWidgets, long long  nrenderCameras, long long  nrenderRects, int tWidth, int tHeight, int startFrame, int endFrame)
{
    QList<IGLSurface *> *pGLWidgets = (QList<IGLSurface *> *)glWidgets;
    QList<QString> *renderCameras = (QList<QString> *)nrenderCameras;
    QList<QRect> *renderRects = (QList<QRect> *)nrenderRects;

    if(RenderSetting().Renderer() == NERenderSetting::OpenGL && renderCameras->size() == 1)
    {
        if(NEBASE.sceneName() == "untitled")
            return NE::kReturnFail;

        QString sceneDataPath = NEBASE.sceneDataPath();
        QString outDir = sceneDataPath + "/" + RenderSetting().OutputDir() + "GL/";
        QDir dir(outDir);
        if(!dir.exists())
        {
            dir.mkpath(outDir);
        }
        QString fileName = NEBASE.sceneName() + "_" + RenderSetting().Camera() + "_" + QString::number(RenderSetting().FrameRate()) + "_" + QString::number(RenderSetting().SpeedUp()) + "_" + QString::number(startFrame) + "-" + QString::number(endFrame) + ".avi";
        QString path = outDir + fileName;
        renderOffline(pGLWidgets->at(0), RenderSetting().Camera(), RenderSetting().Resolution(), path, RenderSetting().FrameRate(), RenderSetting().SpeedUp());
        return NE::kReturnSuccess;
    }
    else if(RenderSetting().Renderer() == NERenderSetting::OpenGL && renderCameras->size() > 1)
    {
        if(NEBASE.sceneName() == "untitled")
            return NE::kReturnFail;

        QString sceneDataPath = NEBASE.sceneDataPath();
        QString outDir = sceneDataPath + "/" + RenderSetting().OutputDir() + "GL/";
        QDir dir(outDir);
        if(!dir.exists())
        {
            dir.mkpath(outDir);
        }

        if(RenderSetting().ToggleSeparate())
        {
            QList<QString> paths;
            QList<QSize> resolutions;
            for(int i = 0; i < renderCameras->size(); i++){
                QString fileName = NEBASE.sceneName() + "_" + renderCameras->at(i) + "_" + QString::number(RenderSetting().FrameRate()) + "_" + QString::number(RenderSetting().SpeedUp())
                        + "_" + QString::number(startFrame) + "-" + QString::number(endFrame)+"_separated_" + QString::number(i) + ".avi";
                QString path = outDir + fileName;
                paths.push_back(path);
                resolutions.push_back(renderRects->at(i).size());
            }
            renderOffline(*pGLWidgets, *renderCameras, resolutions, paths, RenderSetting().FrameRate(), RenderSetting().SpeedUp());
        }
        else
        {

            QString fileName = NEBASE.sceneName() + "_"  + QString::number(RenderSetting().FrameRate()) + "_" + QString::number(RenderSetting().SpeedUp())
                    + "_" + QString::number(startFrame) + "-" + QString::number(endFrame)+"_merged" + ".avi";
            QString path = outDir + fileName;

            renderOffline(*pGLWidgets, *renderCameras, *renderRects, QSize(tWidth, tHeight), path, RenderSetting().FrameRate(), RenderSetting().SpeedUp());

        }
        return NE::kReturnSuccess;
    }

    setRenderSequence( true );

    //create a batch file
    QString outDir = RenderSetting().OutputDir();
    QFile fout(outDir + "batchRender.bat");

    if(!(RenderSetting().Renderer() != NERenderSetting::Vtk))
    {
        if(!fout.open(QFile::WriteOnly | QFile::Truncate))
            return NE::kReturnFail;
        // Create a data stream for the file
        QTextStream out(&fout);
        out.setCodec("UTF-8");

        out << "set numthreads=" << RenderSetting().Threads() << "\n";

        for(int i = startFrame; i <= endFrame; ++i)
        {
            QString fileName = RenderSetting().outputFileName(i);
            //Get the render command and store it in a batch file
            QString renderCommand = RenderSetting().renderFrame(fileName, true);
            out << "@echo ########### RENDER FRAME " << i << " ###########\n";
            out << renderCommand << " -v 2 \n";
        }

        out.flush();
        fout.close();
    }

    for(int i = startFrame; i <= endFrame; i++)
    {
        //
        // set frame
        //
        NESCENE.setCurrentFrame(i);
        // synchronize timeline
        // NEGUI.setCurrentFrame(i);
        // load data
        NESCENE.loadFrameData();
        NESCENE.updateKeyFramedProperties();

//        // export frame with external renderer
//        if(i == startFrame)
//            exportFrameToExternalRenderer(true, false);
//        else
//            exportFrameToExternalRenderer(false, false);

        //
        // Refresh gl output
        //
        //emit updateGLWindows(); // ???
        //NEGUI.updateGLWindows();
        //if( NEBASE.guiMode() )
        //    NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
    }

    setRenderSequence( false );
    return NE::kReturnSuccess;
}

uint NERenderManager::exportSequenceRenderDelete(int startFrame, int endFrame)
{
    for(int i = startFrame; i <= endFrame; i++)
    {
        NESCENE.setCurrentFrame(i);
        NESCENE.loadFrameData();
        NESCENE.updateKeyFramedProperties();

//        //
//        // Export frame with external renderer
//        //
//        if(i == startFrame)
//            exportFrameToExternalRenderer(true, true);
//        else
//            exportFrameToExternalRenderer(false, true);

        //
        // Refresh gl output
        //
        //emit updateGLWindows(); // ???
        //NEGUI.updateGLWindows();
        //if( NEBASE.guiMode() )
        //    NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
    }


    return NE::kReturnSuccess;
}

/*!
    Exports the scene to PBRT
*/
uint NERenderManager::exportToPBRT(void)
{
#if 0
    cout << "export PBRT" << std::endl;

    QString outDir = "";
    QString fileName = "pbrtTest.pbrt";

    // Create file
    QFile fout(outDir + fileName);
    if(!fout.open(QFile::WriteOnly | QFile::Truncate))
        return NE::kReturnFail;
    // Create a data stream for the file
    QTextStream out(&fout);
    out.setCodec("UTF-8");

    // Gather all meshes
    QMapIterator<uint, NESpatial*> i(NESCENE.spatials());
    QString frameNumber;
    frameNumber.sprintf("%06d", NESCENE.CurrentFrame());

    // Output the camera
    if( RenderSetting().camera() )
    {
        NECamera& cam = *(RenderSetting().camera());

        //
        vec3f p = cam.Position();
        vec3f l = cam.LookAt();
        float fov = cam.Fov();

        out << "LookAt " << p.x() << " " << p.y() << " " << -p.z() << "  " <<
               l.x() << " " << l.y() << " " << -l.z() << " 0 1 0" << "\n";
        out << "Camera \"perspective\" \"float fov\" [" << fov << "]\n";

        out << "Sampler \"bestcandidate\" \"integer pixelsamples\" [" << 15 << "]\n";

        out << "PixelFilter \"triangle\"\n\n";

        out << "VolumeIntegrator \"single\" \"float stepsize\" [" << "0.025" << "]\n";

        //
        // Begin scene
        //

        out << "\nWorldBegin\n\n";

        // Add a test light if there are no other defined
        if(lights().size() == 0)
        {
            out << "AttributeBegin\n";
            out << "LightSource \"spot\" \"point from\" [0 50 0] \"point to\" [0 2 0 ]\n";
            out << "\t\"color I\" [500 500 500] \"float coneangle\" [45]\n";
            out << "AttributeEnd\n\n";
        }

        // Then, first output the light headers
        for(NELight* light : lights())
        {
            if(light->Behavior() == NENode::Active && light->ExternalRender())
            {
                if(light->factory().objectName() == "SpotLight")
                {
                    const NESpotLight* sl = static_cast<const NESpotLight*>(light);
                    out << "AttributeBegin\n";

                    out << "LightSource \"spot\" \"point from\" [" << light->Position().x() <<  " " << light->Position().y() << " " << -light->Position().z() << "] \"point to\" ["
                        << sl->LookAt().x() << " " << sl->LookAt().y() << " " << -sl->LookAt().z() << "]\n";
                    out << "\t\"color I\" [" << sl->PbrtColorI().x() << " " << sl->PbrtColorI().y() << " " << sl->PbrtColorI().z() << "] \"float coneangle\" [" << sl->Cutoff() << "]\n";

                    /*if( light->factory().objectName() =="AreaLight" )
                        return exportAreaLight(light, path);
                    else if( light->factory().objectName() =="PointLight" )
                        return exportPointLight(light, path);
                    else*/

                    out << "AttributeEnd\n\n";
                }
                if(light->factory().objectName() == "AreaLight")
                {
                    const NEAreaLight* al = static_cast<const NEAreaLight*>(light);
                    out << "AttributeBegin\n";

                    out << "\tTranslate " << al->Position().x() << " " << al->Position().y() << " " << -al->Position().z() << "\n";

                    out << "AttributeEnd\n\n";

                }
        }
        out << "\n";


        QMapIterator<uint, NESpatial*> i(NESCENE.spatials());
        while(i.hasNext())
        {
            i.next();
            NESpatial* spatial = i.value();
            QString objName = spatial->objectName();

            // Export the density data
#if NE_GRIDFLUID
            NEGridFluid* gridFluid = qobject_cast<NEGridFluid*> (spatial);
            if(gridFluid && spatial->ExternalRender())
            {
                ExportDensityData* densityData = gridFluid->exportDensityData();
                if(densityData && densityData->getResolutionX() > 0 && densityData->getResolutionY() > 0 && densityData->getResolutionZ() > 0)
                {
                    QString fileName_fog = "fog_.pbrt";/*+ QString::number(currentFrame())*/
                    //out << "Include \"geometry/density_render.60.pbrt\"\n" <<
                    out << "Include \"" + fileName_fog + "\"\n" <<
                           "\t\"color sigma_a\" [" << densityData->getAbsorptionCoefficient() << " " <<
                           densityData->getAbsorptionCoefficient() << " " <<
                           densityData->getAbsorptionCoefficient() << "] \"color sigma_s\" [" <<
                           densityData->getScatteringCoefficient() << " " <<
                           densityData->getScatteringCoefficient() << " " <<
                           densityData->getScatteringCoefficient() << "]\n" <<
                           "\t\"color Le\" [0.25 0.25 0.25]\n\n";

                    //create file
                    QFile fout_fog(outDir + fileName_fog);
                    if(!fout_fog.open(QFile::WriteOnly | QFile::Truncate))
                        return NE::kReturnFail;
                    // Create a data stream for the file
                    QTextStream out_fog(&fout_fog);
                    out_fog.setCodec("UTF-8");

                    out_fog << "Volume \"volumegrid\" \"integer nx\" " << densityData->getResolutionX() << " \"integer ny\" " << densityData->getResolutionY() << " \"integer nz\" " << densityData->getResolutionZ() << "\n";
                    out_fog << "\t\"point p0\" [ " << densityData->m_Boundary_p1.x() << " " << densityData->m_Boundary_p1.y() << " " << -densityData->m_Boundary_p1.z() << " ] \"point p1\" [ " << densityData->m_Boundary_p2.x() << " " << densityData->m_Boundary_p2.y() << " " << -densityData->m_Boundary_p2.z() << " ]\n";
                    out_fog << "\t\"float density\" [\n";

                    for(int i = 0; i < densityData->getResolutionZ(); ++i)
                    {
                        for(int j = 0; j < densityData->getResolutionY(); ++j)
                        {
                            for(int k = 0; k < densityData->getResolutionX(); ++k)
                            {
                                out_fog << densityData->getDataSmoothed(k, j, densityData->getResolutionZ() - i - 1) << " ";
                            }
                            out_fog << "\n";
                        }
                    }
                    out_fog << "]\n";
                }
            }
#endif
            // Export the meshes
            NESphRigid* rigid = qobject_cast<NESphRigid*> (spatial);
            if(rigid && spatial->ExternalRender() )
            {
                QString name = rigid->objectName();
                QColor color1 = rigid->Color();

                cout << name.toStdString() << std::endl;

                QString fileName = name + ".mi";

                out << "AttributeBegin\n";
                double r, g, b, a;
                color1.getRgbF(&r, &g, &b, &a);

                out << "\tMaterial \"matte\" \"color Kd\" [" << r  << " " << g << " " << b << "]\n";
                out << "\tTranslate " << rigid->Position().x() << " " << rigid->Position().y() << " " << -rigid->Position().z() << "\n";
                out << "\tScale " << rigid->Scalex() << " " << rigid->Scaley() << " " << rigid->Scalez() << "\n";
                out << "\tRotate " << -rigid->Orientation().phi() << " 1 0 0\n";
                out << "\tRotate " << rigid->Orientation().theta() << " 0 1 0\n";
                out << "\tRotate " << rigid->Orientation().psi() << " 0 0 1\n";
                out << "\t\tInclude \"" << fileName.toStdString().c_str() << "\"\n";

                out << "AttributeEnd\n";

                // Save the mesh points


                QFile fout_mesh(outDir + fileName);
                if(!fout_mesh.open(QFile::WriteOnly | QFile::Truncate))
                    return NE::kReturnFail;

                QTextStream out_mesh(&fout_mesh);
                out_mesh.setCodec("UTF-8");

                // Save vertices
                out_mesh << "Shape \"trianglemesh\" \"point P\"\n";
                out_mesh << "\t[";
                for(uint vId = 0; vId < rigid->triangleMesh().numVertices(); vId++)
                {
                    const vec3f& v = rigid->triangleMesh().vertices()[vId].position;
                    out_mesh << "  " << v.x() << " " << v.y() << " " << -v.z();
                }
                out_mesh << "]\n";

                // Save triangle indices
                out_mesh << "\t\"integer indices\"\n";
                out_mesh << "\t[";
                for(unsigned int tId = 0; tId < rigid->triangleMesh().numTriangles(); tId++)
                {
                    const NETriangleIndexed& t = rigid->triangleMesh().triangles()[tId];
                    //if(invertNormals)
                    out_mesh << "  " << t.p1 << " " << t.p3 << " " << t.p2;
                    //else
                    //ostr <<"		"<<"p " <<t.p1 <<" " <<t.p2 <<" " <<t.p3 <<"\n";
                }
                out_mesh << "]\n";

                out << "\n";
            }
        }

        // End the export
        out << "\nWorldEnd\n";
        }
        out << "\n";
    }
#endif
    return NE::kReturnSuccess;

}

NESpatialRenderer* NERenderManager::getRenderer(NESpatial* spatial)
{
    QMap<NESpatial*, NESpatialRenderer*>::iterator i = P->m_Renderers.find(spatial);
    if(i != P->m_Renderers.end())
        return i.value();
    return createRenderer(spatial);
}

const QMap<QString, NECamera*>&NERenderManager::cameras() const
{
    return P->m_Cameras;
}

const QMap<QString, NEMaterial*>&NERenderManager::materials() const
{
    return P->m_Materials;
}

const QMap<QString, NELight*>&NERenderManager::lights() const
{
    return P->m_Lights;
}

const QMap<QString, NEClipPlane*>&NERenderManager::clipPlanes() const
{
    return P->m_ClipPlanes;
}



bool NERenderManager::renderSequence() const
{
    return P->m_RenderSequence;
}

void NERenderManager::setRenderSequence(bool val) { P->m_RenderSequence = val; }

void NERenderManager::setDrawGrid(bool val){m_bDrawGrid = val;}

void NERenderManager::renderScenePerFrame(IGLSurface* surface, float width, float height, float devPixelRation)
{
    GLManager()->renderScenePerFrame(surface, width, height, devPixelRation);
}
#include <QSize>

//
// Adds a unique suffix to a file name so no existing file has the same file
// name. Can be used to avoid overwriting existing files. Works for both
// files/directories, and both relative/absolute paths. The suffix is in the
// form - "path/to/file.tar.gz", "path/to/file_v1.tar.gz",
// "path/to/file_v2.tar.gz", etc.
//
QString addUniqueSuffix(const QString &fileName)
{
    // If the file doesn't exist return the same name.
    if (!QFile::exists(fileName)) {
        return fileName;
    }

    QFileInfo fileInfo(fileName);
    QString ret;

    // Split the file into 2 parts - dot+extension, and everything else. For
    // example, "path/file.tar.gz" becomes "path/file"+".tar.gz", while
    // "path/file" (note lack of extension) becomes "path/file"+"".
    QString secondPart = fileInfo.completeSuffix();
    QString firstPart;
    if( !secondPart.isEmpty() )
    {
        secondPart = "." + secondPart;
        firstPart = fileName.left(fileName.size() - secondPart.size());
    } else
    {
        firstPart = fileName;
    }

    // Try with an ever-increasing number suffix, until we've reached a file
    // that does not yet exist.
    for (int ii = 1; ; ii++)
    {
        // Construct the new file name by adding the unique number between the
        // first and second part.
        ret = QString("%1_v%2%3").arg(firstPart).arg(ii).arg(secondPart);
        // If no file exists with the new name, return it.
        if( !QFile::exists(ret) )
        {
            return ret;
        }
    }
}

void NERenderManager::render()
{
    QString sceneDataPath = NEBASE.sceneDataPath();
    QString cameraName = RenderSetting().Camera();
    QString outDir = sceneDataPath + "/" + RenderSetting().OutputDir() + "GL/";
    QDir qoutDir( outDir );

    if( !qoutDir.exists() )
    {
      qoutDir.mkdir(".");
    }

    NESCENE.setCurrentFrame( NESCENE.PlaybackStartFrame() );
    NESCENE.setSolveMode( false );

    int startFrame = NESCENE.PlaybackStartFrame();
    int endFrame = NESCENE.PlaybackEndFrame();
    QString fileName = NEBASE.sceneName() + "_" + RenderSetting().Camera() + "_" +
            QString::number(RenderSetting().FrameRate()) + "_" +
            QString::number(RenderSetting().SpeedUp()) + "_" +
            QString::number(startFrame) + "-" + QString::number(endFrame) + ".avi";
    QString path = outDir + fileName;
    path = addUniqueSuffix( path );

    //std::cout << "File NAME " << fileName.toStdString() << std::endl;
    NECamera* cameraObject = qobject_cast< NECamera *>( NESCENE.findChildNode( cameraName ) );
    NEGLOfflineRenderer *offlineRenderer = new NEGLOfflineRenderer(cameraObject,
                                                                   RenderSetting().Resolution().width(),
                                                                   RenderSetting().Resolution().height(),
                                                                   path,
                                                                   RenderSetting().FrameRate(),
                                                                   RenderSetting().SpeedUp());
    //offlineRenderer->copyPropertiesFromWidget( surface) ;

    if(!offlineRenderer->isOneFrame())
    {

        m_offlineRenderers.push_back(offlineRenderer);
        NESCENE.startUpdateTimer(true);
        NESCENE.setExportOfflineRendering(true);
    }
    else
    {
        offlineRenderer->renderOneFrame();
    }
}

void NERenderManager::renderOffline(IGLSurface *surface, QString cameraName,
                                    QSize resolution, QString path, float framerate, float speed)
{
    NEGLOfflineRenderer *offlineRenderer = new NEGLOfflineRenderer(cameras()[cameraName], resolution.width(),
                                                                   resolution.height(), path, framerate, speed);
    offlineRenderer->copyPropertiesFromWidget(surface);

    if(!offlineRenderer->isOneFrame())
    {

        m_offlineRenderers.push_back(offlineRenderer);
        NESCENE.startUpdateTimer(true);
        NESCENE.setExportOfflineRendering(true);
    }
    else{
        offlineRenderer->renderOneFrame();
    }
}

void NERenderManager::renderOffline(QList<IGLSurface *> surfaces, QList<QString> cameraNames,
                                    QList<QSize> resolutions, QList<QString> paths, float framerate, float speed)
{
    for(int i = 0; i < cameraNames.size();i ++)
    {
        QString cameraName = cameraNames[i];
        QSize resolution = resolutions[i];
        QString path = paths[i];

        NEGLOfflineRenderer *offlineRenderer = new NEGLOfflineRenderer(cameras()[cameraName],resolution.width(),
                                                                       resolution.height(), path, framerate, speed);
        offlineRenderer->copyPropertiesFromWidget(surfaces[i]);
        if(!offlineRenderer->isOneFrame())
            m_offlineRenderers.push_back(offlineRenderer);
        else
            offlineRenderer->renderOneFrame();
    }

    NESCENE.startUpdateTimer(true);
    NESCENE.setExportOfflineRendering(true);
}

void NERenderManager::renderOffline(QList<IGLSurface *> surfaces, QList<QString> cameraNames,
                                    QList<QRect> layouts,
                                    QSize totalResolution, QString path, float framerate, float speed)
{

    FFMpegMovieMerger *mergeRecorder = new FFMpegMovieMerger;

    mergeRecorder->m_oStream = { 0 };
    mergeRecorder->m_fileName = path;
    mergeRecorder->m_videoCodec = NULL;
    mergeRecorder->m_frameRate = framerate;
    mergeRecorder->m_width = totalResolution.width();
    mergeRecorder->m_height = totalResolution.height();
    mergeRecorder->m_bMerge = true;

    for(int i = 0; i < cameraNames.size();i ++)
    {
        QString cameraName = cameraNames[i];

        NEGLOfflineRenderer *offlineRenderer = new NEGLOfflineRenderer(layouts[i], path, framerate, speed, mergeRecorder, i);
        offlineRenderer->setCamera(cameras()[cameraName]);
        offlineRenderer->copyPropertiesFromWidget(surfaces[i]);
        if(!offlineRenderer->isOneFrame())
            m_offlineRenderers.push_back(offlineRenderer);
    }

    mergeRecorder->initializeFfmpeg();
    NESCENE.startUpdateTimer(true);
    NESCENE.setExportOfflineRendering(true);
}

