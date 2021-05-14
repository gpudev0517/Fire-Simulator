using namespace std;

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Utility/NEMatrixStack4x4.h"

#include "Rendering/NEGLRenderer.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "Rendering/Resource/NEVertexBuffer.h"
#include "Rendering/Resource/NEIndexBuffer.h"
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
#include "Rendering/Spatial/NEMeasurementItem.h"

#include "AreaTex.h"
#include "SearchTex.h"

#include <QPainter>
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QGraphicsSvgItem>
#include "Mesh/NESpatialMesh.h"

#define glVerify(x) x

IGLSurface::IGLSurface(bool boffline)
{
    m_offlineContext = 0;
    m_offlineSurface = 0;
    m_bOffline = boffline;
    m_ShowGizmo = false;

    if(boffline)
    {
        m_offlineSurface = new QOffscreenSurface();
        m_offlineSurface->create();

        m_offlineContext = new QOpenGLContext();
        m_offlineContext->setShareContext(QOpenGLContext::globalShareContext());
        m_offlineContext->create();

        m_offlineContext->makeCurrent(m_offlineSurface);
    }

    if(!NEBASE.guiMode()){
        if(!GL){
            GL = m_offlineContext->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
            GL41 = m_offlineContext->versionFunctions<QOpenGLFunctions_4_1_Compatibility>();
            GL43 = m_offlineContext->versionFunctions<QOpenGLFunctions_4_3_Compatibility>();

            if(m_offlineContext->hasExtension("GL_EXT_direct_state_access")){
                GLEXTDSA = new QOpenGLExtension_EXT_direct_state_access();
                bool bSucess = GLEXTDSA->initializeOpenGLFunctions();
                if(!bSucess)
                {
                    std::cout << "Current Context is not supported Extension : GL_EXT_direct_state_access" << std::endl;
                }

            }
            if(m_offlineContext->hasExtension("GL_NV_texture_multisample")){
                GLEXTNTM = new QOpenGLExtension_NV_texture_multisample();
                bool bSucess = GLEXTNTM->initializeOpenGLFunctions();
                if(!bSucess)
                {
                    std::cout << "Current Context is not supported Extension : GL_NV_texture_multisample" << std::endl;
                }

            }
        }

        printf("OpenGL: %s %s %s\n", GL->glGetString(GL_VENDOR), GL->glGetString(GL_RENDERER), GL->glGetString(GL_VERSION));
    }

    m_pShaderProgram = 0;

    m_bufferCreated = false;
    m_bufferRendered = false;

    m_msaaSamples = 1;
    m_csaaColorSamples = 4;
    m_csaaCoverageSamples = 8;
    m_bfaaMultiplier = 1;

    m_hbaoAlgorithm = ALGORITHM_HBAO_CLASSIC;
    m_hbaoIntensity = 1.5f;
    m_hbaoBias = 0.1f;
    m_hbaoRadius = 2.f;
    m_hbaoBlur = true;
    m_hbaoBlurSharpness = 1.0f;

    m_skyboxVAO = 0;
    m_skyboxVBO = 0;

    floorVerts = 0;

    // Create matrix stacks for relevant transformation types
    m_pWorldStack = new NEMatrixStack4x4();
    m_pProjectionStack = new NEMatrixStack4x4();
    m_pViewStack = new NEMatrixStack4x4();

    m_pCamera = NULL;

    m_BackgroundTexture = 0;
    m_SkyBoxTexture = 0;
    m_EnvironmentMap = 0;

    m_backgroundGradient = false;
    m_backgroundType     = Color;
    m_backgroundGradColor.setRgb( 0, 0, 0, 255 );
    m_backgroundRealColor.setRgb(252, 249, 247, 255);
    m_environmentType = EnvironmentType::NoEnvironment;
    m_environmentRender = true;
    m_envRotation = 0.0;
    m_transparencyType = GlobalTransparency::NoTransparency;
    m_QualityProfile = QualityProfile::HighQuality;
    m_DynamicQuality = false;
    m_depthPeelingIterations = 6;
    m_DrawGrid = true;
    m_OverlayGrid = false;
    m_AxisThickness = 2.5;
    m_GridThickness  = 2.0;
    m_SubGridThickness = 1.0;
    m_GridColor = QColor(255.0, 255.0, 255.0, 80.0);
    m_SubGridColor = QColor(255.0, 255.0, 255.0, 50.0);
    m_ssao = false;
    m_hbaoType = HbaoType::Hbao;
    m_intensity = 1.5;
    m_radius = 2.0;
    m_blurSharpness = 1.0;
    m_aaType = AntialiasingType::None;
    m_ShowOsd = true;
    m_bShowFPS = true;
    m_GridSize = QSizeF(50.0, 50.0);
    m_CellCount = QSize(10, 10);
    m_addGridLevel = true;
    m_CellDivision = QSize(5,5);
    m_gridMeasurements = false;
    m_SnapUnit = vec3f(1.0, 1.0, 1.0);

    m_ViewAxes = true;
    //m_BackgroundColor.setRgb( 136, 157, 178, 255 );
    m_BackgroundColor.setRgb( 80, 80, 80, 255 );
    m_DrawRenderArea = false;
    m_GlSize = QSize(800, 600);

    m_width = m_lastWidth = m_glWidth = 0;
    m_height = m_lastHeight = m_glHeight = 0;

    m_TransparencyStage = NoTransparencing;
    m_measureStep = IGLSurface::MeasureStep::StandBy;
    m_measureCnt = 0;

    computeCellSize();
}

IGLSurface::~IGLSurface()
{
    //QOpenGLContext * ctx = QOpenGLContext::currentContext();
    releaseBuffer();

    if(floorVerts)
    {
        NERENDERMAN.GLManager()->destroy(floorVerts);
        floorVerts = 0;
    }

    freeTexture( m_BackgroundTexture );
    freeTexture( m_SkyBoxTexture );
    freeTexture ( m_EnvironmentMap );
    delete m_pWorldStack;
    delete m_pProjectionStack;
    delete m_pViewStack;

    if(m_offlineContext){
        m_offlineSurface->destroy();
        delete m_offlineSurface;
        delete m_offlineContext;
    }
    //ctx->doneCurrent();
}

void IGLSurface::initializeGLSurface()
{
    buildFloor(vec3f(0.0, 0.0, 0.0), eulerf(0.0, 0.0, 0.0), m_CellCount, m_CellSize, m_CellDivision);
}

void IGLSurface::setGLSurfaceSize(float width, float height, float pixelRatio)
{
    m_width = width;
    m_height = height;

    // Set up the viewport
    GL->glViewport( 0, 0, width, height);

    // Update buffer only when screen size is changed
    if ( m_lastWidth != width || m_lastHeight != height)
    {
        updateBuffer( width*pixelRatio, height*pixelRatio, aaType() );
        m_lastWidth = width;
        m_lastHeight = height;
    }

    // Push the projection matrix from the active camera into the projectionStack
    if( camera() )
        camera()->computeProjection( width, height );
    if( camera() )
        projectionStack().loadMatrix( camera()->projectionMatrix() );

    // Keep an orthogonal projection matrix for OSD drawing
    osdOrthoMatrix.setToIdentity();
    osdOrthoMatrix.ortho(0.0, (double)width, 0, (double)height, -100, 100);

}

void IGLSurface::setShaderProgram(QOpenGLShaderProgram* program)
{
    m_pShaderProgram = program;
    if(m_pShaderProgram)
    {
        bool bound = m_pShaderProgram->bind();
        Q_ASSERT_X(bound, "NEGLManager::setShaderProgram", "Cannot bind shader program");
        _unused(bound);
    }
    else
    {
        GL->glUseProgram(0);
    }
}

QOpenGLShaderProgram* IGLSurface::shaderProgram()
{
    return m_pShaderProgram;
}

void IGLSurface::freeTexture( QOpenGLTexture* texture )
{
    if( texture )
    {
        texture->destroy();
        delete texture;
        texture = NULL;
    }
}

void IGLSurface::updateEnvironmentUniforms()
{
    if (environmentType() == NoEnvironment)
    {
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "environmented"), 0);
        //return;
    }
    else
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "environmented"), 1);
    // bind pre-computed IBL data
    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "irradianceMap"), 3);
    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "prefilterMap"), 4);
    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "brdfLUT"), 5);
    GL->glActiveTexture(GL_TEXTURE3);
    GL->glBindTexture(GL_TEXTURE_CUBE_MAP, m_envIrradianceMap);
    GL->glActiveTexture(GL_TEXTURE4);
    GL->glBindTexture(GL_TEXTURE_CUBE_MAP, m_envPrefilterMap);
    GL->glActiveTexture(GL_TEXTURE5);
    GL->glBindTexture(GL_TEXTURE_2D, m_envBrdfLUTTexture);
}

void IGLSurface::setCamera( QObject* pCamera )
{
    if(m_pCamera)
        m_pCamera->setViewer(false);
    m_pCamera = qobject_cast<NECamera*>(pCamera);
    if (m_pCamera)
    {
        m_pCamera->setViewer(true);
        if (isVisibleSurface())
        {
            m_FovOld = m_pCamera->Fov();
            m_NearClipOld = m_pCamera->NearClip();
            m_FarClipOld = m_pCamera->FarClip();
            m_OrthoWidthOld = m_pCamera->OrthoWidth();
        }
    }
}

void IGLSurface::buildFloor(const vec3f& center, const eulerf& orientation, const QSize& cellCount, const QSizeF& cellSize, const QSize& cellDivision)
{
    if (cellCount.width() <= 0.0 || cellCount.height() <= 0.0 || cellSize.width() <= 0.0 || cellSize.height() <= 0.0)
        return;
    if (cellDivision.width() < 1 || cellDivision.height() < 1)
        return;

    if (floorVerts != 0)
    {
        NERENDERMAN.GLManager()->destroy(floorVerts);
        floorVerts = 0;
    }
    verticesLvl1.clear();
    verticesLvl2.clear();

    matrix44f translateMatrix;
    translateMatrix.setToIdentity();
    translateMatrix.translate(center);

    matrix44f rotationMatrix;
    rotationMatrix.setToIdentity();
    quatf quatX = quatX.fromAxisAndAngle(1.0, 0.0, 0.0, orientation.phi());
    quatf quatY = quatY.fromAxisAndAngle(0.0, 1.0, 0.0, orientation.theta());
    quatf quatZ = quatZ.fromAxisAndAngle(0.0, 0.0, 1.0, orientation.psi());
    quatf orientationQuat = quatZ*quatY*quatX;
    rotationMatrix.rotate(orientationQuat);

    matrix44f transform = translateMatrix*rotationMatrix;

    float gridHalfWidth = 0.5*cellCount.width()*cellSize.width();
    float gridHalfHeight = 0.5*cellCount.height()*cellSize.height();

    QSizeF smallerCellSize = QSizeF(cellSize.width()/cellDivision.width(), cellSize.height()/cellDivision.height());

    QList<QPair<vec3f, vec3f>> posPairsLvl1;
    QList<QPair<vec3f, vec3f>> posPairsLvl2;

    for (int i = 0; i != cellCount.width()+1; i++)
    {
        float xPos = -gridHalfWidth+i*cellSize.width();
        vec3f pos1(transform*vec3f(xPos, 0, -gridHalfHeight));
        vec3f pos2(transform*vec3f(xPos, 0, gridHalfHeight));
        posPairsLvl1.append(QPair<vec3f, vec3f>(pos1, pos2));
        verticesLvl1.append(pos1);
        verticesLvl1.append(pos2);

        if (i == cellCount.width())
            continue;

        for (int j = 1; j < cellDivision.width(); j++)
        {
            float xPosSub = xPos+j*smallerCellSize.width();
            vec3f posSub1(transform*vec3f(xPosSub, 0, -gridHalfHeight));
            vec3f posSub2(transform*vec3f(xPosSub, 0, gridHalfHeight));
            posPairsLvl2.append(QPair<vec3f, vec3f>(posSub1, posSub2));
            verticesLvl2.append(posSub1);
            verticesLvl2.append(posSub2);
        }
    }
    for (int i = 0; i != cellCount.height()+1; i++)
    {
        float zPos = -gridHalfHeight+i*cellSize.height();
        vec3f pos1(transform*vec3f(-gridHalfWidth, 0, zPos));
        vec3f pos2(transform*vec3f(gridHalfWidth, 0, zPos));
        posPairsLvl1.append(QPair<vec3f, vec3f>(pos1, pos2));
        verticesLvl1.append(pos1);
        verticesLvl1.append(pos2);

        if (i == cellCount.height())
            continue;

        for (int j = 1; j < cellDivision.height(); j++)
        {
            float zPosSub = zPos+j*smallerCellSize.height();
            vec3f posSub1(transform*vec3f(-gridHalfWidth, 0, zPosSub));
            vec3f posSub2(transform*vec3f(gridHalfWidth, 0, zPosSub));
            posPairsLvl2.append(QPair<vec3f, vec3f>(posSub1, posSub2));
            verticesLvl2.append(posSub1);
            verticesLvl2.append(posSub2);
        }
    }

    floorVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4+(posPairsLvl1.size()+posPairsLvl2.size())*2, 0, GL_STATIC_DRAW);
    NEPVertex* vertexPtr = (NEPVertex*) floorVerts->beginLoadData();

    int currentVertex = 0;

    vertexPtr[currentVertex].position = transform*vec3f(-gridHalfWidth, 0.0, 0.0);
    currentVertex++;
    vertexPtr[currentVertex].position = transform*vec3f(gridHalfWidth, 0.0, 0.0);
    currentVertex++;
    vertexPtr[currentVertex].position = transform*vec3f(0.0, 0.0, -gridHalfHeight);
    currentVertex++;
    vertexPtr[currentVertex].position = transform*vec3f(0.0, 0.0, gridHalfHeight);
    currentVertex++;

    for (const QPair<vec3f, vec3f>& posPair : posPairsLvl1)
    {
        vertexPtr[currentVertex].position = posPair.first;
        currentVertex++;
        vertexPtr[currentVertex].position = posPair.second;
        currentVertex++;
    }
    for (const QPair<vec3f, vec3f>& posPair : posPairsLvl2)
    {
        vertexPtr[currentVertex].position = posPair.first;
        currentVertex++;
        vertexPtr[currentVertex].position = posPair.second;
        currentVertex++;
    }

    floorVerts->endLoadData();
}

void IGLSurface::newBuffer(GLuint &glid)
{
    GL->glGenBuffers(1,&glid);
}

void IGLSurface::deleteBuffer(GLuint &glid)
{
    if (glid) GL->glDeleteBuffers(1,&glid);
    glid = 0;
}

void IGLSurface::newTexture(GLuint &glid)
{
    GL->glGenTextures(1,&glid);
}

void IGLSurface::deleteTexture( GLuint &glid )
{
    if (glid) GL->glDeleteTextures(1,&glid);
    glid = 0;
}

void IGLSurface::newFramebuffer(GLuint &glid)
{
    GL->glGenFramebuffers(1,&glid);
}

void IGLSurface::deleteFramebuffer(GLuint &glid)
{
    if (glid) GL->glDeleteFramebuffers(1,&glid);
    glid = 0;
}

void IGLSurface::newRenderbuffer(GLuint &glid)
{
    GL->glGenRenderbuffers(1,&glid);
}

void IGLSurface::deleteRenderbuffer(GLuint &glid)
{
    if (glid) GL->glDeleteRenderbuffers(1,&glid);
    glid = 0;
}

bool IGLSurface::createBuffer()
{
    // Create color and depthstensil buffer for main fbo
    newTexture(m_sceneColorTexture);
    newTexture(m_sceneDepthStencilTexture);
    newFramebuffer(m_sceneFBO);

    // Create final frame buffer and pick buffer
    newTexture(m_frameAndPickColorTexture);
    newTexture(m_frameAndPickDepthStencilTexture);
    newFramebuffer(m_frameAndPickFBO);

    newTexture(m_offlineFinalColorTexture);
    newTexture(m_offlineFinalDepthStencilTexture);
    newFramebuffer(m_offlineFinalFBO);

    //! Register filters
    //mFilterArray.push_back(new NEFilterFXAA());

    //Create OIT background buffer
    newTexture(m_OITBackgroundTexture);
    newFramebuffer(m_OITBackgroundFBO);
    newTexture(m_OITBlenderColorTexture);
    newFramebuffer(m_OITBlenderFBO);

    //Create WeightBlendOIT buffer
    newTexture(m_weightColorAccumulationTexture);
    newTexture(m_weightAlphaAccumulationTexture);

    //Create Depth Peeling OIT Buffer
    for(int i = 0; i < 2 ; i ++)
    {
        newTexture(m_depthPeelingColorTextures[i]);
        newTexture(m_depthPeelingDepthTextures[i]);
        newFramebuffer(m_depthPeelingFBOs[i]);
    }

    //Create Linked List OIT Buffers.
    newBuffer(m_LinkedListCounterBuffer);
    newBuffer(m_LinkedListLinkedListBuffer);
    newTexture(m_LinkedListHeadPointerTexture);
    newBuffer(m_LinkedListHeadPtrClearBuffer);

    /// HBAO
    newTexture(m_hbaoTextures.scene_depthlinear);
    newFramebuffer(m_hbaoFbos.depthlinear);
    newTexture(m_hbaoTextures.hbao_result);
    newTexture(m_hbaoTextures.hbao_blur);
    newFramebuffer(m_hbaoFbos.hbao_calc);

    {
        float numDir = 8; // keep in sync to glsl
        float nv_pi = 3.14159265358979323846264338327950288419716939937510582;

        for(int i=0; i<HBAO_RANDOM_ELEMENTS*HBAO_MAX_SAMPLES; i++)
        {
            float Rand1 = rand() * 1.0f / RAND_MAX;
            float Rand2 = rand() * 1.0f / RAND_MAX;

            // Use random rotation angles in [0,2PI/NUM_DIRECTIONS)
            float Angle = 2.f * nv_pi * Rand1 / numDir;
            m_hbaoRandom[i] = vec4f(cosf(Angle), sinf(Angle), Rand2, 0);
        }

        newTexture(m_hbaoTextures.hbao_random);

        for (int i = 0; i < HBAO_MAX_SAMPLES; i++)
        {
            newTexture(m_hbaoTextures.hbao_randomview[i]);
        }
    }

    newBuffer(m_hbaoBuffers.hbao_ubo);

    /// HBAO+
    newTexture(m_AOResultTexture);
    newFramebuffer(m_AOResultFBO);

    // Water shading
    FluidRenderer *renderer = &m_fluidRenderer;
    newTexture(renderer->mDepthTex);
    newTexture(renderer->mColorTex);
    newTexture(renderer->mDepthBuf);
    //newTexture(renderer->mDepthSmoothTex);
    renderer->mDepthSmoothTex = m_hbaoTextures.scene_depthlinear;
//    newTexture(renderer->mSceneTex);
//    newFramebuffer(renderer->mSceneFbo);
    renderer->mSceneTex = m_AOResultTexture;
    renderer->mSceneFbo = m_AOResultFBO;

    newFramebuffer(renderer->mDepthFbo);
    newTexture(renderer->mThicknessTex);
    newFramebuffer(renderer->mThicknessFbo);

    // SMAA (Subpixel Morphological Antialiasing)
    newFramebuffer(m_smaaEdgeFBO);
    newTexture(m_smaaEdgeTexture);
    newFramebuffer(m_smaaBlendFBO);
    newTexture(m_smaaBlendTexture);
    newTexture(m_smaaDepthStencilTexture);
    newTexture(m_smaaAreaTexture);
    newTexture(m_smaaSearchTexture);

    // Environment
    newTexture(m_envCubemap);
    newFramebuffer(m_envCaptureFBO);
    newTexture(m_envIrradianceMap);
    newTexture(m_envPrefilterMap);
    newTexture(m_envBrdfLUTTexture);

    // Backup buffer for OIT + AO + Water
    newTexture(m_tmpColorTexture);
    newTexture(m_tmpDepthStencilTexture);
    newFramebuffer(m_tmpFBO);

    newTexture(m_PCMapColorTexture);
    newTexture(m_PCMapFlatWorldTexture);
    newTexture(m_PCMapDepthTexture);
    newFramebuffer(m_PCMapFBO);

    return true;
}

void IGLSurface::updateBuffer(int width, int height, int aaType)
{
    releaseBuffer();
    createBuffer();
    FluidRenderer* renderer = &m_fluidRenderer;

    // Final Frame and Pick Buffer
    GL->glBindTexture (GL_TEXTURE_2D, m_frameAndPickColorTexture);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindTexture (GL_TEXTURE_2D, m_frameAndPickDepthStencilTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_frameAndPickFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_frameAndPickColorTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_frameAndPickDepthStencilTexture, 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(m_bOffline)
    {
        // Final Frame and Pick Buffer
        GL->glBindTexture (GL_TEXTURE_2D, m_offlineFinalColorTexture);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        if( GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_offlineFinalDepthStencilTexture);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_offlineFinalFBO);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_offlineFinalColorTexture, 0);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_offlineFinalDepthStencilTexture, 0);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    // SMAA (Subpixel Morphological Antialiasing)
    GL->glBindTexture (GL_TEXTURE_2D, m_smaaEdgeTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindTexture (GL_TEXTURE_2D, m_smaaBlendTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindTexture (GL_TEXTURE_2D, m_smaaAreaTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, AREATEX_WIDTH, AREATEX_HEIGHT);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
    GL->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, AREATEX_WIDTH, AREATEX_HEIGHT, GL_RG, GL_UNSIGNED_BYTE, areaTexBytes);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindTexture (GL_TEXTURE_2D, m_smaaDepthStencilTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindTexture (GL_TEXTURE_2D, m_smaaSearchTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    GL->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, searchTexBytes);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_smaaEdgeFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_smaaEdgeTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_smaaDepthStencilTexture, 0); /// ? same with below fbo?
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_smaaBlendFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_smaaBlendTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_smaaDepthStencilTexture, 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Environment Map

    // setup cubemap to render to and attach to framebuffer
    GL->glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: create an irradiance cubemap
    // ---------------------------------
    GL->glBindTexture(GL_TEXTURE_CUBE_MAP, m_envIrradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: create an prefilter map
    // ----------------------------
    GL->glBindTexture(GL_TEXTURE_CUBE_MAP, m_envPrefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GL->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    // pre-allocate enough memory for the LUT texture.
    GL->glBindTexture(GL_TEXTURE_2D, m_envBrdfLUTTexture);
    GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, 512, 512);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Create color and depthstensil buffer for main fbo
    if (aaType == Msaa && m_msaaSamples > 1){
        int samples = m_msaaSamples;
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_sceneColorTexture);
        GL43->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_weightColorAccumulationTexture);
        GL43->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_weightAlphaAccumulationTexture);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GL43->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_R8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_sceneDepthStencilTexture);
        GL43->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH24_STENCIL8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_tmpColorTexture);
        GL43->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_tmpDepthStencilTexture);
        GL43->glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH24_STENCIL8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);
    }
    else if(aaType == Csaa) {
        int colorSamples = m_csaaColorSamples;
        int coverageSamples = m_csaaCoverageSamples;
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_sceneColorTexture);
        GLEXTNTM->glTexImage2DMultisampleCoverageNV(GL_TEXTURE_2D_MULTISAMPLE, coverageSamples, colorSamples, GL_RGBA8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_sceneDepthStencilTexture);
        GLEXTNTM->glTexImage2DMultisampleCoverageNV(GL_TEXTURE_2D_MULTISAMPLE, coverageSamples, colorSamples, GL_DEPTH24_STENCIL8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_tmpColorTexture);
        GLEXTNTM->glTexImage2DMultisampleCoverageNV(GL_TEXTURE_2D_MULTISAMPLE, coverageSamples, colorSamples, GL_RGBA8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_tmpDepthStencilTexture);
        GLEXTNTM->glTexImage2DMultisampleCoverageNV(GL_TEXTURE_2D_MULTISAMPLE, coverageSamples, colorSamples, GL_DEPTH24_STENCIL8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_weightColorAccumulationTexture);
        GLEXTNTM->glTexImage2DMultisampleCoverageNV(GL_TEXTURE_2D_MULTISAMPLE, coverageSamples, colorSamples, GL_RGBA16F, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);

        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, m_weightAlphaAccumulationTexture);
        GLEXTNTM->glTexImage2DMultisampleCoverageNV(GL_TEXTURE_2D_MULTISAMPLE, coverageSamples, colorSamples, GL_R8, width, height, GL_FALSE);
        GL->glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, 0);
    }
    else
    {
        width = width * m_bfaaMultiplier;
        height = height * m_bfaaMultiplier;

        GL->glBindTexture (GL_TEXTURE_2D, m_sceneColorTexture);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_weightColorAccumulationTexture);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_weightAlphaAccumulationTexture);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_sceneDepthStencilTexture);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_tmpColorTexture);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_tmpDepthStencilTexture);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        GL->glBindTexture (GL_TEXTURE_2D, 0);
    }

    GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_sceneFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_sceneColorTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_weightColorAccumulationTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_weightAlphaAccumulationTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_sceneDepthStencilTexture, 0);
    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    GL->glDrawBuffers(3, drawBuffers);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Backup buffer for OIT + AO + Water
    GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_tmpFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tmpColorTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_tmpDepthStencilTexture, 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //PCMap Buffer
    GL->glBindTexture(GL_TEXTURE_RECTANGLE, m_PCMapColorTexture);
    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    GL->glBindTexture (GL_TEXTURE_RECTANGLE, 0);


    GL->glBindTexture(GL_TEXTURE_RECTANGLE, m_PCMapFlatWorldTexture);
    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    GL->glBindTexture (GL_TEXTURE_RECTANGLE, 0);


    GL->glBindTexture (GL_TEXTURE_RECTANGLE, m_PCMapDepthTexture);
    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL->glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_DEPTH24_STENCIL8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    GL->glBindTexture (GL_TEXTURE_RECTANGLE, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_PCMapFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_PCMapColorTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_PCMapFlatWorldTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_PCMapDepthTexture, 0);
    const GLenum pcmBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    GL->glDrawBuffers(2, pcmBuffers);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // OIT Background
    GL->glBindTexture(GL_TEXTURE_2D, m_OITBackgroundTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GL->glBindTexture (GL_TEXTURE_2D, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_OITBackgroundFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_OITBackgroundTexture, 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Depth Peeling OIT
    for(int i = 0; i < 2; i++){
        GL->glBindTexture(GL_TEXTURE_RECTANGLE, m_depthPeelingColorTextures[i]);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

        GL->glBindTexture(GL_TEXTURE_RECTANGLE, m_depthPeelingDepthTextures[i]);
        if(GL43)
            GL43->glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_DEPTH24_STENCIL8, width, height);
        else
            GL->glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        GL->glBindTexture(GL_TEXTURE_RECTANGLE, 0);

        GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_depthPeelingFBOs[i]);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_depthPeelingColorTextures[i], 0);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthPeelingDepthTextures[i], 0);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    //Weighted Bleend and Depth Peeling OIT
    GL->glBindTexture(GL_TEXTURE_2D, m_OITBlenderColorTexture);
    if(GL43)
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
    else
        GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    GL->glBindTexture(GL_TEXTURE_2D, 0);

    GL->glBindFramebuffer(GL_FRAMEBUFFER,     m_OITBlenderFBO);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_OITBlenderColorTexture, 0);
    GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthPeelingDepthTextures[0], 0);
    GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(GL43){
        //Linked List OIT
        m_LinkedListMaxNodes = 20 * width * height;
        GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node

        GL->glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_LinkedListCounterBuffer);
        GL->glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
        GL->glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_LinkedListCounterBuffer);

        GL->glBindTexture(GL_TEXTURE_2D, m_LinkedListHeadPointerTexture);

        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height);
        GL->glBindTexture(GL_TEXTURE_2D, 0);
        GL43->glBindImageTexture(0, m_LinkedListHeadPointerTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

        GL->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LinkedListLinkedListBuffer);
        GL->glBufferData(GL_SHADER_STORAGE_BUFFER, m_LinkedListMaxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);
        GL->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_LinkedListLinkedListBuffer);

        std::vector<GLuint> headPtrClearBuf(width*height, 0xffffffff);
        GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_LinkedListHeadPtrClearBuffer);
        GL->glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height* sizeof(GLuint), &headPtrClearBuf[0], GL_STATIC_COPY);
        GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


        /// HBAO
        GL->glBindTexture (GL_TEXTURE_2D, m_hbaoTextures.scene_depthlinear);
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_hbaoFbos.depthlinear);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  m_hbaoTextures.scene_depthlinear, 0);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    #if USE_AO_SPECIALBLUR
        GLenum formatAO = GL_RG16F;
        GLint swizzle[4] = {GL_RED,GL_GREEN,GL_ZERO,GL_ZERO};
    #else
        GLenum formatAO = GL_R8;
        GLint swizzle[4] = {GL_RED,GL_RED,GL_RED,GL_RED};
    #endif

        GL->glBindTexture (GL_TEXTURE_2D, m_hbaoTextures.hbao_result);
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, formatAO, width, height);
        GL43->glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindTexture (GL_TEXTURE_2D, m_hbaoTextures.hbao_blur);
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, formatAO, width, height);
        GL43->glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_hbaoFbos.hbao_calc);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hbaoTextures.hbao_result, 0);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_hbaoTextures.hbao_blur, 0);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

        {
            signed short hbaoRandomShort[HBAO_RANDOM_ELEMENTS*HBAO_MAX_SAMPLES*4];

            for(int i=0; i<HBAO_RANDOM_ELEMENTS*HBAO_MAX_SAMPLES; i++)
            {
    #define SCALE ((1<<15))
                hbaoRandomShort[i*4+0] = (signed short)(SCALE*m_hbaoRandom[i].x());
                hbaoRandomShort[i*4+1] = (signed short)(SCALE*m_hbaoRandom[i].y());
                hbaoRandomShort[i*4+2] = (signed short)(SCALE*m_hbaoRandom[i].z());
                hbaoRandomShort[i*4+3] = (signed short)(SCALE*m_hbaoRandom[i].w());
    #undef SCALE
            }

            GL->glBindTexture(GL_TEXTURE_2D_ARRAY, m_hbaoTextures.hbao_random);
            GL43->glTexStorage3D (GL_TEXTURE_2D_ARRAY,1,GL_RGBA16_SNORM,HBAO_RANDOM_SIZE,HBAO_RANDOM_SIZE,HBAO_MAX_SAMPLES);
            GL->glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,0,HBAO_RANDOM_SIZE,HBAO_RANDOM_SIZE,HBAO_MAX_SAMPLES,GL_RGBA,GL_SHORT,hbaoRandomShort);
            GL->glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            GL->glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            GL->glBindTexture(GL_TEXTURE_2D_ARRAY,0);

            for (int i = 0; i < HBAO_MAX_SAMPLES; i++)
            {
                GL43->glTextureView(m_hbaoTextures.hbao_randomview[i], GL_TEXTURE_2D, m_hbaoTextures.hbao_random, GL_RGBA16_SNORM, 0, 1, i, 1);
                GL->glBindTexture(GL_TEXTURE_2D, m_hbaoTextures.hbao_randomview[i]);
                GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                GL->glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        /// HBAO+ and water scene copy
        GL->glBindTexture(GL_TEXTURE_2D, m_AOResultTexture);
        GL43->glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        GL->glBindTexture (GL_TEXTURE_2D, 0);

        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_AOResultFBO);
        GL->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_AOResultTexture, 0);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    /// Water shading
    renderer->mSceneWidth = width;
    renderer->mSceneHeight = height;

    // scene depth texture
    glVerify(GL->glBindTexture(GL_TEXTURE_2D, renderer->mDepthTex));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL));

    glVerify(GL->glBindTexture(GL_TEXTURE_2D, renderer->mColorTex));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

    glVerify(GL->glBindTexture(GL_TEXTURE_2D, renderer->mDepthBuf));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));


