using namespace std;

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Utility/NEMatrixStack4x4.h"

#include "Rendering/NEGLRenderer.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEGLManager_p.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Rendering/Resource/NEIndexBuffer.h"
#include "Rendering/Spatial/Renderers/NESpatialRenderer.h"
#include "Rendering/Spatial/NECamera.h"
#include "Utility/geometry.h"
#include "Rendering/Resource/NEMaterial.h"
#include "Rendering/NEGLRenderableMesh.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Spatial/NELight.h"
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Rendering/Spatial/Light/NEPointLight.h"
#include "Rendering/Spatial/Light/NESpotLight.h"
#include "Rendering/Spatial/NEClipPlane.h"

#include "AreaTex.h"
#include "SearchTex.h"

#define glVerify(x) x

// Static arrays that wrap some relevant OpenGL identifiers
static GLint sBlendSrcFunc[kBlendFuncCount];
static GLint sBlendDestFunc[kBlendFuncCount];
static GLint sDepthFunc[kDepthTestCount];
static GLint sPrimTypeMap[kPrimTypeCount];

//QOpenGLContext *NEGLManagerPrivate::s_sharedContext = NULL;
//QOffscreenSurface *NEGLManagerPrivate::s_offscreenSurface = NULL;

QOpenGLFunctions *NEGLManagerPrivate::s_gl = NULL;
QOpenGLFunctions_3_3_Compatibility *NEGLManagerPrivate::s_gl33 = NULL;
QOpenGLFunctions_4_1_Compatibility *NEGLManagerPrivate::s_gl41 = NULL;
QOpenGLFunctions_4_3_Compatibility *NEGLManagerPrivate::s_gl43 = NULL;
QOpenGLExtension_EXT_direct_state_access *NEGLManagerPrivate::s_glext_direct_state_access = NULL;
QOpenGLExtension_NV_texture_multisample *NEGLManagerPrivate::s_glext_nv_texture_multisample = NULL;

//QOpenGLContext*& NEGLManager::staticShareContext()
//{
//    return NEGLManagerPrivate::s_sharedContext;
//}
//QOffscreenSurface*& NEGLManager::staticOffscreenSurface()
//{
//    return NEGLManagerPrivate::s_offscreenSurface;
//}

QOpenGLFunctions*& NEGLManager::glFunc()
{
    return NEGLManagerPrivate::s_gl;
}
QOpenGLFunctions_3_3_Compatibility*& NEGLManager::glFunc33()
{
    return NEGLManagerPrivate::s_gl33;
}
QOpenGLFunctions_4_1_Compatibility*& NEGLManager::glFunc41()
{
    return NEGLManagerPrivate::s_gl41;
}
QOpenGLFunctions_4_3_Compatibility*& NEGLManager::glFunc43()
{
    return NEGLManagerPrivate::s_gl43;
}

QOpenGLExtension_EXT_direct_state_access*& NEGLManager::glExtDirectStateAccess()
{
    return NEGLManagerPrivate::s_glext_direct_state_access;
}

QOpenGLExtension_NV_texture_multisample*& NEGLManager::glExtNVTextureMultisample()
{
    return NEGLManagerPrivate::s_glext_nv_texture_multisample;
}



NEGLManagerPrivate::NEGLManagerPrivate(NEGLManager* s):S(s)
{
    m_pGLRenderer = new NEGLRenderer;
}

NEGLManagerPrivate::~NEGLManagerPrivate()
{
    dirLights.clear();
    pointLights.clear();
    spotLights.clear();
    clipPlanes.clear();
}

void NEGLManagerPrivate::init()
{
    m_ParticleShader = 0;
    m_PositionShader = 0;
    m_LineStippleShader = 0;
    m_RectYFilterShader = 0;
    m_ColorRampOSDShader = 0;
    m_FloorDrawingShader = 0;
    m_SolidWireframeShader = 0;
    m_QuadWireframeShader = 0;
    m_CPShader = 0;
    m_CPWaterShader = 0;
    m_RectTextureShader = 0;
    m_GVDBComposeShader = 0;
    m_SandShader = 0;
    m_MotionShader = 0;
    m_HairShader = 0;
    m_AOGeomShader = 0;
    m_AODepthLinearizeShader = 0;
    m_AODepthLinearizeMSAAShader = 0;
    m_AOCalcShader = 0;
    m_HBAOBlurShader = 0;
    m_HBAOBlur2Shader = 0;
    m_AOResultShader = 0;

    m_FluidProgram.mPointThicknessProgram = 0;
    m_FluidProgram.mEllipsoidDepthProgram = 0;
    m_FluidProgram.mDepthBlurProgram = 0;
    m_FluidProgram.mDiffuseProgram = 0;

    m_ShaderColorMapper = 0;
    m_bgShader = 0;
    m_pedestalShader = 0;
    m_simpleDepthShader = 0;
    m_shadowMappingShader = 0;
    m_skyboxShader = 0;
    m_skySphereShader = 0;
    m_fxaaShader = 0;
    m_fxaaLowShader = m_fxaaMediumShader = m_fxaaHighShader = m_fxaaExtremeShader = 0;

	m_LinkedListFinalShader = 0;

    m_EdgeShader = 0;

    // SMAA
    m_smaaEdgeShader = 0;
    m_smaaBlendShader = 0;
    m_smaaNeighborShader = 0;

    // IBL
    m_iblEquirectangularToCubemapShader = 0;
    m_iblIrradianceShader = 0;
    m_iblBackgroundShader = 0;
    m_iblPrefilterShader = 0;
    m_iblBrdfShader = 0;

    m_PerPixelVS = m_PerPixelFS = m_PerPixelFlatVS = m_PerPixelFlatFS = 0;
    m_FullScreenVS = m_WaterDisplayFS = 0;
    m_LightsFS = 0;
    m_ClipFS = 0;
    m_LightsSpritesFS = 0;
    m_ColoringFS = 0;
    m_RigidParticleFS = 0;

    sPrimTypeMap[kPointListPrim] = GL_POINTS;
    sPrimTypeMap[kLineListPrim] = GL_LINES;
    sPrimTypeMap[kLineStripPrim] = GL_LINE_STRIP;
    sPrimTypeMap[kLineLoopPrim] = GL_LINE_LOOP;
    sPrimTypeMap[kTriangleListPrim] = GL_TRIANGLES;
    sPrimTypeMap[kTriangleStripPrim] = GL_TRIANGLE_STRIP;
    sPrimTypeMap[kTriangleFanPrim] = GL_TRIANGLE_FAN;
    sPrimTypeMap[kPolyListPrim] = GL_POLYGON;
    sPrimTypeMap[kQuadListPrim] = GL_QUADS;
    sPrimTypeMap[kQuadStripPrim] = GL_QUAD_STRIP;
    sPrimTypeMap[kTriangleListAdjacencyPrim] = GL_TRIANGLES_ADJACENCY;

    // Fill arrays with OpenGL related identifiers
    sBlendSrcFunc[kNoBlendFunc] = GL_ONE;
    sBlendDestFunc[kNoBlendFunc] = GL_ZERO;
    sBlendSrcFunc[kOpacityBlendFunc] = GL_SRC_ALPHA;
    sBlendDestFunc[kOpacityBlendFunc] = GL_ONE_MINUS_SRC_ALPHA;
    sBlendSrcFunc[kAddBlendFunc] = GL_ONE;
    sBlendDestFunc[kAddBlendFunc] = GL_ONE;
    sBlendSrcFunc[kMultiplyBlendFunc] = GL_ZERO;
    sBlendDestFunc[kMultiplyBlendFunc] = GL_SRC_COLOR;

    sDepthFunc[kDisableDepthTest] = GL_ALWAYS;
    sDepthFunc[kGreaterDepthTest] = GL_GREATER;
    sDepthFunc[kGreaterEqualDepthTest] = GL_GEQUAL;
    sDepthFunc[kLessDepthTest] = GL_LESS;
    sDepthFunc[kLessEqualDepthTest] = GL_LEQUAL;

	//Check OpenGL version In this machine.
    QString strVersion((char *)(GL->glGetString(GL_VERSION)));
    if(strVersion.contains(' '))
        strVersion = strVersion.left(strVersion.indexOf(' ') + 1);
	strVersion = strVersion.remove('.');
	m_OpenGLVersion = strVersion.toInt();


	//m_OpenGLVersion = 330;

    // Create the default particle shader

    loadDefaultShaders();
    //loadFoamShaders();

    polyFont = new FTPolygonFont("System/Fonts/tahoma.ttf");
    polyFont->UseDisplayList(true);

    if(polyFont->Error())
    {
        Log() << "Cannot load the font";
        EndLog(NE::kLogError);
    }
    polyFont->FaceSize(1);

    textureFont = new FTTextureFont("System/Fonts/tahoma.ttf");
    textureFont->UseDisplayList(true);
    if(textureFont->Error())
    {
        Log() << "Cannot load the font";
        EndLog(NE::kLogError);
    }
    textureFont->FaceSize(12);

    bgCube = 0;
    bgSphere = 0;

//    m_pGLRenderer = new NEGLRenderer;
//    m_pGLRenderer->init();

    createRenderObject();

#if USE_EXT_GFSDK_SSAO
    //! HBAO+
    m_pAOContext = 0;
#endif


    m_pGLRenderer->init();
}

void NEGLManagerPrivate::disableGPUBuffers()
{
    GL->glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GL->glDisableVertexAttribArray(0);
    GL->glDisableVertexAttribArray(1);
    GL->glDisableVertexAttribArray(2);
    GL->glDisableVertexAttribArray(3);

    GL->glDisableClientState(GL_COLOR_ARRAY);
    GL->glDisableClientState(GL_VERTEX_ARRAY);
}