//    // smoothed depth texture
//    glVerify(GL->glBindTexture(GL_TEXTURE_2D, renderer->mDepthSmoothTex));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
//    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL));
//
//    // scene copy
//    glVerify(GL->glBindTexture(GL_TEXTURE_2D, renderer->mSceneTex));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
//    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
//    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

//    glVerify(GL->glBindFramebuffer(GL_FRAMEBUFFER, renderer->mSceneFbo));
//    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->mSceneTex, 0));

    // frame buffer
    glVerify(GL->glBindFramebuffer(GL_FRAMEBUFFER, renderer->mDepthFbo));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->mDepthTex, 0));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderer->mColorTex, 0));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderer->mDepthBuf, 0));

    // thickness texture
    const int thicknessWidth = width;
    const int thicknessHeight = height;

    glVerify(GL->glBindTexture(GL_TEXTURE_2D, renderer->mThicknessTex));

    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glVerify(GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

#if USE_HDR_DIFFUSE_BLEND
    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, thicknessWidth, thicknessHeight, 0, GL_RGBA, GL_FLOAT, NULL));
#else
    glVerify(GL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, thicknessWidth, thicknessHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
#endif

    // thickness buffer
    glVerify(GL->glBindFramebuffer(GL_FRAMEBUFFER, renderer->mThicknessFbo));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->mThicknessTex, 0));
    glVerify(GL->glBindFramebuffer(GL_FRAMEBUFFER, 0));


    glVerify(GL->glBindTexture(GL_TEXTURE_2D, 0));

    m_bufferCreated = true;
    m_bufferRendered = false;

}

bool IGLSurface::releaseBuffer()
{
    if (!m_bufferCreated) return false;

    // Release filter
    //#define foreach(type,list,it) for(type::iterator it=(list).begin(),end = (list).end();(it)!=end;++(it))
    //mFilterArray.clear();

    // Main
    deleteTexture(m_sceneColorTexture);
    deleteTexture(m_sceneDepthStencilTexture);
    deleteFramebuffer(m_sceneFBO);
    deleteTexture(m_frameAndPickColorTexture);
    deleteTexture(m_frameAndPickDepthStencilTexture);
    deleteFramebuffer(m_frameAndPickFBO);
    if(m_bOffline){
        deleteTexture(m_offlineFinalColorTexture);
        deleteTexture(m_offlineFinalDepthStencilTexture);
        deleteFramebuffer(m_offlineFinalFBO);
    }

    // Water
    FluidRenderer *renderer = &m_fluidRenderer;
    deleteFramebuffer(renderer->mDepthFbo);
    deleteTexture(renderer->mDepthBuf);
    deleteTexture(renderer->mColorTex);
    deleteTexture(renderer->mDepthTex);

    deleteFramebuffer(renderer->mThicknessFbo);
    deleteTexture(renderer->mThicknessTex);

//    glVerify(GL->glDeleteFramebuffers(1, &renderer->mDepthFbo));
//    glVerify(GL->glDeleteTextures(1, &renderer->mDepthBuf));
//    glVerify(GL->glDeleteTextures(1, &renderer->mColorTex));
//    glVerify(GL->glDeleteTextures(1, &renderer->mDepthTex));
    //glVerify(GL->glDeleteTextures(1, &renderer->mDepthSmoothTex));
    //glVerify(GL->glDeleteTextures(1, &renderer->mSceneTex));
    //glVerify(GL->glDeleteFramebuffers(1, &renderer->mSceneFbo));

    glVerify(GL->glDeleteFramebuffers(1, &renderer->mThicknessFbo));
    glVerify(GL->glDeleteTextures(1, &renderer->mThicknessTex));

    //OIT Background
    deleteTexture(m_OITBackgroundTexture);
    deleteFramebuffer(m_OITBackgroundFBO);

    // Weighted Blended OIT
    deleteTexture(m_weightColorAccumulationTexture);
    deleteTexture(m_weightAlphaAccumulationTexture);


    // Depth Peeling OIT
    for( int i = 0 ;i < 2; i++){
        deleteTexture(m_depthPeelingColorTextures[i]);
        deleteTexture(m_depthPeelingDepthTextures[i]);
        deleteFramebuffer(m_depthPeelingFBOs[i]);
    }
    deleteTexture(m_OITBlenderColorTexture);
    deleteFramebuffer(m_OITBlenderFBO);

    deleteBuffer(m_LinkedListCounterBuffer);
    deleteBuffer(m_LinkedListLinkedListBuffer);
    deleteBuffer(m_LinkedListHeadPtrClearBuffer);
    deleteTexture(m_LinkedListHeadPointerTexture);

    // HBAO
    deleteTexture(m_hbaoTextures.scene_depthlinear);
    deleteFramebuffer(m_hbaoFbos.depthlinear);
    deleteTexture(m_hbaoTextures.hbao_result);
    deleteTexture(m_hbaoTextures.hbao_blur);
    deleteFramebuffer(m_hbaoFbos.hbao_calc);
    deleteTexture(m_hbaoTextures.hbao_random);
    for (int i = 0; i < HBAO_MAX_SAMPLES; i++)
    {
        deleteTexture(m_hbaoTextures.hbao_randomview[i]);
    }
    deleteBuffer(m_hbaoBuffers.hbao_ubo);

    // HBAO+
    deleteTexture(m_AOResultTexture);
    deleteFramebuffer(m_AOResultFBO);

    // SMAA (Subpixel Morphological Antialiasing)
    deleteTexture(m_smaaEdgeTexture);
    deleteTexture(m_smaaBlendTexture);
    deleteTexture(m_smaaDepthStencilTexture);
    deleteFramebuffer(m_smaaEdgeFBO);
    deleteFramebuffer(m_smaaBlendFBO);
    deleteTexture(m_smaaAreaTexture);
    deleteTexture(m_smaaSearchTexture);

    // Environment
    deleteTexture(m_envCubemap);
    deleteFramebuffer(m_envCaptureFBO);
    deleteTexture(m_envIrradianceMap);
    deleteTexture(m_envPrefilterMap);
    deleteTexture(m_envBrdfLUTTexture);

    // Backup buffer for OIT + AO + Water
    deleteTexture(m_tmpColorTexture);
    deleteTexture(m_tmpDepthStencilTexture);
    deleteFramebuffer(m_tmpFBO);

    //PCMap buffer
    deleteTexture(m_PCMapColorTexture);
    deleteTexture(m_PCMapFlatWorldTexture);
    deleteTexture(m_PCMapDepthTexture);
    deleteFramebuffer(m_PCMapFBO);

    m_bufferCreated = false;

    return true;
}

const matrix44f& IGLSurface::worldMatrix()
{
    return m_pWorldStack->top();
}
const matrix44f& IGLSurface::viewMatrix()
{
    return m_pViewStack->top();
}
const matrix44f& IGLSurface::projectionMatrix()
{
    return m_pProjectionStack->top();
}

void IGLSurface::setWorldMatrix()
{
    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadMatrixf(m_pWorldStack->top().data());
    m_MVMatrix = m_pViewStack->top() * m_pWorldStack->top();
    m_MVPMatrix = m_pProjectionStack->top() * m_MVMatrix;
}

void IGLSurface::setViewMatrix()
{
    GL->glMatrixMode(GL_MODELVIEW);
    GL->glLoadMatrixf(m_pViewStack->top().data());
    m_MVMatrix = m_pViewStack->top() * m_pWorldStack->top();
    m_MVPMatrix = m_pProjectionStack->top() * m_MVMatrix;
}

void IGLSurface::setProjectionMatrix()
{
    GL->glMatrixMode(GL_PROJECTION);
    GL->glLoadMatrixf(m_pProjectionStack->top().data());
    m_MVPMatrix = m_pProjectionStack->top() * m_MVMatrix;
}

void IGLSurface::setModelViewMatrix()
{
    GL->glMatrixMode(GL_MODELVIEW);
    m_MVMatrix = m_pViewStack->top() * m_pWorldStack->top();
    GL->glLoadMatrixf(m_MVMatrix.data());
    m_MVPMatrix = m_pProjectionStack->top() * m_MVMatrix;
}

void IGLSurface::setModelViewMatrix( matrix44f& m )
{
    GL->glMatrixMode(GL_MODELVIEW);
    m_MVMatrix = m;
    GL->glLoadMatrixf(m_MVMatrix.data());
    m_MVPMatrix = m_pProjectionStack->top() * m_MVMatrix;
}

void IGLSurface::drawBackgroundGradient()
{
    //
    // Gradient or texture background
    //
    bool   bgGradient = backgroundGradient();
    int bgType = backgroundType();
    if( bgType != 0 /* color */ ||  bgGradient == false  )
        return;

    QColor bgColor = backgroundColor();
    QColor bgGradColor = backgroundGradColor();

    NERENDERMAN.GLManager()->drawQuadBackground( this, bgColor, bgGradColor , false);
}

void IGLSurface::drawBackgroundTexture()
{
    int bgType = backgroundType();
    if( bgType != 1 /*texture */ )
        return;

    NERENDERMAN.GLManager()->drawQuadBackgroundTexture( this, m_BackgroundTexture );
}

void IGLSurface::drawBackgroundReal()
{
    int bgType = backgroundType();
    if(bgType != 2 /* real */)
        return;

    QColor realColor = backgroundRealColor();
    QColor realColor1(realColor.red()/2, realColor.green()/2, realColor.blue()/2);

    NERENDERMAN.GLManager()->drawQuadBackground(this, realColor, realColor1, true);
}

void IGLSurface::createCubemapAndIrradianceMap(QOpenGLTexture* hdrTexture)
{
    if (hdrTexture == NULL)
        return;

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glDisable( GL_DEPTH_TEST );

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    matrix44f captureProjection;
    captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
    matrix44f captureViews[6];
    captureViews[0].lookAt(vec3f(0.0f, 0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f));
    captureViews[1].lookAt(vec3f(0.0f, 0.0f, 0.0f), vec3f(-1.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f));
    captureViews[2].lookAt(vec3f(0.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f));
    captureViews[3].lookAt(vec3f(0.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 0.0f, -1.0f));
    captureViews[4].lookAt(vec3f(0.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f), vec3f(0.0f, -1.0f, 0.0f));
    captureViews[5].lookAt(vec3f(0.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, -1.0f), vec3f(0.0f, -1.0f, 0.0f));

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    NERENDERMAN.GLManager()->bindIblEquirectangularToCubemapShader(this);

    GL->glUniform1i( GL->glGetUniformLocation( shaderProgram()->programId(), "equirectangularMap" ), 0 );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "projection" ), 1, GL_FALSE, captureProjection.data() );
    GL->glActiveTexture(GL_TEXTURE0);
    GL->glBindTexture(GL_TEXTURE_2D, hdrTexture->textureId());

    GLint prevFrameBuffer;
    GL->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFrameBuffer);

    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_envCaptureFBO);
    GL->glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "view" ), 1, GL_FALSE, captureViews[i].data() );
        GL->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);

        GL->glClear(GL_COLOR_BUFFER_BIT);

        NERENDERMAN.GLManager()->drawCube();
    }
    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    NERENDERMAN.GLManager()->bindIblIrradianceShader(this);
    GL->glUniform1i( GL->glGetUniformLocation( shaderProgram()->programId(), "equirectangularMap" ), 0 );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "projection" ), 1, GL_FALSE, captureProjection.data() );
    GL->glActiveTexture(GL_TEXTURE0);
    GL->glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_envCaptureFBO);
    GL->glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "view" ), 1, GL_FALSE, captureViews[i].data() );
        GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envIrradianceMap, 0);
        GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        NERENDERMAN.GLManager()->drawCube();
    }
    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    NERENDERMAN.GLManager()->bindIblPrefilterShader(this);
    GL->glUniform1i( GL->glGetUniformLocation( shaderProgram()->programId(), "equirectangularMap" ), 0 );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "projection" ), 1, GL_FALSE, captureProjection.data() );
    GL->glActiveTexture(GL_TEXTURE0);
    GL->glBindTexture(GL_TEXTURE_2D, hdrTexture->textureId());

    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_envCaptureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        GL->glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        GL->glUniform1f( GL->glGetUniformLocation( shaderProgram()->programId(), "roughness" ), roughness );
        for (unsigned int i = 0; i < 6; ++i)
        {
            GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "view" ), 1, GL_FALSE, captureViews[i].data() );
            GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envPrefilterMap, mip);

            GL->glClear(GL_COLOR_BUFFER_BIT);

            NERENDERMAN.GLManager()->drawCube();
        }
    }
    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    // re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    NERENDERMAN.GLManager()->bindIblBrdfShader(this);

    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_envCaptureFBO);
    GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_envBrdfLUTTexture, 0);

    GL->glViewport(0, 0, 512, 512);
    GL->glClear(GL_COLOR_BUFFER_BIT);
    NERENDERMAN.GLManager()->drawCube();

    GL->glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);

    GL->glPopAttrib();
}

void IGLSurface::createIrradianceMap(QOpenGLTexture* skyboxTexture)
{
    if (!skyboxTexture)
        return;
    //
}

void IGLSurface::drawEnvironmentCube()
{
    int envType = environmentType();

    if( envType != 1 /* cubemap */ )
        return;


    NERENDERMAN.GLManager()->drawSkybox( this, m_SkyBoxTexture );
}

void IGLSurface::bindSceneFrameBuffer()
{
    GL->glBindFramebuffer( GL_FRAMEBUFFER, m_sceneFBO );
}

void IGLSurface::drawEnvironmentSphere()
{
    int envType = environmentType();

    if( envType != 2 /* spheremap */ )
        return;

    bindSceneFrameBuffer();
    NERENDERMAN.GLManager()->drawEnvironmentMap(this, m_envCubemap, envRotation());
}

void IGLSurface::drawAllObjectGizmos()
{
    for( int i = 0; i < m_gizmoList.size(); i++ )
    {
        if (m_gizmoList.at(i).second.first != "")
        {
            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
            setShaderProgram(0);
            GL->glEnable(GL_TEXTURE_2D);
            GL->glEnable(GL_BLEND);
            GL->glEnable(GL_DEPTH_TEST);
            GL->glDepthFunc(GL_ALWAYS);
            GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GL->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            QColor gizmoCol = m_gizmoList.at(i).second.second;
            int red = gizmoCol.red();
            int green = gizmoCol.green();
            int blue = gizmoCol.blue();
            QFile file(m_gizmoList.at(i).second.first);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                     return;
            QByteArray svgData = file.readAll();
            int before = 0;
            int test = svgData.indexOf("rgb", before);
            while (svgData.indexOf("rgb", before) > -1)
            {
                int index = svgData.indexOf("rgb", before);
                while (svgData.at(index + 4) != ')')
                    svgData.remove(index + 4, 1);
                svgData.insert(index + 4, QString::number(gizmoCol.red()) + "," + QString::number(gizmoCol.green()) + "," +QString::number(gizmoCol.blue()));
                before = index + 1;
            }

            QImage image(512, 512, QImage::Format_ARGB32);
            image.fill(0);
            QPainter imgPainter(&image);
            QSvgRenderer *svgrenderer = new QSvgRenderer(svgData);
            svgrenderer->render(&imgPainter);
            imgPainter.end();
            file.close();

            QOpenGLTexture* texture = new QOpenGLTexture(image.mirrored());
            texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            texture->setMagnificationFilter(QOpenGLTexture::Linear);
            GLuint textureID = texture->textureId();
            GL->glBindTexture(GL_TEXTURE_2D, textureID);
            GL->glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            worldStack().pushMatrix(m_gizmoList.at(i).first);
            setModelViewMatrix();

            NEVertexBuffer* quadVerts;
            quadVerts = 0;
            quadVerts = NERENDERMAN.GLManager()->createVertexBuffer(kTPFormat, 4, 0, GL_STATIC_DRAW);
            NETPVertex* vertexPtr = (NETPVertex*) quadVerts->beginLoadData();

            vertexPtr[0].position = vec3f(0, 0, 0.5);
            vertexPtr[0].tCoord = vec2f(0.0f, 0.0f);
            vertexPtr[1].position = vec3f(0, 0.5, 0.5);
            vertexPtr[1].tCoord = vec2f(0.0f, 1.0f);
            vertexPtr[2].position = vec3f(0.5, 0.5, 0.5);
            vertexPtr[2].tCoord = vec2f(1.0f, 1.0f);
            vertexPtr[3].position = vec3f(0.5, 0, 0.5);
            vertexPtr[3].tCoord = vec2f(1.0f, 0.0f);

            quadVerts->endLoadData();
            NERENDERMAN.GLManager()->draw(kQuadListPrim, quadVerts);

            GL->glDisable(GL_BLEND);
            worldStack().popMatrix();
            setModelViewMatrix();
            GL->glPopAttrib();
            delete(svgrenderer);
            delete(texture);
        }
    }
}

void IGLSurface::drawAllCFASTPlanes()
{
//    NERENDERMAN.GLManager()->setDepthTest(kDisableDepthTest);
//    NESpatialGroup* CFAST = qobject_cast<NESpatialGroup *>(NESCENE.findChildNode("CFAST"));
//    if (CFAST == NULL) return;
//    if(CFAST->m_loadCompartment)
//    {
//        QString* path = CFAST->m_compartmentPath;
//        QFile file(*path);
//        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//            return;

//        QTextStream in(&file);
//        QString line = in.readLine();
//        while (!line.isNull()) {
//            line = in.readLine();
//            QStringList lineList = line.split(",");
//        }

//    }
//    for(int i = 0; i < m_CFASTList.size(); i++)
//    {
//        matrix44f world = m_CFASTList.at(i).first;
//        worldStack().pushMatrix(world);
//        setModelViewMatrix();
//        vec3f minPos = m_CFASTList.at(i).second.first;
//        vec3f maxPos = m_CFASTList.at(i).second.second;

////        float xrate = 0.5;
////        float yrate = 0.5;
////        float zrate = 0.5;
//        float xrate = CFAST->m_posX;
//        float yrate = CFAST->m_posY;
//        float zrate = CFAST->m_posZ;

//        float posX = minPos.x() + (maxPos.x() - minPos.x()) * xrate;
//        float posY = minPos.y() + (maxPos.y() - minPos.y()) * yrate;
//        float posZ = minPos.z() + (maxPos.z() - minPos.z()) * zrate;
//        float height = 1;
//        float displayHeight = height;

//        if(height < minPos.y())
//            displayHeight = minPos.y();
//        if(height > maxPos.y())
//            displayHeight = maxPos.y();

//        setShaderProgram(0);
//        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
//        GL->glEnable(GL_BLEND);
//        GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//        GL->glColor4f(1, 0, 0, 0.5);
//        NEVertexBuffer* planeVerts;
//        planeVerts = 0;
//        planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
//        NEPVertex* vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

//        //YOZ plane
//        vertexPtr[0].position = world.inverted() * vec3f(posX, minPos.y(), minPos.z());
//        vertexPtr[1].position = world.inverted() * vec3f(posX, minPos.y(), maxPos.z());
//        vertexPtr[2].position = world.inverted() * vec3f(posX, displayHeight, maxPos.z());
//        vertexPtr[3].position = world.inverted() * vec3f(posX, displayHeight, minPos.z());

//        planeVerts->endLoadData();
//        if (displayHeight <= maxPos.y() && CFAST->m_visualX)
//            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

//        GL->glColor4f(0, 0, 1, 0.5);

//        if(planeVerts)
//        {
//            NERENDERMAN.GLManager()->destroy(planeVerts);
//            planeVerts = 0;
//        }
//        planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
//        vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

//        vertexPtr[0].position = world.inverted() * vec3f(posX, displayHeight, minPos.z());
//        vertexPtr[1].position = world.inverted() * vec3f(posX, displayHeight, maxPos.z());
//        vertexPtr[2].position = world.inverted() * vec3f(posX, maxPos.y(), maxPos.z());
//        vertexPtr[3].position = world.inverted() * vec3f(posX, maxPos.y(), minPos.z());

//        planeVerts->endLoadData();

//        if (displayHeight >= minPos.y() && CFAST->m_visualX)
//            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

//        //XOZ Plane

//        if(posY < height)
//            GL->glColor4f(1, 0, 0, 0.5);
//        else
//            GL->glColor4f(0, 0, 2, 0.5);

//        if(planeVerts)
//        {
//            NERENDERMAN.GLManager()->destroy(planeVerts);
//            planeVerts = 0;
//        }
//        planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
//        vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

//        vertexPtr[0].position = world.inverted() * vec3f(minPos.x(), posY, minPos.z());
//        vertexPtr[1].position = world.inverted() * vec3f(minPos.x(), posY, maxPos.z());
//        vertexPtr[2].position = world.inverted() * vec3f(maxPos.x(), posY, maxPos.z());
//        vertexPtr[3].position = world.inverted() * vec3f(maxPos.x(), posY, minPos.z());

//        planeVerts->endLoadData();
//        if(CFAST->m_visualY)
//        NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

//        //XOY Plane

//        GL->glColor4f(1, 0, 0, 0.5);

//        if(planeVerts)
//        {
//            NERENDERMAN.GLManager()->destroy(planeVerts);
//            planeVerts = 0;
//        }
//        planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
//        vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

//        vertexPtr[0].position = world.inverted() * vec3f(minPos.x(), minPos.y(), posZ);
//        vertexPtr[1].position = world.inverted() * vec3f(minPos.x(), displayHeight, posZ);
//        vertexPtr[2].position = world.inverted() * vec3f(maxPos.x(), displayHeight, posZ);
//        vertexPtr[3].position = world.inverted() * vec3f(maxPos.x(), minPos.y(), posZ);

//        planeVerts->endLoadData();
//        if (displayHeight <= maxPos.y() && CFAST->m_visualZ)
//            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

//        GL->glColor4f(0, 0, 1, 0.5);

//        if(planeVerts)
//        {
//            NERENDERMAN.GLManager()->destroy(planeVerts);
//            planeVerts = 0;
//        }
//        planeVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 4, 0, GL_STATIC_DRAW);
//        vertexPtr = (NEPVertex*) planeVerts->beginLoadData();

//        vertexPtr[0].position = world.inverted() * vec3f(minPos.x(), maxPos.y(), posZ);
//        vertexPtr[1].position = world.inverted() * vec3f(minPos.x(), displayHeight, posZ);
//        vertexPtr[2].position = world.inverted() * vec3f(maxPos.x(), displayHeight, posZ);
//        vertexPtr[3].position = world.inverted() * vec3f(maxPos.x(), maxPos.y(), posZ);

//        planeVerts->endLoadData();

//        if (displayHeight >= minPos.y() && CFAST->m_visualZ)
//            NERENDERMAN.GLManager()->draw(kQuadListPrim, planeVerts);

//        worldStack().popMatrix();
//        setModelViewMatrix();
//        GL->glDisable(GL_BLEND);
//        GL->glPopAttrib();

//    }
}

void IGLSurface::drawSnapPt()
{
    if(m_measureStep == IGLSurface::MeasureStep::StandBy || m_measureStep == IGLSurface::MeasureStep::FirstPt)
    {
        if(m_bSnap || m_bSnapEdge)
        {
            //GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
            GL->glColor3f(1.0, 0.0, 0.0);
            GL->glPointSize(5);
            GL->glBegin(GL_POINTS);
            GL->glVertex3f(m_snapPt.x(), m_snapPt.y(), m_snapPt.z());
            GL->glEnd();
            //GL->glPopAttrib();
            //printf("x,y,z::%f, %f, %f\n", m_snapPt.x(), m_snapPt.y(), m_snapPt.z());
        }
    }
}