void NEGLManagerPrivate::loadDefaultShaders()
{
    // Create the default particle shader
    if(m_OpenGLVersion >= 430)
        m_ParticleShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Particle.vert",  "System/Shaders/Default/default_Particle.frag", false);
    else if (m_OpenGLVersion >= 410) {
        m_ParticleShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Particle.vert", "System/Shaders/Default410/default_Particle.frag", false);
        std::cerr << "Linked List Transparency isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;
    }
    else if (m_OpenGLVersion >= 300)
    {
        m_ParticleShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Particle.vert", "System/Shaders/Default300/default_Particle.frag", false);
        std::cerr << "Linked List Transparency isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;
    }

    S->recreateShader(m_ParticleShader);
    m_ParticleShader->shaderProgram()->setObjectName("Particle shader");
    if(!m_ParticleShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(Particle shader)" << std::endl;
        std::cerr << m_ParticleShader->shaderProgram()->log().toStdString() << std::endl;

    }


    /*
        // Create the AO particle shader
        m_AOParticleShader = new ShaderMetaInfo(NULL, "System/Shaders/SSAO/ssao_default_Particle.vert",  "System/Shaders/SSAO/ssao_default_Particle.frag", false);
        S->recreateShader(m_AOParticleShader);
        m_AOParticleShader->shaderProgram()->setObjectName("AO Particle shader");
        if(!m_AOParticleShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_AOParticleShader)" << std::endl;
            std::cerr << m_AOParticleShader->shaderProgram()->log().toStdString() << std::endl;
        }
    */

    // Create the default sand shader
    m_SandShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Sand.vert", "System/Shaders/Default/default_Sand.frag", true);
    //m_SandShader->shaderProgram()->setObjectName("Sand shader");
    //recreateShader(m_SandShader);
    //linkShader(m_SandShader->shaderProgram());

    // Create the default position shader with mesh color
    if(m_OpenGLVersion >= 430 )
        m_PositionShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Position.vert", "System/Shaders/Default/default_Position.frag", false);
    else if(m_OpenGLVersion >= 410)
        m_PositionShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Position.vert", "System/Shaders/Default410/default_Position.frag", false);
    else if(m_OpenGLVersion >= 330)
        m_PositionShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_Position.vert", "System/Shaders/Default300/default_Position.frag", false);

    S->recreateShader(m_PositionShader);
    m_PositionShader->shaderProgram()->setObjectName("Position shader with mesh color");
    if(!m_PositionShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_PositionShader)" << std::endl;
        std::cerr << m_PositionShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Create the line stipple shader
    m_LineStippleShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_line_stipple.vert", "System/Shaders/Default/default_line_stipple.frag", false);
    S->recreateShader(m_LineStippleShader);
    m_LineStippleShader->shaderProgram()->setObjectName("Line Stipple Shader");
    if(!m_LineStippleShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_LineStippleShader)" << std::endl;
        std::cerr << m_LineStippleShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // create color according y value of rectanle
    m_RectYFilterShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_rect_yfilter.vert", "System/Shaders/Default/default_rect_yfilter.frag", false);
    m_RectYFilterShader->addSecondaryFragShader("System/Shaders/Default/coloring.frag");
    S->recreateShader(m_RectYFilterShader);
    m_RectYFilterShader->shaderProgram()->setObjectName("Rect YFilter Shader");
    if(!m_RectYFilterShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_RectYFilterShader)" << std::endl;
        std::cerr << m_RectYFilterShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_ColorRampOSDShader = new ShaderMetaInfo(NULL, "System/Shaders/OSD/colorbar.vert", "System/Shaders/OSD/colorbar.frag", false);
    m_ColorRampOSDShader->addSecondaryFragShader("System/Shaders/Default/coloring.frag");
    S->recreateShader(m_ColorRampOSDShader);
    m_ColorRampOSDShader->shaderProgram()->setObjectName("Color Ramp OSD Shader");
    if(!m_ColorRampOSDShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_ColorRampOSDShader)" << std::endl;
        std::cerr << m_ColorRampOSDShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Create the floor drawing shader
    m_FloorDrawingShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_floor_drawing.vert", "System/Shaders/Default/default_floor_drawing.frag", false);
    S->recreateShader(m_FloorDrawingShader);
    m_FloorDrawingShader->shaderProgram()->setObjectName("Floor Drawing Shader");
//    if(!m_FloorDrawingShader->shaderProgram()->link())
//    {
//        std::cerr << "Linking Error(m_FloorDrawingShader)" << std::endl;
//        std::cerr << m_FloorDrawingShader->shaderProgram()->log().toStdString() << std::endl;
//    }

    //Create the Order Independent Transparency Shader
    m_WeightedBlendedFinalShader = new ShaderMetaInfo(NULL,"System/Shaders/Default/default_quad.vert", "System/Shaders/OIT/weighted_final_fragment.frag", false);
    S->recreateShader(m_WeightedBlendedFinalShader);
    m_WeightedBlendedFinalShader->shaderProgram()->setObjectName("Order Independent Transparency Shader");
    if(!m_WeightedBlendedFinalShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_WeightedBlendedFinalShader)" << std::endl;
        std::cerr << m_WeightedBlendedFinalShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_DepthPeelingBlendShader = new ShaderMetaInfo(NULL,"System/Shaders/Default/default_quad.vert", "System/Shaders/OIT/depthpeeling_blend_fragment.frag", false);
    S->recreateShader(m_DepthPeelingBlendShader);
    m_DepthPeelingBlendShader->shaderProgram()->setObjectName("Blending Shader for Depth Peeling Order Independent Transparency");
    if(!m_DepthPeelingBlendShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_DepthPeelingBlendShader)" << std::endl;
        std::cerr << m_DepthPeelingBlendShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_DepthPeelingFinalShader = new ShaderMetaInfo(NULL,"System/Shaders/Default/default_quad.vert", "System/Shaders/OIT/depthpeeling_final_fragment.frag", false);
    S->recreateShader(m_DepthPeelingFinalShader);
    m_DepthPeelingFinalShader->shaderProgram()->setObjectName("Final Shader for Depth Peeling Order Independent Transparency");
    if(!m_DepthPeelingFinalShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_DepthPeelingFinalShader)" << std::endl;
        std::cerr << m_DepthPeelingFinalShader->shaderProgram()->log().toStdString() << std::endl;
    }

    if (m_OpenGLVersion >= 430) {
        m_LinkedListFinalShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/OIT/linkedlist_final_fragment.frag", false);
        S->recreateShader(m_LinkedListFinalShader);
        m_LinkedListFinalShader->shaderProgram()->setObjectName("Order Independent Transparency Shader");
        if(!m_LinkedListFinalShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_LinkedListFinalShader)" << std::endl;
            std::cerr << m_LinkedListFinalShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Linked List Transparency isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;



    //Create Solid Wireframe Shader.
    if(m_OpenGLVersion >= 430)
        m_SolidWireframeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_SolidWireframe.vert", "System/Shaders/Default/default_SolidWireframe.geom", "System/Shaders/Default/default_SolidWireframe.frag", false);
    else if(m_OpenGLVersion >= 410)
        m_SolidWireframeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_SolidWireframe.vert", "System/Shaders/Default/default_SolidWireframe.geom", "System/Shaders/Default410/default_SolidWireframe.frag", false);
    else if(m_OpenGLVersion >= 330)
        m_SolidWireframeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_SolidWireframe.vert", "System/Shaders/Default/default_SolidWireframe.geom", "System/Shaders/Default300/default_SolidWireframe.frag", false);
    S->recreateShader(m_SolidWireframeShader);
    m_SolidWireframeShader->shaderProgram()->setObjectName("Solid Wireframe shader");
    if(!m_SolidWireframeShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_SolidWireframeShader)" << std::endl;
        std::cerr << m_SolidWireframeShader->shaderProgram()->log().toStdString() << std::endl;
    }


    //Create Quad Wireframe Shader.
    if(m_OpenGLVersion >= 430)
        m_QuadWireframeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_QuadWireframe.vert", "System/Shaders/Default/default_QuadWireframe.frag", false);
    else if(m_OpenGLVersion >= 410)
        m_QuadWireframeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_QuadWireframe.vert", "System/Shaders/Default410/default_QuadWireframe.frag", false);
    else if(m_OpenGLVersion >= 330)
        m_QuadWireframeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_QuadWireframe.vert", "System/Shaders/Default300/default_QuadWireframe.frag", false);
    S->recreateShader(m_QuadWireframeShader);
    m_QuadWireframeShader->shaderProgram()->setObjectName("Quad Wireframe shader");
    if(!m_QuadWireframeShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_QuadWireframeShader)" << std::endl;
        std::cerr << m_QuadWireframeShader->shaderProgram()->log().toStdString() << std::endl;
    }


    // Create the CP shader
    if(m_OpenGLVersion >= 430)
        m_CPShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_CP.vert", "System/Shaders/Default/default_CP.frag", false);
    else if(m_OpenGLVersion >= 410)
        m_CPShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_CP.vert", "System/Shaders/Default410/default_CP.frag", false);
    else if(m_OpenGLVersion >= 300)
        m_CPShader = new ShaderMetaInfo(NULL, "System/Shaders/Default330/default_CP.vert", "System/Shaders/Default300/default_CP.frag", false);

    S->recreateShader(m_CPShader);
    m_CPShader->shaderProgram()->setObjectName("Color-position shader");
    if(!m_CPShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_CPShader)" << std::endl;
        std::cerr << m_CPShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Create the ColAndPos shader
    if(m_OpenGLVersion >= 430)
        m_CPWaterShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_ColAndPos.vert", "System/Shaders/Default/default_ColAndPos.frag", false);
    else if(m_OpenGLVersion >= 410)
        m_CPWaterShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_ColAndPos.vert", "System/Shaders/Default410/default_ColAndPos.frag", false);
    else if(m_OpenGLVersion >= 330)
        m_CPWaterShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_ColAndPos.vert", "System/Shaders/Default300/default_ColAndPos.frag", false);
    S->recreateShader(m_CPWaterShader);
    m_CPWaterShader->shaderProgram()->setObjectName("Color and position shader for water render");
    if(!m_CPWaterShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_CPWaterShader)" << std::endl;
        std::cerr << m_CPWaterShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Create the default camera lit shader
    // m_CamLitShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_CNP.vert","System/Shaders/Default/default_CNP.frag",false);
    // recreateShader(m_CamLitShader);
    // m_CamLitShader->shaderProgram()->setObjectName("Cam Lit shader");
    // linkShader(m_CamLitShader->shaderProgram());

    // Create the default rectangle texture shader
    m_RectTextureShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/Default/default_RectTexture.frag", false);
    S->recreateShader(m_RectTextureShader);
    m_RectTextureShader->shaderProgram()->setObjectName("Rect texture shader");
    if(!m_RectTextureShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_RectTextureShader)" << std::endl;
        std::cerr << m_RectTextureShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_GVDBComposeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/GVDB/composite.frag.glsl", false);
    S->recreateShader(m_GVDBComposeShader);
    m_GVDBComposeShader->shaderProgram()->setObjectName("GVDB compose shader");
    if(!m_GVDBComposeShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_GVDBComposeShader)" << std::endl;
        std::cerr << m_GVDBComposeShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Create the motion shader
    m_MotionShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/motion.vert", "System/Shaders/Default/motion.frag", false);
    S->recreateShader(m_MotionShader);
    m_MotionShader->shaderProgram()->setObjectName("Motion shader");
    if(!m_MotionShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_MotionShader)" << std::endl;
        std::cerr << m_MotionShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Create the default hair shader
    if(m_OpenGLVersion >= 430)
        m_HairShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_perpixel.vert", "System/Shaders/Default/default_perpixel.frag", true);
    else if(m_OpenGLVersion >= 410)
        m_HairShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_perpixel.vert", "System/Shaders/Default410/default_perpixel.frag", true);
    else if(m_OpenGLVersion >= 330)
        m_HairShader = new ShaderMetaInfo(NULL, "System/Shaders/Default330/default_perpixel.vert", "System/Shaders/Default300/default_perpixel.frag", true);
    //recreateShader(m_HairShader);
    //linkShader(m_HairShader->shaderProgram());

    // Create the fluid shader
    m_FluidProgram.mPointThicknessProgram = new ShaderMetaInfo(NULL, "System/Shaders/Fluid/point_depth.vert",  "System/Shaders/Fluid/point_thickness.frag", false);
    S->recreateShader(m_FluidProgram.mPointThicknessProgram);
    m_FluidProgram.mPointThicknessProgram->shaderProgram()->setObjectName("Particle point thickness shader");
    if(!m_FluidProgram.mPointThicknessProgram->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_FluidProgram.mPointThicknessProgram)" << std::endl;
        std::cerr << m_FluidProgram.mPointThicknessProgram->shaderProgram()->log().toStdString() << std::endl;
    }

    m_FluidProgram.mEllipsoidDepthProgram = new ShaderMetaInfo(NULL, "System/Shaders/Fluid/ellipsoid_depth.vert", "System/Shaders/Fluid/ellipsoid_depth.geom", "System/Shaders/Fluid/ellipsoid_depth.frag", false);
    S->recreateShader(m_FluidProgram.mEllipsoidDepthProgram);
    m_FluidProgram.mEllipsoidDepthProgram->shaderProgram()->setObjectName("Particle ellipsoid depth shader");
    if(m_FluidProgram.mEllipsoidDepthProgram->shaderProgram()->link() == false)
    {
        std::cerr << "Linking Error(m_FluidProgram.mEllipsoidDepthProgram)" << std::endl;
        std::cerr << m_FluidProgram.mEllipsoidDepthProgram->shaderProgram()->log().toStdString() << std::endl;
    }


    m_FluidProgram.mDepthBlurProgram = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert",  "System/Shaders/Fluid/blur_depth.frag", false);
    S->recreateShader(m_FluidProgram.mDepthBlurProgram);
    m_FluidProgram.mDepthBlurProgram->shaderProgram()->setObjectName("Particle depth blur shader");
    if(!m_FluidProgram.mDepthBlurProgram->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_FluidProgram.mDepthBlurProgram)" << std::endl;
        std::cerr << m_FluidProgram.mDepthBlurProgram->shaderProgram()->log().toStdString() << std::endl;
    }

    m_FluidProgram.mDiffuseProgram = new ShaderMetaInfo(NULL, "System/Shaders/Fluid/diffuse.vert",  "System/Shaders/Fluid/diffuse.geom",  "System/Shaders/Fluid/diffuse.frag", false);
    S->recreateShader(m_FluidProgram.mDiffuseProgram);
    m_FluidProgram.mDiffuseProgram->shaderProgram()->setObjectName("Particle diffuse shader");
    if(!m_FluidProgram.mDiffuseProgram->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_FluidProgram.mDiffuseProgram)" << std::endl;
        std::cerr << m_FluidProgram.mDiffuseProgram->shaderProgram()->log().toStdString() << std::endl;
    }


    if (m_OpenGLVersion >= 410)
    {
        // Create the HBAO shader
        m_AOGeomShader = new ShaderMetaInfo(NULL, "System/Shaders/SSAO/ssao_geometry.vert.glsl", "System/Shaders/SSAO/ssao_geometry.frag.glsl", true);
        S->recreateShader(m_AOGeomShader);
        m_AOGeomShader->shaderProgram()->setObjectName("HBAO Geom shader");
        if(!m_AOGeomShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_AOGeomShader)" << std::endl;
            std::cerr << m_AOGeomShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Ambient Occlusion isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;

    if (m_OpenGLVersion >= 430)
    {
        m_AODepthLinearizeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/SSAO/depthlinearize.frag.glsl", true);
        m_AODepthLinearizeShader->addFragPrepend("#define DEPTHLINEARIZE_MSAA 0\n");
        S->recreateShader(m_AODepthLinearizeShader);
        m_AODepthLinearizeShader->shaderProgram()->setObjectName("HBAO Depth Linearize shader");
        if(!m_AODepthLinearizeShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_AODepthLinearizeShader)" << std::endl;
            std::cerr << m_AODepthLinearizeShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Ambient Occlusion isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;


    if (m_OpenGLVersion >= 430)
    {
        m_AODepthLinearizeMSAAShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/SSAO/depthlinearize.frag.glsl", true);
        m_AODepthLinearizeMSAAShader->addFragPrepend("#define DEPTHLINEARIZE_MSAA 1\n");
        S->recreateShader(m_AODepthLinearizeMSAAShader);
        m_AODepthLinearizeMSAAShader->shaderProgram()->setObjectName("HBAO Depth Linearize shader");
        if(!m_AODepthLinearizeMSAAShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_AODepthLinearizeMSAAShader)" << std::endl;
            std::cerr << m_AODepthLinearizeMSAAShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Ambient Occlusion isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;

    if (m_OpenGLVersion >= 420)
    {
        m_AOCalcShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/SSAO/hbao.frag.glsl", true);
        m_AOCalcShader->addFragPrepend("#define AO_DEINTERLEAVED 0\n#define AO_BLUR 1\n");
        S->recreateShader(m_AOCalcShader);
        m_AOCalcShader->shaderProgram()->setObjectName("HBAO Calc shader");
        if(!m_AOCalcShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_AOCalcShader)" << std::endl;
            std::cerr << m_AOCalcShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
    {
        std::cerr << "Ambient Occlusion isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;
    }


    m_HBAOBlurShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/SSAO/hbao_blur.frag.glsl", true);
    m_HBAOBlurShader->addFragPrepend("#define AO_BLUR_PRESENT 0\n");
    S->recreateShader(m_HBAOBlurShader);
    m_HBAOBlurShader->shaderProgram()->setObjectName("HBAO Blur shader");
    if(!m_HBAOBlurShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_HBAOBlurShader)" << std::endl;
        std::cerr << m_HBAOBlurShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_HBAOBlur2Shader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/SSAO/hbao_blur.frag.glsl", true);
    m_HBAOBlur2Shader->addFragPrepend("#define AO_BLUR_PRESENT 1\n");
    S->recreateShader(m_HBAOBlur2Shader);
    m_HBAOBlur2Shader->shaderProgram()->setObjectName("HBAO Blur 2 shader");
    if(!m_HBAOBlur2Shader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_HBAOBlur2Shader)" << std::endl;
        std::cerr << m_HBAOBlur2Shader->shaderProgram()->log().toStdString() << std::endl;
    }

    if (m_OpenGLVersion >= 410) {
        m_AOResultShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/SSAO/ao_result.frag", false);
        S->recreateShader(m_AOResultShader);
        m_AOResultShader->shaderProgram()->setObjectName("AO Result shader");
        if(!m_AOResultShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_AOResultShader)" << std::endl;
            std::cerr << m_AOResultShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Ambient Occlusion isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;


    m_ShaderColorMapper = new ShaderMetaInfo(NULL, "System/Shaders/colorManip.vert",  "System/Shaders/colorManip.frag", false);
    S->recreateShader(m_ShaderColorMapper);
    m_ShaderColorMapper->shaderProgram()->setObjectName("ColorManip");
    if(!m_ShaderColorMapper->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_ShaderColorMapper)" << std::endl;
        std::cerr << m_ShaderColorMapper->shaderProgram()->log().toStdString() << std::endl;
    }

    m_bgShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert",  "System/Shaders/Default/default_gradient.frag", false);
    S->recreateShader(m_bgShader);
    m_bgShader->shaderProgram()->setObjectName("BG");
    if(!m_bgShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_bgShader)" << std::endl;
        std::cerr << m_bgShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_pedestalShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_real.vert",  "System/Shaders/Default/default_real.frag", false);
    S->recreateShader(m_pedestalShader);
    m_pedestalShader->shaderProgram()->setObjectName("REAL");
//    m_pedestalShader->shaderProgram()->link();

    m_simpleDepthShader = new ShaderMetaInfo(NULL, "System/Shaders/Shadow/shadow_mapping_depth.vs",  "System/Shaders/Shadow/shadow_mapping_depth.frag", false);
    S->recreateShader(m_simpleDepthShader);
    m_simpleDepthShader->shaderProgram()->setObjectName("SimpleDepth");
    if(!m_simpleDepthShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_simpleDepthShader)" << std::endl;
        std::cerr << m_simpleDepthShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_shadowMappingShader = new ShaderMetaInfo(NULL, "System/Shaders/Shadow/shadow_mapping.vs",  "System/Shaders/Shadow/shadow_mapping.frag", false);
    S->recreateShader(m_shadowMappingShader);
    m_shadowMappingShader->shaderProgram()->setObjectName("ShadowMapping");
    if(!m_shadowMappingShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_shadowMappingShader)" << std::endl;
        std::cerr << m_shadowMappingShader->shaderProgram()->log().toStdString() << std::endl;
    }


    m_skyboxShader = new ShaderMetaInfo(NULL, "System/Shaders/Skybox/skybox.vs",  "System/Shaders/Skybox/skybox.frag", false);
    S->recreateShader(m_skyboxShader);
    m_skyboxShader->shaderProgram()->setObjectName("Skybox");
    if(!m_skyboxShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_skyboxShader)" << std::endl;
        std::cerr << m_skyboxShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_skySphereShader = new ShaderMetaInfo(NULL, "System/Shaders/Skybox/skysphere.vs",  "System/Shaders/Skybox/skysphere.frag", false);
    S->recreateShader(m_skySphereShader);
    m_skySphereShader->shaderProgram()->setObjectName("SkySphere");
    if(!m_skySphereShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_skySphereShader)" << std::endl;
        std::cerr << m_skySphereShader->shaderProgram()->log().toStdString() << std::endl;
    }

    if (m_OpenGLVersion >= 400) {
        m_fxaaShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/PostProcessing/fxaa.frag", false);
        S->recreateShader(m_fxaaShader);
        m_fxaaShader->shaderProgram()->setObjectName("FXAA");
        m_fxaaShader->shaderProgram()->link();

        m_fxaaLowShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/PostProcessing/fxaa_low.frag", false);
        S->recreateShader(m_fxaaLowShader);
        m_fxaaLowShader->shaderProgram()->setObjectName("FXAA Low");
        if(!m_fxaaLowShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_fxaaLowShader)" << std::endl;
            std::cerr << m_fxaaLowShader->shaderProgram()->log().toStdString() << std::endl;
        }

        m_fxaaMediumShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/PostProcessing/fxaa_medium.frag", false);
        S->recreateShader(m_fxaaMediumShader);
        m_fxaaMediumShader->shaderProgram()->setObjectName("FXAA Medium");
        if(!m_fxaaMediumShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_fxaaMediumShader)" << std::endl;
            std::cerr << m_fxaaMediumShader->shaderProgram()->log().toStdString() << std::endl;
        }

        m_fxaaHighShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/PostProcessing/fxaa_high.frag", false);
        S->recreateShader(m_fxaaHighShader);
        m_fxaaHighShader->shaderProgram()->setObjectName("FXAA High");
        if(!m_fxaaHighShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_fxaaHighShader)" << std::endl;
            std::cerr << m_fxaaHighShader->shaderProgram()->log().toStdString() << std::endl;
        }

        m_fxaaExtremeShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert", "System/Shaders/PostProcessing/fxaa_extreme.frag", false);
        S->recreateShader(m_fxaaExtremeShader);
        m_fxaaExtremeShader->shaderProgram()->setObjectName("FXAA Extreme");
        if(!m_fxaaExtremeShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_fxaaExtremeShader)" << std::endl;
            std::cerr << m_fxaaExtremeShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Anti-Aliasing isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;


    m_EdgeShader = new ShaderMetaInfo( NULL, "System/Shaders/Default/default_quad.vert",  "System/Shaders/Default/default_Edge.frag", false);
    S->recreateShader( m_EdgeShader );
    m_EdgeShader->shaderProgram()->setObjectName( "Edge" );
    if(!m_EdgeShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_EdgeShader)" << std::endl;
        std::cerr << m_EdgeShader->shaderProgram()->log().toStdString() << std::endl;
    }

    if (m_OpenGLVersion >= 410) {
        // SMAA
        m_smaaEdgeShader = new ShaderMetaInfo(NULL, "System/Shaders/PostProcessing/smaa_edge.vert", "System/Shaders/PostProcessing/smaa_edge_luma.frag", false);
        S->recreateShader(m_smaaEdgeShader);
        m_smaaEdgeShader->shaderProgram()->setObjectName("SMAA Edge");
        if(!m_smaaEdgeShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_smaaEdgeShader)" << std::endl;
            std::cerr << m_smaaEdgeShader->shaderProgram()->log().toStdString() << std::endl;
        }

        m_smaaBlendShader = new ShaderMetaInfo(NULL, "System/Shaders/PostProcessing/smaa_blend.vert", "System/Shaders/PostProcessing/smaa_blend.frag", false);
        S->recreateShader(m_smaaBlendShader);
        m_smaaBlendShader->shaderProgram()->setObjectName("SMAA Blend");
        if(!m_smaaBlendShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_smaaBlendShader)" << std::endl;
            std::cerr << m_smaaBlendShader->shaderProgram()->log().toStdString() << std::endl;
        }

        m_smaaNeighborShader = new ShaderMetaInfo(NULL, "System/Shaders/PostProcessing/smaa_neighbor.vert", "System/Shaders/PostProcessing/smaa_neighbor.frag", false);
        S->recreateShader(m_smaaNeighborShader);
        m_smaaNeighborShader->shaderProgram()->setObjectName("SMAA Neighbor");
        if(!m_smaaNeighborShader->shaderProgram()->link())
        {
            std::cerr << "Linking Error(m_smaaNeighborShader)" << std::endl;
            std::cerr << m_smaaNeighborShader->shaderProgram()->log().toStdString() << std::endl;
        }
    }
    else
        std::cerr << "Anti-Aliasing isn't support on your machine. Your OpenGL version is " << m_OpenGLVersion << std::endl;


    // IBL
    m_iblEquirectangularToCubemapShader = new ShaderMetaInfo(NULL, "System/Shaders/Material/IBL/cubemap.vs",  "System/Shaders/Material/IBL/equirectangular_to_cubemap.fs", false);
    S->recreateShader(m_iblEquirectangularToCubemapShader);
    m_iblEquirectangularToCubemapShader->shaderProgram()->setObjectName("Equirectangular to Cubemap convertor");
    if(!m_iblEquirectangularToCubemapShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_iblEquirectangularToCubemapShader)" << std::endl;
        std::cerr << m_iblEquirectangularToCubemapShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_iblIrradianceShader = new ShaderMetaInfo(NULL, "System/Shaders/Material/IBL/cubemap.vs",  "System/Shaders/Material/IBL/irradiance_convolution.fs", false);
    S->recreateShader(m_iblIrradianceShader);
    m_iblIrradianceShader->shaderProgram()->setObjectName("Irradiance convolution");
    if(!m_iblIrradianceShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_iblIrradianceShader)" << std::endl;
        std::cerr << m_iblIrradianceShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_iblBackgroundShader = new ShaderMetaInfo(NULL, "System/Shaders/Material/IBL/background.vs",  "System/Shaders/Material/IBL/background.fs", false);
    S->recreateShader(m_iblBackgroundShader);
    m_iblBackgroundShader->shaderProgram()->setObjectName("IBL background");
    if(!m_iblBackgroundShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_iblBackgroundShader)" << std::endl;
        std::cerr << m_iblBackgroundShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_iblPrefilterShader = new ShaderMetaInfo(NULL, "System/Shaders/Material/IBL/cubemap.vs",  "System/Shaders/Material/IBL/prefilter.fs", false);
    S->recreateShader(m_iblPrefilterShader);
    m_iblPrefilterShader->shaderProgram()->setObjectName("Prefilter generation");
    if(!m_iblPrefilterShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_iblPrefilterShader)" << std::endl;
        std::cerr << m_iblPrefilterShader->shaderProgram()->log().toStdString() << std::endl;
    }

    m_iblBrdfShader = new ShaderMetaInfo(NULL, "System/Shaders/Default/default_quad.vert",  "System/Shaders/Material/IBL/brdf.fs", false);
    S->recreateShader(m_iblBrdfShader);
    m_iblBrdfShader->shaderProgram()->setObjectName("Brdf map generation");
    if(!m_iblBrdfShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(m_iblBrdfShader)" << std::endl;
        std::cerr << m_iblBrdfShader->shaderProgram()->log().toStdString() << std::endl;
    }

    // Load the default per-pixel lighting vertex shader
    QString strPath;
    if(m_OpenGLVersion >= 430)
        strPath = QString("System/Shaders/Default/default_perpixel.vert");
    else if( m_OpenGLVersion >= 410)
        strPath = QString("System/Shaders/Default/default_perpixel.vert");
    else if( m_OpenGLVersion >= 330)
        strPath = QString("System/Shaders/Default330/default_perpixel.vert");

    QFile fin(strPath);
    bool val = fin.open(QIODevice::ReadOnly);
    _unused(val);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open default per-pixel vertex shader");
    QTextStream fstrm(&fin);
    m_PerPixelVSCodes = fstrm.readAll();
    fin.close();

    if(m_OpenGLVersion >= 410)
        strPath = QString("System/Shaders/Default/default_perpixel_flat.vert");
    else if( m_OpenGLVersion >= 330)
        strPath = QString("System/Shaders/Default330/default_perpixel_flat.vert");

    fin.setFileName(strPath);
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open default per-pixel flat vertex shader");
    m_PerPixelFlatVSCodes = fstrm.readAll();
    fin.close();

    fin.setFileName("System/Shaders/Default/default_quad.vert");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open full screen quad vertex shader");
    m_FullScreenVSCodes = fstrm.readAll();
    fin.close();

    // Load the default per-pixel lighting fragment shader
    fin.setFileName("System/Shaders/Default/lights.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open lights shader definitions");
    m_LightsFSCodes = fstrm.readAll();
    fin.close();

    fin.setFileName("System/Shaders/Default/rigid_Particle.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open RigidParticle shader definitions");
    m_RigidParticleCodes = fstrm.readAll();
    fin.close();

    // Load the default per-pixel lighting fragment shader
    fin.setFileName("System/Shaders/Default/clip.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open clip shader definitions");
    m_ClipFSCodes = fstrm.readAll();
    fin.close();

    // Load the default per-pixel particle lighting fragment shader
    fin.setFileName("System/Shaders/Default/lights_sprite.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open lights shader definitions");
    m_LightsParticlesFSCodes = fstrm.readAll();
    fin.close();

    if( m_OpenGLVersion >= 430 )
        fin.setFileName("System/Shaders/Default/default_perpixel.frag");
    else if( m_OpenGLVersion >= 410)
        fin.setFileName("System/Shaders/Default410/default_perpixel.frag");
    else if(m_OpenGLVersion >= 330)
        fin.setFileName("System/Shaders/Default300/default_perpixel.frag");

    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open default per-pixel fragment shader");
    m_PerPixelFSCodes = fstrm.readAll();
    fin.close();

    fin.setFileName("System/Shaders/Default/coloring.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open default coloring fragment shader");
    m_ColoringFSCodes = fstrm.readAll();
    fin.close();

    if (m_OpenGLVersion >= 410)
        strPath = QString("System/Shaders/Default/default_perpixel_flat.frag");
    else if(m_OpenGLVersion >= 330)
        strPath = QString("System/Shaders/Default300/default_perpixel_flat.frag");

    fin.setFileName(strPath);
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open default per-pixel flat fragment shader");
    m_PerPixelFlatFSCodes = fstrm.readAll();
    fin.close();

    if(m_OpenGLVersion >= 430)
        fin.setFileName("System/Shaders/Fluid/composite.frag");
    else if(m_OpenGLVersion >= 410)
        fin.setFileName("System/Shaders/Fluid410/composite.frag");
    else if( m_OpenGLVersion >= 330)
        fin.setFileName("System/Shaders/Fluid300/composite.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open water display fragment shader");
    m_WaterDisplayFSCodes = fstrm.readAll();
    fin.close();

    /*
    fin.setFileName("System/Shaders/sobel.frag");
    val = fin.open(QIODevice::ReadOnly);
    Q_ASSERT_X(val, "NERenderer::loadDefaultShaders", "Cannot open default per-pixel flat fragment shader");
    QString tempcode = fstrm.readAll();
    fin.close();
    QOpenGLShader* temp = new QOpenGLShader(QOpenGLShader::Fragment);
    temp->setObjectName("FVS");
    //std::cout << "Loading shader " << tempcode.toStdString() << std::endl;
    if( temp->compileSourceCode(tempcode) == false )
    {
        std::cout << temp->log().toStdString();
    }
    */

    S->updateAllShaders();
}