void IGLSurface::drawMeasurement()
{
    QList<NEMeasurementItem *> mitems = NESCENE.findChildren< NEMeasurementItem *>();
    m_measureCnt = mitems.length() + 1;

    if (m_measureStep == IGLSurface::MeasureStep::StandBy)
    {
        m_firstSnapObject = m_snapObject;
    }
    if (m_measureStep == IGLSurface::MeasureStep::FirstPt)
    {
        float mouseCamX = (m_mousePos.x() / m_width - 0.5) * 2;
        float mouseCamY = (0.5 - m_mousePos.y() / m_height) * 2;
        vec3f mouseCam(mouseCamX, mouseCamY, -0.5);
        vec3f mouseWorld = viewMatrix().inverted() * projectionMatrix().inverted() * mouseCam;
        bool flag = !(m_bSnap || m_bSnapEdge);

        if (m_bSnap || m_bSnapEdge)
        {
            vec3f originFirst;
            vec3f originSecond;
            if (m_bSnapEdge)
            {
                m_measureSecondEdge.first = m_snapObject;
                m_measureSecondEdge.second.first = m_snapEdgeId;
                m_measureSecondEdge.second.second = m_snapObject->transform().inverted() * m_snapPt;
                originFirst = m_measureFirstEdge.first->transform() * m_measureFirstEdge.second.second;
                originSecond = m_snapPt;

                NESpatialMesh* selObj1 = m_measureFirstEdge.first;
                NESpatialMesh* selObj2 = m_measureSecondEdge.first;
                int vertID1 = m_measureFirstEdge.second.first.first;
                int vertID2 = m_measureFirstEdge.second.first.second;
                int vertID3 = m_measureSecondEdge.second.first.first;
                int vertID4 = m_measureSecondEdge.second.first.second;

                vec3f p1 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID1].position;
                vec3f p2 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID2].position;
                vec3f p3 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID3].position;
                vec3f p4 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID4].position;

                vec3f dir1 = (p2-p1).normalized();
                vec3f dir2 = (p4-p3).normalized();
                vec3f dir = vec3f::crossProduct(dir1, dir2);
                float len = 0;
                if(dir.length() < 1e-3)
                    len = ((p3-p1)-(vec3f::dotProduct(p3-p1, p2-p1)/vec3f::dotProduct(p2-p1, p2-p1))*(p2-p1)).length();
                else
                    len = vec3f::dotProduct(p3-p1, dir.normalized());

                len = abs(len);
                if (len < 1e-3 && dir.length() < 1e-3)
                    flag = true;
            }
            else
            {
                m_measureSecondPt.first = m_snapObject;
                m_measureSecondPt.second = m_snapPtId;
                originFirst = m_measureFirstPt.first->transform() * m_measureFirstPt.first->triangleMesh().vertices()[m_measureFirstPt.second].position;
                originSecond = m_snapPt;
            }

            if(!flag)
            {
                vec3f cameraPos = camera()->Position();
                vec3f _cameraPos = viewMatrix().inverted() * vec3f(0,0,0);
                vec3f planDirection = vec3f::crossProduct(originFirst - cameraPos, originSecond - cameraPos);
                planDirection.normalize();
                vec3f planNorm = vec3f::crossProduct(originSecond - originFirst, planDirection);
                planNorm.normalize();

                vec3f rayDirection = mouseWorld - cameraPos;
                rayDirection.normalize();

                //vec3f interPoint = linePlaneInterPoint(cameraPos, rayDirection, m_measureFirstPt, planNorm);
                //float moveAmount = vec3f::dotProduct(interPoint - m_measureFirstPt, planDirection);

                float moveAmount = 1.0f;

    //            firstPt = m_measureFirstPt + moveAmount * planDirection;
    //            secondPt = m_measureSecondPt + moveAmount * planDirection;

                measureInfoMoved.first = planDirection;
                measureInfoMoved.second = moveAmount;

                std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin;
                std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfoOrigin;
                if (m_bSnapEdge)
                {
                    measureEdgeInfoOrigin.push_back(m_measureFirstEdge);
                    measureEdgeInfoOrigin.push_back(m_measureSecondEdge);
                    measureInfoMoved.second = 0;
                }
                else
                {
                    measureInfoOrigin.push_back(m_measureFirstPt);
                    measureInfoOrigin.push_back(m_measureSecondPt);
                }

    //            std::vector<QPair<vec3f, float>> measureInfoMoved;
    //            measureInfoMoved.push_back(firstPt);
    //            measureInfoMoved.push_back(secondPt);

                renderMeasurement(measureInfoOrigin, measureEdgeInfoOrigin,  measureInfoMoved);
            }
        }
        if (flag)
        {
            vec3f originFirst;
            vec3f tempPt;
            if (m_snapMode)
            {
                originFirst = m_measureFirstEdge.first->transform() * m_measureFirstEdge.second.second;
                tempPt = m_snapPt;
            }
            else
            {
                originFirst = m_measureFirstPt.first->transform() * m_measureFirstPt.first->triangleMesh().vertices()[m_measureFirstPt.second].position;
                tempPt = mouseWorld;
            }
            GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
            setShaderProgram(0);
            GL->glBegin(GL_LINES);
            GL->glColor3f(0, 0, 0);
            GL->glVertex3f(originFirst.x(), originFirst.y(), originFirst.z());
//            GL->glVertex3f(mouseWorld.x(), mouseWorld.y(), mouseWorld.z());
            GL->glVertex3f(tempPt.x(), tempPt.y(), tempPt.z());
            GL->glEnd();
            NERENDERMAN.GLManager()->bindPositionShader(this);
            GL->glPopAttrib();
        }
    }
    if (m_measureStep == IGLSurface::MeasureStep::SecondPt)
    {

        vec3f originFirst;
        vec3f originSecond;
        vec3f dir1;
        vec3f dir2;
        bool isParallel = false;
        if (m_snapMode)
        {
            NESpatialMesh* selObj1 = m_measureFirstEdge.first;
            NESpatialMesh* selObj2 = m_measureSecondEdge.first;

            originFirst = selObj1->transform() * m_measureFirstEdge.second.second;
            originSecond = selObj2->transform() * m_measureSecondEdge.second.second;

            int vertID1 = m_measureFirstEdge.second.first.first;
            int vertID2 = m_measureFirstEdge.second.first.second;
            int vertID3 = m_measureSecondEdge.second.first.first;
            int vertID4 = m_measureSecondEdge.second.first.second;

            vec3f p1 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID1].position;
            vec3f p2 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID2].position;
            vec3f p3 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID3].position;
            vec3f p4 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID4].position;

            dir1 = (p2-p1).normalized();
            dir2 = (p4-p3).normalized();
            vec3f dir = vec3f::crossProduct(dir1, dir2);
            float len = 0;
            if(dir.length() < 1e-3)
            {
                len = ((p3-p1)-(vec3f::dotProduct(p3-p1, p2-p1)/vec3f::dotProduct(p2-p1, p2-p1))*(p2-p1)).length();
                vec3f a = p1-p3;
                vec3f n = vec3f::crossProduct(dir2, a);
                vec3f vertical = vec3f::crossProduct(n, dir2);
                vertical = vertical.normalized();
                m_measureFirstEdge.second.second = selObj1->transform().inverted() * (originSecond + vertical * len);
                isParallel = true;
            }
            else
                len = vec3f::dotProduct(p3-p1, dir.normalized());

            len = abs(len);
            if (len < 1e-3 && dir.length() < 1e-3)
            {
                m_measureStep = IGLSurface::MeasureStep::StandBy;
                return;
            }

        }
        else
        {
            originFirst = m_measureFirstPt.first->transform() * m_measureFirstPt.first->triangleMesh().vertices()[m_measureFirstPt.second].position;
            originSecond = m_measureSecondPt.first->transform() * m_measureSecondPt.first->triangleMesh().vertices()[m_measureSecondPt.second].position;
        }
        vec3f cameraPos = camera()->Position();
        vec3f _cameraPos = viewMatrix().inverted() * vec3f(0,0,0);
        vec3f planDirection = vec3f::crossProduct(originFirst - cameraPos, originSecond - cameraPos);

        float mouseCamX = (m_mousePos.x() / m_width - 0.5) * 2;
        float mouseCamY = (0.5 - m_mousePos.y() / m_height) * 2;
        vec3f mouseCam(mouseCamX, mouseCamY, 0.1);
        vec3f mouseWorld = viewMatrix().inverted() * projectionMatrix().inverted() * mouseCam;
        vec3f rayDirection = mouseWorld - cameraPos;
        if(isParallel)
            planDirection = dir1;

        vec3f planNorm = vec3f::crossProduct(originSecond - originFirst, planDirection);
        rayDirection.normalize();
        planDirection.normalize();
        planNorm.normalize();

        vec3f interPoint = linePlaneInterPoint(cameraPos, rayDirection, originFirst, planNorm);

        switch (camera()->OrthoType()) {
        case NECamera::OrthographicType::Top:
            mouseWorld.setY(0);
            planDirection.setY(0);
            planDirection.normalize();
            rayDirection = vec3f(0,-1,0);
            interPoint = linePlaneInterPoint(mouseWorld, rayDirection, originFirst, planNorm);
            break;
        case NECamera::OrthographicType::Left:
            mouseWorld.setX(0);
            planDirection.setX(0);
            planDirection.normalize();
            rayDirection = vec3f(1,0,0);
            interPoint = linePlaneInterPoint(mouseWorld, rayDirection, originFirst, planNorm);
            break;
        case NECamera::OrthographicType::Front:
            mouseWorld.setZ(0);
            planDirection.setZ(0);
            planDirection.normalize();
            rayDirection = vec3f(0,0,-1);
            interPoint = linePlaneInterPoint(mouseWorld, rayDirection, originFirst, planNorm);
            break;
        }

        float moveAmount = vec3f::dotProduct(interPoint - originFirst, planDirection);

//        firstPt = m_measureFirstPt + moveAmount * planDirection;
//        secondPt = m_measureSecondPt + moveAmount * planDirection;
        measureInfoMoved.first = planDirection;
        measureInfoMoved.second = moveAmount;
        std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin;
        std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfoOrigin;
        if (m_snapMode)
        {
            measureEdgeInfoOrigin.push_back(m_measureFirstEdge);
            measureEdgeInfoOrigin.push_back(m_measureSecondEdge);
            if(!isParallel)
            {
                measureInfoMoved.second = 0;
                m_measureStep = IGLSurface::MeasureStep::StandBy;
                NENode* m1 = NESCENE.createObject( "Measurement Item", QString::number(m_measureCnt));
                if (NEMeasurementItem* tmpObject = qobject_cast<NEMeasurementItem*> (m1))
                {
                    tmpObject->setMeasurementEdge( true );
                    tmpObject->SetCameraType( camera()->OrthoType() );
                    tmpObject->measureEdgeInfo = measureEdgeInfoOrigin;
                    tmpObject->measureInfoMoved = measureInfoMoved;
                    m_snapObject->addMeasureObject(m_measureCnt);
                    m_firstSnapObject->addMeasureObject(m_measureCnt);
                    //m_measureCnt += 1;
                }
                renderMeasurement(measureInfoOrigin, measureEdgeInfoOrigin,  measureInfoMoved);
                return;
            }
        }
        else
        {
            measureInfoOrigin.push_back(m_measureFirstPt);
            measureInfoOrigin.push_back(m_measureSecondPt);
        }
///*        std::vector<QPair<vec3f, float>> measureInfoMoved;
//        measureInfoMoved.push_back(firstPt);
//        measureInfoMoved.push_back(secondPt)*/;
//        measureInfo.push_back(firstPt);
//        measureInfo.push_back(secondPt);

        renderMeasurement(measureInfoOrigin, measureEdgeInfoOrigin,  measureInfoMoved);
    }
    if (m_measureStep == IGLSurface::MeasureStep::DrawMeasurement)
    {
//        std::vector<QPair<NESpatialMesh*, int>> measureInfo;
//        measureInfo.push_back(m_measureFirstPt);
//        measureInfo.push_back(m_measureSecondPt);
//        measureInfo.push_back(firstPt);
//        measureInfo.push_back(secondPt);
//        renderMeasurement(rotationMatrix, measureInfo);
        std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin;
        std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfoOrigin;
        if (m_snapMode)
        {
            measureEdgeInfoOrigin.push_back(m_measureFirstEdge);
            measureEdgeInfoOrigin.push_back(m_measureSecondEdge);
        }
        else
        {
            measureInfoOrigin.push_back(m_measureFirstPt);
            measureInfoOrigin.push_back(m_measureSecondPt);
        }
//        std::vector<QPair<vec3f, float>> measureInfoMoved;
//        measureInfoMoved.push_back(firstPt);
//        measureInfoMoved.push_back(secondPt);
        renderMeasurement(measureInfoOrigin, measureEdgeInfoOrigin, measureInfoMoved);

        m_measureStep = IGLSurface::MeasureStep::StandBy;
        NENode* m1 = NESCENE.createObject( "Measurement Item", QString::number(m_measureCnt));
        if (NEMeasurementItem* tmpObject = qobject_cast<NEMeasurementItem*> (m1))
        {
            if(m_snapMode)
                tmpObject->setMeasurementEdge( true );
            tmpObject->SetCameraType( camera()->OrthoType() );

            tmpObject->measureInfoOrigin = measureInfoOrigin;
            tmpObject->measureEdgeInfo = measureEdgeInfoOrigin;
            tmpObject->measureInfoMoved = measureInfoMoved;
            m_snapObject->addMeasureObject(m_measureCnt);
            m_firstSnapObject->addMeasureObject(m_measureCnt);
            //m_measureCnt += 1;
        }
    }
}

void IGLSurface::renderMeasurement(std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin, std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfo, QPair<vec3f, float> measureInfoMoved)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
//    worldStack().pushMatrix( rotationMatrix );
//    setModelViewMatrix();
//    worldStack().popMatrix();
    setShaderProgram(0);

    vec3f originFirstPt;
    vec3f originSecondPt;

    if (m_snapMode)
    {
        originFirstPt = measureEdgeInfo.at(0).first->transform() * measureEdgeInfo.at(0).second.second;
        originSecondPt = measureEdgeInfo.at(1).first->transform() * measureEdgeInfo.at(1).second.second;
    }
    else
    {
        originFirstPt = measureInfoOrigin.at(0).first->transform() * measureInfoOrigin.at(0).first->triangleMesh().vertices()[measureInfoOrigin.at(0).second].position;
        originSecondPt = measureInfoOrigin.at(1).first->transform() * measureInfoOrigin.at(1).first->triangleMesh().vertices()[measureInfoOrigin.at(1).second].position;
    }

//    vec3f originFirstPt = measureInfoOrigin.at(0).first->transform() * measureInfoOrigin.at(0).first->triangleMesh().vertices()[measureInfoOrigin.at(0).second].position;
//    vec3f originSecondPt =measureInfoOrigin.at(1).first->transform() * measureInfoOrigin.at(1).first->triangleMesh().vertices()[measureInfoOrigin.at(1).second].position;
    vec3f movedFirstPt = originFirstPt + measureInfoMoved.second * measureInfoMoved.first;
    vec3f movedSecondPt = originSecondPt + measureInfoMoved.second * measureInfoMoved.first;
    vec3f originNorm = (movedFirstPt - originFirstPt).normalized();
    vec3f movedNorm = (movedSecondPt - movedFirstPt).normalized();

    float len = 0;
    vec3f lengthVec ;
    if(m_snapMode)
    {
        NESpatialMesh* selObj1 = measureEdgeInfo.at(0).first;
        NESpatialMesh* selObj2 = measureEdgeInfo.at(1).first;
        int vertID1 = measureEdgeInfo.at(0).second.first.first;
        int vertID2 = measureEdgeInfo.at(0).second.first.second;
        int vertID3 = measureEdgeInfo.at(1).second.first.first;
        int vertID4 = measureEdgeInfo.at(1).second.first.second;

        vec3f p1 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID1].position;
        vec3f p2 = selObj1->transform() * selObj1->triangleMesh().vertices()[vertID2].position;
        vec3f p3 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID3].position;
        vec3f p4 = selObj2->transform() * selObj2->triangleMesh().vertices()[vertID4].position;

        vec3f dir1 = (p2-p1).normalized();
        vec3f dir2 = (p4-p3).normalized();
        vec3f dir = vec3f::crossProduct(dir1, dir2);
        if(dir.length() < 1e-3)
        {
            lengthVec = ((p3-p1)-(vec3f::dotProduct(p3-p1, p2-p1)/vec3f::dotProduct(p2-p1, p2-p1))*(p2-p1));
            len = lengthVec.length();
        }
        else
        {
            lengthVec  = vec3f(0,0,0);
            len = vec3f::dotProduct(p3-p1, dir.normalized());
        }
        len = abs(len);
    }
    else
    {
        lengthVec = movedSecondPt - movedFirstPt;
        len = lengthVec.length();
    }
    switch (camera()->OrthoType())
    {
    case NECamera::OrthographicType::Top:
        len = vec3f(lengthVec.x(), 0, lengthVec.z()).length();
        break;
    case NECamera::OrthographicType::Left:
        len = vec3f(0, lengthVec.y(), lengthVec.z()).length();
        break;
    case NECamera::OrthographicType::Front:
        len = vec3f(lengthVec.x(), lengthVec.y(), 0).length();
        break;
    }

    FTPolygonFont* pFont = new FTPolygonFont("System/Fonts/tahoma.ttf");
    pFont->FaceSize(1);
    FTBBox box = pFont->BBox(QString::number(len).toUtf8().data());
    vec3f upper = vec3f(box.Upper().X(), box.Upper().Y(), box.Upper().Z());
    vec3f lower = vec3f(box.Lower().X(), box.Lower().Y(), box.Lower().Z());
    vec3f letterLen = upper - lower;
    delete(pFont);

    float size = 0.5;
    vec3f offset = viewMatrix().inverted().mapVector((upper+lower) / 2);
    vec3f pos = (movedSecondPt + movedFirstPt) / 2 - offset * size;

    vec3f scale(1.0, 1.0, 1.0);
    eulerf angle = calculateDrawAngle(NULL);

    lower -= vec3f(0.1, 0.1, 0);
    upper += vec3f(0.1, 0.1, 0);
    vec3f clip0 = lower;
    vec3f clip2 = upper;
    vec3f clip1(clip0.x(), clip2.y(), 0);
    vec3f clip3(clip2.x(), clip0.y(), 0);
    vec3f clip[4] = {clip0, clip1, clip2, clip3};
    matrix44f camMat = viewMatrix().inverted();
    vec3f org = (movedSecondPt + movedFirstPt) / 2;
    vec3f center = (lower + upper) / 2;

    vec3f v1 = viewMatrix() * movedFirstPt;
    vec3f v2 = viewMatrix() * movedSecondPt;
    vec3f dir = v2 - v1;
    vec3f dirnorm = dir.normalized();
    int intercnt = 0;
    vec3f interpoints[4];

    for (int n = 0; n < 4; n++) {
        clip[n] = camMat.mapVector(clip[n] - center) + org;
        clip[n] = viewMatrix() * clip[n];
    }

    for (int n = 0; n < 4; n++) {
        int m = (n + 1) % 4;

        vec3f norm = vec3f::crossProduct(clip[n], clip[m]);
        if (fabs(vec3f::dotProduct(norm, dir)) > 1e-3) {
            vec3f interpt = linePlaneInterPoint(v1, dir, vec3f(), norm);
            if ((vec3f::dotProduct(norm, vec3f::crossProduct(clip[n], interpt)) > 0) &&
                (vec3f::dotProduct(norm, vec3f::crossProduct(interpt, clip[m])) > 0))
            {
                interpoints[intercnt++] = interpt;
            }
        }
    }


    //    movedFirstPt += originNorm * letterLen.y() / 2;
    //    movedSecondPt += originNorm * letterLen.y() / 2;
    vec3f movedHalfFirstPt = (movedFirstPt + movedSecondPt) / 2 - movedNorm * 0.1 - movedNorm * abs(letterLen.x()) / 2;
    vec3f movedHalfSecondPt = (movedFirstPt + movedSecondPt) / 2 + movedNorm * 0.1 + movedNorm * abs(letterLen.x()) / 2 ;


    if (intercnt == 2) {
        float t0, t1;
        t0 = vec3f::dotProduct(interpoints[0] - v1, dirnorm) / len;
        t1 = vec3f::dotProduct(interpoints[1] - v1, dirnorm) / len;
        float tx, tn;
        tn = std::max(std::min(t0, t1), 0.0f);
        tx = std::min(std::max(t0, t1), 1.0f);

        movedHalfFirstPt = movedFirstPt + movedNorm * len * tn;
        movedHalfSecondPt = movedFirstPt + movedNorm * len * tx;
    } else {
        intercnt = 0;

        movedHalfFirstPt = movedFirstPt;
        movedHalfSecondPt = movedSecondPt;
    }

    vec3f arrowPos1 = movedFirstPt + movedNorm * 0.2 - originNorm * 0.15;
    vec3f arrowPos2 = movedFirstPt + movedNorm * 0.2 + originNorm * 0.15;
    vec3f arrowPos3 = movedSecondPt - movedNorm * 0.2 - originNorm * 0.15;
    vec3f arrowPos4 = movedSecondPt - movedNorm * 0.2 + originNorm * 0.15;

    if (movedFirstPt == movedHalfFirstPt) {
        arrowPos1 = movedFirstPt;
        arrowPos2 = movedFirstPt;
    }
    if (movedSecondPt == movedHalfSecondPt) {
        arrowPos3 = movedSecondPt;
        arrowPos4 = movedSecondPt;
    }

    vec3f _arrowPos3 = originSecondPt - movedNorm.normalized() * 0.2;

    GL->glColor3f(0.24f, 0.24f, 0.24f);

    NEVertexBuffer* lineVerts;
    lineVerts = 0;
    lineVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 16, 0, GL_STATIC_DRAW);
    NEPVertex* vertexPtr = (NEPVertex*) lineVerts->beginLoadData();


    vertexPtr[0].position = movedFirstPt;
    vertexPtr[1].position = movedHalfFirstPt;
    vertexPtr[2].position = movedHalfSecondPt;
    vertexPtr[3].position = movedSecondPt;
    vertexPtr[4].position = movedFirstPt;
    vertexPtr[5].position = originFirstPt;
    vertexPtr[6].position = movedSecondPt;
    vertexPtr[7].position = originSecondPt;
    //draw Arrow
    vertexPtr[8].position = movedFirstPt;
    vertexPtr[9].position = arrowPos1;
    vertexPtr[10].position = movedFirstPt;
    vertexPtr[11].position = arrowPos2;
    vertexPtr[12].position = movedSecondPt;
    vertexPtr[13].position = arrowPos3;
    vertexPtr[14].position = movedSecondPt;
    vertexPtr[15].position = arrowPos4;

    lineVerts->endLoadData();
    NERENDERMAN.GLManager()->draw(kLineListPrim, lineVerts);

    GL->glEnable(GL_LINE_STIPPLE);
    GL->glLineStipple(3, 0xaaaa);
    GL->glColor3f(0.0,0.0,1.0);

    if(lineVerts)
    {
        NERENDERMAN.GLManager()->destroy(lineVerts);
        lineVerts = 0;
    }
    lineVerts = NERENDERMAN.GLManager()->createVertexBuffer(kPFormat, 2, 0, GL_STATIC_DRAW);
    NEPVertex* vertexStipplePtr = (NEPVertex*) lineVerts->beginLoadData();
    vertexStipplePtr[0].position = arrowPos3;
    vertexStipplePtr[1].position = _arrowPos3;

    lineVerts->endLoadData();
    NERENDERMAN.GLManager()->draw(kLineListPrim, lineVerts);

    this->drawText3D(QString::number(len), pos, angle, size, scale,Color32(60,60,60,255));
    NERENDERMAN.GLManager()->bindPositionShader(this);
    GL->glPopAttrib();
}

void IGLSurface::drawFloor()
{
    constexpr float a = 1.0/255.0;

    NERENDERMAN.GLManager()->bindPositionShader(this);
    GL->glEnable(GL_BLEND);
    GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL->glLineWidth(gridThickness());
    GL->glColor4f(a*gridColor().red(), a*gridColor().green(), a*gridColor().blue(), a*gridColor().alpha());
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    NERENDERMAN.GLManager()->draw(kLineListPrim, floorVerts, 4, floorVerts->getVertexCount() - 4 - verticesLvl2.length());

    //add grid level : show smaller cells
    if(addGridLevel())
    {
        GL->glLineWidth(subGridThickness());
        GL->glColor4f(a*subGridColor().red(), a*subGridColor().green(), a*subGridColor().blue(), a*subGridColor().alpha());
        NERENDERMAN.GLManager()->updatePositionUniforms(this);
        NERENDERMAN.GLManager()->draw(kLineListPrim, floorVerts, floorVerts->getVertexCount() - verticesLvl2.length(), verticesLvl2.length());
    }

    GL->glLineWidth(axisThickness());
    GL->glColor4f(1, 1, 1, 0.6);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    NERENDERMAN.GLManager()->draw(kLineListPrim, floorVerts, 0, 4);

    //grid measurements
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    setShaderProgram(0);
    if(gridMeasurements())
    {
        int cnt = verticesLvl1.length();
        int cntW = 2*(cellCount().width() + 1);
        int cntH = 2*(cellCount().height() + 1);
        int captionW = 0.5*cellDivision().width() * cellCount().width();
        int captionH = 0.5*cellDivision().height() * cellCount().height();
        Color32 colorW(255,0,0,255);
        Color32 colorH(0,0,255,255);
        int value = captionW;
        eulerf angle = calculateDrawAngle(NULL);
        float textSize = std::min(m_CellSize.width(), m_CellSize.height()) * 0.2;

        quatf cameraquat;
        if( camera() )
        {
            if( camera()->OrthoType() == NECamera::Front )
            {
                cameraquat = quatf::fromAxisAndAngle(1, 0, 0, -90);
                colorH.Set(0, 255, 0, 255);
            }
            else if( camera()->OrthoType() == NECamera::Left )
            {
                cameraquat = quatf::fromAxisAndAngle(0, 0, 1, 90);
                colorW.Set(0, 255, 0, 255);
            }
        }

        for(int i = 0; i < cntW; i+=2)
        {
            this->drawText3D(QString::number(-value),cameraquat.rotatedVector(verticesLvl1[i]),angle,textSize,vec3f(1.0,1.0,1.0),colorW);
            this->drawText3D(QString::number(-value),cameraquat.rotatedVector(verticesLvl1[i+1]),angle,textSize,vec3f(1.0,1.0,1.0),colorW);
            value -= cellDivision().width();
        }
        value = captionH;
        for(int i = cntH; i < cnt; i+=2)
        {
            this->drawText3D(QString::number(-value),cameraquat.rotatedVector(verticesLvl1[i]),angle,textSize,vec3f(1.0,1.0,1.0),colorH);
            this->drawText3D(QString::number(-value),cameraquat.rotatedVector(verticesLvl1[i+1]),angle,textSize,vec3f(1.0,1.0,1.0),colorH);
            value -= cellDivision().height();
        }
    }

    NERENDERMAN.GLManager()->bindPositionShader(this);
    GL->glPopAttrib();

    GL->glLineWidth( 1.0 );
}

void IGLSurface::beginTransparency(float width, float height)
{
    if (transparencyType() !=  NoTransparency  )
    {
        if (!checkTransparencyQualityProfile() || isSceneDynamic())
        {
            return;
        }
        GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
        // Blit current scene as the weight blend OIT background
        GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_sceneFBO);
        GL->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_OITBackgroundFBO);
        GL->glBlitFramebuffer(0,0,width*m_bfaaMultiplier ,height*m_bfaaMultiplier,
                          0,0,width*m_bfaaMultiplier ,height*m_bfaaMultiplier,GL_COLOR_BUFFER_BIT, GL_NEAREST);

        if(transparencyType() == WeightBlendedTransparency ){

            bindSceneFrameBuffer();

            // Make current the OIT buffer
            GL->glDepthFunc(GL_ALWAYS); //!< This will ignore the depth test, but still update the depth buffer

            float clearColorZero[4] = { 0.f, 0.f, 0.f, 0.f };
            float clearColorOne[4]  = { 1.f, 1.f, 1.f, 1.f };
            GL->glClearBufferfv(GL_COLOR, 1, clearColorZero);
            GL->glClearBufferfv(GL_COLOR, 2, clearColorOne);

            // and set blending parameters
            GL->glEnable(GL_BLEND);
            GL->glBlendEquation(GL_FUNC_ADD);
            GL43->glBlendFunci(1, GL_ONE, GL_ONE);
            GL43->glBlendFunci(2, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

            m_TransparencyStage = WeightedBlending;
        }
        else if (isDepthPeeling()) //Depth Peeling OIT
        {
            GL->glBindFramebuffer(GL_FRAMEBUFFER, m_OITBlenderFBO);
            GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);
            GL->glClearColor(0.f, 0.f, 0.f, 1.f);
            GL->glClearDepth(1.f);
            GL->glEnable(GL_DEPTH_TEST);
            GL->glDepthFunc(GL_LESS);

            GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_TransparencyStage = DepthPeelingInit;
        }
        else if(transparencyType() == LinkedListTransparency ){

            GLuint zero = 0;
            GL->glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_LinkedListCounterBuffer);
            GL->glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
            GL->glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_LinkedListCounterBuffer);


            GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_LinkedListHeadPtrClearBuffer);
            GL->glBindTexture(GL_TEXTURE_2D, m_LinkedListHeadPointerTexture);
            GL->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width*m_bfaaMultiplier ,height*m_bfaaMultiplier, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
            GL->glBindTexture(GL_TEXTURE_2D, 0);

            bindSceneFrameBuffer();

            GL->glClearColor(0.f, 0.f, 0.f, 1.f);
            GL->glClearDepth(1.f);
            GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            GL->glEnable(GL_DEPTH_TEST);
            GL->glDepthFunc(GL_ALWAYS);

            m_TransparencyStage = LinkedListOIT;


            // For different orthographic angles, change the orientation of the floor grid
            matrix44f rotationMatrix;

            if( camera() )
            {
                if( camera()->OrthoType() == NECamera::Front )
                    rotationMatrix.rotate( -90, 1, 0, 0 );
                else if( camera()->OrthoType() == NECamera::Left )
                    rotationMatrix.rotate( 90, 0, 0, 1 );
            }

            worldStack().pushMatrix( rotationMatrix );
            setModelViewMatrix();
            if( drawGrid() )
            {
                GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
                if( overlayGrid() )
                    NERENDERMAN.GLManager()->setDepthTest( kDisableDepthTest );

                drawFloor();
                GL->glPopAttrib();
            }
            worldStack().popMatrix();
        }
    }
    else
        m_TransparencyStage = NoTransparencing;
}

void IGLSurface::renderWeightedBlendedOIT()
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glUniform1i(GL->glGetUniformLocation( shaderProgram()->programId(),"ColorTex0"), 0);
    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(),"ColorTex1"), 1);
    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(),"uBackgroundTex"), 2);

    GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_weightColorAccumulationTexture);
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, m_weightAlphaAccumulationTexture);
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, m_OITBackgroundTexture);
    NERENDERMAN.GLManager()->drawQuad();
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, 0);
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, 0);
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, 0);

    GL->glPopAttrib();
}