void NEGLManagerPrivate::deInit()
{
    delete m_ParticleShader;
    delete m_PositionShader;
    delete m_LineStippleShader;
    delete m_RectYFilterShader;
    delete m_ColorRampOSDShader;
    delete m_FloorDrawingShader;
    delete m_SolidWireframeShader;
    delete m_QuadWireframeShader;
    delete m_WeightedBlendedFinalShader;
    delete m_DepthPeelingBlendShader;
    delete m_DepthPeelingFinalShader;
    delete m_LinkedListFinalShader;
    delete m_CPShader;
    delete m_CPWaterShader;
    delete m_RectTextureShader;
    delete m_GVDBComposeShader;
    delete m_SandShader;
    delete m_MotionShader;
    delete m_HairShader;
    delete m_AOGeomShader;
    delete m_AODepthLinearizeShader;
    delete m_AODepthLinearizeMSAAShader;
    delete m_AOCalcShader;
    delete m_HBAOBlurShader;
    delete m_HBAOBlur2Shader;
    delete m_AOResultShader;
    delete m_ShaderColorMapper;
    delete m_bgShader;
    delete m_pedestalShader;
    delete m_simpleDepthShader;
    delete m_shadowMappingShader;
    delete m_skyboxShader;
    delete m_skySphereShader;
    delete m_fxaaShader;
    delete m_fxaaLowShader;
    delete m_fxaaMediumShader;
    delete m_fxaaHighShader;
    delete m_fxaaExtremeShader;

    delete m_FluidProgram.mPointThicknessProgram;
    delete m_FluidProgram.mEllipsoidDepthProgram;
    delete m_FluidProgram.mDepthBlurProgram;
    delete m_FluidProgram.mDiffuseProgram;

    delete m_EdgeShader;
    //	delete m_SandShader;
    //	delete m_HairShader;

    delete m_smaaEdgeShader;
    delete m_smaaBlendShader;
    delete m_smaaNeighborShader;

    delete m_iblEquirectangularToCubemapShader;
    delete m_iblIrradianceShader;
    delete m_iblBackgroundShader;
    delete m_iblPrefilterShader;
    delete m_iblBrdfShader;

    delete m_PerPixelFlatFS;
    delete m_PerPixelFlatVS;
    delete m_PerPixelFS;
    delete m_WaterDisplayFS;
    delete m_FullScreenVS;
    delete m_PerPixelVS;
    delete m_LightsFS;
    delete m_LightsSpritesFS;
    delete m_ClipFS;
    delete m_ColoringFS;
    delete m_RigidParticleFS;

    m_PerPixelFlatFS =
            m_PerPixelFlatVS =
            m_PerPixelFS =
            m_WaterDisplayFS =
            m_FullScreenVS =
            m_PerPixelVS = 0;
    m_LightsFS = m_LightsSpritesFS = m_ClipFS = 0;
    m_ColoringFS = 0;
    m_RigidParticleFS = 0;

    dirLights.clear();
    pointLights.clear();
    spotLights.clear();
    clipPlanes.clear();


    // Deleting the fonts causes crash on windows debug builds
    //	delete polyFont;
    //	delete textureFont;
    if (bgCube)
    {
        S->destroy(bgCube);
        bgCube = 0;
    }

    if (bgSphere)
    {
        S->destroy(bgSphere);
        bgSphere = 0;
    }


    m_pGLRenderer->deInit();
    delete m_pGLRenderer;


}

void NEGLManagerPrivate::createRenderObject()
{
    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };
    bgCube = S->createVertexBuffer(kPFormat, 36, (void*)skyboxVertices, GL_STATIC_DRAW);

    int VERT_SEGS = 128;
    int HORZ_SEGS = 256;
    float PI = 3.1415927;
    GLfloat *sphereVertices = new GLfloat[VERT_SEGS * HORZ_SEGS * 6 * 5];
    for (int isS = 0; isS < VERT_SEGS; isS++)
    {
        float y1 = 1.0f * isS / (float) VERT_SEGS;
        float y2 = 1.0f * (isS + 1) / (float) VERT_SEGS;
        float phi1 = (-1.0f + 2.0f * y1) * PI / 2.0f;
        float phi2 = (-1.0f + 2.0f * y2) * PI / 2.0f;
        for (int j = 0; j < HORZ_SEGS; j++)
        {
            float x1 = 1.0f * j / HORZ_SEGS;
            float x2 = 1.0f * (j + 1) / HORZ_SEGS;

            float theta1 = (2.0f * x1 - 1.0f) * PI;
            float theta2 = (2.0f * x2 - 1.0f) * PI;

            vec3f p11 = vec3f(cosf(phi1) * sinf(theta1), sinf(phi1), -cosf(phi1) * cosf(theta1));
            vec3f p12 = vec3f(cosf(phi1) * sinf(theta2), sinf(phi1), -cosf(phi1) * cosf(theta2));
            vec3f p21 = vec3f(cosf(phi2) * sinf(theta1), sinf(phi2), -cosf(phi2) * cosf(theta1));
            vec3f p22 = vec3f(cosf(phi2) * sinf(theta2), sinf(phi2), -cosf(phi2) * cosf(theta2));
            vec2f t11 = vec2f(x1, y1);
            vec2f t12 = vec2f(x2, y1);
            vec2f t21 = vec2f(x1, y2);
            vec2f t22 = vec2f(x2, y2);

            // 11 - 12
            // |  /
            // 21
            for (int k = 0; k < 3; k++)
            {
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 0 )* 5 + k] = p11[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 1 )* 5 + k] = p12[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 2 )* 5 + k] = p21[k];
            }
            for (int k = 0; k < 2; k++)
            {
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 0 )* 5 + 3 + k] = t11[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 1 )* 5 + 3 + k] = t12[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 2 )* 5 + 3 + k] = t21[k];
            }

            // 21 - 12
            //    /
            // 22
            for (int k = 0; k < 3; k++)
            {
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 3 )* 5 + k] = p21[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 4 )* 5 + k] = p12[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 5 )* 5 + k] = p22[k];
            }
            for (int k = 0; k < 2; k++)
            {
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 3 )* 5 + 3 + k] = t21[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 4 )* 5 + 3 + k] = t12[k];
                sphereVertices[( (isS * HORZ_SEGS + j) * 6 + 5 )* 5 + 3 + k] = t22[k];
            }
        }
    }
    bgSphere = S->createVertexBuffer(kTPFormat, VERT_SEGS * HORZ_SEGS * 6, (void*)sphereVertices, GL_STATIC_DRAW);
    delete [] sphereVertices;
}


//NEGLManager class implmentation

NEGLManager::NEGLManager()
{
    // Initialize the private members
    P = new NEGLManagerPrivate(this);
}

NEGLManager::~NEGLManager()
{
    delete P;
}

void NEGLManager::init()
{
    P->init();
#if USE_EXT_GFSDK_SSAO
    initAOContext();
#endif

}

void NEGLManager::deInit()
{
//    P->m_pGLRenderer->deInit();
//    delete P->m_pGLRenderer;

    //QOpenGLContext *gctx = QOpenGLContext::globalShareContext();
#if USE_EXT_GFSDK_SSAO
    // Delete the private members
    // gustaw temp code.
    //releaseAOContext();
#endif

    P->deInit();
}

void NEGLManager::setBlendFunc(uint blend)
{
    if(blend == kNoBlendFunc)
        GL->glDisable(GL_BLEND);
    else
        GL->glEnable(GL_BLEND);

    GL->glBlendFunc(sBlendSrcFunc[blend], sBlendDestFunc[blend]);
}

void NEGLManager::setFillMode(uint fill)
{
    switch(fill)
    {
    case kWireframeFrontAndBack:
        GL->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        GL->glDisable(GL_CULL_FACE);
        break;
    case kWireframeFront:
        GL->glPolygonMode(GL_FRONT, GL_LINE);
        GL->glEnable(GL_CULL_FACE);
        break;
    case kSolidFront:
        GL->glPolygonMode(GL_FRONT, GL_FILL);
        GL->glEnable(GL_CULL_FACE);
        break;
    case kSolidBackWireFront:
        GL->glPolygonMode(GL_BACK, GL_FILL);
        GL->glPolygonMode(GL_FRONT, GL_LINE);
        GL->glEnable(GL_CULL_FACE);
        break;
    case kSolidFrontAndBack:
    default:
        GL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        GL->glDisable(GL_CULL_FACE);
        break;
    }
}

void NEGLManager::setShadeMode(NEShadeMode shade)
{
    if(shade == kFlatShaded)
        GL->glShadeModel(GL_FLAT);
    else if(shade == kGouraudShaded)
        GL->glShadeModel(GL_SMOOTH);
}

NEShadeMode NEGLManager::shadeMode()
{
    GLint mode;
    GL->glGetIntegerv(GL_SHADE_MODEL, &mode);

    switch(mode)
    {
    case GL_FLAT:
        return kFlatShaded;
    case GL_SMOOTH:
        return kGouraudShaded;
    default:
        Q_ASSERT_X(false, "NEGLManager::shadeMode()", "OpenGL returned an undefined shading mode id");
        return kGouraudShaded;
    }
}

void NEGLManager::setDepthTest(uint func)
{
    if(func == kDisableDepthTest)
        GL->glDisable(GL_DEPTH_TEST);
    else
        GL->glEnable(GL_DEPTH_TEST);

    GL->glDepthFunc(sDepthFunc[func]);
}

uint NEGLManager::depthTest()
{
    GLint mode;
    GL->glGetIntegerv(GL_DEPTH_TEST, &mode);
    if(mode == GL_FALSE)
        return kDisableDepthTest;

    GL->glGetIntegerv(GL_DEPTH_FUNC, &mode);
    for(unsigned int func = 0; func < kDepthTestCount; ++func)
    {
        if(mode == sDepthFunc[func])
            return (NEDepthTestFunc)(func);
    }

    Q_ASSERT_X(false, "NEGLManager::shadeMode()", "OpenGL returned an undefined depth function id");
    return kLessEqualDepthTest;
}

void NEGLManager::drawParticles(NEVertexBuffer* vertexBuffer, int count)
{
    drawParticles(NULL, vertexBuffer, count);
}

void NEGLManager::drawParticles(IGLSurface* surface, NEVertexBuffer* vertexBuffer, int count)
{
    Q_ASSERT_X(vertexBuffer, "NEGLManager::drawParticles", "vertexBuffer points to NULL");
    updateCommonParticleUniforms(surface);
    updatePositionUniforms(surface);
    bool vbActive = vertexBuffer->makeActive();
    Q_ASSERT_X(vbActive, "NEGLManager::drawParticles", "vertexBuffer cannot be activated");
    _unused(vbActive);
    GL->glEnable(GL_POINT_SPRITE);
    GL->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	float pointSizeMax;
    GL->glGetFloatv(GL_POINT_SIZE_MAX, &pointSizeMax);
    GL->glPointSize(pointSizeMax);
    GL->glDrawArrays(GL_POINTS, 0, count);
    GL->glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    GL->glDisable(GL_POINT_SPRITE);

    P->disableGPUBuffers();
}

void NEGLManager::drawParticles(GLenum format, GLsizei stride, const GLvoid* pointer, int first, int count)
{
    updateCommonParticleUniforms(NULL);
    //glInterleavedArrays(format, stride, pointer);
    GL->glEnable(GL_POINT_SPRITE);
    GL->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	float pointSizeMax;
    GL->glGetFloatv(GL_POINT_SIZE_MAX, &pointSizeMax);
    GL->glPointSize(pointSizeMax);

    GL->glDrawArrays(GL_POINTS, 0, count);

    GL->glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    GL->glDisable(GL_POINT_SPRITE);
}


void NEGLManager::beginParticleDraw_Thickness( IGLSurface* surface, float radius,  float thicknessParticleScale )
{
    FluidRenderer& fluidRenderer = surface->getFluidRenderer();
    GLuint& thicknessFbo = fluidRenderer.mThicknessFbo;
    GLuint& thicknessTex = fluidRenderer.mThicknessTex;

    GL->glBindFramebuffer( GL_FRAMEBUFFER, thicknessFbo );
    GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thicknessTex, 0);
    GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL->glDisable(GL_CULL_FACE);

    GL->glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    GL->glEnable(GL_POINT_SPRITE);
    GL->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    GL->glEnable(GL_DEPTH_TEST);
    GL->glEnable(GL_BLEND);
    GL->glBlendFunc(GL_ONE, GL_ONE);
    GL->glDepthMask(GL_FALSE);

    surface->setShaderProgram( getFluidPointThicknessShader()->shaderProgram());

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );

    // make sprites larger to get smoother thickness texture
    const float thicknessScale = thicknessParticleScale;

    float window_w = surface->getGLWidth();
    float fov = surface->camera()->Fov()*M_PI/180.0f;

    GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "pointRadius"), radius * thicknessScale );
    GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "pointScale"), window_w / tanf(fov*0.5f) );
}

void NEGLManager::beginParticleDraw_Depth(IGLSurface* surface,  float depthParticleScale)
{
    FluidRenderer& fluidRenderer = surface->getFluidRenderer();

    GLuint& depthFbo = fluidRenderer.mDepthFbo;
    GLuint& depthTex = fluidRenderer.mDepthTex;
    GLuint& colorTex = fluidRenderer.mColorTex;


    glVerify(GL->glBindFramebuffer( GL_FRAMEBUFFER, depthFbo ) );
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTex, 0));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorTex, 0));
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    GL->glDrawBuffers(2, buffers);

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    surface->setShaderProgram(getFluidEllipsoidDepthProgram()->shaderProgram());

    setClipPlanesForShader(surface->shaderProgram());

    float fov = surface->camera()->Fov()*M_PI/180.0f;
    float screenWidth = surface->getGLWidth();
    float screenHeight = surface->getGLHeight();
    const float screenAspect = screenWidth / screenHeight;
    const float viewHeight = tanf(fov/2.0f);
    float invViewport[3] = {1.0f/screenWidth, 1.0f/screenHeight, 1.0f};
    float invProjection[3] = {screenAspect*viewHeight, viewHeight, 1.0f};
    GL->glUniform3fv( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "invViewport"), 1, invViewport);
    GL->glUniform3fv( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "invProjection"), 1, invProjection);
    GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "particleScale"), (float)depthParticleScale);

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );
}

void NEGLManager::drawParticleBlurDepth(IGLSurface* surface, float radius, float smoothingRadius,
                                        float maxRadialSamples, float depthEdgetFalloff, float depthBlurFalloff )
{
    FluidRenderer& fluidRenderer = surface->getFluidRenderer();

    GLuint& depthSmoothTex = fluidRenderer.mDepthSmoothTex;
    GLuint& depthFbo = fluidRenderer.mDepthFbo;
    GLuint& depthTex= fluidRenderer.mDepthTex;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glDisable(GL_DEPTH_TEST);
    GL->glDepthMask(GL_FALSE);

    GL->glBindFramebuffer( GL_FRAMEBUFFER, depthFbo );
    GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthSmoothTex, 0);
    GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);
    surface->setShaderProgram(getFluidDepthBlurProgram()->shaderProgram());

    GL->glDisable(GL_DEPTH_TEST);

    GL->glActiveTexture(GL_TEXTURE0);
    GL->glEnable(GL_TEXTURE_2D);
    GL->glBindTexture(GL_TEXTURE_2D, depthTex);

    float screenWidth = surface->getGLWidth();
    float screenHeight = surface->getGLHeight();
    float fov = surface->camera()->Fov() * M_PI / 180.0f;
    float invTexScale[2] = {1.0f/screenWidth, 1.0f/screenHeight};

    glVerify(GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "blurRadiusWorld"), radius*smoothingRadius));	// blur half the radius by default
    glVerify(GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "blurScale"), screenHeight * (1.0f / (tanf(fov*0.5f)))));
    glVerify(GL->glUniform2fv( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "invTexScale"), 1, invTexScale));
    glVerify(GL->glUniform1i( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "depthTex"), 0));
    glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "debug"), 0));
    GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "maxRadialSamples"), maxRadialSamples );
    GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "depthEdgeFalloff"), depthEdgetFalloff );
    GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "depthBlurFalloff"), depthBlurFalloff );

    drawQuad();

    GL->glPopAttrib();
}

void NEGLManager::drawParticleDisplay(IGLSurface* surface,
                                      NEMaterial* material, float alpha)
{
    FluidRenderer& fluidRenderer = surface->getFluidRenderer();

    //GLuint& sceneFbo = fluidRenderer.mSceneFbo;
    GLuint sceneFbo = surface->getSceneFBO();
    GLuint& depthSmoothTex = fluidRenderer.mDepthSmoothTex;
    GLuint& colorTex = fluidRenderer.mColorTex;
    GLuint& thicknessTex= fluidRenderer.mThicknessTex;
    GLuint& sceneTex = fluidRenderer.mSceneTex;

    glVerify(GL->glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo));
    glVerify(GL->glEnable(GL_DEPTH_TEST));
    glVerify(GL->glDepthMask(GL_TRUE));

    NEMaterial* waterMaterial = material ? material : getDefaultFluidMaterial();
    QOpenGLShaderProgram* shader = waterMaterial->shaderProgram();

    if(shader && shader->isLinked())
    {
        surface->setShaderProgram(shader);
        waterMaterial->updateUniforms();

        updateWaterUniforms(surface);

        setLightsForShader(surface, shader);
    }
    else
    {
        return;
    }

    // smoothed depth tex
    glVerify(GL->glActiveTexture(GL_TEXTURE0));
    glVerify(GL->glEnable(GL_TEXTURE_2D));
    glVerify(GL->glBindTexture(GL_TEXTURE_2D, depthSmoothTex));

    glVerify(GL->glActiveTexture(GL_TEXTURE1));
    glVerify(GL->glEnable(GL_TEXTURE_2D));
    glVerify(GL->glBindTexture(GL_TEXTURE_2D, colorTex) );

    // thickness tex
    glVerify(GL->glActiveTexture(GL_TEXTURE2));
    glVerify(GL->glEnable(GL_TEXTURE_2D));
    glVerify(GL->glBindTexture(GL_TEXTURE_2D, thicknessTex) );

    // scene tex
    glVerify(GL->glActiveTexture(GL_TEXTURE3));
    glVerify(GL->glEnable(GL_TEXTURE_2D));
    glVerify(GL->glBindTexture(GL_TEXTURE_2D, sceneTex));

    glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "tex"), 0));
    glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "colorTex"), 1));
    glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "thicknessTex"), 2));
    glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "sceneTex"), 3));

    if( surface )
    {
        GL->glUniform1i( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "kindOIT"), surface->getOITType() );
        GL->glUniform1i( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "MaxNodes"), surface->getOITMaxNodes() );
        GL->glUniform1f( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "opacityAlpha"), alpha );
    }

    // -- end shadowing

    // ignores projection matrices
    drawQuad();

    // reset state
    GL->glActiveTexture(GL_TEXTURE3);
    GL->glDisable(GL_TEXTURE_2D);
    GL->glActiveTexture(GL_TEXTURE2);
    GL->glDisable(GL_TEXTURE_2D);
    GL->glActiveTexture(GL_TEXTURE1);
    GL->glDisable(GL_TEXTURE_2D);
    GL->glActiveTexture(GL_TEXTURE0);
    GL->glDisable(GL_TEXTURE_2D);

    GL->glEnable(GL_DEPTH_TEST);
    GL->glDepthMask(GL_TRUE);
    GL->glDisable(GL_BLEND);
}

void NEGLManager::drawWaterMesh(IGLSurface* surface, const NEIndexedTriMesh *mesh, matrix44f modelMatrix )
{
    FluidRenderer& fluidRenderer = surface->getFluidRenderer();

    GLuint& sceneTex = fluidRenderer.mSceneTex;

    Q_ASSERT_X( isBuffered( const_cast< NEIndexedTriMesh *>( mesh ) ) == true,
                "NERenderer::drawWaterMesh", "Meshes without buffering cannot be rendered");
    if( mesh->triangles().size() < 1 )
        return;

    // If the mesh is associated with a material, draw it using its shader
    if( material( const_cast< NEIndexedTriMesh *>( mesh ) ) )
    {
        QOpenGLShaderProgram* shader = material( const_cast< NEIndexedTriMesh *>( mesh ) )->shaderProgram();

        if(shader && shader->isLinked())
        {
            surface->setShaderProgram(shader);
            material( const_cast< NEIndexedTriMesh *>( mesh ) )->updateUniforms();

            updatePositionUniforms(this);
            setModelMatrixForPBR(surface, modelMatrix);
            //glUniformMatrix4fv( glGetUniformLocation( shader->programId(), "modelMatrix" ), 1, GL_FALSE, modelMatrix.data() );

            surface->shaderProgram()->setUniformValue("TwoSided", true);
            surface->shaderProgram()->setUniformValue("isReceiveShadow", false);
            if( surface )
            {
                surface->shaderProgram()->setUniformValue("kindOIT", surface->getOITType());
                surface->shaderProgram()->setUniformValue("MaxNodes", surface->getOITMaxNodes());
            }
            // scene tex
            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE3, GL_TEXTURE_2D, sceneTex);

            glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "sceneTex"), 3));
            surface->shaderProgram()->setUniformValue("screenWidth", surface->getGLWidth());
            surface->shaderProgram()->setUniformValue("screenHeight", surface->getGLHeight());

            setLightsForShader(surface, shader);
            setClipPlanesForShader(shader);
        }
    }
    else	// Draw it using color shader otherwise
    {
        bindPositionShader(surface);
        updatePositionUniforms(this);
    }

    draw( kTriangleListPrim, vertexBuffer( mesh ), indexBuffer( mesh ) );
    surface->setShaderProgram(0);
}

void NEGLManager::drawWaterMesh_CP(IGLSurface* surface, const NEIndexedTriMesh *mesh, matrix44f modelMatrix, bool useVertexColor)
{

    FluidRenderer& fluidRenderer = surface->getFluidRenderer();

    GLuint& sceneTex = fluidRenderer.mSceneTex;


    Q_ASSERT_X( isBuffered( const_cast< NEIndexedTriMesh *>( mesh ) ) == true,
                "NERenderer::drawWaterMesh", "Meshes without buffering cannot be rendered");
    if( mesh->triangles().size() < 1 )
        return;

    // If the mesh is associated with a material, draw it using its shader
    if( material( const_cast< NEIndexedTriMesh *>( mesh ) ) )
    {
        QOpenGLShaderProgram* shader = material( const_cast< NEIndexedTriMesh *>( mesh ) )->shaderProgram();

        if(shader && shader->isLinked())
        {
            surface->setShaderProgram(shader);
            material( const_cast< NEIndexedTriMesh *>( mesh ) )->updateUniforms();

            updatePositionUniforms(this);
            setModelMatrixForPBR(surface, modelMatrix);
            //glUniformMatrix4fv( glGetUniformLocation( shader->programId(), "modelMatrix" ), 1, GL_FALSE, modelMatrix.data() );

            GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "useVertexColor" ), useVertexColor );
            surface->shaderProgram()->setUniformValue("TwoSided", true);
            surface->shaderProgram()->setUniformValue("isReceiveShadow", false);

            // scene tex
            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE3, GL_TEXTURE_2D, sceneTex );

            glVerify(GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "sceneTex"), 3));
            surface->shaderProgram()->setUniformValue("screenWidth", surface->getGLWidth());
            surface->shaderProgram()->setUniformValue("screenHeight", surface->getGLHeight());

            setLightsForShader(surface, shader);
            setClipPlanesForShader(shader);
        }
    }
    else	// Draw it using color shader otherwise
    {
        bindCPWaterShader(surface);
        updateCPWaterUniforms( surface, useVertexColor);
        setModelMatrixForPBR(surface, modelMatrix);
        if (getCPWaterShader())
            setClipPlanesForShader(getCPWaterShader()->shaderProgram());
    }

    draw( kTriangleListPrim, vertexBuffer( mesh ), indexBuffer( mesh ) );
    surface->setShaderProgram(0);
}




void NEGLManager::drawVolume(NEVolume* volume)
{
}


void NEGLManager::draw(NEPrimType primType, NEVertexBuffer* vertexBuffer, NEIndexBuffer* indexBuffer)
{
    Q_ASSERT_X(vertexBuffer, "NEGLManager::draw", "vertexBuffer points to NULL");
    Q_ASSERT_X(indexBuffer, "NEGLManager::draw", "indexBuffer points to NULL");

    bool vbActive = vertexBuffer->makeActive();
    Q_ASSERT_X(vbActive, "NEGLManager::draw", "vertexBuffer cannot be activated");
    _unused(vbActive);

    bool ibActive = indexBuffer->makeActive();
    Q_ASSERT_X(ibActive, "NEGLManager::draw", "indexBuffer cannot be activated");
    _unused(ibActive);

    GL->glDrawElements(sPrimTypeMap[primType], indexBuffer->m_NumIndices, GL_UNSIGNED_INT, 0);
    P->disableGPUBuffers();
}

void NEGLManager::draw(NEPrimType primType, NEVertexBuffer* vertexBuffer)
{
    Q_ASSERT_X(vertexBuffer, "NEGLManager::draw", "vertexBuffer points to NULL");

    bool vbActive = vertexBuffer->makeActive();
    Q_ASSERT_X(vbActive, "NEGLManager::draw", "vertexBuffer cannot be activated");
    _unused(vbActive);

    GL->glDrawArrays(sPrimTypeMap[primType], 0, vertexBuffer->m_NumVertices);
    P->disableGPUBuffers();
}

void NEGLManager::draw(NEPrimType primType, NEVertexBuffer* vertexBuffer, GLuint first, GLuint count)
{
    Q_ASSERT_X(vertexBuffer, "NEGLManager::draw", "vertexBuffer points to NULL");

    bool vbActive = vertexBuffer->makeActive();
    Q_ASSERT_X(vbActive, "NEGLManager::draw", "vertexBuffer cannot be activated");
    _unused(vbActive);

    GL->glDrawArrays(sPrimTypeMap[primType], first, count);
    P->disableGPUBuffers();
}


void NEGLManager::drawRaw( const NEIndexedTriMesh* mesh, IGLSurface* surface , bool pick , bool boundingbox )
{
    bindPositionShader(surface);
    updatePositionUniforms(surface);

    if(boundingbox)
        draw( kTriangleListPrim, boundingBoxBuffer( mesh ) );
    else
        draw( kTriangleListPrim, vertexBuffer( mesh ), indexBuffer( mesh ) );
    surface->setShaderProgram(0);

}

void NEGLManager::drawRaw( const NEVolume* volume, bool pick /*= false*/ )
{

}

void NEGLManager::drawRaw(const AABB &aabb)
{
    P->m_pGLRenderer->drawRaw( aabb );
}

void NEGLManager::drawSolidWireframe(const NEIndexedTriMesh *mesh, IGLSurface* surface, QColor solidColor, float thickness, float threshold, matrix44f &modelMatrix, bool bUseDash)
{
    bindSolidWireframeShader(surface);
    updateSolidWireframeUniforms(surface, solidColor, thickness, threshold * M_PI / 180.f, bUseDash);

    setModelMatrixForPBR(surface, modelMatrix);
    //    glUniformMatrix4fv( glGetUniformLocation( shaderProgram()->programId(), "modelMatrix"), 1, GL_FALSE,  (GLfloat *)(&modelMatrix.data()[0]));
    setClipPlanesForShader(surface->shaderProgram());

    draw( kTriangleListAdjacencyPrim, vertexBuffer( mesh ), indexBufferWithAdjacency( mesh ) );
    surface->setShaderProgram(0);
}

void NEGLManager::drawQuadWireframe(const NEIndexedTriMesh *mesh, IGLSurface* surface, QColor wireColor, matrix44f &modelMatrix, bool boundingbox, bool isHideBackfacing)
{
    bindQuadWireframeShader(surface);
    bool swap = mesh->swapUpAxes();
    updateQuadWireframeUniforms(surface, wireColor, isHideBackfacing, swap);

    setModelMatrixForPBR(surface, modelMatrix);
    setClipPlanesForShader(surface->shaderProgram());

    if(boundingbox)
        draw(kTriangleListPrim, unIndexedBoundingBoxBuffer(mesh));
    else
        draw(kTriangleListPrim, unIndexedVBuffer(mesh));

    surface->setShaderProgram(0);
}


void NEGLManager::drawQuadBackground(IGLSurface *surface, const QColor &bgColor1 , const QColor &bgColor2, bool bRadial)
{
    GL->glDisable( GL_DEPTH_TEST );

    bindBgShader(surface);

    float meshColor1[3];
    meshColor1[0] = bgColor1.red()/255.0;
    meshColor1[1] = bgColor1.green()/255.0;
    meshColor1[2] = bgColor1.blue()/255.0;

    float meshColor2[3];
    meshColor2[0] = bgColor2.red()/255.0;
    meshColor2[1] = bgColor2.green()/255.0;
    meshColor2[2] = bgColor2.blue()/255.0;


    GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor1"), 1, meshColor1 );
    GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor2"), 1, meshColor2 );


    GL->glUniform1i( GL->glGetUniformLocation(surface->shaderProgram()->programId(), "bRadial"), (int) bRadial);

    drawQuad();

    GL->glEnable( GL_DEPTH_TEST );
}

void NEGLManager::drawQuadBackgroundTexture( IGLSurface *surface, QOpenGLTexture* ttex )
{
    if( !ttex )
        return;

    GL->glDisable( GL_DEPTH_TEST );

    bindRectTextureShader(surface);

    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "colorMap" ), 0 );
    ttex->bind(0);
    drawQuad();
    ttex->release();

    GL->glEnable( GL_DEPTH_TEST );
}

void NEGLManager::drawPedestal(IGLSurface* surface, QColor &realColor, matrix44f &mvpMat, float radial)
{

    GL->glEnable(GL_CULL_FACE);
    GL->glCullFace(GL_FRONT);

    bindPedestalShader(surface);

    surface->shaderProgram()->setUniformValue("isReceiveShadow", true);
    setLightsForShader(surface, surface->shaderProgram());

    float realColorFinal[3];
    realColorFinal[0] = realColor.red()/255.0;
    realColorFinal[1] = realColor.green()/255.0;
    realColorFinal[2] = realColor.blue()/255.0;

    GL->glUniform3fv(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "realColor"), 1, realColorFinal);
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelviewprojection"), 1, GL_FALSE,  mvpMat.data() );
    GL->glUniform1f(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "radial"), radial);

    drawRect();

    GL->glDisable(GL_CULL_FACE);
    GL->glCullFace(GL_BACK);


}

void NEGLManager::drawSkybox( IGLSurface *surface, QOpenGLTexture *cubeMapTexture )
{
    if (!cubeMapTexture)
        return;

    GLboolean enabLight = GL->glIsEnabled( GL_LIGHTING );

    GL->glDisable( GL_LIGHTING );
    GL->glDisable( GL_DEPTH_TEST );

    bindSkyboxShader(surface);

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "view"), 1, GL_FALSE,  modelview );

    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "skybox" ), 0 );
    cubeMapTexture->bind(0);

    drawCube();

    cubeMapTexture->release();
    if( enabLight )
        GL->glEnable( GL_LIGHTING );

    GL->glEnable( GL_DEPTH_TEST );
}
void NEGLManager::drawEnvironmentMap(IGLSurface *surface, GLuint envCubemap, float envRotation)
{
    GLboolean enabLight = GL->glIsEnabled( GL_LIGHTING );

    GL->glDisable( GL_LIGHTING );
    GL->glDisable( GL_DEPTH_TEST );

    bindIblBackgroundShader(surface);

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "view"), 1, GL_FALSE,  modelview );

    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "environmentMap" ), 0 );

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_CUBE_MAP, envCubemap);
    drawCube();

    if( enabLight )
        GL->glEnable( GL_LIGHTING );

    GL->glEnable( GL_DEPTH_TEST );

    /*
    if (!skySphereTexture)
        return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    GLboolean enabLight = glIsEnabled( GL_LIGHTING );

    setFillMode(NEFillMode::kSolidFront);
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );

    bindSkySphereShader();

    GLfloat modelview[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    glUniformMatrix4fv( glGetUniformLocation( shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    glUniformMatrix4fv( glGetUniformLocation( shaderProgram()->programId(), "view"), 1, GL_FALSE,  modelview );

    glUniform1i( glGetUniformLocation( shaderProgram()->programId(), "skySphere" ), 0 );
    glUniform1f( glGetUniformLocation( shaderProgram()->programId(), "envScale" ), envScale );
    glUniform1f( glGetUniformLocation( shaderProgram()->programId(), "envRotation" ), envRotation * 3.1415927 / 180 );
    skySphereTexture->bind(0);

    drawSphere();

    skySphereTexture->release();
    if( enabLight )
        glEnable( GL_LIGHTING );

    glEnable( GL_DEPTH_TEST );

    glPopAttrib();
    */
}
void NEGLManager::drawFlat(const NEIndexedTriMesh* mesh, IGLSurface* surface, bool boundingbox)
{
    Q_ASSERT_X( isBuffered( const_cast< NEIndexedTriMesh *>( mesh ) ) == true,
                "NERenderer::draw", "Meshes without buffering cannot be rendered");
    if(mesh->triangles().size() < 1)
        return;

    // If the mesh is associated with a material, draw it using its shader
    if( material( const_cast< NEIndexedTriMesh *>( mesh ) ) )
    {
        QOpenGLShaderProgram* shader = material( const_cast< NEIndexedTriMesh *>( mesh ) )->shaderProgram();

//        for(int i = 0 ; i < shader->shaders().size(); i ++)
//        {
//            std::cout << shader->shaders()[i]->objectName().toStdString() << ", " << std::endl;
//        }

//        shader->removeShader( P->m_PerPixelVS );
//        shader->removeShader( P->m_PerPixelFS );
//        shader->removeShader( P->m_ColoringFS );

//        shader->addShader( P->m_PerPixelFlatVS );
//        shader->addShader( P->m_PerPixelFlatFS );
//        shader->link();

        if(shader && shader->isLinked())
        {
            surface->setShaderProgram( shader );
            material( const_cast< NEIndexedTriMesh *>(mesh) )->updateUniforms();

            updatePositionUniforms(surface);

            surface->shaderProgram()->setUniformValue("TwoSided", true);
            surface->shaderProgram()->setUniformValue("isFlat", true);

            setLightsForShader(surface, shader);
        }
    }

    if(boundingbox)
        draw(kTriangleListPrim, boundingBoxBuffer( mesh ));
    else
        draw(kTriangleListPrim, vertexBuffer( mesh ) , indexBuffer( mesh ) );

    surface->shaderProgram()->setUniformValue("isFlat", false);
    surface->setShaderProgram( 0 );

//    if( material( const_cast< NEIndexedTriMesh *>( mesh ) ) )
//    {
//        QOpenGLShaderProgram* shader = material( const_cast< NEIndexedTriMesh *>(mesh) )->shaderProgram();
//        shader->removeShader(P->m_PerPixelFlatVS );
//        shader->removeShader( P->m_PerPixelFlatFS );
//        shader->addShader( P->m_PerPixelVS );
//        shader->addShader( P->m_PerPixelFS );
//        shader->addShader( P->m_ColoringFS );
//        shader->link();
//    }
}