void IGLSurface::renderLinkedListOIT()
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    GL43->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(),"uBackgroundTex"), 0);

    //    GL->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_LinkedListLinkedListBuffer);
    //    GL43->glBindImageTexture(0, m_LinkedListHeadPointerTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    GL->glClear(GL_COLOR_BUFFER_BIT);

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_OITBackgroundTexture);
    NERENDERMAN.GLManager()->drawQuad();
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, 0);

    GL->glPopAttrib();
}

void IGLSurface::doTransparency(float width, float height)
{
    if (transparencyType() == NoTransparency )
    {
        return;
    }
    if (!checkTransparencyQualityProfile() || isSceneDynamic())
    {
        return;
    }

    // Restore blending setting
    GL->glPopAttrib();

    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (transparencyType() == WeightBlendedTransparency )
    {
        // Blend weight OIT
        GL->glBindFramebuffer( GL_FRAMEBUFFER, m_OITBlenderFBO);
        NERENDERMAN.GLManager()->bindWeightedBlendedFinalShader(this);

        renderWeightedBlendedOIT();

        GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_OITBlenderFBO);
        GL->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_sceneFBO);
        GL->glBlitFramebuffer(0,0,width*m_bfaaMultiplier,height*m_bfaaMultiplier,
                          0,0,width*m_bfaaMultiplier,height*m_bfaaMultiplier,GL_COLOR_BUFFER_BIT, GL_NEAREST);
        bindSceneFrameBuffer();
    }
    else if (isDepthPeeling())
    {
        GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_OITBlenderFBO);
        GL->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_sceneFBO);
        GL->glBlitFramebuffer(0,0,width*m_bfaaMultiplier,height*m_bfaaMultiplier,
                          0,0,width*m_bfaaMultiplier,height*m_bfaaMultiplier,GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

        GLuint queryId;
        GL->glGenQueries(1, &queryId);
        for(int layer = 1; layer < depthPeelingIterations(); layer ++){

            // ---------------------------------------------------------------------
            // 2.2. Peel the next depth layer
            // ---------------------------------------------------------------------

            int32_t currId = layer % 2;
            int32_t prevId = 1 - currId;

            GL->glBindFramebuffer(GL_FRAMEBUFFER, m_depthPeelingFBOs[currId]);
            GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);

            GL->glClearColor(0, 0, 0, 0);
            GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GL->glDisable(GL_BLEND);
            GL->glEnable(GL_DEPTH_TEST);
            GL->glDepthFunc(GL_LESS);

            GL->glBeginQuery(GL_SAMPLES_PASSED, queryId);

            GL->glActiveTexture(GL_TEXTURE1);
            GL->glBindTexture(GL_TEXTURE_RECTANGLE, m_depthPeelingDepthTextures[prevId]);

            m_TransparencyStage = DepthPeelingPeel;

            // For different orthographic angles, change the orientation of the floor grid
            matrix44f rotationMatrix;

            if( camera() )
            {
                if( camera()->OrthoType() == NECamera::Front )
                    rotationMatrix.rotate( -90, 1, 0, 0 );
                else if( camera()->OrthoType() == NECamera::Left )
                    rotationMatrix.rotate( 90, 0, 0, 1 );
            }

            worldStack().pushMatrix( rotationMatrix );
            setModelViewMatrix();
            if( drawGrid() )
            {
                GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
                if( overlayGrid() )
                    NERENDERMAN.GLManager()->setDepthTest( kDisableDepthTest );

                drawFloor();
                GL->glPopAttrib();
            }
            worldStack().popMatrix();

            renderSpatials(NESpatial::kRigid);

            GL->glEndQuery(GL_SAMPLES_PASSED);

            // ---------------------------------------------------------------------
            // 2.2. Blend the current layer
            // ---------------------------------------------------------------------

            GL->glBindFramebuffer(GL_FRAMEBUFFER, m_OITBlenderFBO);
            GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);

            GL->glDisable(GL_DEPTH_TEST);
            GL->glEnable(GL_BLEND);

            // UNDER operator
            GL->glBlendEquation(GL_FUNC_ADD);
            GL->glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

            NERENDERMAN.GLManager()->bindDepthPeelingBlendShader(this);
            GL->glUniform1i(GL->glGetUniformLocation( shaderProgram()->programId(),"TempTex"), 0);
            GL->glActiveTexture(GL_TEXTURE0);
            GL->glBindTexture(GL_TEXTURE_RECTANGLE, m_depthPeelingColorTextures[currId]);
            NERENDERMAN.GLManager()->drawQuad();
            GL->glBindTexture(GL_TEXTURE_RECTANGLE, 0);

            GL->glDisable(GL_BLEND);

            GLuint sample_count;
            GL->glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &sample_count);
            if (sample_count == 0)
            {
                break;
            }
        }
        GL->glDeleteQueries(1, &queryId);


        bindSceneFrameBuffer();
        GL->glDisable(GL_DEPTH_TEST);

        NERENDERMAN.GLManager()->bindDepthPeelingFinalShader(this);
        GL->glUniform1i(GL->glGetUniformLocation( shaderProgram()->programId(),"ColorTex"), 0);
        GL->glUniform1i(GL->glGetUniformLocation( shaderProgram()->programId(),"bgTex"), 1);

        GL->glActiveTexture(GL_TEXTURE0);
        GL->glBindTexture(GL_TEXTURE_2D, m_OITBlenderColorTexture);
        GL->glActiveTexture(GL_TEXTURE1);
        GL->glBindTexture(GL_TEXTURE_2D, m_OITBackgroundTexture);

        NERENDERMAN.GLManager()->drawQuad();
    }
    if (transparencyType() == LinkedListTransparency )
    {
        // Blend linked-list OIT
        bindSceneFrameBuffer();
        //glDepthMask(GL_FALSE);
        NERENDERMAN.GLManager()->bindLinkedListFinalShader(this);
        renderLinkedListOIT();
    }

    GL->glPopAttrib();
}

void IGLSurface::endTransparency()
{
    if(transparencyType() == LinkedListTransparency)
    {
        GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    m_TransparencyStage = NoTransparencing;
}

void IGLSurface::renderSpatials(int renderStage)
{
    QMap<NEObject*,NEObject*>& selectedObjects = *(NESCENE.selectedObjects());

    QMultiMap<int, NESpatial*> sortedMap;

    // Sort all spatials according to their render priority
    for( NESpatial* object : NESCENE.spatials( ))
    {
        if( object )
            sortedMap.insert( object->RenderPriority(), object );
    }

    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );

    for( NESpatial* object : sortedMap )
    {
        if( object->needsGLTransform() )
        {
            worldStack().loadMatrix(object->transform());
            NERENDERMAN.GLManager()->normalMatrix( worldStack().top().normalMatrix() );
        }
        // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
        else
            worldStack().loadMatrix(matrix44f());

        // Update the world matrix in GL manager
        NERENDERMAN.GLManager()->worldMatrix( worldStack().top() );

        // Set the model-view matrix in OpenGL side
        setModelViewMatrix();
        NERENDERMAN.GLManager()->modelViewMatrix( m_MVMatrix );

        // The color of the object is supplied in case per vertex coloring or texture is not specified

        GL->glColor4f( object->Color().redF(), object->Color().greenF(), object->Color().blueF(), object->Color().alphaF() );
        GL->glUseProgram( 0 );
        if (object->spatialType() != renderStage)
            continue;

        NERENDERMAN.GLManager()->renderSpatialOnlyDepth(object, this);

    }

    GL->glPopAttrib();


    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Render all spatials in the scene
    for( NESpatial* object : sortedMap )
    {
        NERENDERMAN.GLManager()->setFillMode(NEFillMode::kSolidFrontAndBack);
        // Load the object's transformation into the world matrix stack if its geometry requires OpenGL transformation
        if( object->needsGLTransform() )
        {
            worldStack().loadMatrix(object->transform());
            NERENDERMAN.GLManager()->normalMatrix( worldStack().top().normalMatrix() );
        }
        // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
        else
            worldStack().loadMatrix(matrix44f());

        // Update the world matrix in GL manager
        NERENDERMAN.GLManager()->worldMatrix( worldStack().top() );

        // Set the model-view matrix in OpenGL side
        setModelViewMatrix();
        NERENDERMAN.GLManager()->modelViewMatrix( m_MVMatrix );

        // The color of the object is supplied in case per vertex coloring or texture is not specified

        GL->glColor4f( object->Color().redF(), object->Color().greenF(), object->Color().blueF(), object->Color().alphaF() );
        GL->glUseProgram( 0 );
        if (object->spatialType() != renderStage)
            continue;

        NERENDERMAN.GLManager()->renderSpatial(object, this);
        switch (renderStage)
        {
        case NESpatial::kFluid:
            NERENDERMAN.GLManager()->renderWater(object, this);
            break;
        }

        // Render the selected object as a green wireframe overlay mesh
        if( selectedObjects.find(object) != selectedObjects.end() )
        {
            NERENDERMAN.GLManager()->setFillMode( NEFillMode::kWireframeFrontAndBack );
            NERENDERMAN.GLManager()->bindPositionShader(this);
            GL->glColor4f( 0.7f, 0.7f, 0.7f, 1.0f );
            GL->glEnable( GL_POLYGON_OFFSET_LINE );
            GL->glPolygonOffset( -0.1f, -0.1f );
            NERENDERMAN.GLManager()->updatePositionUniforms(this, true);
            matrix44f modelMat = object->transform();
            NERENDERMAN.GLManager()->setModelMatrixForPBR(this, modelMat);
            //            GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "modelMatrix"), 1, GL_FALSE, object->transform().data() );
            NERENDERMAN.GLManager()->setClipPlanesForShader(shaderProgram());
            NERENDERMAN.GLManager()->renderPicked(object, this);
            GL->glDisable( GL_POLYGON_OFFSET_LINE );
            GL->glLineWidth( 1.0f );
        }

        if( object == NESCENE.selectedObject() )
        {
            NERENDERMAN.GLManager()->setFillMode( NEFillMode::kWireframeFrontAndBack );
            NERENDERMAN.GLManager()->bindPositionShader(this);
            GL->glColor4f( 0.0, 0.0, 1.0, 1.0 );
            GL->glEnable( GL_POLYGON_OFFSET_LINE );
            GL->glPolygonOffset( -0.1f, -0.1f );
            NERENDERMAN.GLManager()->updatePositionUniforms(this, true);
            matrix44f modelMat = object->transform();
            NERENDERMAN.GLManager()->setModelMatrixForPBR(this, modelMat);
            //            GL->glUniformMatrix4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "modelMatrix"), 1, GL_FALSE, object->transform().data() );
            NERENDERMAN.GLManager()->setClipPlanesForShader(shaderProgram());
            NERENDERMAN.GLManager()->renderPicked(object, this);
            GL->glDisable( GL_POLYGON_OFFSET_LINE );
            GL->glLineWidth( 1.0f );
        }
        // Test to check scene graph rendering
        //if(object->children().size())
        //    renderSceneContentsRecursive(object);
    }

    //this->drawBackgroundGradient();

    GL->glPopAttrib();
}

void IGLSurface::setHBAOParameter(float intensity, float radius, float blurSharpness)
{
    m_hbaoIntensity = intensity;
    m_hbaoRadius = radius;
    m_hbaoBlurSharpness = blurSharpness;
}

void IGLSurface::renderSceneToLight(NELight* light)
{
    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );

    if(light->needShadowUpdate()){
        light->updateShadowTexture();
        light->setNeedShadowUpdate(false);
    }

    light->bindShadowFramebuffer();

    GL->glDisable(GL_CULL_FACE);
    GL->glViewport(0, 0, light->getShadowMapSize(), light->getShadowMapSize());
    GL->glClearDepth(1.0);
    GL->glClear(GL_DEPTH_BUFFER_BIT);

    GL->glEnable(GL_POLYGON_OFFSET_FILL);
    GL->glPolygonOffset(4.0f, 32.f);

    GL->glEnable(GL_DEPTH_TEST);
    GL->glDepthFunc(GL_LESS);

    GL->glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    //Calculate cube that include all objects
    AABB sceneAABB = NESCENE.getSceneAABBForShadow();
    vec3f minPos = sceneAABB.minPos();
    vec3f maxPos = sceneAABB.maxPos();

    vec3f tmp((maxPos - minPos).length(), 0, (maxPos - minPos).length());
    sceneAABB.insertPoint(tmp);

    tmp = vec3f(-(maxPos - minPos).length(), 0, -(maxPos - minPos).length());
    sceneAABB.insertPoint(tmp);

    minPos = sceneAABB.minPos();
    maxPos = sceneAABB.maxPos();

    vec3f centerPos = (minPos + maxPos)*0.5;
    vec3f deltaVec = maxPos-minPos;


    // Compute the MVP matrix from the light's point of view
    matrix44f depthVP;


    vec3f lightPosition(0,0,0);
    vec3f lightInvDir(0,0,0);

    float dis = deltaVec.length()/2.f;
    float fNear = 0.0f;
    float fFar = 0.0f;
    if(light->factory().nodeName() == "Directional Light")
    {
        NEDirectionalLight* dirlight = (NEDirectionalLight*)light;
        lightInvDir = dirlight->Direction();
        lightInvDir.normalize();
        lightPosition = centerPos-lightInvDir*dis*1.5;

        vec3f up;

        vec3f tmp;
        if(abs(lightInvDir.x()) > 0.0001 || abs(lightInvDir.y()) > 0.0001 )
            tmp = vec3f(-lightInvDir.y(), lightInvDir.x(), 0);
        else
            tmp = vec3f(1, 0, 0);

        up = vec3f::crossProduct(lightInvDir, tmp);
        up.normalize();

        float maxDis = dis;
#if 1
        vec3f right = vec3f::crossProduct(lightInvDir, up);
        right.normalize();
        maxDis = 0.f;
        for(int i = 0;i < 2; i++)
        {
            vec3f p;
            if(i==0)
                p.x() = minPos.x();
            else
                p.x() = maxPos.x();
            for(int j = 0; j<2; j++)
            {
                if(j==0)
                    p.y() = minPos.y();
                else
                    p.y() = maxPos.y();
                for(int k = 0; k<2; k++)
                {
                    if(k==0)
                        p.z() = minPos.z();
                    else
                        p.z() = maxPos.z();

                    vec3f deltaV = p-centerPos;
                    if(maxDis < abs(vec3f::dotProduct(deltaV,up)))
                        maxDis = abs(vec3f::dotProduct(deltaV,up));
                    if(maxDis < abs(vec3f::dotProduct(deltaV,right)))
                        maxDis = abs(vec3f::dotProduct(deltaV,right));

                }
            }
        }
#endif

        matrix44f viewMatrix;
        viewMatrix.lookAt(lightPosition, centerPos, up);
        dirlight->setViewMatrix(viewMatrix);

        depthVP.ortho(-maxDis,maxDis,-maxDis,maxDis,0,dis*3);
        depthVP.lookAt(lightPosition, centerPos, up);

        fNear = 10;
        fFar = dis*3;
        light->setNear(fNear);
        light->setFar(fFar);

        vec2f lightRadiusUV;
        lightRadiusUV.x() = (float)dirlight->LightSize()/(maxDis*2.f);
        lightRadiusUV.y() = (float)dirlight->LightSize()/(maxDis*2.f);
        light->setLightRadiusUV(lightRadiusUV);

    }
    else if(light->factory().nodeName() == "Spot Light")
    {
        NESpotLight* spotlight = (NESpotLight*)light;
        lightPosition = spotlight->Position();
        vec3f lookPos = spotlight->LookAt();
        float cutOff = spotlight->Cutoff();

        lightInvDir = lookPos - lightPosition;
        vec3f objInvDir = centerPos - lightPosition;

        vec3f nLightInvDir = lightInvDir.normalized();
        vec3f nObjInvDir = objInvDir.normalized();

        vec3f up;
        if(vec3f::dotProduct(nLightInvDir, nObjInvDir) > 0.9999){
            vec3f tmp;
            if(abs(nLightInvDir.x()) > 0.0001 || abs(nLightInvDir.y()) > 0.0001 )
                tmp = vec3f(-nLightInvDir.y(), nLightInvDir.x(), 0);
            else
                tmp = vec3f(1, 0, 0);

            up = vec3f::crossProduct(nObjInvDir, tmp);
        }
        else{
            up = vec3f::crossProduct(nLightInvDir, nObjInvDir);
        }
        up.normalize();

#if 1
        vec3f right = vec3f::crossProduct(nObjInvDir, up);
        right.normalize();
        float minDot = 1.0f;
        for(int i = 0;i < 2; i++)
        {
            vec3f p;
            if(i==0)
                p.x() = minPos.x();
            else
                p.x() = maxPos.x();
            for(int j = 0; j<2; j++)
            {
                if(j==0)
                    p.y() = minPos.y();
                else
                    p.y() = maxPos.y();
                for(int k = 0; k<2; k++)
                {
                    if(k==0)
                        p.z() = minPos.z();
                    else
                        p.z() = maxPos.z();

                    vec3f deltaV = (p - lightPosition).normalized();
                    float tmp = vec3f::dotProduct(deltaV, nObjInvDir);

                    if(minDot > tmp)
                        minDot = tmp;
                }
            }
        }
#endif

        fNear = objInvDir.length() - dis > 1.0 ? objInvDir.length() - dis : 1.0;
        fFar = dis+objInvDir.length();

        float objCutOff = acosf(minDot) * 360.f / M_PI;
        if(objCutOff > 175)
            objCutOff = 175;

        float finalCutOff = objCutOff > cutOff ? cutOff : objCutOff;
        vec3f finalLookPos = objCutOff > cutOff ? lookPos : centerPos;

        matrix44f viewMatrix;
        viewMatrix.lookAt(lightPosition, finalLookPos, up);
        spotlight->setViewMatrix(viewMatrix);

        depthVP.perspective(finalCutOff,1.0f,fNear,fFar);
        depthVP.lookAt(lightPosition,finalLookPos,up);


        light->setNear(fNear);
        light->setFar(fFar);

        float radius = 2.f * fFar * tanf(finalCutOff * M_PI / 360.f );

        vec2f lightRadiusUV;
        lightRadiusUV.x() = (float)spotlight->LightSize()/(dis * 2);
        lightRadiusUV.y() = (float)spotlight->LightSize()/(dis * 2);
        light->setLightRadiusUV(lightRadiusUV);
    }

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform~
    NERENDERMAN.GLManager()->bindSimpleDepthShader(this);
    GLuint depthMatrixID = GL->glGetUniformLocation(shaderProgram()->programId(),"lightSpaceMatrix");
    GL->glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, depthVP.data());

    matrix44f depthModelMatrix;

    QMultiMap<int, NESpatial*> sortedMap;
    // Sort all spatials according to their render priority
    for(NESpatial* object: NESCENE.spatials())
        sortedMap.insert( object->RenderPriority(), object );

    // Render all spatials in the scene
    for( NESpatial* object: sortedMap )
    {
        // Load the object's transformation into the world matrix stack if its geometry requires OpenGL transformation
        if( object->needsGLTransform() )
        {
            depthModelMatrix = object->transform();
        }

        NERENDERMAN.GLManager()->bindSimpleDepthShader(this);
        GLuint modelMatrixID = GL->glGetUniformLocation(shaderProgram()->programId(),"model");
        GL->glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, depthModelMatrix.data());

        NERENDERMAN.GLManager()->renderShadow(object, this);
    }
#if 0

    uchar *ubuffer = new uchar[light->getShadowMapSize() * light->getShadowMapSize()];
    GL->glReadPixels(0, 0, light->getShadowMapSize(), light->getShadowMapSize(), GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, ubuffer);
    QImage img(ubuffer, light->getShadowMapSize(), light->getShadowMapSize(), QImage::Format_Grayscale8);
    img.save("ShadowTexture.png");
#endif


    matrix44f biasMatrix(
                0.5, 0.0, 0.0, 0.5,
                0.0, 0.5, 0.0, 0.5,
                0.0, 0.0, 0.5, 0.5,
                0.0, 0.0, 0.0, 1.0
                );

    light->setDepthBiasMVP(biasMatrix*depthVP);

    bindScreenFrameBuffer();

    GL->glPopAttrib();
}

void IGLSurface::prepareHbaoData(int width, int height)
{
    // projection

    matrix44f proj = camera()->projectionMatrix();
    vec4f projInfoPerspective(
                2.0f / proj(0, 0),       // (x) * (R - L)/N
                2.0f / proj(1, 1),       // (y) * (T - B)/N
                -( 1.0f - proj(2, 0)) / proj(0, 0), // L/N
                -( 1.0f + proj(2, 1)) / proj(1, 1) // B/N
                );

    vec4f projInfoOrtho(
                2.0f / proj(0, 0),      // ((x) * R - L)
                2.0f / proj(1, 1),      // ((y) * T - B)
                -( 1.0f + proj(3, 0)) / proj(0, 0), // L
                -( 1.0f - proj(3, 1)) / proj(1, 1) // B
                );

    int useOrtho = camera()->fixedOrthographic() ? 1 : 0;
    if(camera()->customOrthographic())
    {
        useOrtho = 1;
    }
    m_hbaoUbo.projOrtho = useOrtho;
    m_hbaoUbo.projInfo  = useOrtho ? projInfoOrtho : projInfoPerspective;

    float projScale;
    if (useOrtho){
        projScale = float(height) / (projInfoOrtho.y());
    }
    else {
        projScale = float(height) / (tanf( camera()->Fov() * 0.5f) * 2.0f);
    }

    // radius
    float meters2viewspace = 1.0f;
    float R = m_hbaoRadius * meters2viewspace;
    m_hbaoUbo.R2 = R * R;
    m_hbaoUbo.NegInvR2 = -1.0f / m_hbaoUbo.R2;
    m_hbaoUbo.RadiusToScreen = R * 0.5f * projScale;

    // ao
    m_hbaoUbo.PowExponent = std::max(m_hbaoIntensity,0.0f);
    m_hbaoUbo.NDotVBias = std::min(std::max(0.0f, m_hbaoBias),0.9f);
    m_hbaoUbo.AOMultiplier = 1.0f / (1.0f - m_hbaoUbo.NDotVBias);

    // resolution
    int quarterWidth  = ((width+3)/4);
    int quarterHeight = ((height+3)/4);

    m_hbaoUbo.InvQuarterResolution = vec2f(1.0f/float(quarterWidth),1.0f/float(quarterHeight));
    m_hbaoUbo.InvFullResolution = vec2f(1.0f/float(width),1.0f/float(height));

#if USE_AO_LAYERED_SINGLEPASS
    for (int i = 0; i < HBAO_RANDOM_ELEMENTS; i++){
        m_hbaoUbo.float2Offsets[i] = vec2f(float(i % 4) + 0.5f, float(i / 4) + 0.5f);
        m_hbaoUbo.jitters[i] = m_hbaoRandom[i];
    }
#endif
}

void IGLSurface::drawLinearDepth(int width, int height, int sampleIdx)
{
    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_hbaoFbos.depthlinear);

    bool ortho = camera()->fixedOrthographic();
    if( camera()->customOrthographic() )
        ortho = true;
    float nearplane = camera()->NearClip();
    float farplane = camera()->FarClip();

    if (NERENDERMAN.GLManager()->OpenGLVersion() >= 430) {
        if (m_msaaSamples > 1) {
            setShaderProgram(NERENDERMAN.GLManager()->getAODepthLinearizeMSAAShader()->mShader);
            GL->glUniform4f(0, nearplane * farplane, nearplane - farplane, farplane, ortho ? 0.0f : 1.0f);
            GL->glUniform1i(1, sampleIdx);

            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, m_tmpDepthStencilTexture);
            NERENDERMAN.GLManager()->drawQuad();
            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        else if (aaType() == Csaa) {
            setShaderProgram(NERENDERMAN.GLManager()->getAODepthLinearizeMSAAShader()->mShader);
            GL->glUniform4f(0, nearplane * farplane, nearplane - farplane, farplane, ortho ? 0.0f : 1.0f);
            GL->glUniform1i(1, sampleIdx);

            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, m_tmpDepthStencilTexture);
            NERENDERMAN.GLManager()->drawQuad();
            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        else {
            setShaderProgram(NERENDERMAN.GLManager()->getAODepthLinearizeShader()->mShader);
            GL->glUniform4f(0, nearplane * farplane, nearplane - farplane, farplane, ortho ? 0.0f : 1.0f);

            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_tmpDepthStencilTexture);
            NERENDERMAN.GLManager()->drawQuad();
            GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, 0);
        }
    }

}

void IGLSurface::drawHbaoBlur(int width, int height, int sampleIdx)
{
    float meters2viewspace = 1.0f;

    setShaderProgram( NERENDERMAN.GLManager()->getHBAOBlurShader()->mShader );
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, m_hbaoTextures.scene_depthlinear);

    shaderProgram()->setUniformValue("g_Sharpness", m_hbaoBlurSharpness / meters2viewspace);

    GL->glDrawBuffer(GL_COLOR_ATTACHMENT1);

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_hbaoTextures.hbao_result);
    shaderProgram()->setUniformValue("texSource", 0);
    //glUniform2f(1,1.0f/float(width),0);
    shaderProgram()->setUniformValue("g_InvResolutionDirection", 1.0f / float(width), 0);
    NERENDERMAN.GLManager()->drawQuad();

    // final output to main fbo
    GL->glBindFramebuffer(GL_FRAMEBUFFER, m_AOResultFBO);
    GL->glDisable(GL_DEPTH_TEST);
    GL->glEnable(GL_BLEND);
    GL->glBlendFunc(GL_ONE,GL_ZERO);
    if (m_msaaSamples > 1 || aaType() == Csaa){
        GL->glEnable(GL_SAMPLE_MASK);
        GL->glSampleMaski(0, 1<<sampleIdx);
    }

#if USE_AO_SPECIALBLUR
    setShaderProgram( NERENDERMAN.GLManager()->getHBAOBlur2Shader()->mShader );
    //glUniform1f(0,m_hbaoBlurSharpness/meters2viewspace);
    shaderProgram()->setUniformValue("g_Sharpness", m_hbaoBlurSharpness / meters2viewspace);
#endif

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_hbaoTextures.hbao_blur);
    shaderProgram()->setUniformValue("texSource", 0);
    //glUniform2f(1,0,1.0f/float(height));
    shaderProgram()->setUniformValue("g_InvResolutionDirection", 0, 1.0f / float(height));
    NERENDERMAN.GLManager()->drawQuad();
}

void IGLSurface::drawHbaoClassic(int width, int height, int sampleIdx)
{
    prepareHbaoData(width,height);

    drawLinearDepth(width, height, sampleIdx);

    // ssaocalc
    if (m_hbaoBlur)
    {
        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_hbaoFbos.hbao_calc);
        GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    else
    {
        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_AOResultFBO);
        GL->glDisable(GL_DEPTH_TEST);
        GL->glEnable(GL_BLEND);
        GL->glBlendFunc(GL_ONE,GL_ZERO);
        if (m_msaaSamples > 1 || aaType() == Csaa){
            GL->glEnable(GL_SAMPLE_MASK);
            GL->glSampleMaski(0, 1<<sampleIdx);
        }
    }

    //ShaderMetaInfo* hbao_calc_shader = USE_AO_SPECIALBLUR && m_hbaoBlur ? NERENDERMAN.GLManager()->getAOCalcBlurShader() : NERENDERMAN.GLManager()->getAOCalcShader();
    ShaderMetaInfo* hbao_calc_shader = NERENDERMAN.GLManager()->getAOCalcShader();
    setShaderProgram( hbao_calc_shader->mShader );

    GL->glBindBuffer(GL_UNIFORM_BUFFER, m_hbaoBuffers.hbao_ubo);
    GL->glBufferData(GL_UNIFORM_BUFFER, sizeof(HBAOData),&m_hbaoUbo, GL_STATIC_DRAW);
    GL->glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_hbaoBuffers.hbao_ubo);

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_hbaoTextures.scene_depthlinear);
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, m_hbaoTextures.hbao_randomview[sampleIdx]);
    NERENDERMAN.GLManager()->drawQuad();

    // blur
    if (m_hbaoBlur){
        drawHbaoBlur(width,height,sampleIdx);
    }

    GL->glEnable(GL_DEPTH_TEST);
    GL->glDisable(GL_BLEND);
    GL->glDisable(GL_SAMPLE_MASK);
    GL->glSampleMaski(0, ~0);

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, 0);
    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, 0);

    GL->glUseProgram(0);
}

void IGLSurface::drawAO(int width, int height)
{
    NEGLManager::AOParams params = {width, height, 1,
                                     m_pProjectionStack->top().data(),
                                     m_hbaoRadius,
                                     m_hbaoBias,
                                     m_hbaoIntensity,
                                     m_hbaoBlur,
                                     m_hbaoBlurSharpness};

    int nSamples = 1;

    if(aaType() == Csaa)
        nSamples = m_csaaColorSamples;
    else
        nSamples = m_msaaSamples;

    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    if(type() == Hbao) { /// HBAO
        for( int sample = 0; sample < nSamples; sample++)
            drawHbaoClassic(width, height, sample);
    }
    else { /// HBAO+
        GL->glDisable(GL_DEPTH_TEST);
        GL->glDisable(GL_STENCIL_TEST);
        GL->glDisable(GL_CULL_FACE);
        GL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        params.samples = nSamples;
        params.radius *= 100; // Metric is different with HBAO and HBAO+
#if USE_EXT_GFSDK_SSAO
        NERENDERMAN.GLManager()->drawAO(m_AOResultFBO, m_tmpDepthStencilTexture, params);
#endif
    }
    GL->glPopAttrib();

    /// rendering AO result texture with depth and transparency as if it is located in 3D
    if (isDepthPeeling()) //Depth Peeling OIT
        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_OITBlenderFBO);
    else
        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);

    NERENDERMAN.GLManager()->bindAOResultShader(this);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);

    GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "aoMap"), 1);

    if (aaType() == Msaa && m_msaaSamples > 1){
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "colorMapMS"), 0);
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "depthMapMS"), 2);

        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "isMultiSample"), true);

        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, m_tmpColorTexture);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D_MULTISAMPLE, m_tmpDepthStencilTexture);

    }
    else if(aaType() == Csaa) {
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "colorMapMS"), 0);
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "depthMapMS"), 2);

        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "isMultiSample"), true);

        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, m_tmpColorTexture);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D_MULTISAMPLE, m_tmpDepthStencilTexture);

    }
    else{
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "colorMap"), 0);
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "depthMap"), 2);

        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "isMultiSample"), false);

        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_tmpColorTexture);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, m_tmpDepthStencilTexture);

    }

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, m_AOResultTexture);

    NERENDERMAN.GLManager()->drawQuad();

    if (aaType() == Msaa && m_msaaSamples > 1){
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, 0);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D_MULTISAMPLE, 0);

    }
    else if(aaType() == Csaa) {
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D_MULTISAMPLE, 0);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D_MULTISAMPLE, 0);

    }
    else{
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, 0);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, 0);

    }

    GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, 0);
}

void IGLSurface::drawPostProcessing(int width, int height)
{
    if (aaType() == Fxaa && checkAAQualityProfile() && !isSceneDynamic())
    {
        NERENDERMAN.GLManager()->bindFXAAShader(this);
        GL->glUniform1f( GL->glGetUniformLocation( shaderProgram()->programId(), "FXAA_REDUCE_MUL" ), 0.0 );
        GL->glUniform1f( GL->glGetUniformLocation( shaderProgram()->programId(), "FXAA_SUBPIX_SHIFT" ), 0.0 );

        GL->glUniform1i( GL->glGetUniformLocation( shaderProgram()->programId(), "uSourceTex" ), 0 );
        GL->glUniform1f( GL->glGetUniformLocation( shaderProgram()->programId(), "rt_w" ), (float)width );
        GL->glUniform1f( GL->glGetUniformLocation( shaderProgram()->programId(), "rt_h" ), (float)height );

        GL->glActiveTexture(GL_TEXTURE0);
        GL->glBindTexture(GL_TEXTURE_2D, m_frameAndPickColorTexture);
        NERENDERMAN.GLManager()->drawQuad();
        GL->glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if (aaType() == Smaa && checkAAQualityProfile() && !isSceneDynamic())
    {
        bool bDepthTest = GL->glIsEnabled(GL_DEPTH_TEST);
        GLboolean bDepthMask = false;
        GL->glGetBooleanv(GL_DEPTH_WRITEMASK, &bDepthMask);
        GLboolean  bBlend = GL->glIsEnabled(GL_BLEND);
        GLboolean  bAlphaTest = GL->glIsEnabled(GL_ALPHA_TEST);
        GLboolean  bStencilTest = GL->glIsEnabled(GL_STENCIL_TEST);
        GLint nStencilFunc = GL_ALWAYS;
        GL->glGetIntegerv(GL_STENCIL_FUNC, &nStencilFunc);
        GLint nStencilRef = 0;
        GL->glGetIntegerv(GL_STENCIL_REF, &nStencilRef);
        GLint nStencilMask = 0xff;
        GL->glGetIntegerv(GL_STENCIL_VALUE_MASK, &nStencilMask);
        GLint arrStencilOps[3] = {GL_KEEP,};
        GL->glGetIntegerv(GL_STENCIL_FAIL, &arrStencilOps[0]);
        GL->glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &arrStencilOps[1]);
        GL->glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &arrStencilOps[2]);

#if 0 // for debug
        std::cout<<"GL_STENCIL_TEST : "<< (int)bStencilTest << std::endl;
        std::cout<<"GL_STENCIL_FUNC : "<< nStencilFunc << std::endl;
        std::cout<<"GL_STENCIL_REF : "<<nStencilRef<<std::endl;
        std::cout<<"GL_STENCIL_VALUE_MASK : "<<nStencilMask<<std::endl;
        std::cout<<"GL_STENCIL_FAIL : "<<arrStencilOps[0]<<std::endl;
        std::cout<<"GL_STENCIL_PASS_DEPTH_FAIL : "<<arrStencilOps[1]<<std::endl;
        std::cout<<"GL_STENCIL_PASS_DEPTH_PASS : "<<arrStencilOps[2]<<std::endl;
#endif

        GL->glDisable(GL_DEPTH_TEST);
        GL->glDepthMask(GL_FALSE);
        GL->glDisable(GL_BLEND);
        GL->glDisable(GL_ALPHA_TEST);
        GL->glEnable(GL_STENCIL_TEST);
        GL->glClearStencil(0x0);

        /** SMAA edge detection **/
        GL->glStencilFunc(GL_ALWAYS, 0x1, 0x1);
        GL->glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_smaaEdgeFBO);
        GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        NERENDERMAN.GLManager()->bindSmaaEdgeShader(this);
        GLfloat SMAA_RT_METRICS[4] = {1.f/GLfloat(width), 1.f/GLfloat(height), GLfloat(width), GLfloat(height)};
        GL->glUniform4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "SMAA_RT_METRICS" ), 1, SMAA_RT_METRICS );
        GL->glUniform1i( GL->glGetUniformLocation( shaderProgram()->programId(), "colorTex" ), 0 );


        GL->glActiveTexture(GL_TEXTURE0);
        GL->glEnable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, m_frameAndPickColorTexture);

        NERENDERMAN.GLManager()->drawRect();

        GL->glDisable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, 0);

        /** SMAA blend weight calculation **/
        GL->glStencilFunc(GL_EQUAL, 0x1, 0x1);
        GL->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_smaaBlendFBO);

        NERENDERMAN.GLManager()->bindSmaaBlendShader(this);
        GL->glUniform4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "SMAA_RT_METRICS" ), 1, SMAA_RT_METRICS );
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "edgesTex"), 0);
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "areaTex"), 1);
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "searchTex"), 2);

        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, m_smaaEdgeTexture);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, m_smaaAreaTexture);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, m_smaaSearchTexture);

        NERENDERMAN.GLManager()->drawRect();

        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE0, GL_TEXTURE_2D, 0);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE1, GL_TEXTURE_2D, 0);
        GLEXTDSA->glBindMultiTextureEXT(GL_TEXTURE2, GL_TEXTURE_2D, 0);

#if 0 // for debug
        GL->glDisable(GL_STENCIL_TEST);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
        NERENDERMAN.GLManager()->bindRectTextureShader();
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "colorMap"), 0);
        GL->glActiveTexture(GL_TEXTURE0);
        GL->glEnable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, m_smaaBlendTexture);
        NERENDERMAN.GLManager()->drawQuad();

        GL->glActiveTexture(GL_TEXTURE2);
        GL->glDisable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, 0);
#endif

        /** SMAA neighborhood blending **/
        GL->glDisable(GL_STENCIL_TEST);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer());
        NERENDERMAN.GLManager()->bindSmaaNeighborShader(this);
        GL->glUniform4fv( GL->glGetUniformLocation( shaderProgram()->programId(), "SMAA_RT_METRICS" ), 1, SMAA_RT_METRICS );
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "colorTex"), 0);
        GL->glUniform1i(GL->glGetUniformLocation(shaderProgram()->programId(), "blendTex"), 1);

        GL->glActiveTexture(GL_TEXTURE0);
        GL->glEnable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, m_frameAndPickColorTexture);
        GL->glActiveTexture(GL_TEXTURE1);
        GL->glEnable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, m_smaaBlendTexture);

        NERENDERMAN.GLManager()->drawRect();

        GL->glActiveTexture(GL_TEXTURE1);
        GL->glDisable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, 0);
        GL->glActiveTexture(GL_TEXTURE0);
        GL->glDisable(GL_TEXTURE_2D);
        GL->glBindTexture(GL_TEXTURE_2D, 0);

        // restore state
        if(bDepthTest)
            GL->glEnable(GL_DEPTH_TEST);
        GL->glDepthMask(bDepthMask);
        if(bBlend)
            GL->glEnable(GL_BLEND);
        if(bAlphaTest)
            GL->glEnable(GL_ALPHA_TEST);
        if(bStencilTest)
            GL->glEnable(GL_STENCIL_TEST);
        GL->glStencilFunc(nStencilFunc, nStencilRef, nStencilMask);
        GL->glStencilOp(arrStencilOps[0], arrStencilOps[1], arrStencilOps[2]);
    }
    else
    {
#if 1
        GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_sceneFBO);
        GL->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebuffer());
        GL->glBlitFramebuffer(0,0,width,height,
                          0,0,width,height,GL_COLOR_BUFFER_BIT, GL_NEAREST);
        GL->glBlitFramebuffer(0,0,width,height,
                          0,0,width,height,GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        GL->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer());
        GL->glFinish();

#else
        GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
        GL->glDisable( GL_DEPTH_TEST );

        NERENDERMAN.GLManager()->bindRectTextureShader(this);
        GL->glUniform1i( GL->glGetUniformLocation( shaderProgram()->programId(), "colorMap" ), 0 );
        GL->glActiveTexture(GL_TEXTURE0);
        GL->glBindTexture(GL_TEXTURE_2D, m_frameAndPickColorTexture);
        NERENDERMAN.GLManager()->drawQuad();
        GL->glBindTexture(GL_TEXTURE_2D, 0);

        GL->glPopAttrib();
#endif
    }
}

void IGLSurface::beginWaterRendering(int width, int height)
{
    // Blit main render target to fluid scene so that we can use it as water refraction background
    GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_sceneFBO);
    GL->glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fluidRenderer.mSceneFbo);
    GL->glBlitFramebuffer(0,0,width,height,
                      0,0,width,height,GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Get prepare for water related buffers
    GL->glBindFramebuffer( GL_FRAMEBUFFER, m_fluidRenderer.mThicknessFbo );
    GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fluidRenderer.mThicknessTex, 0);
    GL->glDrawBuffer(GL_COLOR_ATTACHMENT0);
    GL->glClearColor(0, 0, 0, 0);
    GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glVerify(GL->glBindFramebuffer( GL_FRAMEBUFFER, m_fluidRenderer.mDepthFbo ));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fluidRenderer.mDepthTex, 0));
    glVerify(GL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_fluidRenderer.mColorTex, 0));
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    GL->glDrawBuffers(2, buffers);
    GL->glClearColor(0, 0, 0, 0);
    GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void IGLSurface::endWaterRendering()
{
}

#define DIVISION_LENGTH 0.01
void IGLSurface::drawBoundingBox(float width, float height)
{
    QMultiMap<int, NESpatial*> sortedMap;

    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Sort all spatials according to their render priority
    for( NESpatial* object : NESCENE.spatials( ))
        sortedMap.insert( object->RenderPriority(), object );

    // Render all BoundingBoxs in the scene
    for( NESpatial* object : sortedMap )
    {
        if(!object->ShowBoundingBox())
            continue;
        if( (object->BoundingBoxType() == NESpatial::OrientedBoundingBox || object->BoundingBoxType() == NESpatial::AxisAlignedBoundingBox))
        {
            NERENDERMAN.GLManager()->setFillMode(NEFillMode::kSolidFrontAndBack);
            // Load the object's transformation into the world matrix stack if its geometry requires OpenGL transformation
            if( object->BoundingBoxType() == NESpatial::OrientedBoundingBox && object->needsGLTransform() )
            {
                worldStack().loadMatrix(object->transform());
                NERENDERMAN.GLManager()->normalMatrix( worldStack().top().normalMatrix() );
            }
            // Do not apply any world transformation if the geometry is bound to the simulation data (does not require OpenGL transformation)
            else
                worldStack().loadMatrix(matrix44f());

            // Update the world matrix in GL manager
            NERENDERMAN.GLManager()->worldMatrix( worldStack().top() );

            // Set the model-view matrix in OpenGL side
            setModelViewMatrix();
            NERENDERMAN.GLManager()->modelViewMatrix( m_MVMatrix );

            /*

                                       1 ___________ 2                  ____1______
                                        /|         /|                  /|         /|
                  y ^                  / |        / |                 / |        / |
                    |                 /  |       /  |               9/  |     10/  |
                    |                /___|______/   |               /__4|_2____/   |7
                    |             5 |    |      |6  |              |    |      |   |
                    |               |  0 |_ _ _ |_ _| 3            |    |_ _0_ |_ _|
                    |               |   /       |   /              |   /       |   /
                    |__________>    |  /        |  /              5| 8/       6|  /11
                   /           x    | /         | /                | /         | /
                  /                 |/__________|/                 |/_____3____|/
                 /                4              7
                / z


            */
            //boundbox vertexs
            int nVertiecs = 8;
            int nEdges = 12;
            vec3f vertices[8];
            vec3f minP = object->aabb().minPos();
            vec3f maxP = object->aabb().maxPos();
            vertices[0] = minP;
            vertices[6] = maxP;
            vertices[1] = vec3f(minP.x(),maxP.y(),minP.z());
            vertices[2] = vec3f(maxP.x(),maxP.y(),minP.z());
            vertices[3] = vec3f(maxP.x(),minP.y(),minP.z());
            vertices[4] = vec3f(minP.x(),minP.y(),maxP.z());
            vertices[5] = vec3f(minP.x(),maxP.y(),maxP.z());
            vertices[7] = vec3f(maxP.x(),minP.y(),maxP.z());

            if (object->BoundingBoxType() == NESpatial::AxisAlignedBoundingBox)
            {
                AABB worldAABB;
                for (int i = 0; i < 8; i++)
                    worldAABB.insertPoint(object->transform() * vertices[i]);

                minP = worldAABB.minPos();
                maxP = worldAABB.maxPos();

                vertices[0] = minP;
                vertices[6] = maxP;
                vertices[1] = vec3f(minP.x(), maxP.y(), minP.z());
                vertices[2] = vec3f(maxP.x(), maxP.y(), minP.z());
                vertices[3] = vec3f(maxP.x(), minP.y(), minP.z());
                vertices[4] = vec3f(minP.x(), minP.y(), maxP.z());
                vertices[5] = vec3f(minP.x(), maxP.y(), maxP.z());
                vertices[7] = vec3f(maxP.x(), minP.y(), maxP.z());
            }

//            for (int i = 0; i < nVertiecs; i++)
//            {
//                std::cout << "NEGLWidget::drawBoundingBox vertices[" << i << "] x=" << vertices[i].x() << ", y=" << vertices[i].y() << ", z=" << vertices[i].z() << std::endl;
//            }

            int EdgeIndex[12][6] = {
                {0, 3, 0, 4, 1, 0}, {1, 2, 1, 0, 5, 1}, {5, 6, 5, 1, 4, 5}, {4, 7, 4, 5, 0, 4},
                {0, 1, 0, 3, 4, 0}, {4, 5, 4, 0, 7, 4}, {7, 6, 7, 4, 3, 7}, {3, 2, 3, 7, 0, 3},
                {0, 4, 3, 0, 0, 1}, {1, 5, 0, 1, 1, 2}, {2, 6, 1, 2, 2, 3}, {3, 7, 2, 3, 3, 0} };
            int EdgeDirect[12][8] = {  // 0, 1 - edge index, 2, 3 - first aide, 4, 5 - second side, 6 - first axis, 7 - second axis
                {0, 3, 4, 0, 1, 0, 2, 1}, {1, 2, 0, 1, 5, 1, 1, 2}, {5, 6, 1, 5, 4, 5, 2, 1}, {4, 7, 5, 4, 0, 4, 1, 2},
                {0, 1, 3, 0, 4, 0, 0, 2}, {4, 5, 0, 4, 7, 4, 2, 0}, {7, 6, 4, 7, 3, 7, 0, 2}, {3, 2, 7, 3, 0, 3, 2, 0},
                {0, 4, 3, 0, 1, 0, 0, 1}, {1, 5, 0, 1, 2, 1, 1, 0}, {2, 6, 1, 2, 3, 2, 0, 1}, {3, 7, 2, 3, 0, 3, 1, 0} };

            vec4f ScnPos4f[8]; vec3f ScnPos3f[9]; vec2f ScnPos2f[9];
            matrix44f ModelMatrix, ViewMatrix, ProjMatrix;
            //float       *outPtr;

            ModelMatrix = m_pWorldStack->top();
            ViewMatrix = m_pViewStack->top();
            ProjMatrix = m_pProjectionStack->top();
/*
            outPtr =  ModelMatrix.data();
            std::cout << "NEGLWidget::drawBoundingBox m_pWorldStack [1,1]=" << outPtr[0] << ", [1,2]=" << outPtr[4] << ", [1,3]=" << outPtr[8] << ", [1,4]=" << outPtr[12] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [2,1]=" << outPtr[1] << ", [2,2]=" << outPtr[5] << ", [2,3]=" << outPtr[9] << ", [2,4]=" << outPtr[13] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [3,1]=" << outPtr[2] << ", [3,2]=" << outPtr[6] << ", [3,3]=" << outPtr[10] << ", [3,4]=" << outPtr[14] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [4,1]=" << outPtr[3] << ", [4,2]=" << outPtr[7] << ", [4,3]=" << outPtr[11] << ", [4,4]=" << outPtr[15] << std::endl;
            outPtr =  ViewMatrix.data();
            std::cout << "NEGLWidget::drawBoundingBox m_pViewStack  [1,1]=" << outPtr[0] << ", [1,2]=" << outPtr[4] << ", [1,3]=" << outPtr[8] << ", [1,4]=" << outPtr[12] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [2,1]=" << outPtr[1] << ", [2,2]=" << outPtr[5] << ", [2,3]=" << outPtr[9] << ", [2,4]=" << outPtr[13] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [3,1]=" << outPtr[2] << ", [3,2]=" << outPtr[6] << ", [3,3]=" << outPtr[10] << ", [3,4]=" << outPtr[14] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [4,1]=" << outPtr[3] << ", [4,2]=" << outPtr[7] << ", [4,3]=" << outPtr[11] << ", [4,4]=" << outPtr[15] << std::endl;
            outPtr =  ProjMatrix.data();
            std::cout << "NEGLWidget::drawBoundingBox m_pProjection [1,1]=" << outPtr[0] << ", [1,2]=" << outPtr[4] << ", [1,3]=" << outPtr[8] << ", [1,4]=" << outPtr[12] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [2,1]=" << outPtr[1] << ", [2,2]=" << outPtr[5] << ", [2,3]=" << outPtr[9] << ", [2,4]=" << outPtr[13] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [3,1]=" << outPtr[2] << ", [3,2]=" << outPtr[6] << ", [3,3]=" << outPtr[10] << ", [3,4]=" << outPtr[14] << std::endl;
            std::cout << "NEGLWidget::drawBoundingBox               [4,1]=" << outPtr[3] << ", [4,2]=" << outPtr[7] << ", [4,3]=" << outPtr[11] << ", [4,4]=" << outPtr[15] << std::endl;
*/

            // get convex hull
            bool    isOutsidePoint[9];
            int     StartIndex = -1;
            float   minValueX = -1;
            float   minValueY = -1;
            float   maxValueZ = -1;
            float   minimumAngle = 360.0f;
            int     scnWidth = width;
            int     scnHeight = height;
            float   cameraNear = camera()->NearClip();
            float   cameraFar = camera()->FarClip();
            vec3f   cameraLookAt(0,0,0);
            bool    swapFlag = false;

            for (int i = 0; i < nVertiecs; i++)
            {
                ScnPos4f[i] = ProjMatrix * ViewMatrix * ModelMatrix * vec4f(vertices[i], 1.0f);
//                std::cout << "NEGLWidget::drawBoundingBox Pos4f(0)[" << i << "] x=" << vertices[i].x() << ", y=" << vertices[i].y() << ", z=" << vertices[i].z() << std::endl;
//                ScnPos4f[i] = m_pWorldStack->top() * vec4f(vertices[i], 1.0f);
//                std::cout << "NEGLWidget::drawBoundingBox Pos4f(1)[" << i << "] x=" << ScnPos4f[i].x() << ", y=" << ScnPos4f[i].y() << ", z=" << ScnPos4f[i].z() << ", w=" << ScnPos4f[i].w() << std::endl;
//                ScnPos4f[i] = m_pViewStack->top() * ScnPos4f[i];
//                std::cout << "NEGLWidget::drawBoundingBox Pos4f(2)[" << i << "] x=" << ScnPos4f[i].x() << ", y=" << ScnPos4f[i].y() << ", z=" << ScnPos4f[i].z() << ", w=" << ScnPos4f[i].w() << std::endl;
//                ScnPos4f[i] = m_pProjectionStack->top() * ScnPos4f[i];
//                std::cout << "NEGLWidget::drawBoundingBox Pos4f(3)[" << i << "] x=" << ScnPos4f[i].x() << ", y=" << ScnPos4f[i].y() << ", z=" << ScnPos4f[i].z() << ", w=" << ScnPos4f[i].w() << std::endl;

                ScnPos3f[i].setX(ScnPos4f[i].x() / fabs(ScnPos4f[i].w()));
                ScnPos3f[i].setY(ScnPos4f[i].y() / fabs(ScnPos4f[i].w()));
                ScnPos3f[i].setZ(ScnPos4f[i].z() / fabs(ScnPos4f[i].w()));
                ScnPos3f[i].setX((ScnPos3f[i].x() + 1.0f) * scnWidth / 2.0f);   // Xscn = Xndc * Width / 2 + (X0 + Width / 2)        X0 is 0
                ScnPos3f[i].setY((ScnPos3f[i].y() + 1.0f) * scnHeight / 2.0f);  // Yscn = Yndc * Height / 2 + (Y0 + Height / 2)      Y0 is 0
                ScnPos3f[i].setZ(ScnPos3f[i].z() * (cameraFar - cameraNear) / 2.0f + (cameraFar + cameraNear) / 2.0f);   // Zscn = Zndc * (Far - Near) / 2 + (Far + Near) / 2
                ScnPos2f[i].setX(ScnPos3f[i].x());
                ScnPos2f[i].setY(ScnPos3f[i].y());
                isOutsidePoint[i] = false;
//                std::cout << "NEGLWidget::drawBoundingBox Pos4f[" << i << "] x=" << ScnPos3f[i].x() << ", y=" << ScnPos3f[i].y() << ", z=" << ScnPos3f[i].z() << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox minIndex : " << StartIndex << ", minY=" << minValueY << ", minX=" << minValueX << ", check : " << i << ", curY=" << ScnPos2f[i].y() << ", minX=" << ScnPos2f[i].x() << std::endl;
                if (StartIndex == -1)
                {
                    swapFlag = true;
                } else
                {
                    if (fabs(minValueY - ScnPos2f[i].y()) < 0.001)
                    {
                        if (fabs(minValueX - ScnPos2f[i].x()) < 0.001)
                        {
                            if (maxValueZ < ScnPos3f[i].z())
                                swapFlag = true;
                        } else
                        {
                            if (minValueX > ScnPos2f[i].x())
                                swapFlag = true;
                        }
                    } else
                    {
                        if (minValueY > ScnPos2f[i].y())
                            swapFlag = true;
                    }
                }
                if (swapFlag)
                {
                    minValueX = ScnPos2f[i].x();
                    minValueY = ScnPos2f[i].y();
                    maxValueZ = ScnPos3f[i].z();
                    StartIndex = i;
                    swapFlag = false;
                }
//                std::cout << "NEGLWidget::drawBoundingBox ScnPos4f[" << i << "] x=" << ScnPos4f[i].x() << ", y=" << ScnPos4f[i].y() << ", z=" << ScnPos4f[i].z() << ", w=" << ScnPos4f[i].w() << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox ScnPos3f[" << i << "] x=" << ScnPos3f[i].x() << ", y=" << ScnPos3f[i].y() << ", z=" << ScnPos3f[i].z() << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox ScnPos2f[" << i << "] x=" << ScnPos2f[i].x() << ", y=" << ScnPos2f[i].y() << std::endl;
            }
            if (StartIndex == -1) return; // error
            isOutsidePoint[StartIndex] = true;
//            std::cout << "NEGLWidget::drawBoundingBox StartIndex=" << StartIndex << std::endl;
//            std::cout << "NEGLWidget::drawBoundingBox atan2(0.5, -0.5)=" << RADIAN_TO_DEGREE(atan2(0.5, -0.5)) << std::endl;  // 135
//            std::cout << "NEGLWidget::drawBoundingBox atan2(-0.5, 0.5)=" << RADIAN_TO_DEGREE(atan2(-0.5, 0.5)) << std::endl;   // -45
//            std::cout << "NEGLWidget::drawBoundingBox atan2(-0.5, -0.5)=" << RADIAN_TO_DEGREE(atan2(-0.5, -0.5)) << std::endl; // -135

            /*

              y^
               |
               |      P2(P1)   P1(P2)
               |       o      o
               |       |     /
               |       |    /
               |       |   /
               |       |  /
               |       | / minimum angle
               |       |/)_________
               |    P0 *
               |
               |
           ----|-----------------------> x
               |
               |

             */
            vec2f   P0, P1, P2;
            int     P0_Index, P1_Index;
            float   checkAngle;
            std::vector<int>    outsidePointIndexList;

            outsidePointIndexList.push_back(StartIndex);  // push index of first start point
            ScnPos2f[nVertiecs] = ScnPos2f[StartIndex]; ScnPos3f[nVertiecs] = ScnPos3f[StartIndex]; isOutsidePoint[nVertiecs] = false;
//            std::cout << "NEGLWidget::drawBoundingBox -0- PointList[" << StartIndex << "] x=" << ScnPos2f[StartIndex].x() << ", y=" << ScnPos2f[StartIndex].y() << std::endl;

            P0 = ScnPos2f[StartIndex];
            minimumAngle = 360.0f;
            StartIndex = -1; maxValueZ = -1;
            swapFlag = false;
            for (int i = 0; i < nVertiecs; i++)
            {
                if (isOutsidePoint[i])
                    continue;
                P1 = vec2f(ScnPos2f[i]);
                P1 -= P0;

                if (fabs(P1.x()) < 0.001)
                {
                    if (P1.y() > 0.001)
                        checkAngle = 90.0f;
                    else
                        continue;
                }
                else
                    checkAngle = RADIAN_TO_DEGREE(atan2(P1.y(), P1.x()));
//                std::cout << "NEGLWidget::drawBoundingBox minIndex : " << StartIndex << " minAngle=" << minimumAngle << ", check : " << i << ", chkAngle=" << checkAngle << std::endl;
                if (fabs(minimumAngle - checkAngle) < 0.001)
                {
                    if (fabs(ScnPos2f[StartIndex].y() - ScnPos2f[i].y()) < 0.001)
                    {
                        if (maxValueZ < ScnPos3f[i].z())
                            swapFlag = true;
                    } else
                    {
                        if (ScnPos2f[StartIndex].y() < ScnPos2f[i].y())
                            swapFlag = true;
                    }
                } else
                {
                    if ((checkAngle >= -0.001) && (minimumAngle > checkAngle))
                        swapFlag = true;
                }
                if (swapFlag)
                {
                    minimumAngle = checkAngle;
                    maxValueZ = ScnPos3f[i].z();
                    StartIndex = i;
                    swapFlag = false;
                }
            }
            if (StartIndex == -1) return; // error
            outsidePointIndexList.push_back(StartIndex);  // push index of second start point
            isOutsidePoint[StartIndex] = true;
//            std::cout << "NEGLWidget::drawBoundingBox -1- PointList[" << StartIndex << "] x=" << ScnPos2f[StartIndex].x() << ", y=" << ScnPos2f[StartIndex].y() << std::endl;

            do
            {
                minimumAngle = 360.0f;
                StartIndex = -1;
                swapFlag = false;
                float  maxLength = -1; maxValueZ = -1;
                P0_Index = outsidePointIndexList[outsidePointIndexList.size() - 2];
                P1_Index = outsidePointIndexList[outsidePointIndexList.size() - 1];
                for (int i = 0; i <= nVertiecs; i++)
                {
                    P0 = ScnPos2f[P0_Index];
                    P1 = ScnPos2f[P1_Index];
                    P2 = ScnPos2f[i];
                    P2 -= P1;
                    P1 -= P0;
                    float sinTheta = (P1.x() * P2.y() - P1.y() * P2.x()) / (P1.length() * P2.length());
                    float cosTheta = (P1.x() * P2.x() + P1.y() * P2.y()) / (P1.length() * P2.length());
                    if (sinTheta < 0 || fabs(sinTheta) < 0.00001 || P2.length() < 0.01)
                        continue;

                    if (fabs(cosTheta) < 0.001)
                    {
                        checkAngle = 90.0f;
                    }
                    else
                        checkAngle = RADIAN_TO_DEGREE(atan2(sinTheta, cosTheta));
//                    std::cout << "NEGLWidget::drawBoundingBox minIndex : " << StartIndex << " minAngle=" << minimumAngle << ", cur : " << i << ", curAngle=" << checkAngle << ", sin=" << sinTheta << ", cos=" << cosTheta << std::endl;
                    if (fabs(minimumAngle - checkAngle) < 0.001)
                    {
                        if (fabs(maxLength - P2.length()) < 0.01)
                        {
                            if (maxValueZ < ScnPos3f[i].z())
                                swapFlag = true;
                        } else
                        {
                            if (maxLength < P2.length())
                                swapFlag = true;
                        }
                    } else {
                        if (checkAngle > 0 && minimumAngle > checkAngle)
                            swapFlag = true;
                    }
                    if (swapFlag)
                    {
                        minimumAngle = checkAngle;
                        maxLength = P2.length();
                        maxValueZ = ScnPos3f[i].z();
                        StartIndex = i;
                        swapFlag = false;
                    }
                }
                if (StartIndex == nVertiecs || isOutsidePoint[StartIndex] || StartIndex == -1)
                    break;
                P0 = ScnPos2f[StartIndex];
                maxLength = 1;
                for (int i = 0; i <= nVertiecs; i++)
                {
                    if (isOutsidePoint[i])
                    {
                        P1 = ScnPos2f[i];
                        P1 -= P0;
                        maxLength = P1.length();
                        if (maxLength < 0.01)
                        {
                            break;
                        }
                    }
                }
                if (maxLength < 0.0001)
                    break;
                outsidePointIndexList.push_back(StartIndex);  //  push index of next start point
                isOutsidePoint[StartIndex] = true;
            } while (true);


//            for (int i = 0; i < outsidePointIndexList.size(); i++)
//            {
//                int Index = outsidePointIndexList[i];
//                P0 = ScnPos2f[Index];
//                std::cout << "NEGLWidget::drawBoundingBox PointList[" << Index << "] x=" << P0.x() << ", y=" << P0.y() << std::endl;
//            }

            // calculate draw angle
            eulerf      drawAngle;
            if (object->BoundingBoxType() == NESpatial::OrientedBoundingBox)
                drawAngle = calculateDrawAngle(object);
            else if (object->BoundingBoxType() == NESpatial::AxisAlignedBoundingBox)
                drawAngle = calculateDrawAngle(NULL);

            float       xLength = (vertices[3]- vertices[0]).length();
            float       yLength = (vertices[1]- vertices[0]).length();
            float       zLength = (vertices[4]- vertices[0]).length();

            vec3f objectScale;
            if (object->BoundingBoxType() == NESpatial::OrientedBoundingBox)
                objectScale = vec3f(object->Scalex(), object->Scaley(), object->Scalez());
            else if (object->BoundingBoxType() == NESpatial::AxisAlignedBoundingBox)
                objectScale = vec3f(1, 1, 1);

            // scale of axis text is change 12/22/2017, 01/07/2018
            xLength *= objectScale.x(); yLength *= objectScale.y(); zLength *= objectScale.z();

            //scale of axis text value is not 0
            if(xLength == 0)
                xLength = 1;
            if(yLength == 0)
                yLength = 1;
            if(zLength == 0)
                zLength = 1;

            float root3Length = qPow(xLength*yLength*zLength, 1.0/3.0);

            float cosAngleForLookat;
            vec3f   V0, V1, V2;
            cameraLookAt = vec3f(0, 0, -1);
//            if (camera() && camera()->fixedOrthographic() && camera()->OrthoType() == 2) // front
//                cameraLookAt = vec3f(0, 0, 1);
//            std::cout << "NEGLWidget::drawBoundingBox cameraLookAt x=" << cameraLookAt.x() << ", y=" << cameraLookAt.y() << ", z=" << cameraLookAt.z() << std::endl;
//            std::cout << "NEGLWidget::drawBoundingBox crossProduct=(y1*z2-z1*y2, z1*x2-x1*z2, x1*y2-y1*x2)" << std::endl;
//            std::cout << "NEGLWidget::drawBoundingBox dotProduct=(x1*x2+y1*y2+z1*z2)" << std::endl;

            // calculate tick count for all axis
            int     commonTickCount[3] = { INT_MAX, INT_MAX, INT_MAX };
            float   commonTickUnit[3] = { 0.01f, 0.01f, 0.01f };
            int     oneTickCount;
            float   oneTickUnit;
            QColor  colorListOfAxis[3];
            colorListOfAxis[0] = object->XAxisColor();
            colorListOfAxis[1] = object->YAxisColor();
            colorListOfAxis[2] = object->ZAxisColor();
            for (int i = 0; i < nEdges; i++)
            {
                getDrawTicksCount( EdgeDirect[i][0], EdgeDirect[i][1], i / 4, ScnPos3f, vertices, objectScale, &oneTickCount, &oneTickUnit, object->ZeroCenterGrid());
                if( (commonTickUnit[i/4] < oneTickUnit) ||
                    ((fabs(commonTickUnit[i/4] - oneTickUnit) < 0.001) && (commonTickCount[i/4] > oneTickCount)) )
                {
                    commonTickCount[i/4] = oneTickCount;
                    commonTickUnit[i/4]  = oneTickUnit;
                }
            }
//            for (int i = 0; i < nEdges / 4; i++)
//            {
//                std::cout << "commonTickCount[" << i << "]=" << commonTickCount[i] << ", commonTickUnit[" << i << "]=" << commonTickUnit[i] << std::endl;
//            }
            // draw ticks for all axis
            for (int i = 0; i < nEdges; i++)
            {
                P0_Index = EdgeIndex[i][0];
                P1_Index = EdgeIndex[i][1];
                if (isOutsidePoint[P0_Index] && isOutsidePoint[P1_Index])
                {
                    V1 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];
                    P0_Index = EdgeIndex[i][2];
                    P1_Index = EdgeIndex[i][3];
                    V2 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];

                    V0 = vec3f::crossProduct(V1, V2);
                    V0.normalize();
                    cosAngleForLookat = vec3f::dotProduct(V0, cameraLookAt);
                    if(camera()->Ortho())
                        cosAngleForLookat = abs(cosAngleForLookat);
                    if(cosAngleForLookat > 0.001)
                    {
                        drawTicksBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][2], EdgeDirect[i][3],
                                i / 4, vertices, root3Length, objectScale,
                                commonTickCount[i/4], commonTickUnit[i/4], object->ZeroCenterGrid());
                    }
                    P0_Index = EdgeIndex[i][4];
                    P1_Index = EdgeIndex[i][5];
                    V2 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];

                    V0 = vec3f::crossProduct(V1, V2);
                    V0.normalize();
                    cosAngleForLookat = vec3f::dotProduct(V0, cameraLookAt);
                    if(camera()->Ortho())
                        cosAngleForLookat = abs(cosAngleForLookat);
                    if(cosAngleForLookat > 0.001)
                    {
                        drawTicksBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][4], EdgeDirect[i][5],
                                i / 4, vertices, root3Length, objectScale,
                                commonTickCount[i/4], commonTickUnit[i/4], object->ZeroCenterGrid());
                    }
                } else
                {
                    V1 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];
                    P0_Index = EdgeIndex[i][2];
                    P1_Index = EdgeIndex[i][3];
                    V2 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];

                    V0 = vec3f::crossProduct(V1, V2);
                    V0.normalize();
                    cosAngleForLookat = vec3f::dotProduct(V0, cameraLookAt);
                    if(camera()->Ortho())
                        cosAngleForLookat = abs(cosAngleForLookat);
                    if(cosAngleForLookat > 0.001)
                    {
                        drawTicksBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][2], EdgeDirect[i][3],
                                i / 4, vertices, root3Length, objectScale,
                                commonTickCount[i/4], commonTickUnit[i/4], object->ZeroCenterGrid(), true);
                    }
                    P0_Index = EdgeIndex[i][4];
                    P1_Index = EdgeIndex[i][5];
                    V2 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];

                    V0 = vec3f::crossProduct(V1, V2);
                    V0.normalize();
                    cosAngleForLookat = vec3f::dotProduct(V0, cameraLookAt);
                    if(camera()->Ortho())
                        cosAngleForLookat = abs(cosAngleForLookat);
                    if(cosAngleForLookat > 0.001)
                    {
                        drawTicksBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][4], EdgeDirect[i][5],
                                i / 4, vertices, root3Length, objectScale,
                                commonTickCount[i/4], commonTickUnit[i/4], object->ZeroCenterGrid(), true);
                    }
                }
            }
            if( object->ShowBoundingBoxCenter() )
            {
                drawCenterTicksBoundingAxis(vertices, root3Length, objectScale);
            }
            // draw text for all axis
            for (int i = 0; i < nEdges; i++)
            {
                P0_Index = EdgeIndex[i][0];
                P1_Index = EdgeIndex[i][1];
                if (isOutsidePoint[P0_Index] && isOutsidePoint[P1_Index])
                {
                    V1 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];
                    P0_Index = EdgeIndex[i][2];
                    P1_Index = EdgeIndex[i][3];
                    V2 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];

                    V0 = vec3f::crossProduct(V1, V2);
                    V0.normalize();
                    cosAngleForLookat = vec3f::dotProduct(V0, cameraLookAt);
                    if(camera()->Ortho())
                        cosAngleForLookat = abs(cosAngleForLookat);