void NEGLManager::draw(const NEIndexedTriMesh *mesh, bool boundingbox, bool pick, bool twoSided, bool isReceiveShadow, matrix44f modelMat)
{
    return draw( mesh, NULL, boundingbox, pick, twoSided, isReceiveShadow, modelMat );
}

void NEGLManager::draw( const NEIndexedTriMesh* mesh, IGLSurface* surface, bool boundingbox, bool pick, bool twoSided, bool isReceiveShadow, matrix44f modelMatrix, int isInactive )
{
    Q_ASSERT_X( isBuffered( const_cast< NEIndexedTriMesh *>( mesh ) ) == true,
                "NERenderer::draw", "Meshes without buffering cannot be rendered");

    if( mesh->triangles().size() < 1 )
        return;

    if( !pick )
    {
        // If the mesh is associated with a material, draw it using its shader
        if( material( const_cast< NEIndexedTriMesh *>( mesh ) )  && isInactive == 0)
        {
            QOpenGLShaderProgram* shader = material( const_cast< NEIndexedTriMesh *>( mesh ) )->shaderProgram();

            if(shader && shader->isLinked())
            {
                surface->setShaderProgram(shader);
                material( const_cast< NEIndexedTriMesh *>( mesh ) )->updateUniforms();
                if (surface) surface->updateEnvironmentUniforms();

                updatePositionUniforms(surface);
                setModelMatrixForPBR(surface, modelMatrix);
                //                glUniformMatrix4fv( glGetUniformLocation( shader->programId(), "modelMatrix" ), 1, GL_FALSE, modelMatrix.data() );

                surface->shaderProgram()->setUniformValue("TwoSided", twoSided);
                surface->shaderProgram()->setUniformValue("isReceiveShadow", isReceiveShadow);

                setLightsForShader(surface, shader);
                setClipPlanesForShader(shader);
            }
        }
        else if(isInactive == 1)
        {
            NEMaterial *pInactiveMaterial = NERENDERMAN.materials()["DefaultInactiveMaterial"];
            QOpenGLShaderProgram *shader = pInactiveMaterial->shaderProgram();
            if(shader && shader->isLinked()){
                surface->setShaderProgram(shader);
                pInactiveMaterial->updateUniforms();
                if (surface)
                    surface->updateEnvironmentUniforms();

                updatePositionUniforms(surface, false, isInactive);
                setModelMatrixForPBR(surface, modelMatrix);
                //                glUniformMatrix4fv( glGetUniformLocation( shader->programId(), "modelMatrix" ), 1, GL_FALSE, modelMatrix.data() );

                surface->shaderProgram()->setUniformValue("TwoSided", twoSided);
                surface->shaderProgram()->setUniformValue("isReceiveShadow", isReceiveShadow);

                setLightsForShader(surface, shader);
                setClipPlanesForShader(shader);

                surface->shaderProgram()->setUniformValue("meshColor", pInactiveMaterial->Color());
            }
        }
        else	// Draw it using color shader otherwise
        {
            bindPositionShader(surface);
            updatePositionUniforms(surface, false, isInactive);
        }
    }

    if(boundingbox)
        draw( kTriangleListPrim, boundingBoxBuffer(mesh) );
    else
        draw( kTriangleListPrim, vertexBuffer( mesh ), indexBuffer( mesh ) );
    surface->setShaderProgram(0);
}

void NEGLManager::drawWithDefaultMaterial( const NEIndexedTriMesh* mesh, IGLSurface* surface, bool twoSided, bool isReceiveShadow, matrix44f modelMatrix )
{
    Q_ASSERT_X( isBuffered( const_cast< NEIndexedTriMesh *>( mesh ) ) == true,
                "NERenderer::draw", "Meshes without buffering cannot be rendered");

    if( mesh->triangles().size() < 1 )
        return;

    // If the mesh is associated with a material, draw it using its shader
    NEMaterial* material = NERENDERMAN.materials()["DefaultMaterial"];
    QOpenGLShaderProgram* shader = material->shaderProgram();

    if(shader && shader->isLinked())
    {
        surface->setShaderProgram(shader);
        material->updateUniforms();
        if (surface) surface->updateEnvironmentUniforms();

        updatePositionUniforms(surface);
        setModelMatrixForPBR(surface, modelMatrix);
        //                glUniformMatrix4fv( glGetUniformLocation( shader->programId(), "modelMatrix" ), 1, GL_FALSE, modelMatrix.data() );

        surface->shaderProgram()->setUniformValue("TwoSided", twoSided);
        surface->shaderProgram()->setUniformValue("isReceiveShadow", isReceiveShadow);

        setLightsForShader(surface, shader);
        setClipPlanesForShader(shader);
    }

    draw( kTriangleListPrim, vertexBuffer( mesh ), indexBuffer( mesh ) );
    surface->setShaderProgram(0);
}

void NEGLManager::beginHairDraw(IGLSurface* surface)
{
    bindHairShader(surface);
    setLightsForShader(surface, surface->shaderProgram());
}

void NEGLManager::endHairDraw(IGLSurface *surface)
{
    surface->setShaderProgram(0);
}

void NEGLManager::beginLitDraw(IGLSurface* surface, bool twoSided)
{
    //if(cout == 0)
    //	return;

    QOpenGLShaderProgram* shader = P->m_DefaultMaterial->shaderProgram();
    if(shader && shader->isLinked())
    {
        surface->setShaderProgram(shader);
        P->m_DefaultMaterial->updateUniforms();

        if(twoSided)
            surface->shaderProgram()->setUniformValue("TwoSided", true);

        setLightsForShader(surface, shader);
    }
}


//NEFEXP
void NEGLManager::endLitDraw(IGLSurface *surface)
{
    surface->shaderProgram()->setUniformValue("TwoSided", false);
    surface->setShaderProgram(0);
}


void NEGLManager::setLightAndClipPlanForTerrain(IGLSurface* surface, QOpenGLShaderProgram* topographyShader)
{
    setLightsForShader(surface, topographyShader);
    setClipPlanesForShader(topographyShader);
}

void NEGLManager::drawShadowMap(IGLSurface *surface, const NEIndexedTriMesh* mesh)
{
    Q_ASSERT_X(isBuffered(const_cast<NEIndexedTriMesh *>(mesh)) == true,
               "NERenderer::draw", "Meshes without buffering cannot be rendered");
    if (mesh->triangles().size() < 1)
        return;

    bindSimpleDepthShader(surface);
    draw( kTriangleListPrim,vertexBuffer( mesh ),indexBuffer( mesh ) );
    surface->setShaderProgram(0);
}


void NEGLManager::drawInMotion(IGLSurface *surface, const NEIndexedTriMesh* mesh)
{
    surface->shaderProgram()->setAttributeArray(1, (QVector3D*)&(mesh->velocities()[0]), 0);
    surface->shaderProgram()->enableAttributeArray(1);

    if(mesh)
        draw(kTriangleListPrim, vertexBuffer( mesh ), indexBuffer( mesh ) );

    surface->shaderProgram()->disableAttributeArray(1);
}



void NEGLManager::drawQuad()
{
    GL->glDrawArrays(GL_TRIANGLES,0,3);
}

void NEGLManager::drawRect()
{
    GL->glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void NEGLManager::drawCube()
{
    draw(kTriangleListPrim, P->bgCube);
}

void NEGLManager::drawSphere()
{
    draw(kTriangleListPrim, P->bgSphere);
}


//NEFEXP
void NEGLManager::beginUnLitDraw(IGLSurface *surface)
{
    bindPositionShader(surface);
}

//NEFEXP
void NEGLManager::endUnLitDraw(IGLSurface *surface)
{
    surface->setShaderProgram(0);
}

//NEFEXP
void NEGLManager::beginParticleDraw( float radius, bool lighting, bool picked, bool asSand /*= false*/, bool isPicking /*= false */ )
{
    beginParticleDraw(NULL, radius, lighting, picked, asSand, isPicking);
}

//NEFEXP
void NEGLManager::beginParticleDraw( IGLSurface* surface, float radius, bool lighting, bool picked, bool asSand /*= false*/, bool isPicking /*= false */, bool isPCMap/* = false*/)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glEnable(GL_POINT_SPRITE);
    GL->glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    GL->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    GL->glDepthMask(GL_TRUE);
    GL->glEnable(GL_DEPTH_TEST);
    GL->glDisable( GL_CULL_FACE );
    if(asSand)
    {
        bindSandShader(surface);
        setLightsForShader(surface, surface->shaderProgram());
    }
    else
    {
        bindParticleShader(surface);
        setClipPlanesForShader(surface->shaderProgram());
    }
    surface->shaderProgram()->setUniformValue("pointRadius", radius);
    surface->shaderProgram()->setUniformValue("lighting", lighting);
    surface->shaderProgram()->setUniformValue("picked", picked);
    surface->shaderProgram()->setUniformValue("isPicking", isPicking);
    surface->shaderProgram()->setUniformValue("isPCMap", isPCMap);
    if (surface)
    {
        surface->shaderProgram()->setUniformValue("MaxNodes", surface->getOITMaxNodes());
        surface->shaderProgram()->setUniformValue("kindOIT", surface->getOITType());
    }

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );
}

//NEFEXP
void NEGLManager::endParticleDraw(IGLSurface *surface)
{
    //	glBindTexture(GL_TEXTURE_2D, 0);
    //	glDepthMask(GL_TRUE);
    //	glEnable(GL_DEPTH_TEST);
    //	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //	glDisable(GL_TEXTURE_2D);
    //	glDisable(GL_BLEND);
    //	glDisable(GL_POINT_SPRITE);
    surface->setShaderProgram( 0 );
    GL->glPopAttrib();
}

void NEGLManager::renderSpatial(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialRenderer* renderer = NERENDERMAN.getRenderer(spatial);
    if (renderer != NULL)
    {
        setClipPlanesForSpatial(spatial);
        renderer->render(spatial, surface);
        setClipPlanesForSpatial(NULL);
        return;
    }

    NESpatialRenderer::renderStatic(spatial, surface);
}

void NEGLManager::renderSpatialOnlyDepth(NESpatial *spatial, IGLSurface* surface)
{
    NESpatialRenderer* renderer = NERENDERMAN.getRenderer(spatial);
    if (renderer != NULL)
    {
        setClipPlanesForSpatial(spatial);
        renderer->renderOnlyDepth(spatial, surface);
        setClipPlanesForSpatial(NULL);
        return;
    }
}


void NEGLManager::renderWater(NESpatial* spatial, IGLSurface* surface)
{
    NESpatialRenderer* renderer = NERENDERMAN.getRenderer(spatial);
    if (renderer != NULL)
    {
        setClipPlanesForSpatial(spatial);
        renderer->renderWater(spatial, surface);
        setClipPlanesForSpatial(NULL);
        return;
    }
}
void NEGLManager::renderPicked(NESpatial *spatial, IGLSurface* surface)
{
    P->m_pGLRenderer->renderPicked( spatial, surface );
}

void NEGLManager::renderPickable(NESpatial *spatial, IGLSurface* surface)
{
    P->m_pGLRenderer->renderPickable( spatial, surface );
}

void NEGLManager::renderShadow(NESpatial *spatial, IGLSurface* surface)
{
    P->m_pGLRenderer->renderShadow( spatial, surface );
}

void NEGLManager::renderScenePerFrame(IGLSurface *surface,  float width, float height, float devPixelRatio)
{
    surface->renderScene(width*devPixelRatio, height*devPixelRatio);
}

#if 1
//! HBAO+
#if USE_EXT_GFSDK_SSAO
void NEGLManager::initAOContext()
{
    if(P->m_pAOContext) {
        P->m_pAOContext->AddRef();
        return;
    }

    GFSDK_SSAO_CustomHeap CustomHeap;
    CustomHeap.new_ = ::operator new;
    CustomHeap.delete_ = ::operator delete;

    GFSDK_SSAO_Status status;
    GFSDK_SSAO_GLFunctions sGLFuncs;
    GFSDK_SSAO_INIT_GL_FUNCTIONS(sGLFuncs)

    status = GFSDK_SSAO_CreateContext_GL(&P->m_pAOContext, &sGLFuncs, &CustomHeap);
    Q_ASSERT(status == GFSDK_SSAO_OK);
}

void NEGLManager::releaseAOContext()
{
    if(P->m_pAOContext)
        P->m_pAOContext->Release();
}