//                    std::cout << "NEGLWidget::drawBoundingBox V1       x=" << V1.x() << ", y=" << V1.y() << ", z=" << V1.z() << std::endl;
//                    std::cout << "NEGLWidget::drawBoundingBox V2       x=" << V2.x() << ", y=" << V2.y() << ", z=" << V2.z() << std::endl;
//                    std::cout << "NEGLWidget::drawBoundingBox V0       x=" << V0.x() << ", y=" << V0.y() << ", z=" << V0.z() << std::endl;
//                    std::cout << "NEGLWidget::drawBoundingBox x-axis angleForLookat of first  side ["
//                              << EdgeIndex[i][0] << ", "  << EdgeIndex[i][1] << "] ["
//                              << EdgeIndex[i][2] << ", "  << EdgeIndex[i][3] << "] =" << cosAngleForLookat << std::endl;
                    if(cosAngleForLookat > 0.001)
                    {
                        if(object->BoundingBoxType() == NESpatial::OrientedBoundingBox)
                            drawText2DBoundingAxis(
                                    EdgeDirect[i][0], EdgeDirect[i][1],
                                    EdgeDirect[i][2], EdgeDirect[i][3], i / 4, EdgeDirect[i][6],
                                    ScnPos3f, vertices,
                                    drawAngle, root3Length, objectScale, commonTickCount[i/4], commonTickUnit[i/4],
                                    object->StringScale(), colorListOfAxis[i/4], object->ZeroCenterGrid(),
                                    object->NumbersFromWorldUnits(), object->transform());
                        else if( object->BoundingBoxType() == NESpatial::AxisAlignedBoundingBox)
                            drawText2DBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][2], EdgeDirect[i][3], i / 4, EdgeDirect[i][6],
                                ScnPos3f, vertices,
                                drawAngle, root3Length, objectScale, commonTickCount[i / 4], commonTickUnit[i / 4],
                                object->StringScale(), colorListOfAxis[i / 4], object->ZeroCenterGrid(),
                                object->NumbersFromWorldUnits(), matrix44f());
                    }
                    P0_Index = EdgeIndex[i][4];
                    P1_Index = EdgeIndex[i][5];
                    V2 = ScnPos3f[P1_Index] - ScnPos3f[P0_Index];

                    V0 = vec3f::crossProduct(V1, V2);
                    V0.normalize();
                    cosAngleForLookat = vec3f::dotProduct(V0, cameraLookAt);
                    if(camera()->Ortho())
                        cosAngleForLookat = abs(cosAngleForLookat);

//                    std::cout << "NEGLWidget::drawBoundingBox x-axis angleForLookat of second  side ["
//                              << EdgeIndex[i][0] << ", "  << EdgeIndex[i][1] << "] ["
//                              << EdgeIndex[i][4] << ", "  << EdgeIndex[i][5] << "] =" << cosAngleForLookat << std::endl;
                    if(cosAngleForLookat > 0.001)
                    {
                        if (object->BoundingBoxType() == NESpatial::OrientedBoundingBox)
                            drawText2DBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][4], EdgeDirect[i][5], i / 4, EdgeDirect[i][7],
                                ScnPos3f, vertices,
                                drawAngle, root3Length, objectScale, commonTickCount[i / 4], commonTickUnit[i / 4],
                                object->StringScale(), colorListOfAxis[i / 4], object->ZeroCenterGrid(),
                                object->NumbersFromWorldUnits(), object->transform());
                        else if (object->BoundingBoxType() == NESpatial::AxisAlignedBoundingBox)
                            drawText2DBoundingAxis(
                                EdgeDirect[i][0], EdgeDirect[i][1],
                                EdgeDirect[i][4], EdgeDirect[i][5], i / 4, EdgeDirect[i][7],
                                ScnPos3f, vertices,
                                drawAngle, root3Length, objectScale, commonTickCount[i / 4], commonTickUnit[i / 4],
                                object->StringScale(), colorListOfAxis[i / 4], object->ZeroCenterGrid(),
                                object->NumbersFromWorldUnits(), matrix44f());

                    }
                }
            }
        }
    }
    GL->glPopAttrib();
}

void IGLSurface::drawText3D(QString text, vec3f pos, eulerf angle, float size, vec3f scale, Color32 color)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glDisable(GL_DEPTH_TEST);
    matrix44f letterMatrix; // = worldStack().top();
    letterMatrix.translate(pos.x(),pos.y(),pos.z());

    letterMatrix.scale(size/scale.x(), size/scale.y(), size/scale.z());

    quatf quatX, quatY, quatZ, quatXYZ;
    quatX = quatX.fromAxisAndAngle( 1,0,0, angle.phi() );
    quatY = quatY.fromAxisAndAngle( 0,1,0, angle.theta() );
    quatZ = quatZ.fromAxisAndAngle( 0,0,1, angle.psi() );
    quatXYZ = quatZ*quatY*quatX;
    letterMatrix.rotate( quatXYZ );

    worldStack().pushMatrix(letterMatrix);
    setModelViewMatrix();
    GL->glColor3ubv((GLubyte*)&color);
    NERENDERMAN.GLManager()->glPrint3D(text.toUtf8(), -1);
    worldStack().popMatrix();
    GL->glPopAttrib();
}

void IGLSurface::drawText3D_new(QString text, vec3f pos, eulerf angle, float size, vec3f scale, Color32 color)
{
    matrix44f letterMatrix = worldStack().top();
    letterMatrix.translate(pos.x(),pos.y(),pos.z());

    quatf quatX, quatY, quatZ, quatXYZ;
    quatX = quatX.fromAxisAndAngle( 1,0,0, angle.phi() );
    quatY = quatY.fromAxisAndAngle( 0,1,0, angle.theta() );
    quatZ = quatZ.fromAxisAndAngle( 0,0,1, angle.psi() );
    quatXYZ = quatZ*quatY*quatX;
    //letterMatrix.rotate( quatXYZ );

    letterMatrix.scale(size, size, size);

    worldStack().pushMatrix(letterMatrix);
    setModelViewMatrix();
    GL->glColor3ubv((GLubyte*)&color);
    NERENDERMAN.GLManager()->glPrint3D(text.toUtf8(), -1);

    worldStack().popMatrix();
}

void IGLSurface::getDrawTicksCount(int fromIndex, int toIndex, int axisIndex, vec3f* positions, vec3f* vertices, vec3f objectScale, int *pOneTickCount, float *pOneTickUnit, bool calcFromCenter)
{
    vec3f P0, P1;
    vec2f Q0, Q1, dispLine;
    float dispScale;
    P0 = vertices[fromIndex];
    P1 = vertices[toIndex];
    Q0 = vec2f(positions[fromIndex].x(),positions[fromIndex].y());
    Q1 = vec2f(positions[toIndex].x(),positions[toIndex].y());
    dispLine = Q1 - Q0;
//    std::cout << "NEGLWidget::getDrawTicksCount Q0(pos)   x=" << Q0.x() << ", y=" << Q0.y() << std::endl;
//    std::cout << "NEGLWidget::getDrawTicksCount Q1(pos)   x=" << Q1.x() << ", y=" << Q1.y() << std::endl;
//    std::cout << "NEGLWidget::getDrawTicksCount dispLine  x=" << dispLine.x() << ", y=" << dispLine.y() << std::endl;
    switch (axisIndex)
    {
    default:
    case 0:
        dispScale   = objectScale.x();
        break;
    case 1:
        dispScale   = objectScale.y();
        break;
    case 2:
        dispScale   = objectScale.z();
        break;
    }

    float   maxDispLength = fabs(dispLine.x());
    if (fabs(dispLine.y()) > maxDispLength)
        maxDispLength = fabs(dispLine.y());
    float   dispTickCount   = maxDispLength / 60.0f; // ???
    if (calcFromCenter) dispTickCount /= 2.0f;
    dispTickCount = MIN(100, MAX(1.0f, dispTickCount));
    float   unitTable[] = { 0.01f, 0.02f,  0.05f, 0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f };
    float   axisLength  = (P1 - P0).length() * dispScale;
    if (calcFromCenter) axisLength /= 2.0f;
    float   dispUnit    = axisLength / dispTickCount;
    dispUnit = MIN(10000.0f, dispUnit);
    for (int i = 0; i < 19; i++)
    {
        if (dispUnit <= unitTable[i])
        {
            dispUnit = unitTable[i];
            break;
        }
    }
    int nDispTickCount = axisLength / dispUnit;
    if ((float)nDispTickCount * dispUnit < axisLength) nDispTickCount += 1;
    else if (fabs((float)nDispTickCount * dispUnit - axisLength) < 0.0001) nDispTickCount += 1;

    if (calcFromCenter) nDispTickCount *= 2;
    nDispTickCount = MAX(1, nDispTickCount);
//    std::cout << "NEGLWidget::getDrawTicksCount nDispTickCount=" << nDispTickCount << std::endl;
//    std::cout << "NEGLWidget::getDrawTicksCount dispUnit=" << dispUnit << std::endl << std::endl;

    if (pOneTickCount)
        *pOneTickCount = nDispTickCount;
    if (pOneTickUnit)
        *pOneTickUnit = dispUnit;
}

void IGLSurface::drawCenterTicksBoundingAxis(vec3f *vertices, float root3Length, vec3f objectScale)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    NERENDERMAN.GLManager()->bindPositionShader(this);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    vec3f*  verticesArray;
    verticesArray = new vec3f[4*4];
    verticesArray[0] = (vertices[0] + vertices[1] + vertices[2] + vertices[3]) / 4;
    verticesArray[4] = (vertices[4] + vertices[5] + vertices[6] + vertices[7]) / 4;
    verticesArray[8] = (vertices[0] + vertices[1] + vertices[4] + vertices[5]) / 4;
    verticesArray[12] = (vertices[2] + vertices[3] + vertices[6] + vertices[7]) / 4;

    GL->glEnableClientState(GL_VERTEX_ARRAY);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)verticesArray);
    GL->glDrawArrays(GL_LINES,0,4*4);

    delete[] verticesArray;
    GL->glDisableClientState(GL_VERTEX_ARRAY);

    GL->glPopAttrib();
}

void IGLSurface::drawTicksBoundingAxis(int fromIndex, int toIndex, int dirFromIndex, int dirToIndex, int axisIndex, vec3f* vertices,
            float root3Length, vec3f objectScale, int commonTickCount, float commonTickUnit, bool calcFromCenter, bool hideTick)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);
    GL->glColor3f(1,1,1);

    vec3f P0, P1, P_Center, directVector3D, deltaPos, deltaTick, dirVectorPos3D, axisNorVecPos3D;
    float axisVectorLength, dirVectorLength;
    float dispScale;
    P0 = vertices[fromIndex];
    P1 = vertices[toIndex];
    P_Center = (P0 + P1) * 0.5f;
    directVector3D = (vertices[dirToIndex] - vertices[dirFromIndex]);
    dirVectorLength = directVector3D.length();
    directVector3D.normalize();

    deltaPos = directVector3D;
    deltaPos *= 0.5*DIVISION_LENGTH*root3Length;
    deltaPos *= vec3f(1/objectScale.x(), 1/objectScale.y(), 1/objectScale.z());
    deltaTick = directVector3D;
    deltaTick *= 1*DIVISION_LENGTH*root3Length;
    deltaTick *= vec3f(1/objectScale.x(), 1/objectScale.y(), 1/objectScale.z());
    if (hideTick)
    {
        deltaPos = vec3f(0.0f, 0.0f, 0.0f);
        deltaTick = vec3f(0.0f, 0.0f, 0.0f);
    }
//    std::cout << "NEGLWidget::drawTicksBoundingAxis deltaPos       x=" << deltaPos.x() << ", y=" << deltaPos.y() << ", z=" << deltaPos.z() << std::endl;
//    std::cout << "NEGLWidget::drawTicksBoundingAxis deltaTick      x=" << deltaTick.x() << ", y=" << deltaTick.y() << ", z=" << deltaTick.z() << std::endl;
//    std::cout << "NEGLWidget::drawTicksBoundingAxis directVector3D x=" << directVector3D.x() << ", y=" << directVector3D.y() << ", z=" << directVector3D.z() << ", root3Length=" << root3Length << std::endl;

    switch (axisIndex)
    {
    default:
    case 0:
        dispScale   = objectScale.x();
        break;
    case 1:
        dispScale   = objectScale.y();
        break;
    case 2:
        dispScale   = objectScale.z();
        break;
    }

    GLint nDispSubTickCount = 10;
    if (commonTickUnit < 0.1f)
        nDispSubTickCount = 1;
    else
        commonTickUnit /= 10.0f;

    vec3f*  verticesArray;
    GLint   axisCount = commonTickCount * nDispSubTickCount; // 100;
    if (calcFromCenter)
        axisCount = MAX(1, std::round((axisCount + 1) / 2) );
//    std::cout << "NEGLWidget::drawTicksBoundingAxis commonTickCount=" << commonTickCount << std::endl;
//    std::cout << "NEGLWidget::drawTicksBoundingAxis commonTickUnit=" << commonTickUnit << std::endl;
//    std::cout << "NEGLWidget::drawTicksBoundingAxis axisCount=" << axisCount << std::endl;

    NERENDERMAN.GLManager()->bindPositionShader(this);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    GL->glEnableClientState(GL_VERTEX_ARRAY);

    verticesArray = new vec3f[4*(axisCount+2)];
    dirVectorPos3D = directVector3D;
    dirVectorPos3D *= dirVectorLength;
//    std::cout << "NEGLWidget::drawTicksBoundingAxis directVector3D(0) x=" << dirVectorPos3D.x() << ", y=" << dirVectorPos3D.y() << ", z=" << dirVectorPos3D.z() << ", dirVectorLength=" << dirVectorLength << ", dirVecScale=" << dirVecScale << std::endl;

    if (calcFromCenter == false)
    {
        axisNorVecPos3D = P1 - P0;
        axisVectorLength = axisNorVecPos3D.length();
        axisNorVecPos3D.normalize();
        int realDispCount = 0;
        for(; realDispCount < axisCount; realDispCount++)
        {
//            std::cout << "NEGLWidget::drawTicksBoundingAxis realDispCount=" << realDispCount << ", commonTickUnit=" << commonTickUnit << ", dispScale=" << dispScale << ", axisVectorLength=" << axisVectorLength << ", " << (float)realDispCount * commonTickUnit << std::endl;
            if ((float)realDispCount * commonTickUnit > axisVectorLength * dispScale)
                break;
            verticesArray[4*realDispCount]   = P0 + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+1] = P0 + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+2] = P0 + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+3] = P0 + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+1] += deltaTick;
            if(realDispCount % nDispSubTickCount == 0)
            {
                verticesArray[4*realDispCount+1] += deltaTick;
            }
            if(realDispCount % nDispSubTickCount == 0)
            {
                verticesArray[4*realDispCount+2] -= deltaPos;
                verticesArray[4*realDispCount+2] -= dirVectorPos3D;
            }
        }
        verticesArray[4*realDispCount]   = P1 + deltaPos;
        verticesArray[4*realDispCount+1] = P1 + deltaPos + 2 * deltaTick;
        verticesArray[4*realDispCount+2] = P1 - dirVectorPos3D;
        verticesArray[4*realDispCount+3] = P1 + deltaPos;


        verticesArray[4*(realDispCount+1)]   = P0 + deltaPos;
        verticesArray[4*(realDispCount+1)+1] = P1 + deltaPos;
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)verticesArray);
        GL->glDrawArrays(GL_LINES,0,4*(realDispCount+2));
    } else
    {
        axisNorVecPos3D = P1 - P_Center;
        axisVectorLength = axisNorVecPos3D.length();
        axisNorVecPos3D.normalize();
        int realDispCount = 0;
        for(; realDispCount < axisCount; realDispCount++)
        {
//            std::cout << "NEGLWidget::drawTicksBoundingAxis realDispCount=" << realDispCount << ", commonTickUnit=" << commonTickUnit << ", dispScale=" << dispScale << ", axisVectorLength=" << axisVectorLength << ", " << (float)realDispCount * commonTickUnit << std::endl;
            if ((float)realDispCount * commonTickUnit > axisVectorLength * dispScale)
                break;
            verticesArray[4*realDispCount]   = P_Center + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+1] = P_Center + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+2] = P_Center + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+3] = P_Center + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+1] += deltaTick;
            if(realDispCount % nDispSubTickCount == 0)
            {
                verticesArray[4*realDispCount+1] += deltaTick;
            }
            if(realDispCount % nDispSubTickCount == 0)
            {
                verticesArray[4*realDispCount+2] -= deltaPos;
                verticesArray[4*realDispCount+2] -= dirVectorPos3D;
            }
        }
        verticesArray[4*realDispCount]   = P1 + deltaPos;
        verticesArray[4*realDispCount+1] = P1 + deltaPos + 2 * deltaTick;
        verticesArray[4*realDispCount+2] = P1 - dirVectorPos3D;
        verticesArray[4*realDispCount+3] = P1 + deltaPos;


        verticesArray[4*(realDispCount+1)]   = P0 + deltaPos;
        verticesArray[4*(realDispCount+1)+1] = P1 + deltaPos;
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)verticesArray);
        GL->glDrawArrays(GL_LINES,0,4*(realDispCount+2));

        axisNorVecPos3D = P_Center - P0;
        axisVectorLength = axisNorVecPos3D.length();
        axisNorVecPos3D.normalize();
        realDispCount = 0;
        for(; realDispCount < axisCount; realDispCount++)
        {
//            std::cout << "NEGLWidget::drawTicksBoundingAxis realDispCount=" << -realDispCount << ", commonTickUnit=" << commonTickUnit << ", dispScale=" << dispScale << ", axisVectorLength=" << axisVectorLength << ", " << (float)realDispCount * commonTickUnit << std::endl;
            if ((float)realDispCount * commonTickUnit > axisVectorLength * dispScale)
                break;
            verticesArray[4*realDispCount]   = P_Center - axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+1] = P_Center - axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+2] = P_Center - axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+3] = P_Center - axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
            verticesArray[4*realDispCount+1] += deltaTick;
            if(realDispCount % nDispSubTickCount == 0)
            {
                verticesArray[4*realDispCount+1] += deltaTick;
            }
            if(realDispCount % nDispSubTickCount == 0)
            {
                verticesArray[4*realDispCount+2] -= deltaPos;
                verticesArray[4*realDispCount+2] -= dirVectorPos3D;
            }
        }
        verticesArray[4*realDispCount]   = P0 + deltaPos;
        verticesArray[4*realDispCount+1] = P0 + deltaPos + 2 * deltaTick;
        verticesArray[4*realDispCount+2] = P0 - dirVectorPos3D;
        verticesArray[4*realDispCount+3] = P0 + deltaPos;


        verticesArray[4*(realDispCount+1)]   = P0 + deltaPos;
        verticesArray[4*(realDispCount+1)+1] = P1 + deltaPos;
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)verticesArray);
        GL->glDrawArrays(GL_LINES,0,4*(realDispCount+2));
    }

    delete[] verticesArray;
    GL->glDisableClientState(GL_VERTEX_ARRAY);

    GL->glPopAttrib();
}