void NEGLManager::drawAO(GLuint dstFB, GLuint srcDepthTex, NEGLManager::AOParams params)
{
    if(P->m_pAOContext) {
        // Input parameters
        GFSDK_SSAO_InputData_GL Input;

        Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
        Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4(params.matProj);
        Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
        Input.DepthData.MetersToViewSpaceUnits = 0.005f;

        Input.DepthData.FullResDepthTexture.Target = (params.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
        Input.DepthData.FullResDepthTexture.TextureId = srcDepthTex;

        // Output parameters
        GFSDK_SSAO_Output_GL Output;
        Output.OutputFBO = dstFB;
        Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

        // AO parameters
        GFSDK_SSAO_Parameters AOParams;
        AOParams.Radius = params.radius;
        AOParams.Bias = params.bias;
        AOParams.SmallScaleAO = 1.0;
        AOParams.LargeScaleAO = 1.0;
        AOParams.PowerExponent = params.intensity;
        AOParams.Blur.Enable = params.blur;
        AOParams.Blur.Sharpness = params.blurSharpness;
        AOParams.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;

        GFSDK_SSAO_Status Status;
        Status = P->m_pAOContext->RenderAO(Input, AOParams, Output);
        // printf("Status : %d\n", Status);
        // printf("Allocated Memory : %d\n", P->m_pAOContext->GetAllocatedVideoMemoryBytes());
        Q_ASSERT(Status == GFSDK_SSAO_OK);
    }
}
#endif
#endif

void NEGLManager::setLightsForShader(IGLSurface* surface, QOpenGLShaderProgram* shader)
{
    for(int_size i=0; i<P->dirLights.size(); ++i)
        P->dirLights[i]->updateUniforms(surface, shader, (int)i, P->dirLights[i]->Behavior() == NENode::Active);

    for(int_size i=0; i<P->spotLights.size(); ++i)
        P->spotLights[i]->updateUniforms(surface, shader, (int)i, P->spotLights[i]->Behavior() == NENode::Active);

    for(int_size i=0; i<P->pointLights.size(); ++i)
        P->pointLights[i]->updateUniforms(shader, (int)i, P->pointLights[i]->Behavior() == NENode::Active);
}

#define MAXCLIP 32
void NEGLManager::setClipPlanesForShader(QOpenGLShaderProgram* shader)
{
    int clipCnt = P->clipPlanes.size()<MAXCLIP?P->clipPlanes.size():MAXCLIP;
    GL->glUniform1i(GL->glGetUniformLocation(shader->programId(),"nClipPlanes"), clipCnt);
    for(int_size i=0; i<clipCnt; ++i)
    {
        //        if(clipPlanes[i]->Behavior() == NENode::Active)
        P->clipPlanes[i]->updateUniforms(shader, (int)i, P->clipPlanes[i]->Behavior() == NENode::Active);
    }

}

void NEGLManager::setClipPlanesForSquare(bool isEnable)
{
    int clipCnt = P->clipPlanes.size()<6?P->clipPlanes.size():6;

    for(int i=0; i<clipCnt; ++i)
    {
        if(isEnable && P->clipPlanes[i] && P->clipPlanes[i]->Behavior() == NENode::Active)
        {
            vec3f normalDir = quaternionsFromEulerAngles(P->clipPlanes.at(i)->Orientation()).rotatedVector(P->clipPlanes.at(i)->PlaneNormal());
            vec4f val = vec4f(normalDir.x(), normalDir.y(), normalDir.z(), -vec3f::dotProduct(normalDir, P->clipPlanes.at(i)->Position()));
            vec4d planeEqn(val.x(), val.y(), val.z(), val.w());
            if(P->clipPlanes.at(i)->ClipInvert())
                planeEqn = -planeEqn;
            GL->glClipPlane( GL_CLIP_PLANE0+i, (double*)&planeEqn );
            GL->glEnable( GL_CLIP_PLANE0 + i );
        }
        else
            GL->glDisable( GL_CLIP_PLANE0 + i );
    }
}

void NEGLManager::setClipPlanesForSpatial(NESpatial *object)
{
    int clipCnt = P->clipPlanes.size()<MAXCLIP?P->clipPlanes.size():MAXCLIP;

    for(int_size i=0; i<clipCnt; ++i)
    {
        NEClipPlane* clipPlane = P->clipPlanes.at(i);
        if(!clipPlane || clipPlane->Behavior() == NENode::Inactive)
        {
            clipPlane->setProcess(false);
            continue;
        }
        const QMap<uint,NENode::NEConn>& srcConnsMap = P->clipPlanes[i]->sourceConnections();
        //        std::cout<<i<<"th SourceConnection Count:"<<srcConns.size()<<std::endl;
        QMap<uint, NENode::NEConn>::const_iterator iter = srcConnsMap.begin();

        if(!object || srcConnsMap.size() == 0)
            clipPlane->setProcess(true);
        else
        {
            for(; iter!=srcConnsMap.end(); ++iter)
            {
                NESpatial* pSrc = qobject_cast< NESpatial *>(iter->source);
                if(object == pSrc)
                {
                    clipPlane->setProcess(true);
                    return;
                }
            }
            clipPlane->setProcess(false);
        }

    }
}


NEMaterial* NEGLManager::getDefaultFluidMaterial()
{
    return P->m_DefaultFluidMaterial;
}

void NEGLManager::updateLightShaders()
{
    P->dirLights.clear();
    P->pointLights.clear();
    P->spotLights.clear();

    // Count total lights for each different type of light source
    Q_FOREACH(NELight* light, NERENDERMAN.lights())
    {
        if(light->Behavior() == NENode::Active)
        {
            if(light->factory().nodeName() == "Directional Light")
                P->dirLights.push_back((NEDirectionalLight*)light);
            else if(light->factory().nodeName() == "Point Light")
                P->pointLights.push_back((NEPointLight*)light);
            else if(light->factory().nodeName() == "Spot Light")
                P->spotLights.push_back((NESpotLight*)light);
        }
    }
    // Remove default shaders from all materials before editing
    Q_FOREACH(NEMaterial* material, NERENDERMAN.materials())
    {
        delete material->shaderProgram();
        material->setShaderProgram(new QOpenGLShaderProgram());
        material->shaderProgram()->addShader(material->defaultFS());
//        material->shaderProgram()->removeShader(P->m_PerPixelVS);
//        material->shaderProgram()->removeShader(P->m_PerPixelFS);
//        material->shaderProgram()->removeShader(P->m_PerPixelFlatVS);
//        material->shaderProgram()->removeShader(P->m_PerPixelFlatFS);
//        material->shaderProgram()->removeShader(P->m_FullScreenVS);
//        material->shaderProgram()->removeShader(P->m_WaterDisplayFS);
//        material->shaderProgram()->removeShader(P->m_LightsFS);
//        material->shaderProgram()->removeShader(P->m_ColoringFS);
    }

    if(getPedestalShader()->shaderProgram()->shaders().size() >= 3){
        getPedestalShader()->shaderProgram()->removeShader(P->m_LightsFS);
    }

    if(P->m_FloorDrawingShader->shaderProgram()->shaders().size() >= 3){
        P->m_FloorDrawingShader->shaderProgram()->removeShader(P->m_LightsFS);
    }

    // Delete the shaders to prevent memory leaks
    if(P->m_LightsFS)
    {
        delete P->m_LightsFS;
        P->m_LightsFS = 0;
    }
    if(P->m_LightsSpritesFS)
    {
        delete P->m_LightsSpritesFS;
        P->m_LightsSpritesFS = 0;
    }

    // Add the preprocessor macros on top of the shader
    // to transfer number of lights for each different type of light
    QString lightFragFinal;
    QString lightFragParticleFinal;

    QTextStream shEdit(&lightFragFinal);

    if (OpenGLVersion() >= 410)
    {
        shEdit << "#version 410 core" << '\n'
            << "#define nDirLights " << P->dirLights.size() << '\n'
            << "#define nPointLights " << P->pointLights.size() << '\n'
            << "#define nSpotLights " << P->spotLights.size() << '\n'
            << P->m_LightsFSCodes;
    }
    else
    {
        shEdit << "#version 330 core" << '\n'
            << "#define nDirLights " << P->dirLights.size() << '\n'
            << "#define nPointLights " << P->pointLights.size() << '\n'
            << "#define nSpotLights " << P->spotLights.size() << '\n'
            << P->m_LightsFSCodes;
    }


    QTextStream shEdit2(&lightFragParticleFinal);
    if (OpenGLVersion() >= 410)
    {
        shEdit2 << "#version 410 core" << '\n'
            << "#define nDirLights " << P->dirLights.size() << '\n'
            << "#define nPointLights " << P->pointLights.size() << '\n'
            << "#define nSpotLights " << P->spotLights.size() << '\n'
            << P->m_LightsParticlesFSCodes;
    }
    else
    {
        shEdit2 << "#version 330 core" << '\n'
            << "#define nDirLights " << P->dirLights.size() << '\n'
            << "#define nPointLights " << P->pointLights.size() << '\n'
            << "#define nSpotLights " << P->spotLights.size() << '\n'
            << P->m_LightsParticlesFSCodes;
    }


    P->m_LightsFS = new QOpenGLShader(QOpenGLShader::Fragment);
    P->m_LightsFS->setObjectName("Lights FS");
    if(!P->m_LightsFS->compileSourceCode(lightFragFinal))
    {
        qWarning() << "Lights.frag" <<  " " << P->m_LightsFS->log();
        std::cerr << "Lights.frag" << " " << P->m_LightsFS->log().toStdString() << std::endl;
    }

    P->m_LightsSpritesFS = new QOpenGLShader(QOpenGLShader::Fragment);
    P->m_LightsSpritesFS->setObjectName("Lights Particle FS");
    P->m_LightsSpritesFS->compileSourceCode(lightFragParticleFinal);

    //    lightsUpdated(P->m_LightsFS, P->m_LightsSpritesFS);

    getPedestalShader()->shaderProgram()->addShader( P->m_LightsFS );
    if (!getPedestalShader()->shaderProgram()->link())
    {
        qWarning() << "Pedestal shader program link failed: " <<  " " << getPedestalShader()->shaderProgram()->log();
        std::cerr << "Pedestal shader program link failed: " << " " << getPedestalShader()->shaderProgram()->log().toStdString() << std::endl;
    }

    P->m_FloorDrawingShader->shaderProgram()->addShader(P->m_LightsFS);
    if (!P->m_FloorDrawingShader->shaderProgram()->link())
    {
        qWarning() << "m_FloorDrawingShader shader program link failed: " <<  " " << P->m_FloorDrawingShader->shaderProgram()->log();
        std::cerr << "m_FloorDrawingShader shader program link failed: " << " " << P->m_FloorDrawingShader->shaderProgram()->log().toStdString() << std::endl;
    }
}


void NEGLManager::updateClipShaders()
{
    P->clipPlanes.clear();

    Q_FOREACH(NEClipPlane* cPlane, NERENDERMAN.clipPlanes())
    {
        if(cPlane->Behavior() == NENode::Active)
        {
            P->clipPlanes.push_back(cPlane);
        }
    }

//    // Remove default shaders from all materials before editing
//    Q_FOREACH(NEMaterial* material, NERENDERMAN.materials())
//    {
//        material->shaderProgram()->removeShader(P->m_PerPixelVS);
//        material->shaderProgram()->removeShader(P->m_PerPixelFS);
//        material->shaderProgram()->removeShader(P->m_PerPixelFlatVS);
//        material->shaderProgram()->removeShader(P->m_PerPixelFlatFS);
//        material->shaderProgram()->removeShader(P->m_FullScreenVS);
//        material->shaderProgram()->removeShader(P->m_WaterDisplayFS);
//        material->shaderProgram()->removeShader(P->m_ClipFS);
//        material->shaderProgram()->removeShader(P->m_ColoringFS);
//    }
    // Delete the shaders to prevent memory leaks
    if(P->m_ClipFS)
    {
        delete P->m_ClipFS;
        P->m_ClipFS = 0;
    }

    // Add the preprocessor macros on top of the shader
    // to transfer number of lights for each different type of light
    QString clipFragFinal;

    QTextStream shEdit(&clipFragFinal);

    shEdit  << "#version 410 core" << '\n'
            << "#define nClipPlanes " << P->clipPlanes.size() << '\n'
            << P->m_ClipFSCodes;

    P->m_ClipFS = new QOpenGLShader(QOpenGLShader::Fragment);
    P->m_ClipFS->setObjectName("Clip FS");
    if(!P->m_ClipFS->compileSourceCode(clipFragFinal))
    {
        qWarning() << "Clip.frag" <<  " " << P->m_ClipFS->log();
        std::cerr << "Clip.frag" << " " << P->m_ClipFS->log().toStdString() << std::endl;
    }

}


void NEGLManager::updateAllMaterialShaders()
{
    // Supply the default shaders to all materials
    Q_FOREACH(NEMaterial* material, NERENDERMAN.materials())
    {
        updateMaterialShaders(material);
    }
}

void NEGLManager::updateMaterialShaders( NEMaterial* material )
{
    switch(material->materialType())
    {
    case NEMaterial::Default:
    default:
        material->shaderProgram()->addShader( P->m_PerPixelVS );
        material->shaderProgram()->addShader( P->m_PerPixelFS );
        material->shaderProgram()->addShader( P->m_ColoringFS );
        break;
    case NEMaterial::Water:
        material->shaderProgram()->addShader( P->m_FullScreenVS );
        material->shaderProgram()->addShader( P->m_WaterDisplayFS );
        break;
    }

    material->shaderProgram()->addShader( material->defaultFS());
    material->shaderProgram()->addShader( P->m_LightsFS );
    material->shaderProgram()->addShader( P->m_ClipFS );
    material->shaderProgram()->addShader( P->m_RigidParticleFS );
    if (!material->shaderProgram()->link())
    {
        qWarning() << "Material shader program link failed: " <<  " " << material->shaderProgram()->log();
        std::cerr << "Material shader program link failed: " << " " << material->shaderProgram()->log().toStdString() << std::endl;
    }
    //Q_ASSERT_X(material->shaderProgram()->isLinked(), "NERenderer::updateAllShaders", "Cannot link material shader program");
    //printf("%s", material->shaderProgram()->log().toUtf8());
}


void NEGLManager::updateAllShaders()
{
    if( NEBASE.guiMode() == false )
        return;

    P->m_DefaultMaterial = NERENDERMAN.materials()[ "DefaultMaterial" ];
    P->m_DefaultFluidMaterial = NERENDERMAN.materials()[ "DefaultFluidMaterial" ];

    //
    updateLightShaders();
    updateClipShaders();

    if(!P->m_PerPixelVS)
    {
        P->m_PerPixelVS = new QOpenGLShader(QOpenGLShader::Vertex);
        P->m_PerPixelVS->setObjectName("Per Pixel VS");
        if( P->m_PerPixelVS->compileSourceCode(P->m_PerPixelVSCodes) == false )
        {
            qWarning() << P->m_PerPixelVS->log();
            std::cerr << P->m_PerPixelVS->log().toStdString() << std::endl;
        }
    }

    if(!P->m_PerPixelFS)
    {
        P->m_PerPixelFS = new QOpenGLShader(QOpenGLShader::Fragment);
        P->m_PerPixelFS->setObjectName("Per Pixel FS");
        if( P->m_PerPixelFS->compileSourceCode(P->m_PerPixelFSCodes) == false )
        {
            qWarning() << P->m_PerPixelFS->log();
            std::cerr << P->m_PerPixelFS->log().toStdString() << std::endl;
        }
    }

    if(!P->m_PerPixelFlatVS)
    {
        P->m_PerPixelFlatVS = new QOpenGLShader(QOpenGLShader::Vertex);
        P->m_PerPixelFlatVS->setObjectName("Per Pixel Flat VS");
        if( P->m_PerPixelFlatVS->compileSourceCode(P->m_PerPixelFlatVSCodes) == false )
        {
            qWarning() << P->m_PerPixelFlatVS->log();
            std::cerr << P->m_PerPixelFlatVS->log().toStdString() << std::endl;
        }
    }

    if(!P->m_PerPixelFlatFS)
    {
        P->m_PerPixelFlatFS = new QOpenGLShader(QOpenGLShader::Fragment);
        P->m_PerPixelFlatFS->setObjectName("Per Pixel Flat FS");
        if( P->m_PerPixelFlatFS->compileSourceCode(P->m_PerPixelFlatFSCodes) == false )
        {
            qWarning() << P->m_PerPixelFlatFS->log();
            std::cerr << P->m_PerPixelFlatFS->log().toStdString() << std::endl;
        }
    }

    if (!P->m_FullScreenVS)
    {
        P->m_FullScreenVS = new QOpenGLShader(QOpenGLShader::Vertex);
        P->m_FullScreenVS->setObjectName("Full Screen Quad VS");
        if( P->m_FullScreenVS->compileSourceCode(P->m_FullScreenVSCodes) == false )
        {
            qWarning() << P->m_FullScreenVS->log();
            std::cerr << P->m_FullScreenVS->log().toStdString() << std::endl;
        }
    }

    if (!P->m_WaterDisplayFS)
    {
        P->m_WaterDisplayFS = new QOpenGLShader(QOpenGLShader::Fragment);
        P->m_WaterDisplayFS->setObjectName("Water Display FS");
        if( P->m_WaterDisplayFS->compileSourceCode(P->m_WaterDisplayFSCodes) == false )
        {
            qWarning() << P->m_WaterDisplayFS->log();
            std::cerr << P->m_WaterDisplayFS->log().toStdString() << std::endl;
        }
    }

    if(!P->m_ColoringFS)
    {
        P->m_ColoringFS = new QOpenGLShader(QOpenGLShader::Fragment);
        P->m_ColoringFS->setObjectName("Coloring FS");
        if( P->m_ColoringFS->compileSourceCode(P->m_ColoringFSCodes) == false )
        {
            qWarning() << P->m_ColoringFS->log();
            std::cerr << P->m_ColoringFS->log().toStdString() << std::endl;
        }
    }
    if(!P->m_RigidParticleFS)
    {
        P->m_RigidParticleFS = new QOpenGLShader(QOpenGLShader::Fragment);
        P->m_RigidParticleFS->setObjectName("Rigid Particle FS");
        if( P->m_RigidParticleFS->compileSourceCode(P->m_RigidParticleCodes) == false )
        {
            qWarning() << P->m_RigidParticleFS->log();
            std::cerr << P->m_RigidParticleFS->log().toStdString() << std::endl;
        }
    }

    updateAllMaterialShaders();
}



std::vector<NEDirectionalLight*> NEGLManager::getDirectionLights()
{
    return P->dirLights;
}

std::vector<NEPointLight*> NEGLManager::getPointLights()
{
    return P->pointLights;
}

std::vector<NESpotLight*> NEGLManager::getSpotLights()
{
    return P->spotLights;
}

std::vector<NEClipPlane*> NEGLManager::getClipPlanes()
{
    return P->clipPlanes;
}

void NEGLManager::glPrint(QString name, int x, int y, Color32 color)
{
    GL->glColor3ubv((GLubyte*)&color);
    P->textureFont->Render(name.toUtf8(), -1, FTPoint(x, y));
}

void NEGLManager::glPrint3D(QString text, int len)
{
    P->polyFont->Render(text.toUtf8(), -1);
}


NEVertexBuffer* NEGLManager::createVertexBuffer(NEVertexFormat format, uint numVertices, void* data /*= 0*/, uint usage /*= kStreamDraw*/)
{
    NEVertexBuffer* newBuffer = new NEVertexBuffer();
    uint tmpErr = newBuffer->create(format, numVertices, data, usage);
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createVertexBuffer", "Cannot create the vertex buffer");
    _unused(tmpErr);
    return newBuffer;
}

NEVertexBuffer* NEGLManager::createVertexBuffer(NEVertexFormat format)
{
    NEVertexBuffer* newBuffer = new NEVertexBuffer();
    uint tmpErr = newBuffer->create(format);
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createVertexBuffer", "Cannot create the vertex buffer");
    _unused(tmpErr);
    return newBuffer;
}


void NEGLManager::destroy(NEVertexBuffer* vb)
{
    vb->destroy();
    delete vb;
    vb = 0;
}

NEIndexBuffer* NEGLManager::createIndexBuffer(uint numIndices, void* data /*= 0*/, uint usage /*= kStreamDraw*/)
{
    NEIndexBuffer* newBuffer = new NEIndexBuffer();
    uint tmpErr = newBuffer->create(numIndices, data, usage);
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createIndexBuffer", "Cannot create the index buffer");
    _unused(tmpErr);

    return newBuffer;
}

QString makePair(int index1, int index2)
{
    return QString::number(index1) + QString(",") + QString::number(index2);
}

NEVertexBuffer* NEGLManager::generateUnindexedVBuffer(NEVertexFormat format, uint numQauds, void* indices, void* vertices, uint usage)
{
    NEPBVertex *unIndexedVBuf = new NEPBVertex[numQauds * 6];

	NECTNPVertex *nverts = (NECTNPVertex *)vertices;
	uint *nindices = (uint *)indices;



    for(int i = 0; i < numQauds; i++)
    {
        uint newidx[6];
        int pairEquals[3];
        int pairSize = 0;

        for(int j = 0; j < 3; j++)
        {
            pairEquals[j] = -1;
            for(int k = 3; k < 6; k++)
            {
                if(nindices[6*i+j] == nindices[6*i+k] )
                {
                    pairEquals[j] = 6*i+k;
                    pairSize ++;

                }
            }
        }
        if(pairSize == 2)
        {
            if(pairEquals[0] == -1)
            {
                newidx[0] = nindices[6*i+0];
                newidx[1] = nindices[6*i+1];
                newidx[2] = nindices[6*i+2];

                newidx[3] = nindices[pairEquals[1]];
                newidx[5] = nindices[pairEquals[2]];
                for(int j = 3; j <6; j++ )
                {
                    if( nindices[6*i+j] != nindices[pairEquals[1]] && nindices[6*i+j] != nindices[pairEquals[2]])
                        newidx[4] = nindices[6*i+j];
                }
            }
            else if(pairEquals[1] == -1)
            {
                newidx[0] = nindices[6*i+1];
                newidx[1] = nindices[6*i+2];
                newidx[2] = nindices[6*i+0];

                newidx[3] = nindices[pairEquals[2]];
                newidx[5] = nindices[pairEquals[0]];
                for(int j = 3; j <6; j++ )
                {
                    if( nindices[6*i+j] != nindices[pairEquals[2]] && nindices[6*i+j] != nindices[pairEquals[0]])
                        newidx[4] = nindices[6*i+j];
                }
            }
            else if(pairEquals[2] == -1)
            {
                newidx[0] = nindices[6*i+2];
                newidx[1] = nindices[6*i+0];
                newidx[2] = nindices[6*i+1];

                newidx[3] = nindices[pairEquals[0]];
                newidx[5] = nindices[pairEquals[1]];
                for(int j = 3; j <6; j++ )
                {
                    if( nindices[6*i+j] != nindices[pairEquals[0]] && nindices[6*i+j] != nindices[pairEquals[1]])
                        newidx[4] = nindices[6*i+j];
                }
            }

            unIndexedVBuf[6*i+0].position = nverts[newidx[0]].position;
            unIndexedVBuf[6*i+0].barycentric = vec3f(0, 0, 1);

            unIndexedVBuf[6*i+1].position = nverts[newidx[1]].position;
            unIndexedVBuf[6*i+1].barycentric = vec3f(0, 1, 0);

            unIndexedVBuf[6*i+2].position = nverts[newidx[2]].position;
            unIndexedVBuf[6*i+2].barycentric = vec3f(1, 0, 1);

            unIndexedVBuf[6*i+3].position = nverts[newidx[3]].position;
            unIndexedVBuf[6*i+3].barycentric = vec3f(0, 1, 0);

            unIndexedVBuf[6*i+4].position = nverts[newidx[4]].position;
            unIndexedVBuf[6*i+4].barycentric = vec3f(0, 0, 1);

            unIndexedVBuf[6*i+5].position = nverts[newidx[5]].position;
            unIndexedVBuf[6*i+5].barycentric = vec3f(1, 0, 1);

        }
        else
        {
            newidx[0] = nindices[6*i+0];
            newidx[1] = nindices[6*i+1];
            newidx[2] = nindices[6*i+2];
            newidx[3] = nindices[6*i+3];
            newidx[4] = nindices[6*i+4];
            newidx[5] = nindices[6*i+5];

            unIndexedVBuf[6*i+0].position = nverts[newidx[0]].position;
            unIndexedVBuf[6*i+0].barycentric = vec3f(0, 0, 1);

            unIndexedVBuf[6*i+1].position = nverts[newidx[1]].position;
            unIndexedVBuf[6*i+1].barycentric = vec3f(0, 1, 0);

            unIndexedVBuf[6*i+2].position = nverts[newidx[2]].position;
            unIndexedVBuf[6*i+2].barycentric = vec3f(1, 0, 0);

            unIndexedVBuf[6*i+3].position = nverts[newidx[3]].position;
            unIndexedVBuf[6*i+3].barycentric = vec3f(0, 1, 0);

            unIndexedVBuf[6*i+4].position = nverts[newidx[4]].position;
            unIndexedVBuf[6*i+4].barycentric = vec3f(0, 0, 1);

            unIndexedVBuf[6*i+5].position = nverts[newidx[5]].position;
            unIndexedVBuf[6*i+5].barycentric = vec3f(1, 0, 0);
        }



    }

	NEVertexBuffer* newBuffer = new NEVertexBuffer();
    uint tmpErr = newBuffer->create(format, numQauds * 6, unIndexedVBuf, usage);
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createVertexBuffer", "Cannot create the vertex buffer");
    _unused(tmpErr);

    delete[] unIndexedVBuf;

    return newBuffer;
}


NEVertexBuffer* NEGLManager::generateBoundingBoxBuffer(NEVertexFormat formatShaded, NEVertexFormat formatWireframe, uint numVertices, void* vertices, uint usage, NEVertexBuffer* &outUnindexedBuffer)
{
    NECTNPVertex *nverts = (NECTNPVertex *)vertices;
    AABB globalAABB;
    for(int i = 0; i < numVertices; i++)
        globalAABB.insertPoint( nverts[i].position );


    vec3f vtx[8];
    vec3f minP = globalAABB.minPos();
    vec3f maxP = globalAABB.maxPos();
    vtx[0] = minP;
    vtx[6] = maxP;
    vtx[1] = vec3f(minP.x(),maxP.y(),minP.z());
    vtx[2] = vec3f(maxP.x(),maxP.y(),minP.z());
    vtx[3] = vec3f(maxP.x(),minP.y(),minP.z());
    vtx[4] = vec3f(minP.x(),minP.y(),maxP.z());
    vtx[5] = vec3f(minP.x(),maxP.y(),maxP.z());
    vtx[7] = vec3f(maxP.x(),minP.y(),maxP.z());

    NECTNPVertex *bbVertics = new NECTNPVertex[36];

    // build back face
    bbVertics[0].position = vtx[1];
    bbVertics[1].position = vtx[2];
    bbVertics[2].position = vtx[0];

    bbVertics[3].position = vtx[2];
    bbVertics[4].position = vtx[3];
    bbVertics[5].position = vtx[0];

    for(int i = 0; i < 6; i++)
        bbVertics[i].normal = vec3f(0, 0, -1);

    // build front face
    bbVertics[6].position = vtx[5];
    bbVertics[7].position = vtx[4];
    bbVertics[8].position = vtx[6];

    bbVertics[9].position = vtx[4];
    bbVertics[10].position = vtx[7];
    bbVertics[11].position = vtx[6];

    for(int i = 0; i < 6; i++)
        bbVertics[i + 6].normal = vec3f(0, 0, 1);

    // build left face
    bbVertics[12].position = vtx[5];
    bbVertics[13].position = vtx[1];
    bbVertics[14].position = vtx[4];

    bbVertics[15].position = vtx[1];
    bbVertics[16].position = vtx[0];
    bbVertics[17].position = vtx[4];

    for(int i = 0; i < 6; i++)
        bbVertics[i + 12].normal = vec3f(-1, 0, 0);

    // build right face
    bbVertics[18].position = vtx[6];
    bbVertics[19].position = vtx[7];
    bbVertics[20].position = vtx[2];

    bbVertics[21].position = vtx[7];
    bbVertics[22].position = vtx[3];
    bbVertics[23].position = vtx[2];

    for(int i = 0; i < 6; i++)
        bbVertics[i + 18].normal = vec3f(1, 0, 0);

    //build top face
    bbVertics[24].position = vtx[1];
    bbVertics[25].position = vtx[5];
    bbVertics[26].position = vtx[2];

    bbVertics[27].position = vtx[5];
    bbVertics[28].position = vtx[6];
    bbVertics[29].position = vtx[2];

    for(int i = 0; i < 6; i++)
        bbVertics[i + 24].normal = vec3f(0, 1, 0);

    //build bottom face
    bbVertics[30].position = vtx[0];
    bbVertics[31].position = vtx[3];
    bbVertics[32].position = vtx[4];

    bbVertics[33].position = vtx[3];
    bbVertics[34].position = vtx[7];
    bbVertics[35].position = vtx[4];

    for(int i = 0; i < 6; i++)
        bbVertics[i + 30].normal = vec3f(0, -1, 0);

    for(int i = 0; i < 36; i ++)
    {
        bbVertics[i].color = nverts[0].color;
        bbVertics[i].tCoord = nverts[0].tCoord;
    }

    NEVertexBuffer* newBuffer = new NEVertexBuffer();
    uint tmpErr = newBuffer->create(formatShaded, 36, bbVertics, usage);
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createVertexBuffer", "Cannot create the vertex buffer");
    _unused(tmpErr);



    NEPBVertex * bbVerticsUnindexed = new NEPBVertex[36];

    for(int i = 0; i < 36; i ++)
    {
        bbVerticsUnindexed[i].position = bbVertics[i].position;

    }


    for(int i = 0; i < 6; i++)
    {
        bbVerticsUnindexed[6*i+0].barycentric = vec3f(0, 0, 1);
        bbVerticsUnindexed[6*i+1].barycentric = vec3f(0, 1, 0);
        bbVerticsUnindexed[6*i+2].barycentric = vec3f(1, 0, 1);
        bbVerticsUnindexed[6*i+3].barycentric = vec3f(0, 1, 0);
        bbVerticsUnindexed[6*i+4].barycentric = vec3f(0, 0, 1);
        bbVerticsUnindexed[6*i+5].barycentric = vec3f(1, 0, 1);
    }

    outUnindexedBuffer = new NEVertexBuffer();
    tmpErr = outUnindexedBuffer->create(formatWireframe, 36, bbVerticsUnindexed, usage);
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createVertexBuffer", "Cannot create the vertex buffer");
    _unused(tmpErr);

    delete[] bbVertics;
    delete[] bbVerticsUnindexed;

    return newBuffer;
}

NEIndexBuffer* NEGLManager::generateAdjacencyIndexBuffer(uint numIndices, void* data, uint usage)
{
    uint *indices = (uint *)data;

    QMap<QString, int> adjEdgeMap;

    uint *indicesWithAdj = new uint[numIndices * 2];

    //printf("New Mesh\n\n");
    for(uint i = 0; i < numIndices / 3; i++){
        QString edge = makePair(indices[3 * i], indices[3 * i + 1]);
        adjEdgeMap.insert(edge, indices[3*i+2]);

        edge = makePair(indices[3 * i + 1], indices[3 * i + 2]);
        adjEdgeMap.insert(edge, indices[3*i]);

        edge = makePair(indices[3 * i + 2], indices[3 * i]);
        adjEdgeMap.insert(edge, indices[3*i + 1]);
    }

    //for( QMap<QString, int>::iterator iter = adjEdgeMap.begin(); iter != adjEdgeMap.end(); iter++)
    //    printf("(%s)-->(%d)\n", iter.key().toStdString().c_str(), iter.value());

    //printf("Face\n\n");
    for(uint i = 0; i < numIndices / 3; i ++){
        indicesWithAdj[6 * i] = indices[3 * i];
        indicesWithAdj[6 * i + 2] = indices[3 * i + 1];
        indicesWithAdj[6 * i + 4] = indices[3 * i + 2];
        QString edge = makePair(indices[3 * i + 1], indices[3 * i]);
        if(adjEdgeMap.contains(edge))
            indicesWithAdj[6 * i + 1] = adjEdgeMap.value(edge);
        else
            indicesWithAdj[6 * i + 1] = indices[3 * i + 1];

        edge = makePair(indices[3 * i + 2], indices[3 * i + 1]);
        if(adjEdgeMap.contains(edge))
            indicesWithAdj[6 * i + 3] = adjEdgeMap.value(edge);
        else
            indicesWithAdj[6 * i + 3] = indices[3 * i + 2];

        edge = makePair(indices[3 * i], indices[3 * i + 2]);
        if(adjEdgeMap.contains(edge))
            indicesWithAdj[6 * i + 5] = adjEdgeMap.value(edge);
        else
            indicesWithAdj[6 * i + 5] = indices[3 * i];
    }

    //for( int i = 0; i < numIndices / 3; i++)
    //{
    //    printf("Index: ");
    //    for( int j = 0; j < 6; j++)
    //    {
    //        printf("%d ", indicesWithAdj[i*6 + j]);
    //    }
    //    printf("\n");
    //}
    //printf("\n");

    NEIndexBuffer* newBuffer = new NEIndexBuffer();
    uint tmpErr = newBuffer->create(numIndices * 2, indicesWithAdj, usage);
    delete [] indicesWithAdj;
    Q_ASSERT_X(tmpErr == NE::kReturnSuccess, "NEGLManager::createIndexBuffer", "Cannot create the index buffer");
    _unused(tmpErr);

    return newBuffer;
}

void NEGLManager::destroy(NEIndexBuffer* ib)
{
    ib->destroy();
    delete ib;
    ib = 0;
}

void NEGLManager::bindPositionShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_PositionShader->shaderProgram());
}