void IGLSurface::drawText2DBoundingAxis(int fromIndex, int toIndex, int dirFromIndex, int dirToIndex, int axisIndex, int dirAxisIndex, vec3f* positions, vec3f* vertices,
            eulerf drawAngle, float root3Length, vec3f objectScale, int commonTickCount, float commonTickUnit, double stringScale, QColor stringColor, bool calcFromCenter, bool drawInWorldUnits, matrix44f transform)
{

     GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    QString text;

    vec3f P0, P1, P_Center, directVector3D, deltaPos, dispPos, axisNorVecPos3D;
    float axisVectorLength;
    P0 = vertices[fromIndex];
    P1 = vertices[toIndex];
    P_Center = (P0 + P1) * 0.5;
    directVector3D = (vertices[dirToIndex] - vertices[dirFromIndex]);
    directVector3D.normalize();
    deltaPos = directVector3D;
    deltaPos *= 5*DIVISION_LENGTH*root3Length;
    deltaPos *= vec3f(1/objectScale.x(), 1/objectScale.y(), 1/objectScale.z());

    setShaderProgram(0);

    GLint   axisCount = commonTickCount;
    if (calcFromCenter)
        axisCount = MAX(1, std::round((axisCount + 1) / 2) );
//    std::cout << "NEGLWidget::drawText2DBoundingAxis commonTickCount=" << commonTickCount << std::endl;
//    std::cout << "NEGLWidget::drawText2DBoundingAxis commonTickUnit=" << commonTickUnit << std::endl;
//    std::cout << "NEGLWidget::drawText2DBoundingAxis axisCount=" << axisCount << std::endl;

    float   disNum, dispScale, textDispScale;
    switch (axisIndex)
    {
    default:
    case 0:
        dispScale = objectScale.x();
        break;
    case 1:
        dispScale = objectScale.y();
        break;
    case 2:
        dispScale = objectScale.z();
        break;
    }
    textDispScale = 6*DIVISION_LENGTH*root3Length;
    textDispScale *= stringScale;
    if (calcFromCenter == false)
    {
        axisNorVecPos3D = P1 - P0;
        axisVectorLength = axisNorVecPos3D.length();
        axisNorVecPos3D.normalize();
        int realDispCount = 0;
        for(; realDispCount <= axisCount; realDispCount++)
        {
//            std::cout << "NEGLWidget::drawText2DBoundingAxis realDispCount=" << realDispCount << ", TickUnit=" << commonTickUnit
//                      << ", dispScale=" << dispScale << ", axisLength=" << axisVectorLength << ", " << (float)realDispCount * commonTickUnit << " * " << axisVectorLength * dispScale << std::endl;
            if ((float)realDispCount * commonTickUnit > axisVectorLength * dispScale){
                if((float)(realDispCount-1) * commonTickUnit >= axisVectorLength * dispScale)
                    break;
                else{
                    dispPos = P0 + axisNorVecPos3D * axisVectorLength + deltaPos;

                    if( drawInWorldUnits ){
                        vec4f dPos = transform * vec4f(dispPos , 1.0);
                        text = "(" + QString::number(dPos.x(), 'f', 2) + ", " + QString::number(dPos.y(), 'f', 2) + ", " + QString::number(dPos.z(), 'f', 2) + ")";
                    }
                    else{
                        disNum = axisVectorLength * dispScale;
                        disNum = std::round(disNum*100)/100;
                        text.setNum(disNum);
                    }
                    drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));
                }
            }
            else
            {

                dispPos = P0 + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
                if( drawInWorldUnits ){
                    vec4f dPos = transform * vec4f(dispPos , 1.0);
                    text = "(" + QString::number(dPos.x(), 'f', 2) + ", " + QString::number(dPos.y(), 'f', 2) + ", " + QString::number(dPos.z(), 'f', 2) + ")";
                }
                else{
                    disNum = realDispCount * commonTickUnit;
                    disNum = std::round(disNum*100)/100;
                    text.setNum(disNum);
                }
                drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));
    //            std::cout << "NEGLWidget::drawText2DBoundingAxis deltaPos       x=" << deltaPos.x() << ", y=" << deltaPos.y() << ", z=" << deltaPos.z() << std::endl;
    //            std::cout << "NEGLWidget::drawText2DBoundingAxis directVector3D x=" << directVector3D.x() << ", y=" << directVector3D.y() << ", z=" << directVector3D.z() << ", root3Length=" << root3Length << std::endl;
            }
        }
    } else
    {
        axisNorVecPos3D = P1 - P_Center;
        axisVectorLength = axisNorVecPos3D.length();
        axisNorVecPos3D.normalize();
        int realDispCount = 0;
        for(; realDispCount <= axisCount; realDispCount++)
        {
//            std::cout << "NEGLWidget::drawText2DBoundingAxis realDispCount=" << realDispCount << ", TickUnit=" << commonTickUnit
//                      << ", dispScale=" << dispScale << ", axisLength=" << axisVectorLength << ", " << (float)realDispCount * commonTickUnit << " * " << axisVectorLength * dispScale << std::endl;
            if ((float)realDispCount * commonTickUnit > axisVectorLength * dispScale)
            {
                if((float)(realDispCount-1) * commonTickUnit >= axisVectorLength * dispScale)
                    break;
                else{
                    dispPos = P_Center + axisNorVecPos3D * axisVectorLength + deltaPos;

                    if( drawInWorldUnits ){
                        vec4f dPos = transform * vec4f(dispPos , 1.0);
                        text = "(" + QString::number(dPos.x(), 'f', 2) + ", " + QString::number(dPos.y(), 'f', 2) + ", " + QString::number(dPos.z(), 'f', 2) + ")";
                    }
                    else{
                        disNum = axisVectorLength * dispScale;
                        disNum = std::round(disNum*100)/100;
                        text.setNum(disNum);
                    }
                    drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));
                }
            }
            else{
                dispPos = P_Center + axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
                if( drawInWorldUnits ){
                    vec4f dPos = transform * vec4f(dispPos , 1.0);
                    text = "(" + QString::number(dPos.x(), 'f', 2) + ", " + QString::number(dPos.y(), 'f', 2) + ", " + QString::number(dPos.z(), 'f', 2) + ")";
                }
                else{
                    disNum = realDispCount * commonTickUnit;
                    disNum = std::round(disNum*100)/100;
                    text.setNum(disNum);
                }
                drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));
    //            std::cout << "NEGLWidget::drawText2DBoundingAxis deltaPos       x=" << deltaPos.x() << ", y=" << deltaPos.y() << ", z=" << deltaPos.z() << std::endl;
    //            std::cout << "NEGLWidget::drawText2DBoundingAxis directVector3D x=" << directVector3D.x() << ", y=" << directVector3D.y() << ", z=" << directVector3D.z() << ", root3Length=" << root3Length << std::endl;

            }
        }
        axisNorVecPos3D = P_Center - P0;
        axisVectorLength = axisNorVecPos3D.length();
        axisNorVecPos3D.normalize();
        realDispCount = 0;
        for(; realDispCount <= axisCount; realDispCount++)
        {
//            std::cout << "NEGLWidget::drawText2DBoundingAxis realDispCount=" << -realDispCount << ", TickUnit=" << commonTickUnit
//                      << ", dispScale=" << dispScale << ", axisLength=" << axisVectorLength << ", " << (float)realDispCount * commonTickUnit << " * " << axisVectorLength * dispScale << std::endl;
            if ((float)realDispCount * commonTickUnit > axisVectorLength * dispScale)
            {
                if((float)(realDispCount-1) * commonTickUnit >= axisVectorLength * dispScale)
                    break;
                else{
                    dispPos = P_Center - axisNorVecPos3D * axisVectorLength + deltaPos;

                    if( drawInWorldUnits ){
                        vec4f dPos = transform * vec4f(dispPos , 1.0);
                        text = "(" + QString::number(dPos.x(), 'f', 2) + ", " + QString::number(dPos.y(), 'f', 2) + ", " + QString::number(dPos.z(), 'f', 2) + ")";
                    }
                    else{
                        disNum = axisVectorLength * dispScale;
                        disNum = std::round(disNum*100)/100;
                        text.setNum(disNum);
                    }
                    drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));
                }
            }
            else
            {
                dispPos = P_Center - axisNorVecPos3D * realDispCount * commonTickUnit / dispScale + deltaPos;
                if( drawInWorldUnits ){
                    vec4f dPos = transform * vec4f(dispPos , 1.0);
                    text = "(" + QString::number(dPos.x(), 'f', 2) + ", " + QString::number(dPos.y(), 'f', 2) + ", " + QString::number(dPos.z(), 'f', 2) + ")";
                }
                else{
                    disNum = realDispCount * commonTickUnit;
                    disNum = std::round(disNum*100)/100;
                    text.setNum(disNum);
                }
                drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));
    //            std::cout << "NEGLWidget::drawText2DBoundingAxis deltaPos       x=" << deltaPos.x() << ", y=" << deltaPos.y() << ", z=" << deltaPos.z() << std::endl;
    //            std::cout << "NEGLWidget::drawText2DBoundingAxis directVector3D x=" << directVector3D.x() << ", y=" << directVector3D.y() << ", z=" << directVector3D.z() << ", root3Length=" << root3Length << std::endl;
            }

        }
    }

    switch (axisIndex)
    {
    default:
    case 0:
        text = "X";
        break;
    case 1:
        text = "Y";
        break;
    case 2:
        text = "Z";
        break;
    }

    deltaPos = directVector3D;
    deltaPos *= 10*DIVISION_LENGTH*root3Length;
    deltaPos *= vec3f(1/objectScale.x(), 1/objectScale.y(), 1/objectScale.z());
    dispPos = P_Center + deltaPos;
    textDispScale = 8*DIVISION_LENGTH*root3Length;
    textDispScale *= stringScale;
//    std::cout << "NEGLWidget::drawBoundingBox fromPos=" << fromIndex << ", toPos" << toIndex << ", fromDirect=" << dirFromIndex << ", toDirect=" << dirToIndex << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox P0          x=" << P0.x() << ", y=" << P0.y() << ", z=" << P0.z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox P1          x=" << P1.x() << ", y=" << P1.y() << ", z=" << P1.z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox P_Center    x=" << P_Center.x() << ", y=" << P_Center.y() << ", z=" << P_Center.z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox dirP0       x=" << vertices[dirFromIndex].x() << ", y=" << vertices[dirFromIndex].y() << ", z=" << vertices[dirFromIndex].z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox dirP1       x=" << vertices[dirToIndex].x() << ", y=" << vertices[dirToIndex].y() << ", z=" << vertices[dirToIndex].z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox directVec   x=" << directVector3D.x() << ", y=" << directVector3D.y() << ", z=" << directVector3D.z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox delta       x=" << delta.x() << ", y=" << delta.y() << ", z=" << delta.z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox dispPos     x=" << dispPos.x() << ", y=" << dispPos.y() << ", z=" << dispPos.z() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox drawAngle   phi=" << drawAngle.phi() << ", theta=" << drawAngle.theta() << ", psi=" << drawAngle.psi() << std::endl;
//    std::cout << "NEGLWidget::drawBoundingBox objectScale x=" << objectScale.x() << ", y=" << objectScale.y() << ", z=" << objectScale.z() << ", size =" << 10*DIVISION_LENGTH*root3Length << std::endl << std::endl;

    drawText3D(text, dispPos, drawAngle, textDispScale, objectScale, Color32(stringColor.red(),stringColor.green(),stringColor.blue(),255));

    NERENDERMAN.GLManager()->bindPositionShader(this);

    GL->glPopAttrib();
}

eulerf IGLSurface::calculateDrawAngle(NESpatial* object)
{
    // calculate draw angle
    vec3f   cameraUp(0,0,0);
    vec3f   cameraSide(0,0,0);
    vec3f   cameraLookAt(0,0,0);
    if(camera())
    {
        cameraUp = camera()->up();
        cameraUp.normalize();
        cameraSide = camera()->side();
        cameraSide.normalize();
        cameraLookAt = camera()->look();
        cameraLookAt.normalize();
    }

    eulerf      drawAngle = vec3f(0, 0, 0);
    eulerf      drawAngle1 = vec3f(0, 0, 0);

    if (camera()->fixedOrthographic())
    {
        switch (camera()->OrthoType())
        {
        case NECamera::Top: // top

            drawAngle = vec3f(-90, 0, 0);
            if(camera()->FlipSide()){
                drawAngle.setPhi(drawAngle.phi() + 180);
                drawAngle.setTheta(drawAngle.theta() + 180);
            }
            if(camera()->FlipUp()){
                drawAngle.setPhi(drawAngle.phi() + 180);
            }

            break;
        case NECamera::Left: // left
            drawAngle = vec3f(0, -90, 0);
            if(camera()->FlipSide()){
                drawAngle.setTheta(drawAngle.theta() + 180);
            }
            if(camera()->FlipUp()){
                drawAngle.setPhi(drawAngle.phi() + 180);
            }
            break;
        case NECamera::Front: // front
            drawAngle = vec3f(0, 0, 0);
            if(camera()->FlipSide()){
                drawAngle.setTheta(drawAngle.theta() + 180);
            }
            if(camera()->FlipUp()){
                drawAngle.setPhi(drawAngle.phi() + 180);
            }
            break;
        default:
            break;
        }
    } else
    {
        cameraLookAt.setX(-cameraLookAt.x()); cameraLookAt.setY(-cameraLookAt.y()); cameraLookAt.setZ(-cameraLookAt.z());
        drawAngle = eulerAnglesFromDirection(cameraLookAt);
        drawAngle1 = eulerAnglesFromDirection(cameraUp);
        if (fabs(cameraUp.y()) < 0.001 || (fabs(cameraLookAt.x()) < 0.001 && fabs(cameraLookAt.z()) < 0.001))
        {
            if (cameraLookAt.y() < 0)
                drawAngle.setTheta(drawAngle1.theta());
            else
            {
                if (drawAngle1.theta() >= 0)
                    drawAngle.setTheta(drawAngle1.theta() - 180.0);
                else
                    drawAngle.setTheta(drawAngle1.theta() + 180.0);
            }
        } else
        {
            if (cameraUp.y() < 0)
            {
                if (drawAngle.theta() >= 0)
                    drawAngle.setTheta(drawAngle.theta() - 180.0);
                else
                    drawAngle.setTheta(drawAngle.theta() + 180.0);
                if (drawAngle.phi() >= 0)
                    drawAngle.setPhi(-drawAngle.phi() + 180.0);
                else
                    drawAngle.setPhi(-drawAngle.phi() - 180.0);
            }
        }
    }

    /*
#ifdef _DRAW_BOUNDINGBOX_UNITTEST_
    {
        float specialAngle[5] = { 0.0f, 90.0f, 180.0f, -180.0f, -90.0f };
        bool  resCorrect = false;

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                for (int k = 0; k < 5; k++)
                {
                    resCorrect = false;

                    eulerf      objAngle1 = eulerf(specialAngle[i], specialAngle[j], specialAngle[k]);
                    eulerf      objAngle2 = eulerf(specialAngle[i], specialAngle[j], specialAngle[k] + 1);
                    eulerf      objAngle3 = eulerf(specialAngle[i], specialAngle[j], specialAngle[k] - 1);
                    quatf       drawQuat = quaternionsFromEulerAngles(drawAngle);
                    quatf       objQuat1 = quaternionsFromEulerAngles(objAngle1);
                    quatf       objQuat2 = quaternionsFromEulerAngles(objAngle2);
                    quatf       objQuat3 = quaternionsFromEulerAngles(objAngle3);

                    matrix33f rotateMatOfDrawAlgne = matrixFromQuaternions(drawQuat);

                    matrix33f
                    rotateMatOfObjectAngle = matrixFromQuaternions(objQuat2);
                    rotateMatOfObjectAngle.invert();
                    rotateMatOfObjectAngle = rotateMatOfObjectAngle * rotateMatOfDrawAlgne;
                    eulerf      drawAngle_Inc = eulerAnglesFromMatrix(rotateMatOfObjectAngle);
                    rotateMatOfObjectAngle = matrixFromQuaternions(objQuat3);
                    rotateMatOfObjectAngle.invert();
                    rotateMatOfObjectAngle = rotateMatOfObjectAngle * rotateMatOfDrawAlgne;
                    eulerf      drawAngle_Dec = eulerAnglesFromMatrix(rotateMatOfObjectAngle);

                    rotateMatOfObjectAngle = matrixFromQuaternions(objQuat1);
                    rotateMatOfObjectAngle.invert();
                    rotateMatOfObjectAngle = rotateMatOfObjectAngle * rotateMatOfDrawAlgne;
                    eulerf      drawAngle_M = eulerAnglesFromMatrix(rotateMatOfObjectAngle);

                    float       *outPtr;
                    outPtr =  rotateMatOfObjectAngle.data();

                    if (fabs(drawAngle_Inc.phi()) < 0.00001)
                        drawAngle_Inc.setPhi(0);
                    if (fabs(drawAngle_Inc.theta()) < 0.00001)
                        drawAngle_Inc.setTheta(0);
                    if (fabs(drawAngle_Inc.psi()) < 0.00001)
                        drawAngle_Inc.setPsi(0);
                    if (fabs(drawAngle_Dec.phi()) < 0.00001)
                        drawAngle_Dec.setPhi(0);
                    if (fabs(drawAngle_Dec.theta()) < 0.00001)
                        drawAngle_Dec.setTheta(0);
                    if (fabs(drawAngle_Dec.psi()) < 0.00001)
                        drawAngle_Dec.setPsi(0);
                    if (fabs(drawAngle_M.phi()) < 0.00001)
                        drawAngle_M.setPhi(0);
                    if (fabs(drawAngle_M.theta()) < 0.00001)
                        drawAngle_M.setTheta(0);
                    if (fabs(drawAngle_M.psi()) < 0.00001)
                        drawAngle_M.setPsi(0);

                    if (((fabs(drawAngle_M.phi() - drawAngle_Inc.phi()) < 5) &&
                         (fabs(drawAngle_M.theta() - drawAngle_Inc.theta()) < 5) &&
                         (fabs(drawAngle_M.psi() - drawAngle_Inc.psi()) < 5) ) ||
                        (((fabs(drawAngle_M.phi()) - 90) > 5) &&
                         (fabs(drawAngle_M.phi() + drawAngle_Inc.phi()) < 5) &&
                         (fabs(drawAngle_M.theta() - drawAngle_Inc.theta()) < 5) &&
                         (fabs(drawAngle_M.psi() - drawAngle_Inc.psi()) < 5) ) ||
                        ((fabs(drawAngle_M.phi() - drawAngle_Inc.phi()) < 5) &&
                         ((fabs(drawAngle_M.theta()) - 90) > 5) &&
                         (fabs(drawAngle_M.theta() + drawAngle_Inc.theta()) < 5) &&
                         (fabs(drawAngle_M.psi() - drawAngle_Inc.psi()) < 5) ) ||
                        ((fabs(drawAngle_M.phi() - drawAngle_Inc.phi()) < 5) &&
                         (fabs(drawAngle_M.theta() - drawAngle_Inc.theta()) < 5) &&
                         ((fabs(drawAngle_M.psi()) - 90) > 5) &&
                         (fabs(drawAngle_M.psi() + drawAngle_Inc.psi()) < 5) ) )
                    {
                        resCorrect = true;
                    } else
                    {
                        if (((fabs(drawAngle_M.phi() - drawAngle_Dec.phi()) < 5) &&
                             (fabs(drawAngle_M.theta() - drawAngle_Dec.theta()) < 5) &&
                             (fabs(drawAngle_M.psi() - drawAngle_Dec.psi()) < 5) ) ||
                            (((fabs(drawAngle_M.phi()) - 90) > 5) &&
                             (fabs(drawAngle_M.phi() + drawAngle_Dec.phi()) < 5) &&
                             (fabs(drawAngle_M.theta() - drawAngle_Dec.theta()) < 5) &&
                             (fabs(drawAngle_M.psi() - drawAngle_Dec.psi()) < 5) ) ||
                            ((fabs(drawAngle_M.phi() - drawAngle_Dec.phi()) < 5) &&
                             ((fabs(drawAngle_M.theta()) - 90) > 5) &&
                             (fabs(drawAngle_M.theta() + drawAngle_Dec.theta()) < 5) &&
                             (fabs(drawAngle_M.psi() - drawAngle_Dec.psi()) < 5) ) ||
                            ((fabs(drawAngle_M.phi() - drawAngle_Dec.phi()) < 5) &&
                             (fabs(drawAngle_M.theta() - drawAngle_Dec.theta()) < 5) &&
                             ((fabs(drawAngle_M.psi()) - 90) > 5) &&
                             (fabs(drawAngle_M.psi() + drawAngle_Dec.psi()) < 5) ) )
                        {
                            resCorrect = true;
                        }
                    }

                    if (true)
                    {
                        if (resCorrect != true)
                        {
                            std::cout << "(object Angle) phi=" << objAngle1.phi() << ", theta=" << objAngle1.theta() << ", psi=" << objAngle1.psi();

                            std::cout << " (draw Angle_M) phi=" << drawAngle_M.phi() << ", theta=" << drawAngle_M.theta() << ", psi=" << drawAngle_M.psi();
                            std::cout << " (draw Angle_I) phi=" << drawAngle_Inc.phi() << ", theta=" << drawAngle_Inc.theta() << ", psi=" << drawAngle_Inc.psi();
                            std::cout << " (draw Angle_D) phi=" << drawAngle_Dec.phi() << ", theta=" << drawAngle_Dec.theta() << ", psi=" << drawAngle_Dec.psi();
                            std::cout << " (result) Not equal !!!" << std::endl;
                        } else
                        {
//                                    std::cout << " (result) Equal !!!" << std::endl;
                        }

                        if (resCorrect != true)
                        {
                            for (int i = 0; i < 9; i++)
                            {
                                if (fabs(outPtr[i]) < 0.000001)
                                    outPtr[i] = 0.0f;
                            }

                            std::cout << "rotateMat [1,1]=" << outPtr[0] << ", [1,2]=" << outPtr[3] << ", [1,3]=" << outPtr[6] << std::endl;
                            std::cout << " (invert) [2,1]=" << outPtr[1] << ", [2,2]=" << outPtr[4] << ", [2,3]=" << outPtr[7] << std::endl;
                            std::cout << "          [3,1]=" << outPtr[2] << ", [3,2]=" << outPtr[5] << ", [3,3]=" << outPtr[8] << std::endl;

                            int delta_x_I = drawAngle_Inc.phi() - drawAngle_M.phi();
                            int delta_y_I = drawAngle_Inc.theta() - drawAngle_M.theta();
                            int delta_z_I = drawAngle_Inc.psi() - drawAngle_M.psi();
                            if (abs(delta_x_I) > 5)
                                std::cout << " delta phi of Inc =" << delta_x_I;
                            if (abs(delta_y_I) > 5)
                                std::cout << " delta theta of Inc =" << delta_y_I;
                            if (abs(delta_z_I) > 5)
                                std::cout << " delta psi of Inc =" << delta_z_I;

                            int delta_x_D = drawAngle_Dec.phi() - drawAngle_M.phi();
                            int delta_y_D = drawAngle_Dec.theta() - drawAngle_M.theta();
                            int delta_z_D = drawAngle_Dec.psi() - drawAngle_M.psi();
                            if (abs(delta_x_D) > 5)
                                std::cout << "   delta phi of Dec =" << delta_x_D;
                            if (abs(delta_y_D) > 5)
                                std::cout << " delta theta of Dec =" << delta_y_D;
                            if (abs(delta_z_D) > 5)
                                std::cout << " delta psi of Dec =" << delta_z_D << std::endl << std::endl;
                        }
                    }
                }
            }
        }
    }
#endif
    */


    eulerf      objectAngle;
    if (object)
        objectAngle = object->Orientation();
    else
        objectAngle = vec3f(0, 0, 0);

    quatf       drawQuat = quaternionsFromEulerAngles(drawAngle);
    quatf       objectQuat = quaternionsFromEulerAngles(objectAngle);
//            float       *outPtr;
//                std::cout << "NEGLWidget::drawBoundingBox drawAngle(0) phi=" << drawAngle.phi() << ", theta=" << drawAngle.theta() << ", psi=" << drawAngle.psi() << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox drawQuat     x=" << drawQuat.x() << ", y=" << drawQuat.y() << ", z=" << drawQuat.z() << ", scalar=" << drawQuat.scalar() << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox object Angle phi=" << object->Orientation().phi() << ", theta=" << object->Orientation().theta() << ", psi=" << object->Orientation().psi() << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox objectQuat   x=" << objectQuat.x() << ", y=" << objectQuat.y() << ", z=" << objectQuat.z() << ", scalar=" << objectQuat.scalar() << std::endl;
    matrix33f rotateMatOfDrawAlgne = matrixFromQuaternions(drawQuat);
//                outPtr =  rotateMatOfDrawAlgne.data();
//                std::cout << "NEGLWidget::drawBoundingBox rotateMat [1,1]=" << outPtr[0] << ", [1,2]=" << outPtr[3] << ", [1,3]=" << outPtr[6] << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox  (draw)   [2,1]=" << outPtr[1] << ", [2,2]=" << outPtr[4] << ", [2,3]=" << outPtr[7] << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox           [3,1]=" << outPtr[2] << ", [3,2]=" << outPtr[5] << ", [3,3]=" << outPtr[8] << std::endl;
    matrix33f rotateMatOfObjectAngle = matrixFromQuaternions(objectQuat);
//                outPtr =  rotateMatOfObjectAngle.data();
//                std::cout << "NEGLWidget::drawBoundingBox rotateMat [1,1]=" << outPtr[0] << ", [1,2]=" << outPtr[3] << ", [1,3]=" << outPtr[6] << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox  (object) [2,1]=" << outPtr[1] << ", [2,2]=" << outPtr[4] << ", [2,3]=" << outPtr[7] << std::endl;
//                std::cout << "NEGLWidget::drawBoundingBox           [3,1]=" << outPtr[2] << ", [3,2]=" << outPtr[5] << ", [3,3]=" << outPtr[8] << std::endl;
    rotateMatOfObjectAngle.invert();
    rotateMatOfObjectAngle = rotateMatOfObjectAngle * rotateMatOfDrawAlgne;
    drawAngle = eulerAnglesFromMatrix(rotateMatOfObjectAngle);
//                std::cout << "NEGLWidget::drawBoundingBox drawAngle(1) phi=" << drawAngle.phi() << ", theta=" << drawAngle.theta() << ", psi=" << drawAngle.psi() << std::endl << std::endl;

    return drawAngle;
}

void IGLSurface::bindScreenFrameBuffer()
{
    GL->glBindFramebuffer( GL_FRAMEBUFFER, defaultFramebuffer());
}

void IGLSurface::blitScene(int width, int height)
{
    // blit to screen
    GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_sceneFBO);
    GL->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameAndPickFBO);
    if( m_bfaaMultiplier > 1) {
        GL->glBlitFramebuffer(0,0,width*m_bfaaMultiplier,height*m_bfaaMultiplier,
                          0,0,width,height,GL_COLOR_BUFFER_BIT, GL_LINEAR);
        GL->glBlitFramebuffer(0,0,width*m_bfaaMultiplier,height*m_bfaaMultiplier,
                          0,0,width,height,GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_LINEAR);
    }
    else {
        GL->glBlitFramebuffer(0,0,width,height,
                          0,0,width,height,GL_COLOR_BUFFER_BIT, GL_NEAREST);
        GL->glBlitFramebuffer(0,0,width,height,
                          0,0,width,height,GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    }
    GL->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer());
}

void IGLSurface::glPrint3D( QString text, float x, float y, float z, float size /*= 0.2*/, Color32 color /*= Color32(255,255,255,255)*/ )
{
    matrix44f letterMatrix;
    letterMatrix.translate(x,y,z);

    worldStack().pushMatrix(letterMatrix);
    setModelViewMatrix();

    matrix44f bbMatrix = m_MVMatrix;

    for( int i=0; i<3; i++ )
        for(int j=0; j<3; j++ )
        {
            if ( i==j )
                bbMatrix(i,j) = 1.0;
            else
                bbMatrix(i,j) = 0.0;
        }

    bbMatrix.scale(size);
    setModelViewMatrix(bbMatrix);
    GL->glColor3ubv((GLubyte*)&color);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    NERENDERMAN.GLManager()->glPrint3D(text.toUtf8(), -1);

    worldStack().popMatrix();
}