void NEGLManager::bindLineStippleShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_LineStippleShader->shaderProgram());
}

void NEGLManager::bindRectYFilterShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_RectYFilterShader->shaderProgram());
}

void NEGLManager::bindColorRampOSDShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_ColorRampOSDShader->shaderProgram());
}



void NEGLManager::bindFloorDrawingShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_FloorDrawingShader->shaderProgram());
}

void NEGLManager::bindSolidWireframeShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_SolidWireframeShader->shaderProgram());
}

void NEGLManager::bindQuadWireframeShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_QuadWireframeShader->shaderProgram());
}

void NEGLManager::bindWeightedBlendedFinalShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_WeightedBlendedFinalShader->shaderProgram());
}

void NEGLManager::bindLinkedListFinalShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_LinkedListFinalShader->shaderProgram());
}


void NEGLManager::bindDepthPeelingBlendShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_DepthPeelingBlendShader->shaderProgram());
}

void NEGLManager::bindDepthPeelingFinalShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_DepthPeelingFinalShader->shaderProgram());
}


void NEGLManager::bindCPShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_CPShader->shaderProgram());
}

void NEGLManager::bindCPWaterShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_CPWaterShader->shaderProgram());
}

void NEGLManager::bindCamLitShader(IGLSurface *surface)
{
    //surface->setShaderProgram(m_CamLitShader->shaderProgram());
}

void NEGLManager::bindRectTextureShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_RectTextureShader->shaderProgram());
}

void NEGLManager::bindGVDBComposeShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_GVDBComposeShader->shaderProgram());
}

void NEGLManager::bindParticleShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_ParticleShader->shaderProgram());
}

void NEGLManager::bindAOParticleShader(IGLSurface *surface)
{
    //surface->setShaderProgram(P->m_AOParticleShader->shaderProgram());
}

void NEGLManager::bindAOResultShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_AOResultShader->shaderProgram());
}

void NEGLManager::bindSandShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_SandShader->shaderProgram());
}

void NEGLManager::bindHairShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_HairShader->shaderProgram());
}

void NEGLManager::bindColorMapperShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_ShaderColorMapper->shaderProgram());
}

void NEGLManager::bindMotionShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_MotionShader->shaderProgram());
}

void NEGLManager::bindBgShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_bgShader->shaderProgram());
}

void NEGLManager::bindPedestalShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_pedestalShader->shaderProgram());
}

void NEGLManager::bindSimpleDepthShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_simpleDepthShader->shaderProgram());
}

void NEGLManager::bindShadowMappingShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_shadowMappingShader->shaderProgram());
}

void NEGLManager::bindSkyboxShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_skyboxShader->shaderProgram());
}

void NEGLManager::bindSkySphereShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_skySphereShader->shaderProgram());
}

void NEGLManager::bindFXAAShader(IGLSurface *surface)
{
    surface->setShaderProgram(P->m_fxaaShader->shaderProgram());
}

void NEGLManager::bindFXAAShader(int level, IGLSurface *surface)
{
    if(level == 0)
        surface->setShaderProgram(P->m_fxaaLowShader->shaderProgram());
    else if(level == 1)
        surface->setShaderProgram(P->m_fxaaMediumShader->shaderProgram());
    else if(level == 2)
        surface->setShaderProgram(P->m_fxaaHighShader->shaderProgram());
    else if(level == 3)
        surface->setShaderProgram(P->m_fxaaExtremeShader->shaderProgram());
}

void NEGLManager::bindEdgeShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_EdgeShader->shaderProgram() );
}

void NEGLManager::bindSmaaEdgeShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_smaaEdgeShader->shaderProgram() );
}

void NEGLManager::bindSmaaBlendShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_smaaBlendShader->shaderProgram() );
}

void NEGLManager::bindSmaaNeighborShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_smaaNeighborShader->shaderProgram() );
}

void NEGLManager::bindIblEquirectangularToCubemapShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_iblEquirectangularToCubemapShader->shaderProgram() );
}

void NEGLManager::bindIblIrradianceShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_iblIrradianceShader->shaderProgram() );
}

void NEGLManager::bindIblBackgroundShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_iblBackgroundShader->shaderProgram() );
}

void NEGLManager::bindIblPrefilterShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_iblPrefilterShader->shaderProgram() );
}

void NEGLManager::bindIblBrdfShader(IGLSurface *surface)
{
    surface->setShaderProgram( P->m_iblBrdfShader->shaderProgram() );
}

void NEGLManager::updateCommonParticleUniforms(IGLSurface* surface)
{
    float pointScale;
    if(surface->shaderProgram() && surface->camera())
    {
		
        if(surface->camera()->fixedOrthographic())
        {
            pointScale = (float)surface->getGLWidth() / surface->camera()->OrthoWidth();
            surface->shaderProgram()->setUniformValue("pointScale", pointScale);
            surface->shaderProgram()->setUniformValue("ortho", true);
        }
        else if(surface->camera()->customOrthographic())
        {
            float orthoWidth = tan(DEGREE_TO_RADIAN(surface->camera()->Fov()) * 0.5) * surface->camera()->OrthoWidth() * 20.0;
            pointScale = (float)surface->getGLWidth() / orthoWidth;
            surface->shaderProgram()->setUniformValue("pointScale", pointScale);
            surface->shaderProgram()->setUniformValue("ortho", true);
        }
        else
        {
            pointScale = (float)surface->getGLWidth() / tanf(surface->camera()->Fov() * RADIANS_IN_1_DEGREE_OVER_2);
            surface->shaderProgram()->setUniformValue("pointScale", pointScale);
            surface->shaderProgram()->setUniformValue("ortho", false);
        }

    }
}

void NEGLManager::updatePositionUniforms(bool isPicked)
{
    updatePositionUniforms(NULL, isPicked);
}

void NEGLManager::updatePositionUniforms(IGLSurface* surface, bool isPicked, int isInactive)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    matrix44f newModelViewMat(modelview);

    if(isInactive == 2){
        matrix44f scaleMat;
        scaleMat.setToIdentity();
        scaleMat.scale(1.03, 1.03, 1.03);

        matrix44f transMat;
        transMat.setToIdentity();
        transMat.translate(-modelview[3], -modelview[7], -modelview[11]);
        newModelViewMat = scaleMat * transMat * newModelViewMat;
        transMat.setToIdentity();
        transMat.translate(modelview[3], modelview[7], modelview[11]);

        newModelViewMat = transMat *newModelViewMat;
    }
    newModelViewMat = newModelViewMat.transposed();


    float currentColor[4];
    if(isInactive == 0){
        GL->glGetFloatv( GL_CURRENT_COLOR, currentColor );
    }
    else if( isInactive == 1){
        currentColor[0] = 0.3;
        currentColor[1] = 0.3;
        currentColor[2] = 0.3;
        currentColor[3] = 1.0;
    }
    else if( isInactive == 2){
        currentColor[0] = 1.0;
        currentColor[1] = 1.0;
        currentColor[2] = 1.0;
        currentColor[3] = 1.0;
    }


    GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), 1, currentColor );

    GL->glUniformMatrix4fv(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  newModelViewMat.data());

    if (surface)
    {
        surface->shaderProgram()->setUniformValue( "kindOIT", surface->getOITType() );
        surface->shaderProgram()->setUniformValue( "MaxNodes", surface->getOITMaxNodes() );
        surface->shaderProgram()->setUniformValue( "depthPeelingDepthTex", 1);
    }

    surface->shaderProgram()->setUniformValue( "isPicked", isPicked );
    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "useVertexColor" ), false );
}

void NEGLManager::updateLineStippleUniforms(IGLSurface* surface, float dashSize, float gapSize)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );
    matrix44f mvmat(modelview);
    matrix44f projmat(proj);
    matrix44f mvp = projmat * mvmat;
    //surface->setModelViewMatrix();
    //matrix44f mvpmat = surface->m_MVPMatrix;
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_mvp"), 1, GL_FALSE, mvp.data() );

    float currentColor[4];
    GL->glGetFloatv( GL_CURRENT_COLOR, currentColor );
    GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_meshColor" ), 1, currentColor );

    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_dashSize" ), dashSize );
    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_gapSize" ), gapSize );

    GLfloat resolution[2] = {500, 500};
    GL->glUniform2fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_resolution" ), 1, resolution );
}

void NEGLManager::updateRectYFilterUniforms(IGLSurface* surface, QColor minColor, QColor maxColor, float upperValue, float lowerValue, float layerHeight)
{
    if(surface)
    {
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "upperValue" ), upperValue );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "lowerValue" ), lowerValue );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "layerheight" ), layerHeight );

        vec3f minColorv = vec3f(minColor.red() / 255.0, minColor.green() / 255.0, minColor.blue() / 255.0);
        vec3f maxColorv = vec3f(maxColor.red() / 255.0, maxColor.green() / 255.0, maxColor.blue() / 255.0);

        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), true );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), 2 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[0]").toUtf8() ), 1, &minColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[0]").toUtf8() ), 0.0 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[1]").toUtf8() ), 1, &maxColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[1]").toUtf8() ), 1.0 );

    }
}

void NEGLManager::updateColorRampOSDUniforms(IGLSurface* surface, QColor minColor, QColor maxColor)
{
    if(surface)
    {
        vec3f minColorv = vec3f(minColor.red() / 255.0, minColor.green() / 255.0, minColor.blue() / 255.0);
        vec3f maxColorv = vec3f(maxColor.red() / 255.0, maxColor.green() / 255.0, maxColor.blue() / 255.0);

        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "interpolationMode" ), true );
        GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "rampCPCount" ), 2 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[0]").toUtf8() ), 1, &minColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[0]").toUtf8() ), 0.0 );
        GL->glUniform3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampColor[1]").toUtf8() ), 1, &maxColorv[0] );
        GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), QString("rampPos[1]").toUtf8() ), 1.0 );
    }
}

void NEGLManager::updateFloorDrawingUniforms(IGLSurface* surface, float Xmin, float Xmax, float Zmin, float Zmax)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE, modelview);

    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_Xmin" ), Xmin );
    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_Xmax" ), Xmax );
    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_Zmin" ), Zmin );
    GL->glUniform1f( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_Zmax" ), Zmax );
}

void NEGLManager::updateSolidWireframeUniforms(IGLSurface* surface, QColor color, float thickness, float threshold, bool bUseDash)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );

    //glUniform4fv(glGetUniformLocation( shaderProgram()->programId(), "solidwirecolor"), 1, color);
    surface->shaderProgram()->setUniformValue(QString("solidwirecolor").toUtf8().constData(), color );
    GL->glUniform1f(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "thickness"), thickness);
    GL->glUniform1f(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "threshold"), threshold);

    float resolution[2];
    resolution[0] = surface->getWidth();
    resolution[1] = surface->getHeight();
    GL->glUniform2fv(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_resolution"), 1, resolution );

    float dashSize = 10.f;
    float gapSize = bUseDash ? 10.f : 0.f;
    GL->glUniform1f(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_dashSize"), dashSize );
    GL->glUniform1f(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_gapSize"), gapSize);
    //GL->glUniform1f(GL->glGetUniformLocation( surface->shaderProgram()->programId(), "u_gapSize"), bUseDash ? 10.f : 0.f );

    // for depth peeling
    if (surface)
    {
        surface->shaderProgram()->setUniformValue( "kindOIT", surface->getOITType() );
        surface->shaderProgram()->setUniformValue( "MaxNodes", surface->getOITMaxNodes() );
        surface->shaderProgram()->setUniformValue( "depthPeelingDepthTex", 1);
    }

}

void NEGLManager::updateQuadWireframeUniforms(IGLSurface* surface, QColor color, bool hideBackface, bool swapUpAxes)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );

    //glUniform4fv(glGetUniformLocation( shaderProgram()->programId(), "solidwirecolor"), 1, color);
    surface->shaderProgram()->setUniformValue(QString("meshColor").toUtf8().constData(), color );
    surface->shaderProgram()->setUniformValue(QString("hideBackfacing").toUtf8().constData(), hideBackface );
    surface->shaderProgram()->setUniformValue(QString("swapUpAxes").toUtf8().constData(), swapUpAxes );

    // for depth peeling
    if (surface)
    {
        surface->shaderProgram()->setUniformValue( "kindOIT", surface->getOITType() );
        surface->shaderProgram()->setUniformValue( "MaxNodes", surface->getOITMaxNodes() );
        surface->shaderProgram()->setUniformValue( "depthPeelingDepthTex", 1);
    }

}


void NEGLManager::updateCPUniforms(IGLSurface *surface)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );
}

void NEGLManager::updateCPWaterUniforms(IGLSurface* surface, bool useVertexColor)
{
    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );

    float currentColor[4];
    GL->glGetFloatv( GL_CURRENT_COLOR, currentColor );

    GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), 1, currentColor );
    GL->glUniform1i( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "useVertexColor" ), useVertexColor );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );

    if( surface )
    {
        surface->shaderProgram()->setUniformValue( "kindOIT", surface->getOITType() );
        surface->shaderProgram()->setUniformValue( "MaxNodes", surface->getOITMaxNodes() );
        surface->shaderProgram()->setUniformValue( "depthPeelingDepthTex", 1);
    }

}

void NEGLManager::updateWaterUniforms(IGLSurface* surface)
{
    float screenWidth = surface->getGLWidth();
    float screenHeight = surface->getGLHeight();
    float fov = surface->camera()->Fov() * M_PI / 180.0f;
    const float screenAspect = screenWidth / screenHeight;
    float invTexScale[2] = {1.0f/screenWidth, 1.0f/screenHeight};
    float clipPosToEye[2] = {tanf(fov*0.5f)*screenAspect, tanf(fov*0.5f)};
    float ior = 1.0f;

    GL->glUniform2fv(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "invTexScale"), 1, invTexScale);
    GL->glUniform2fv(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "clipPosToEye"), 1, clipPosToEye);
    GL->glUniform1f(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "ior"),  ior);
    GL->glUniform1i(GL->glGetUniformLocation(surface->shaderProgram()->programId(), "debug"), 0);

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );
    GLfloat proj[16];
    GL->glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat*) proj );
    matrix44f mat_modelview(modelview);
    mat_modelview = mat_modelview.transposed(); // note that matrix44f is transposed compared to opengl
    bool isInvertible;
    matrix44f mat_inv_modelview = mat_modelview.inverted(&isInvertible);
    matrix44f mat_proj(proj);
    mat_proj = mat_proj.transposed(); // note that matrix44f is transposed compared to opengl
    matrix44f mat_inv_proj = mat_proj.inverted(&isInvertible);
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projection"), 1, GL_FALSE, proj );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelview"), 1, GL_FALSE,  modelview );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelviewinv"), 1, GL_FALSE,  (GLfloat*)(&mat_inv_modelview.data()[0]) );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "projectionMatInv"), 1, GL_FALSE,  (GLfloat*)(&mat_inv_proj.data()[0]) );
}

// this function is purpose for pbr material render
// this function call after update modelview Uniforms
void NEGLManager::setModelMatrixForPBR(IGLSurface *surface, matrix44f &modelMatrix)
{
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "modelMatrix"), 1, GL_FALSE,  (GLfloat *)(&modelMatrix.data()[0]));

    GLfloat modelview[16];
    GL->glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat*) modelview );

    matrix33f normalMat33 =
    { modelview[0], modelview[4], modelview[8],
      modelview[1], modelview[5], modelview[9],
      modelview[2], modelview[6], modelview[10]};
    GLfloat *modelMatrixArray = modelMatrix.data();
    matrix33f modelMatrix33 =
    { modelMatrixArray[0], modelMatrixArray[4], modelMatrixArray[8],
      modelMatrixArray[1], modelMatrixArray[5], modelMatrixArray[9],
      modelMatrixArray[2], modelMatrixArray[6], modelMatrixArray[10]};
    matrix33f viewRot = normalMat33 * modelMatrix33.transposed();

    GL->glUniformMatrix3fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "viewRot"), 1, GL_FALSE,  (GLfloat *)(&viewRot.data()[0]));
}


/*
        Callback function for NERenderer::updateLightShaders()
        Every shaderProgram that makes use of lights should remove the old
*/
void NEGLManager::lightsUpdated(QOpenGLShader* lightShader, QOpenGLShader* lightSpriteShader)
{
    //recreateShader(m_SandShader);
    //m_SandShader->shaderProgram()->addShader(lightShader);
    //linkShader(m_SandShader->shaderProgram());

    QOpenGLShaderProgram* shader = P->m_SandShader->mShader;
    if(shader != NULL)
        delete shader;
    shader = new QOpenGLShaderProgram();
    shader->setObjectName("Sand shader");
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, P->m_SandShader->mVertexPath);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, P->m_SandShader->mFragmentPath);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "System/Shaders/Material/PerPixel/sand.frag");
    shader->addShader(lightSpriteShader);
    P->m_SandShader->mShader = shader;
    if(!P->m_SandShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(Particle shader)" << std::endl;
        std::cerr << P->m_SandShader->shaderProgram()->log().toStdString() << std::endl;
    }

    shader = P->m_HairShader->mShader;
    if(shader != NULL)
        delete shader;
    shader = new QOpenGLShaderProgram();
    shader->setObjectName("Hair Shader");
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex, P->m_HairShader->mVertexPath);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, P->m_HairShader->mFragmentPath);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "System/Shaders/Material/PerPixel/hair.frag");
    shader->addShader(lightShader);
    P->m_HairShader->mShader = shader;
    if(!P->m_HairShader->shaderProgram()->link())
    {
        std::cerr << "Linking Error(Hair shader)" << std::endl;
        std::cerr << P->m_HairShader->shaderProgram()->log().toStdString() << std::endl;

    }

}

const matrix44f& NEGLManager::normalMatrix()
{
    return P->m_NormalMatrix;
}

const matrix44f& NEGLManager::modelViewMatrix()
{
    return P->m_ModelViewMatrix;
}

const matrix44f& NEGLManager::worldMatrix()
{
    return P->m_WorldMatrix;
}

const matrix44f& NEGLManager::viewMatrix()
{
    return P->m_ViewMatrix;
}

void NEGLManager::normalMatrix(const matrix44f &nMatrix)
{
    P->m_NormalMatrix = nMatrix;
}

void NEGLManager::modelViewMatrix(const matrix44f &nmvMatrix)
{
    P->m_ModelViewMatrix = nmvMatrix;
}

void NEGLManager::worldMatrix(const matrix44f &wMatrix)
{
    P->m_WorldMatrix = wMatrix;
}

void NEGLManager::viewMatrix(const matrix44f &vMatrix)
{
    P->m_ViewMatrix = vMatrix;
}

QString NEGLManager::getContent(QString filename)
{
    QFile shaderFile(filename);
    shaderFile.open(QIODevice::ReadOnly);
    QString shaderCode(shaderFile.readAll());

    return shaderCode;
}

QString NEGLManager::manualInclude(QString filename, QString prepend)
{
    if (filename == ""){
        return "";
    }

    std::string source = getContent(filename).toStdString();

    std::stringstream stream;
    stream << source;
    std::string line, text;

    // Handle command line defines
    text += prepend.toStdString();
    int lineCount  = 0;
    while(std::getline(stream, line))
    {
        std::size_t Offset = 0;
        lineCount++;

        // Version
        Offset = line.find("#version");
        if(Offset != std::string::npos)
        {
            std::size_t CommentOffset = line.find("//");
            if(CommentOffset != std::string::npos && CommentOffset < Offset)
                continue;

            // Reorder so that the #version line is always the first of a shader text
            text = line + std::string("\n") + text + std::string("//") + line + std::string("\n");
            continue;
        }

        text += line + "\n";
    }

    return QString(text.c_str());
}

void NEGLManager::recreateShader(ShaderMetaInfo* shaderInfo)
{
    QOpenGLShaderProgram* shader = shaderInfo->mShader;
    if(shader != NULL)
        delete shader;
    shader = new QOpenGLShaderProgram();
    if(!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, shaderInfo->mVertexPath))
    {
        qWarning() << shaderInfo->mVertexPath <<  " " << shader->log();
        std::cerr << shaderInfo->mVertexPath.toStdString() << " " << shader->log().toStdString() << std::endl;
    }
    if(shaderInfo->mGeometryPath != "" && !shader->addShaderFromSourceFile(QOpenGLShader::Geometry, shaderInfo->mGeometryPath))
    {
        qWarning() << shaderInfo->mGeometryPath <<  " " << shader->log();
        std::cerr << shaderInfo->mGeometryPath.toStdString() << " " << shader->log().toStdString() << std::endl;
    }
    QString fragSource = manualInclude(shaderInfo->mFragmentPath, shaderInfo->mFragPrepend);
    if(!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource))
    {
        qWarning() <<  shaderInfo->mFragmentPath << " " << shader->log() ;
        std::cerr << shaderInfo->mFragmentPath.toStdString() << " " << shader->log().toStdString() << std::endl;
    }
    for( int i = 0; i < shaderInfo->mSecondaryFragmentList.size(); i++ )
    {
        QString fragmentFile = shaderInfo->mSecondaryFragmentList[i];
        if(!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentFile))
        {
            qWarning() << fragmentFile <<  " " << shader->log();
            std::cerr << fragmentFile.toStdString() << " " << shader->log().toStdString() << std::endl;
        }
    }
    shaderInfo->mShader = shader;
}

void NEGLManager::setOSDFontSize(NEGUIItemSize size)
{
    switch(size)
    {
    case Smallest:
        P->textureFont->FaceSize(10);
        break;
    case Smaller:
        P->textureFont->FaceSize(12);
        break;
    case Small:
        P->textureFont->FaceSize(14);
        break;
    case Normal:
        P->textureFont->FaceSize(16);
        break;
    case Large:
        P->textureFont->FaceSize(18);
        break;
    case Larger:
        P->textureFont->FaceSize(20);
        break;
    case Largest:
        P->textureFont->FaceSize(22);
        break;

    }
}

std::shared_ptr<NEGLRenderableMesh> NEGLManager::getOrCreateRenderableMesh(NEIndexedTriMesh *mesh)
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr = m_RenderableMeshes.find( mesh );
    if( itr == m_RenderableMeshes.end())
    {
        m_RenderableMeshes[ mesh ] = std::shared_ptr< NEGLRenderableMesh >( new NEGLRenderableMesh );
    }
    std::shared_ptr< NEGLRenderableMesh > glRenderable = m_RenderableMeshes[ mesh ];
    return glRenderable;
}

bool NEGLManager::isBuffered(NEIndexedTriMesh *mesh)
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if (glRenderable)
    {
        return glRenderable->isBuffered();
    }

    return false;
}

NEMaterial *NEGLManager::material(NEIndexedTriMesh *mesh)
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        return glRenderable->material();
    }
    return NULL;
}

void NEGLManager::setMaterial(NEIndexedTriMesh *mesh, NEMaterial* material )
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->setMaterial( material );
    }
}

void NEGLManager::createDrawingBuffers( NEIndexedTriMesh *mesh, uint mode )
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->createDrawingBuffers( mesh, mode );
    }
}

void NEGLManager::clearDrawingBuffers( NEIndexedTriMesh *mesh )
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->clearDrawingBuffers( mesh );
    }
}

void NEGLManager::rehintDrawingBuffers( NEIndexedTriMesh *mesh, uint mode )
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->rehintDrawingBuffers( mesh, mode );
    }
}

void NEGLManager::updateDrawingBuffers( NEIndexedTriMesh *mesh, uint mode )
{
    if( NEBASE.guiMode() == false )
        return;

    clearDrawingBuffers(mesh);

    if(mesh->numTriangles() > 0)
        createDrawingBuffers(mesh, mode);
}

void NEGLManager::calcNormalsMWA(NEIndexedTriMesh *mesh)
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->calcNormalsMWA( mesh );
    }
}

void NEGLManager::calcNormalsMWASEL(NEIndexedTriMesh *mesh)
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->calcNormalsMWASEL( mesh );
    }
}

void NEGLManager::calcNormalsMWE(NEIndexedTriMesh *mesh)
{
    std::shared_ptr< NEGLRenderableMesh > glRenderable = getOrCreateRenderableMesh(mesh);
    if( glRenderable )
    {
        glRenderable->calcNormalsMWE( mesh );
    }
}

NEGLRenderer *NEGLManager::getGLRenderer()
{
    return P->m_pGLRenderer;
}

NEIndexBuffer *NEGLManager::indexBuffer(const NEIndexedTriMesh *mesh)
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr =
            m_RenderableMeshes.find( const_cast< NEIndexedTriMesh *>( mesh ) );
    if( itr != m_RenderableMeshes.end() )
    {
        return (*itr)->indexBuffer();
    }

    return NULL;
}

NEIndexBuffer* NEGLManager::indexBufferWithAdjacency( const NEIndexedTriMesh* mesh )
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr =
            m_RenderableMeshes.find( const_cast< NEIndexedTriMesh *>( mesh ) );
    if( itr != m_RenderableMeshes.end() )
    {
        return (*itr)->indexBufferWithAdjacency();
    }

    return NULL;
}

NEVertexBuffer *NEGLManager::unIndexedVBuffer( const NEIndexedTriMesh* mesh)
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr =
            m_RenderableMeshes.find( const_cast< NEIndexedTriMesh *>( mesh ) );
    if( itr != m_RenderableMeshes.end() )
    {
        return (*itr)->unIndexedVBuffer();
    }

    return NULL;
}

NEVertexBuffer *NEGLManager::boundingBoxBuffer( const NEIndexedTriMesh* mesh)
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr =
            m_RenderableMeshes.find( const_cast< NEIndexedTriMesh *>( mesh ) );
    if( itr != m_RenderableMeshes.end() )
    {
        return (*itr)->boundingBoxBuffer();
    }

    return NULL;
}

NEVertexBuffer *NEGLManager::unIndexedBoundingBoxBuffer( const NEIndexedTriMesh* mesh)
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr =
            m_RenderableMeshes.find( const_cast< NEIndexedTriMesh *>( mesh ) );
    if( itr != m_RenderableMeshes.end() )
    {
        return (*itr)->unIndexedBoundingBoxBuffer();
    }

    return NULL;
}

NEVertexBuffer *NEGLManager::vertexBuffer(const NEIndexedTriMesh *mesh)
{
    QMap< NEIndexedTriMesh *, std::shared_ptr< NEGLRenderableMesh > >::const_iterator itr =
            m_RenderableMeshes.find( const_cast< NEIndexedTriMesh *>( mesh ) );
    if( itr != m_RenderableMeshes.end() )
    {
        return (*itr)->vertexBuffer();
    }

    return NULL;
}

ShaderMetaInfo *NEGLManager::getPositionShader()
{
    return P->m_PositionShader;
}

ShaderMetaInfo *NEGLManager::getLineStippleShader()
{
    return P->m_LineStippleShader;
}

ShaderMetaInfo *NEGLManager::getRectYFilterShader()
{
    return P->m_RectYFilterShader;
}

ShaderMetaInfo *NEGLManager::getColorRampOSDShader()
{
    return P->m_ColorRampOSDShader;
}


ShaderMetaInfo *NEGLManager::getFloorDrawingShader()
{
    return P->m_FloorDrawingShader;
}

ShaderMetaInfo *NEGLManager::getSolidWireframeShader()
{
    return P->m_SolidWireframeShader;
}

ShaderMetaInfo *NEGLManager::getQuadWireframeShader()
{
    return P->m_QuadWireframeShader;
}

ShaderMetaInfo* NEGLManager::getCPShader()
{
    return P->m_CPShader;
}

ShaderMetaInfo* NEGLManager::getCPWaterShader()
{
    return P->m_CPWaterShader;
}

ShaderMetaInfo* NEGLManager::getAOGeomShader()
{
    return P->m_AOGeomShader;
}

ShaderMetaInfo* NEGLManager::getAODepthLinearizeShader()
{
    return P->m_AODepthLinearizeShader;
}

ShaderMetaInfo* NEGLManager::getAODepthLinearizeMSAAShader()
{
    return P->m_AODepthLinearizeMSAAShader;
}

ShaderMetaInfo* NEGLManager::getAOCalcShader()
{
    return P->m_AOCalcShader;
}

ShaderMetaInfo* NEGLManager::getHBAOBlurShader()
{
    return P->m_HBAOBlurShader;
}

ShaderMetaInfo* NEGLManager::getHBAOBlur2Shader()
{
    return P->m_HBAOBlur2Shader;
}

ShaderMetaInfo* NEGLManager::getBgShader()
{
    return P->m_bgShader;
}

ShaderMetaInfo* NEGLManager::getPedestalShader()
{
    return P->m_pedestalShader;
}

ShaderMetaInfo* NEGLManager::getFluidPointThicknessShader()
{
    return P->m_FluidProgram.mPointThicknessProgram;
}

ShaderMetaInfo* NEGLManager::getFluidEllipsoidDepthProgram()
{
    return P->m_FluidProgram.mEllipsoidDepthProgram;
}

ShaderMetaInfo* NEGLManager::getFluidDepthBlurProgram()
{
    return P->m_FluidProgram.mDepthBlurProgram;
}

ShaderMetaInfo* NEGLManager::getFluidDiffuseProgram()
{
    return P->m_FluidProgram.mDiffuseProgram;
}

ShaderMetaInfo* NEGLManager::getSkyboxShader()
{
    return P->m_skyboxShader;
}

ShaderMetaInfo* NEGLManager::getSkySphereShader()
{
    return P->m_skySphereShader;
}

ShaderMetaInfo* NEGLManager::hairShader()
{
    return P->m_HairShader;
}

ShaderMetaInfo* NEGLManager::edgeShader()
{
    return P->m_EdgeShader;
}

ShaderMetaInfo* NEGLManager::smaaEdgeShader()
{
    return P->m_smaaEdgeShader;
}

ShaderMetaInfo* NEGLManager::smaaBlendShader()
{
    return P->m_smaaBlendShader;
}

ShaderMetaInfo* NEGLManager::smaaNeighborShader()
{
    return P->m_smaaNeighborShader;
}

uint NEGLManager::OpenGLVersion()
{
	return P->m_OpenGLVersion;
}

QOpenGLShaderProgram*ShaderMetaInfo::shaderProgram() const
{
    return mShader;
}

ShaderMetaInfo::ShaderMetaInfo(): mShader(NULL), mVertexPath(""), mGeometryPath(""), mFragmentPath(""), mUsesLights(false)
{
}

ShaderMetaInfo::ShaderMetaInfo(QOpenGLShaderProgram* shader, QString vertexPath, QString fragmentPath, bool usesLights)
    : mShader(shader), mVertexPath(vertexPath), mFragmentPath(fragmentPath), mUsesLights(usesLights)
{
}

ShaderMetaInfo::ShaderMetaInfo(QOpenGLShaderProgram* shader, QString vertexPath, QString geometryPath, QString fragmentPath, bool usesLights)
    : mShader(shader), mVertexPath(vertexPath), mGeometryPath(geometryPath), mFragmentPath(fragmentPath), mUsesLights(usesLights)
{
}

ShaderMetaInfo::~ShaderMetaInfo()
{
    delete mShader;
}

void ShaderMetaInfo::addFragPrepend(QString prepend)
{
    mFragPrepend = prepend;
}

void ShaderMetaInfo::addSecondaryFragShader(QString fragmentPath)
{
    mSecondaryFragmentList << fragmentPath;
}