void IGLSurface::drawAxes()
{
    if(!camera())
        return;

    viewStack().pushIdentity();
    worldStack().pushIdentity();

    matrix44f smallAxisTranslate;
    smallAxisTranslate.translate(32,32,0);
    matrix44f smallAxisRotation(camera()->pureRotation().transposed());
    viewStack().loadMatrix(smallAxisTranslate*smallAxisRotation);
    GL->glLineWidth( 1.0f );
    setModelViewMatrix();

    vec3f q3[2];

    GL->glEnableClientState(GL_VERTEX_ARRAY);

    GL->glColor4f(1,0,0,1);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    q3[0] = vec3f(0, 0, 0);
    q3[1] = vec3f(30, 0, 0);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
    GL->glDrawArrays(GL_LINE_LOOP,0,2);

    GL->glColor4f(0,1,0,1);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    q3[0] = vec3f(0, 0, 0);
    q3[1] = vec3f(0, 30, 0);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
    GL->glDrawArrays(GL_LINE_LOOP,0,2);

    GL->glColor4f(0,0,1,1);
    NERENDERMAN.GLManager()->updatePositionUniforms(this);
    q3[0] = vec3f(0, 0, 0);
    q3[1] = vec3f(0, 0, 30);
    GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
    GL->glDrawArrays(GL_LINE_LOOP,0,2);

    GL->glDisableClientState(GL_VERTEX_ARRAY);

    glPrint3D("x", 32, 0, 0, 12, Color32(255,0,0,255));
    glPrint3D("y", 0, 32, 0, 12, Color32(0,255,0,255));
    glPrint3D("z", 0, 0, 32, 12, Color32(0,0,255,255));

    viewStack().popMatrix();
    worldStack().popMatrix();
}

void IGLSurface::backupFrameBuffer(int width, int height, bool bSceneBuffer)
{
    if (isDepthPeeling() && (!bSceneBuffer)) //Depth Peeling OIT
        GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_OITBlenderFBO);
    else
        GL->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_sceneFBO);

    GL->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_tmpFBO);
    GL->glBlitFramebuffer(0,0,width,height,0,0,width,height,GL_COLOR_BUFFER_BIT, GL_NEAREST);
    GL->glBlitFramebuffer(0,0,width,height,0,0,width,height,GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

    if (isDepthPeeling() && (!bSceneBuffer)) //Depth Peeling OIT
        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_OITBlenderFBO);
    else
        GL->glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
}

void IGLSurface::renderScene(float width, float height)
{
    //QMutexLocker renderLocker(&m_renderMutex);
    std::vector<NEDirectionalLight*> dirLights = NERENDERMAN.GLManager()->getDirectionLights();
    std::vector<NESpotLight*> spotLights = NERENDERMAN.GLManager()->getSpotLights();
    std::vector<NELight*> lights;
    m_gizmoList.clear();
    m_CFASTList.clear();

    for (unsigned i = 0; i < dirLights.size(); i++)
        lights.push_back(dirLights[i]);
    for (unsigned i = 0; i < spotLights.size(); i++)
        lights.push_back(spotLights[i]);

    for(unsigned i = 0; i < lights.size(); i++)
    {
        if(lights[i]->ShadowMode() != NELight::NEShadowMode::NoShadow)
            renderSceneToLight(lights[i]);
    }

    // Update the opengl width and height in the gl manager
    m_glWidth = width;
    m_glHeight = height;

    // Switch to the regular front and back buffers
    //GL->glDrawBuffer( GL_FRONT_AND_BACK );

    // If there is a camera set for the widget
    if( camera() )
    {   // Update the gl manager with the camera
        if(!FAreEqual(m_FovOld, camera()->Fov())
                || !FAreEqual(m_NearClipOld, camera()->NearClip())
                || !FAreEqual(m_FarClipOld, camera()->FarClip())
                || !FAreEqual(m_OrthoWidthOld, camera()->OrthoWidth()))
        {
            setCamera( camera() );
        }
    }

    // Change some of the opengl state
    NERENDERMAN.GLManager()->setDepthTest( kLessDepthTest );
    NERENDERMAN.GLManager()->setShadeMode( kGouraudShaded );
    NERENDERMAN.GLManager()->setFillMode( kSolidFrontAndBack );
    NERENDERMAN.GLManager()->setBlendFunc( kNoBlendFunc );

    bindSceneFrameBuffer();

    //
    // Clear the background
    //
    QColor bgColor = backgroundColor();
    GL->glClearColor( bgColor.redF(), bgColor.greenF(), bgColor.blueF(), bgColor.alphaF() );
    GL->glClearDepth(1.0f);
    GL->glClearStencil(0);

    // Enable general anti-aliasing and point smoothing
    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    GL->glEnable( GL_MULTISAMPLE );
    GL->glEnable( GL_POINT_SMOOTH );
    GL->glLineWidth( 1.0f );
    GL->glPointSize( 1.0f );

    //
    // Clear color & depth buffers
    //
    GL->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Push the projection matrix from the active camera into the projectionStack
    if( camera() )
        camera()->computeProjection( width, height );
    if( camera() )
        projectionStack().loadMatrix( camera()->projectionMatrix() );


    // Supply the projection matrix to OpenGL
    setProjectionMatrix();


    viewStack().pushIdentity();
    // If there is an active camera, load its view matrix into the view matrix stack
    if( camera() )
    {
        viewStack().loadMatrix(camera()->viewMatrix());
        NERENDERMAN.GLManager()->viewMatrix( camera()->viewMatrix() );
    }

    // Enable clipping planes
    worldStack().pushMatrix(matrix44f());

    //setWorldMatrix();
    setModelViewMatrix();

    // Set viewport for BFAA
    GL->glViewport( 0, 0, width*m_bfaaMultiplier, height*m_bfaaMultiplier);
    m_glWidth = width*m_bfaaMultiplier;
    m_glHeight = height*m_bfaaMultiplier;

    // Enable depth test to correctly draw the overlapping  primitives

    drawBackgroundGradient();
    drawBackgroundTexture();
    drawBackgroundReal();

    // Convert the source HDR image onto a cubemap texture and create irradiance map
    if (!m_bufferRendered)
    {
        if (EnvironmentTypeSphere())
            createCubemapAndIrradianceMap(m_EnvironmentMap);
        else if (EnvironmentTypeCube())
            createIrradianceMap(m_SkyBoxTexture);
    }
    if (environmentRender())
    {
        drawEnvironmentCube();
        drawEnvironmentSphere();
    }


    NERENDERMAN.GLManager()->setDepthTest( kLessEqualDepthTest );

    worldStack().popMatrix();

    NERENDERMAN.GLManager()->setClipPlanesForSquare(true);
    beginTransparency(width, height);

    // Normal 3D scene rendering
    renderSpatials(NESpatial::kRigid);

    // doTransparency();
    // endTransparency();
    backupFrameBuffer(width*m_bfaaMultiplier, height*m_bfaaMultiplier, false);

    // Ambient occlusion
    NERENDERMAN.GLManager()->setFillMode( NEFillMode::kSolidFrontAndBack );
    // Enable SSAO when it's enabled, quality profile is high enough, and scene is not dynamic
    if( ssao() == true && checkAOQualityProfile() && !isSceneDynamic() && !isDepthPeeling())
    {
        // Adapt AO according to the scene size
        AABB sceneAABB = NESCENE.getSceneAABB();
        vec3f sceneScale = sceneAABB.maxPos() - sceneAABB.minPos();
        float sceneSize = pow(sceneScale.x() * sceneScale.y() * sceneScale.z(), 1.0/3);

        setHBAOParameter(
                    intensity(),
                    radius() * sceneSize / 10,
                    blurSharpness());

        drawAO(width*m_bfaaMultiplier, height *m_bfaaMultiplier);
    }

    // Water rendering
    beginWaterRendering(width*m_bfaaMultiplier, height*m_bfaaMultiplier);
    bindSceneFrameBuffer();
    renderSpatials(NESpatial::kFluid);
    endWaterRendering();

    bindSceneFrameBuffer();
    doTransparency(width, height);
    endTransparency();

    bindSceneFrameBuffer();
    renderSpatials(NESpatial::kGVDB);

    NERENDERMAN.GLManager()->setClipPlanesForSquare(false);

    // For different orthographic angles, change the orientation of the floor grid
    matrix44f rotationMatrix;

    if( camera() )
    {
        if( camera()->OrthoType() == NECamera::Front )
            rotationMatrix.rotate( -90, 1, 0, 0 );
        else if( camera()->OrthoType() == NECamera::Left )
            rotationMatrix.rotate( 90, 0, 0, 1 );
    }

    if( drawGrid() )
    {
        worldStack().pushMatrix( rotationMatrix );
        setModelViewMatrix();

        worldStack().popMatrix();
        GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
        if( overlayGrid() )
            NERENDERMAN.GLManager()->setDepthTest( kDisableDepthTest );
        drawFloor();
        GL->glPopAttrib();
        setModelViewMatrix();
    }


    //glPushAttrib();
    GL->glEnable(GL_BLEND);
    GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderSpatials(NESpatial::kClip);
    GL->glDisable(GL_BLEND);

    if( camera() )
    {
        if( camera()->OrthoType() == NECamera::Front )
            rotationMatrix.rotate( -90, 1, 0, 0 );
        else if( camera()->OrthoType() == NECamera::Left )
            rotationMatrix.rotate( 90, 0, 0, 1 );
    }

    worldStack().pushMatrix( rotationMatrix );
    setModelViewMatrix();
    worldStack().popMatrix();
    GL->glPushAttrib( GL_ALL_ATTRIB_BITS );
    int bgType = backgroundType();
    if(bgType == 2 /*Real */){
        QColor realColor = backgroundRealColor();
        GL->glEnable(GL_BLEND);
        GL->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        AABB sceneAABB = NESCENE.getSceneAABB();
        vec3f dist =  sceneAABB.maxPos() - sceneAABB.minPos();

        float maxdis = dist.length();

        NERENDERMAN.GLManager()->drawPedestal(this, realColor, m_MVPMatrix, maxdis);
        GL->glDisable(GL_BLEND);
    }
    GL->glPopAttrib();


//    setModelViewMatrix();
    renderSpatials(NESpatial::kGizmo);
    // Render all transparent gizmos
    drawAllObjectGizmos();
    drawAllCFASTPlanes();


    worldStack().pushMatrix(matrix44f());
    setModelViewMatrix();
    worldStack().popMatrix();
    drawSnapPt();
    drawMeasurement();

    // draw bounding box of spatials
    NERENDERMAN.GLManager()->bindPositionShader(this);
    drawBoundingBox(width, height);

    // Post processing (FXAA, SMAA) and render to the screen
    blitScene(width, height);
    bindScreenFrameBuffer();
    // Restore viewport
    GL->glViewport( 0, 0, width, height);
    m_glWidth = width;
    m_glHeight = height;
    drawPostProcessing(width, height);

    GL->glViewport( 0, 0, width, height);
    // Render 2D spatial OSD
    GL->glClear( GL_DEPTH_BUFFER_BIT );

    NERENDERMAN.GLManager()->setDepthTest( kDisableDepthTest );
    projectionStack().pushMatrix( osdOrthoMatrix );
    setProjectionMatrix();
    projectionStack().popMatrix();
    renderSpatials(NESpatial::kOSD);
    setProjectionMatrix();

    NERENDERMAN.GLManager()->bindPositionShader(this);
    // Draw the manipulator for the object if the object is a spatial and non-static
    if( NESCENE.selectedObject() && NESCENE.selectedObject()->inherits("NESpatial"))
    {
        NERENDERMAN.GLManager()->setDepthTest( kLessDepthTest );
        worldStack().pushMatrix( matrix44f() );
        setModelViewMatrix();
        worldStack().popMatrix();

        drawManipulator( (NESpatial*) NESCENE.selectedObject() );
    }

    // Disable depth test to draw 2D osd contents on top
    NERENDERMAN.GLManager()->setDepthTest( kDisableDepthTest );

    // Draw the 2D OSD
    // Since textured fonts are being used, change fill mode to solid
    NERENDERMAN.GLManager()->setFillMode( NEFillMode::kSolidFrontAndBack );
    // Push all previous matrix stacks
    projectionStack().pushMatrix( osdOrthoMatrix );
    viewStack().pushIdentity();
    worldStack().pushIdentity();
    setProjectionMatrix();
    setModelViewMatrix();

    //glUseProgram( 0 );
    //glEnable( GL_TEXTURE_2D );
    drawOSDMessage(width, height);

    if(drawRenderArea())	// Draw the render area
    {
        GL->glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
        GL->glLineWidth(2.0f);

        NERENDERMAN.GLManager()->updatePositionUniforms(this);

        float xLeft = 0.5*(static_cast<float>(getWidth())-GlSize().width());
        float xRight = 0.5*(static_cast<float>(getWidth())+GlSize().width());
        float yBottom = 0.5*(static_cast<float>(getHeight())-GlSize().height());
        float yTop = 0.5*(static_cast<float>(getHeight())+GlSize().height());
        vec3f q3[4];
        q3[0] = vec3f(xLeft, yBottom, 0);
        q3[1] = vec3f(xRight, yBottom, 0);
        q3[2] = vec3f(xRight, yTop, 0);
        q3[3] = vec3f(xLeft, yTop, 0);

        GL->glEnableClientState(GL_VERTEX_ARRAY);
        GL->glVertexPointer(3, GL_FLOAT, 0, (float*)&q3);
        GL->glDrawArrays(GL_LINE_LOOP,0,4);
        GL->glDisableClientState(GL_VERTEX_ARRAY);

        GL->glPopAttrib();
    }

    NERENDERMAN.GLManager()->setDepthTest( kLessDepthTest );

    // Draw the coordinate axes
    if( viewAxes() )
        drawAxes();

    viewStack().popMatrix();

    // Pop all matrix stacks back
    projectionStack().popMatrix();
    viewStack().popMatrix();
    worldStack().popMatrix();

    GL->glPopAttrib();
    m_bufferRendered = true;
}


// Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
QOpenGLTexture* loadImage(const char* theFileName)
{
    // Load cube.png image
    QOpenGLTexture* texture = new QOpenGLTexture(QImage(theFileName).mirrored());
    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    // Wrap textureENV coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
    return texture;
}

bool IGLSurface::loadBackgroundTexture( QString path )
{
    if(path.isEmpty())
        return false;
    m_BackgroundTexture = loadImage( path.toUtf8() );
    return true;
}

void IGLSurface::computeCellSize()
{
    m_CellSize = QSizeF(m_GridSize.width()/m_CellCount.width(), m_GridSize.height()/m_CellCount.height());
}

NEGLOfflineRenderer::NEGLOfflineRenderer(QObject* camera, float width, float height, QString &fileName, float framerate, float speed) : QObject(NULL),
    IGLSurface(true)
{
    if(fileName.contains(".png", Qt::CaseInsensitive))
        m_bOneFrame = true;
    else
        m_bOneFrame = false;

    m_Recorder = new FFMpegMovieMerger;
    m_bMerge = false;
    m_Recorder->m_bMerge = false;


    m_Recorder->m_oStream = { 0 };
    m_Recorder->m_fileName = fileName;
    m_Recorder->m_videoCodec = NULL;
    m_Recorder->m_frameRate = framerate;
    m_Recorder->m_width = width;
    m_Recorder->m_height = height;


    m_frameRate = framerate;
    m_speedFactor = speed;

    m_width = width;
    m_height = height;

    m_frameBuffer = new GLubyte[(int)m_width * (int)m_height * 3];

    m_FrameRatio = 0;

//    connect( this, SIGNAL( finished() ), this, SLOT( deleteLater()) );
//    connect(this, SIGNAL( frameReady(void*)), this, SLOT(processFrame(void*)));

    QOpenGLContext *curCtx = QOpenGLContext::currentContext();
    QSurface *surf = curCtx->surface();

    m_offlineContext->makeCurrent(m_offlineSurface);
    initializeGLSurface();
    setGLSurfaceSize(m_width, m_height, 1);

    setCamera(camera);
    if(!m_bOneFrame){
        m_Recorder->initializeFfmpeg();
    }
    curCtx->makeCurrent(surf);
}

void NEGLOfflineRenderer::renderOneFrame()
{
    QOpenGLContext *curCtx = QOpenGLContext::currentContext();
    QSurface *surf = curCtx->surface();
    m_offlineContext->makeCurrent(m_offlineSurface);

    if(aaType() == AntialiasingType::Fxaa || aaType() == AntialiasingType::Csaa ){
        renderScene(m_width, m_height);
    }

    renderScene(m_width, m_height);
    GL->glPixelStorei(GL_PACK_ALIGNMENT, 1);
    GL->glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
    QImage img(m_frameBuffer, m_width, m_height, QImage::Format_RGB888);
    img = img.mirrored(false, true);
    img.save(m_Recorder->m_fileName);

    curCtx->makeCurrent(surf);
}


NEGLOfflineRenderer::NEGLOfflineRenderer(QRect layout, QString &fileName, float framerate, float speed, FFMpegMovieMerger *recorder, int index) : QObject(NULL),
    IGLSurface(true)
{

    if(fileName.contains(".png", Qt::CaseInsensitive))
        m_bOneFrame = true;
    else
        m_bOneFrame = false;

    m_layout = layout;

    m_Recorder = recorder;
    m_bMerge = true;
    m_Recorder->m_bMerge = true;
    m_index = index;

    m_frameRate = framerate;
    m_speedFactor = speed;

    m_width = layout.size().width();
    m_height = layout.size().height();

    m_frameBuffer = new GLubyte[(int)m_width * (int)m_height * 3];

    m_FrameRatio = 0;

//    connect( this, SIGNAL( finished() ), this, SLOT( deleteLater()) );
//    connect(this, SIGNAL( frameReady(void*)), this, SLOT(processFrame(void*)));

    QOpenGLContext *curCtx = QOpenGLContext::currentContext();
    QSurface *surf = curCtx->surface();

    m_offlineContext->makeCurrent(m_offlineSurface);
    initializeGLSurface();
    setGLSurfaceSize(m_width, m_height, 1);
    curCtx->makeCurrent(surf);
}

NEGLOfflineRenderer::~NEGLOfflineRenderer()
{
    if(!m_bOneFrame){
        if(!m_bMerge || m_index == NERENDERMAN.m_offlineRenderers.size() - 1 ){
            m_Recorder->finalizeFfmpeg();
            delete m_Recorder;
        }
    }
    delete m_frameBuffer;
    m_frameBuffer = NULL;
}

void NEGLOfflineRenderer::copyPropertiesFromWidget(IGLSurface *widget)
{

    setTransparencyType(widget->transparencyType());
    setSsao(widget->ssao());
    setAaType(widget->aaType());
    setBackgroundType(widget->backgroundType());
    setBackgroundColor(widget->backgroundColor());
    setBackgroundTexture(widget->backgroundTexture());
    setDrawGrid(widget->drawGrid());

}

void NEGLOfflineRenderer::update()
{
    QOpenGLContext *curCtx = QOpenGLContext::currentContext();
    QSurface *surf = NULL;
    if(curCtx)
        surf = curCtx->surface();

    m_offlineContext->makeCurrent(m_offlineSurface);
    setDrawGrid(NERENDERMAN.bDrawGrid());
    renderScene(m_width, m_height);
    GL->glPixelStorei(GL_PACK_ALIGNMENT, 1);
    GL->glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
#if 0
    QImage img(m_frameBuffer, m_width, m_height, QImage::Format_RGB888);
    img.save("offline.png");
#endif
    if(curCtx)
        curCtx->makeCurrent(surf);
    //emit frameReady(m_frameBuffer);
    processFrame(m_frameBuffer);
}

void NEGLOfflineRenderer::processFrame(GLubyte *fbuffer)
{
    m_FrameRatio += m_frameRate / NESCENE.RecordFrameRate() / m_speedFactor;

    if(m_FrameRatio > 0.99){
        while(m_FrameRatio > 0.99 ){
            if(!m_bMerge || NERENDERMAN.m_offlineRenderers.size() <= 1)
                m_Recorder->write_video_frame(m_Recorder->m_oContext, &m_Recorder->m_oStream, fbuffer);
            else{
                m_Recorder->fillMergedFrame( m_layout, fbuffer );
                if(m_index == NERENDERMAN.m_offlineRenderers.size() - 1){
                    m_Recorder->write_video_frame(m_Recorder->m_oContext, &m_Recorder->m_oStream, m_Recorder->m_totalFrameBuffer);
                }
            }
            m_FrameRatio -= 1.0;
        }
    }
}



FFMpegMovieMerger::FFMpegMovieMerger()
{

}
FFMpegMovieMerger::~FFMpegMovieMerger()
{
    if(m_bMerge && NERENDERMAN.m_offlineRenderers.size() > 1){
        delete m_totalFrameBuffer;
        m_totalFrameBuffer = NULL;
    }

}

#define STREAM_FRAME_RATE 50 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
#define SCALE_FLAGS SWS_BICUBIC


#undef av_err2str
#define av_err2str(errnum) \
av_make_error_string((char*) alloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)

#undef av_ts2str
#define av_ts2str(ts) av_ts_make_string((char*)alloca(AV_ERROR_MAX_STRING_SIZE), ts)

#undef av_ts2timestr
#define av_ts2timestr(ts, tb) av_ts_make_time_string((char*)alloca(AV_ERROR_MAX_STRING_SIZE), ts, tb)


int FFMpegMovieMerger::initializeFfmpeg()
{

    if(m_bMerge)
        m_totalFrameBuffer = new GLubyte[(int)m_width * (int)m_height * 3];
//    avformat_alloc_output_context2(&m_oContext, NULL, NULL, m_fileName.toLocal8Bit().data());
//    if (!m_oContext) {
//        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&m_oContext, NULL, "avi", m_fileName.toLocal8Bit().data());
//    }
    if (!m_oContext)
        return 1;

    m_oFormat = m_oContext->oformat;

    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (m_oFormat->video_codec != AV_CODEC_ID_NONE) {
        add_stream(&m_oStream, m_oContext, &m_videoCodec, m_oFormat->video_codec);
    }

    AVDictionary *opt = NULL;
    // loss-less
    av_dict_set(&opt, "preset", "ultrafast", 0);
    //av_dict_set(&opt, "profile", "baseline", 0);
    // To ensure compatibility, you should always therefore add
    // the option -pix_fmt yuv420p and choose a lossy encoding method,
    // ideally by using CRF encoding, and setting -crf to a reasonable value.
    // CRF default is 23, but anything from 18–28 is fine, where lower means better quality.
    av_dict_set(&opt, "crf", "23", 0);
    //av_dict_set(&opt, "crf", "0", 0);
    //av_dict_set(&opt, "qp", "0", 0);
    av_dict_set(&opt, "tune", "film", 0);

    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    open_video(m_oContext, m_videoCodec, &m_oStream, opt);

    av_dump_format(m_oContext, 0, m_fileName.toLocal8Bit().data(), 1);

    /* open the output file, if needed */
    if (!(m_oFormat->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&m_oContext->pb, m_fileName.toLocal8Bit().data(), AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            fprintf(stderr, "Could not open '%s': %s\n", m_fileName.toLocal8Bit().data(), av_err2str(ret));
            return 1;
        }
    }

    /* Write the stream header, if any. */
    int ret = avformat_write_header(m_oContext, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Error occurred when opening output file: %s\n", av_err2str(ret));
        return 1;
    }
    return 0;
}

void FFMpegMovieMerger::finalizeFfmpeg()
{
    /* Write the trailer, if any. The trailer must be written before you
    * close the CodecContexts open when you wrote the header; otherwise
    * av_write_trailer() may try to use memory that was freed on
    * av_codec_close(). */
    av_write_trailer(m_oContext);

    /* Close each codec. */
    close_stream(m_oContext, &m_oStream);

    if (!(m_oFormat->flags & AVFMT_NOFILE))
       /* Close the output file. */
       avio_closep(&m_oContext->pb);

    /* free the stream */
    avformat_free_context(m_oContext);

}

void FFMpegMovieMerger::fillMergedFrame(QRect layout, GLubyte *framebuffer){

    for(int j = layout.top(); j < layout.bottom(); j++ ){
        int offset = (j * m_width + layout.left()) * 3;
        memcpy(m_totalFrameBuffer + offset, framebuffer + 3 * layout.width() * (j - layout.top()), layout.width() * 3);
    }
}

/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
int FFMpegMovieMerger::write_video_frame(AVFormatContext *oc, OutputStream *ost, void *framebuffer)
{
    int ret;
    AVCodecContext *c;
    AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = { 0 };

    c = ost->enc;

    frame = get_video_frame(ost, framebuffer);

    if (frame == 0)
        return 1;

    av_init_packet(&pkt);

    /* encode the image */
#if  1
    ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
#else
    if (c->codec_type == AVMEDIA_TYPE_VIDEO ) {
        ret = avcodec_send_frame(c, frame);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        }
        else {
            if (ret >= 0)
                pkt.size = 0;
            ret = avcodec_receive_packet(c, &pkt);
            if (ret >= 0)
                got_packet = 1;
        }
    }
#endif
    if (ret < 0) {
        fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
        exit(1);
    }

    if (got_packet) {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
    } else {
        ret = 0;
    }

    if (ret < 0) {
        fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
        exit(1);
    }

    return (frame || got_packet) ? 0 : 1;
}

void FFMpegMovieMerger::log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

int FFMpegMovieMerger::write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    //log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

AVFrame *FFMpegMovieMerger::get_video_frame(OutputStream *ost, void *framebuffer)
{
    AVCodecContext *c = ost->enc;

    /* check if we want to generate more frames */
    //    if (av_compare_ts(ost->next_pts, c->time_base,
    //                      STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
    //        return NULL;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(ost->frame) < 0)
        exit(1);

    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        //        if (!ost->sws_ctx) {
        //            ost->sws_ctx = sws_getContext(c->width, c->height,
        //                                          AV_PIX_FMT_YUV420P,
        //                                          c->width, c->height,
        //                                          c->pix_fmt,
        //                                          SCALE_FLAGS, NULL, NULL, NULL);
        //            if (!ost->sws_ctx) {
        //                fprintf(stderr,
        //                        "Could not initialize the conversion context\n");
        //                exit(1);
        //            }
        //        }
        //        //fill_yuv_image(ost->tmp_frame, ost->next_pts, c->width, c->height);
        //        sws_scale(ost->sws_ctx, (const uint8_t * const *) ost->tmp_frame->data,
        //                  ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
        //                  ost->frame->linesize);
    } else {
        if (!ost->sws_ctx) {
            ost->sws_ctx = sws_getContext(c->width, c->height,
                                          AV_PIX_FMT_RGB24,
                                          c->width, c->height,
                                          AV_PIX_FMT_YUV420P,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }

        }

        //flip processing.
        for(int i = 0; i < c->height; i++)
        {
            memcpy((unsigned char *)ost->tmp_frame->data[0] + i * ost->tmp_frame->linesize[0], (unsigned char*)framebuffer + (c->height - i - 1) * c->width * 3, c->width * 3 );
        }

        //memcpy(ost->tmp_frame->data[0], framebuffer, c->width * c->height * 3);
        sws_scale(ost->sws_ctx, (const uint8_t * const *) ost->tmp_frame->data,
                  ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
                  ost->frame->linesize);

    }

    ost->frame->pts = ost->next_pts++;
    ost->frame->pkt_pts = ost->frame->pts;

    return ost->frame;
}

/* Add an output stream. */
void FFMpegMovieMerger::add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id)
{
    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt  = (*codec)->sample_fmts ?
                    (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = 64000;
        c->sample_rate = 44100;
        if ((*codec)->supported_samplerates) {
            c->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                if ((*codec)->supported_samplerates[i] == 44100)
                    c->sample_rate = 44100;
            }
        }
        c->channels  = av_get_channel_layout_nb_channels(c->channel_layout);
        c->channel_layout = AV_CH_LAYOUT_STEREO;
        if ((*codec)->channel_layouts) {
            c->channel_layout = (*codec)->channel_layouts[0];
            for (i = 0; (*codec)->channel_layouts[i]; i++) {
                if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    c->channel_layout = AV_CH_LAYOUT_STEREO;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        ost->st->time_base = (AVRational){ 1, c->sample_rate };
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        //c->bit_rate = 400000;
        //c->bit_rate = 685000;
        c->bit_rate = 1500 * 1000 * 4 * m_frameRate / 25.0 ; // 1500kbps
        /* Resolution must be a multiple of two. */
        c->width    = m_width;
        c->height   = m_height;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        //ost->st->time_base = (AVRational){ 1, STREAM_FRAME_RATE };
        ost->st->time_base = (AVRational){ 100, (int)(m_frameRate * 100.f) };
        c->time_base       = ost->st->time_base;

        c->gop_size      = 12; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = STREAM_PIX_FMT;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B-frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
        break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void FFMpegMovieMerger::open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* allocate and init a re-usable frame */
    ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    ost->tmp_frame = NULL;
    ost->tmp_frame = alloc_picture(AV_PIX_FMT_RGB24, c->width, c->height);
    if (!ost->tmp_frame) {
        fprintf(stderr, "Could not allocate temporary picture\n");
        exit(1);
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}

AVFrame *FFMpegMovieMerger::alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

void FFMpegMovieMerger::close_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}
