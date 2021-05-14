#pragma once

class QOpenGLShader;
class QOpenGLTexture;
class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFunctions;
class QOpenGLFunctions_3_3_Compatibility;
class QOpenGLFunctions_4_1_Compatibility;
class QOpenGLFunctions_4_3_Compatibility;
class QOpenGLExtension_EXT_direct_state_access;
class QOpenGLExtension_NV_texture_multisample;

class NEIndexedTriMesh;
class NEGLRenderableMesh;
class NEMaterial;
class NECamera;
class NELight;
class NEMatrixStack4x4;
class NEVertexBuffer;
class NESpatial;
class NESpatialMesh;

#include "Utility/FilePath.h"


struct FluidRenderer
{
    GLuint mDepthFbo;
    GLuint mColorTex;
    GLuint mDepthBuf;
    GLuint mDepthTex;
    GLuint mDepthSmoothTex;
    GLuint mSceneFbo;
    GLuint mSceneTex;
    GLuint mReflectTex;

    GLuint mThicknessFbo;
    GLuint mThicknessTex;

    int mSceneWidth;
    int mSceneHeight;
};


class NECEXP_NERENDERING IGLSurface
{
protected:

    //! Some camera related parameters which should trigger resizeGL function when changed
    float m_FovOld;
    float m_NearClipOld;
    float m_FarClipOld;
    float m_OrthoWidthOld;

    //! Stores a pointer to the active camera of this widget.
    NECamera* m_pCamera;

    //! The matrices below are kept updated for convenience
    matrix44f osdOrthoMatrix;	// The orthogonal projection matrix for Osd related output.
    matrix44f m_MVPMatrix;		// The model-view-projection matrix.
    matrix44f m_MVMatrix;		// The view-projection matrix.
    matrix44f m_NormalMatrix;	// The normal matrix for transforming the normals.
    //! The matrix stacks below replace the matrix stacks deprecated from OpenGL
    NEMatrixStack4x4* m_pWorldStack;		// Stack for keeping world transformations.
    NEMatrixStack4x4* m_pViewStack;		// Stack for keeping view transformation.
    NEMatrixStack4x4* m_pProjectionStack;	// Stack for keeping projection transformation.

    QOpenGLTexture* m_BackgroundTexture;
    QOpenGLTexture* m_SkyBoxTexture;
    QOpenGLTexture* m_EnvironmentMap;

    float m_width;
    float m_height;
    float m_lastWidth; 	//!< The screen size of last framebuffer
    float m_lastHeight; 	//!<

    float m_glWidth;
    float m_glHeight;
    // Pointer to the ground grid
    NEVertexBuffer* floorVerts;
    QList<vec3f> verticesLvl1;
    QList<vec3f> verticesLvl2;

    QOpenGLContext *m_offlineContext;
    QOffscreenSurface *m_offlineSurface;
    bool m_bOffline;


    vec3f m_snapPt;
    NESpatialMesh* m_firstSnapObject;
    NESpatialMesh* m_snapObject;
    int m_snapPtId;
    QPair<int, int> m_snapEdgeId;
    bool m_bSnap = false;
    bool m_bSnapEdge = false;
    bool m_snapMode = 0; /*0:snap_point, 1:snap_edge*/

    enum MeasureStep
    {
        StandBy,
        FirstPt,
        SecondPt,
        DrawMeasurement
    };
    uint m_measureStep;
    QPair<NESpatialMesh*, int> m_measureFirstPt;
    QPair<NESpatialMesh*, int> m_measureSecondPt;
    QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>> m_measureFirstEdge;
    QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>> m_measureSecondEdge;
    QPair<vec3f, float> measureInfoMoved;


//    vec3f m_measureSecondPt;
//    vec3f m_measureFirstPt;
//    vec3f firstPt;
//    vec3f secondPt;
    vec3f m_measureLen;
    QPoint m_mousePos;
    int m_measureCnt;

    //QMutex m_renderMutex;


public:

    enum HbaoType
    {
        Hbao = 0,
        HbaoPlus = 1,
    };

    enum BackgroundType
    {
        Color = 0,
        Texture = 1,
        Real = 2
    };

    enum EnvironmentType
    {
        NoEnvironment = 0,
        Cubemap = 1,
        SphereMap = 2
    };

    enum ManipSpace
    {
        Local = 0,
        World = 1,
        View  = 2
    };

    enum AntialiasingType
    {
        None = 0,
        Msaa = 1,
        Fxaa = 2,
        Smaa = 3,
        Csaa = 4,
        Bfaa = 5
    };

    enum MsaaSampling
    {
        x2 = 1,
        x4 = 2,
        x8 = 3,
        x16 = 4
    };

    enum FxaaProfile
    {
        Fxaa_Low = 0,
        Fxaa_Medium,
        Fxaa_High,
        Fxaa_Extreme
    };

    enum CsaaMode
    {
        mode8x = 1,
        mode8xQ = 2,
        mode16x = 3,
        mode16xQ = 4
    };

    enum BfaaMode
    {
        multi2x = 1,
        multi4x = 2,
        multi8x = 3
    };

    enum GlobalTransparency
    {
        NoTransparency = 0,
        WeightBlendedTransparency,
        DepthPeelingTransparency,
        LinkedListTransparency
    };

    enum QualityProfile
    {
        LowQuality = 0,
        MedQuality,
        HighQuality
    };


    enum TrasparencyType
    {
        NoTransparencing = 0,
        WeightedBlending = 1,
        DepthPeelingInit = 2,
        DepthPeelingPeel = 3,
        LinkedListOIT = 4
    };



    TrasparencyType m_TransparencyStage;    
    std::vector<QPair<matrix44f, QPair< QString, QColor>>> m_gizmoList;
    std::vector<QPair<matrix44f, QPair<vec3f, vec3f>>> m_CFASTList;
    IGLSurface(bool boffline = false);
    ~IGLSurface();

    void initializeGLSurface();
    void setGLSurfaceSize(float width, float height, float pixelRatio);

    void buildFloor(const vec3f& center, const eulerf& orientation, const QSize& cellCount, const QSizeF& cellSize, const QSize& cellDivision);
    void freeTexture( QOpenGLTexture* texture );

    //! Returns the model-view matrix
    const matrix44f& modelViewMatrix() { return m_MVMatrix; }
    //! Returns the current (topmost) world transformation matrix in the worldStack.
    const matrix44f& worldMatrix();
    //! Sets the GL model-view transformation using the (topmost) world transformation matrix in the worldStack.
    void setWorldMatrix();
    //! Returns the current (topmost) view transformation matrix in the viewStack.
    const matrix44f& viewMatrix();
    //! Sets the GL model-view transformation using the (topmost) view transformation matrix in the viewStack.
    void setViewMatrix();
    //! Returns the current (topmost) projection transformation matrix in the projectionStack.
    const matrix44f& projectionMatrix();
    //! Sets the GL projection transformation from the (topmost) projection transformation matrix in the projectionStack.
    void setProjectionMatrix();
    //! Sets the GL model-view transformation using the (topmost) view and world matrices from their respective stacks.
    void setModelViewMatrix();
    //! Sets the given matrix as the current modelview matrix directly
    void setModelViewMatrix(matrix44f& m);
    //! Returns a reference to the stack which keeps world transformations
    NEMatrixStack4x4& worldStack() { return *m_pWorldStack; }
    //! Returns a reference to the stack which keeps view transformations
    NEMatrixStack4x4& viewStack() { return *m_pViewStack; }
    //! Returns a reference to the stack which keeps projection transformations
    NEMatrixStack4x4& projectionStack() { return *m_pProjectionStack; }


    //
    //! Load background Texture file
    //
    bool  loadBackgroundTexture( QString path );


    //! Initializes and destroy standard FBO and ambient occlusion related data
    bool createBuffer();
    void updateBuffer(int width, int height, int aaType);
    bool releaseBuffer();

    //! OpenGL stuff are rendered here
    void renderScene(float width, float height);

    //! OpengGL stuff are renderde here from the light's point of view
    void renderSceneToLight(NELight* light);

    virtual void drawText3D(QString text, vec3f pos, eulerf angle, float size, vec3f scale, Color32 color);
    virtual void drawText3D_new(QString text, vec3f pos, eulerf angle, float size, vec3f scale, Color32 color);

    // Standard buffers
    bool m_bufferCreated;
    bool m_bufferRendered; //!< indicate whether new buffer is rendered for pre-processing

    GLuint m_sceneFBO;
    GLuint m_sceneColorTexture, m_sceneDepthStencilTexture;

    // final Frame and Pick
    GLuint m_frameAndPickFBO;
    GLuint m_frameAndPickColorTexture, m_frameAndPickDepthStencilTexture;

    // offline final Framebuffer
    GLuint m_offlineFinalFBO;
    GLuint m_offlineFinalColorTexture, m_offlineFinalDepthStencilTexture;

    // Water shading
    FluidRenderer m_fluidRenderer;

    // HBAO Related

    // optimizes blur, by storing depth along with ssao calculation
    // avoids accessing two different textures
#define USE_AO_SPECIALBLUR          1

    // optimizes the cache-aware technique by rendering all temporary layers at once
    // instead of individually
#define AO_LAYERED_OFF    0
#define AO_LAYERED_IMAGE  1
#define AO_LAYERED_GS     2

#define USE_AO_LAYERED_SINGLEPASS   AO_LAYERED_GS
#define AO_RANDOMTEX_SIZE 4
#define HBAO_RANDOM_SIZE AO_RANDOMTEX_SIZE
#define HBAO_RANDOM_ELEMENTS HBAO_RANDOM_SIZE*HBAO_RANDOM_SIZE
#define HBAO_MAX_SAMPLES 8

    struct {
          GLuint
            depthlinear,
            viewnormal,
            hbao_calc,
            hbao2_deinterleave,
            hbao2_calc;
        } m_hbaoFbos;

    struct {
          GLuint
            scene_vbo,
            scene_ibo,
            scene_ubo,
            hbao_ubo;
        } m_hbaoBuffers;

    struct {
      GLuint
        scene_depthlinear,
        scene_viewnormal,
        hbao_result,
        hbao_blur,
        hbao_random,
        hbao_randomview[HBAO_MAX_SAMPLES],
        hbao2_deptharray,
        hbao2_depthview[HBAO_RANDOM_ELEMENTS],
        hbao2_resultarray;
    } m_hbaoTextures;

    struct HBAOData {
      float   RadiusToScreen;        // radius
      float   R2;     // 1/radius
      float   NegInvR2;     // radius * radius
      float   NDotVBias;

      vec2f    InvFullResolution;
      vec2f    InvQuarterResolution;

      float   AOMultiplier;
      float   PowExponent;
      vec2f    _pad0;

      vec4f    projInfo;
      vec2f    projScale;
      int     projOrtho;
      int     _pad1;

      vec4f    float2Offsets[AO_RANDOMTEX_SIZE*AO_RANDOMTEX_SIZE];
      vec4f    jitters[AO_RANDOMTEX_SIZE*AO_RANDOMTEX_SIZE];
    } m_hbaoUbo;

    vec4f      m_hbaoRandom[HBAO_RANDOM_ELEMENTS * HBAO_MAX_SAMPLES];


    //! HBAO+
    GLuint m_AOResultFBO;
    GLuint m_AOResultTexture;

    GLuint m_skyboxVAO, m_skyboxVBO;

    // General OIT Background
    GLuint m_OITBackgroundTexture;
    GLuint m_OITBackgroundFBO;

    // WeightBlendOIT buffers
    GLuint m_weightColorAccumulationTexture;    //!< Weight color accumulation
    GLuint m_weightAlphaAccumulationTexture;    //!< Alpha accumulation

    //Depth Peeling buffers
    GLuint m_depthPeelingFBOs[2];
    GLuint m_depthPeelingColorTextures[2];
    GLuint m_depthPeelingDepthTextures[2];

    //Blender Buffer for Weight Blend and Depth Peeling.
    GLuint m_OITBlenderFBO;
    GLuint m_OITBlenderColorTexture;

    //Linked List Buffers.
    GLuint m_LinkedListCounterBuffer;
    GLuint m_LinkedListLinkedListBuffer;
    GLuint m_LinkedListHeadPointerTexture;
    GLuint m_LinkedListHeadPtrClearBuffer;
    GLuint m_LinkedListMaxNodes;

    GLuint m_PCMapFBO;
    GLuint m_PCMapColorTexture;
    GLuint m_PCMapFlatWorldTexture;
    GLuint m_PCMapDepthTexture;


    // Standard parameters
    int   m_msaaSamples;
    int   m_csaaColorSamples;
    int   m_csaaCoverageSamples;
    int   m_bfaaMultiplier;

    // HBAO
    enum AlgorithmType {
      ALGORITHM_HBAO_CACHEAWARE = 1,
      ALGORITHM_HBAO_CLASSIC,
      NUM_ALGORITHMS,
    } m_hbaoAlgorithm;
    float m_hbaoIntensity;
    float m_hbaoBias;
    float m_hbaoRadius;
    bool  m_hbaoBlur;
    float m_hbaoBlurSharpness;

    // SMAA (Subpixel Morphological Antialiasing)
    GLuint m_smaaEdgeFBO;
    GLuint m_smaaEdgeTexture;
    GLuint m_smaaBlendFBO;
    GLuint m_smaaBlendTexture;
    GLuint m_smaaDepthStencilTexture;
    GLuint m_smaaAreaTexture; // precomputed texture
    GLuint m_smaaSearchTexture; // precomputed texture

    // Environment map
    GLuint m_envCubemap;
    GLuint m_envCaptureFBO;
    GLuint m_envIrradianceMap;
    GLuint m_envPrefilterMap;
    GLuint m_envBrdfLUTTexture;

    // Backup buffer for OIT + AO + Water
    GLuint m_tmpFBO;
    GLuint m_tmpColorTexture;
    GLuint m_tmpDepthStencilTexture;

    void setShaderProgram(QOpenGLShaderProgram* program);
    QOpenGLShaderProgram* shaderProgram();
    eulerf calculateDrawAngle(NESpatial* object);

protected:

    QOpenGLShaderProgram *m_pShaderProgram;

    bool m_backgroundGradient;
    BackgroundType m_backgroundType;

    QColor m_backgroundGradColor;
    QColor m_backgroundRealColor;

    EnvironmentType m_environmentType;
    bool m_environmentRender;
    double m_envRotation;

    GlobalTransparency m_transparencyType;

    //! Quality profile
    QualityProfile m_QualityProfile;
    bool m_DynamicQuality;

    int m_depthPeelingIterations;
    //! Flag abount drawing of the grid in zero plane
    bool m_DrawGrid;
    bool m_OverlayGrid;
    double m_AxisThickness;
    double m_GridThickness;
    double m_SubGridThickness;
    QColor m_GridColor;
    QColor m_SubGridColor;

    bool m_ssao;
    HbaoType m_hbaoType;
    double m_intensity;
    double m_radius;
    double m_blurSharpness;

    AntialiasingType m_aaType;

    //! OSD draw flag
    bool m_ShowOsd;
    bool m_bShowFPS;

    QSizeF m_GridSize;
    QSize m_CellCount;
    QSizeF m_CellSize;
    bool m_addGridLevel;
    QSize m_CellDivision;
    bool m_gridMeasurements;
    //! Controls whether draw axes are drawn or not (should be moved to somewhere else)
    bool m_ViewAxes;
    bool m_ShowGizmo;
    vec3f m_SnapUnit;

    //! Background color of the gl widgets (should be moved to somewhere else)
    QColor m_BackgroundColor;

    FilePath m_backgroundTexture;

    //! Flag about drawing the white rectangle that represents the render area (should be moved)
    bool m_DrawRenderArea;
    //! Holds the desired size (width, height) of the gl window for saved gl frames.
    NEImageSize m_GlSize;

    virtual bool isVisibleSurface() { return true;}

    // Utilities
    void newBuffer(GLuint &glid);
    void deleteBuffer(GLuint &glid);
    void newTexture(GLuint &glid);
    void deleteTexture( GLuint &glid );
    void newFramebuffer(GLuint &glid);
    void deleteFramebuffer(GLuint &glid);
    void newRenderbuffer(GLuint &glid);
    void deleteRenderbuffer(GLuint &glid);

    //
    //! Draw background gradient
    //!
    void drawBackgroundGradient();
    ///
    /// \brief drawBackgroundTexture
    /// Background environment texture
    ///
    void drawBackgroundTexture();
    void drawBackgroundReal();

    void createCubemapAndIrradianceMap(QOpenGLTexture* hdrTexture);
    void createIrradianceMap(QOpenGLTexture* skyboxTexture);

    //
    //! Draw background cube
    //! //
    void drawEnvironmentCube();

    //
    //! Draw background environment map
    //
    void drawEnvironmentSphere();


    void beginTransparency(float width, float height);
    void doTransparency(float width, float height);
    void endTransparency();

    void drawAllObjectGizmos();
    void drawAllCFASTPlanes();
    void drawSnapPt();
    void drawMeasurement();
    void renderMeasurement(std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin, std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfo, QPair<vec3f, float> measureInfoMoved);
    //! Draws the floor
    void drawFloor();

    void renderWeightedBlendedOIT();
    void renderLinkedListOIT();

    void renderSpatials(int renderStage);

    // HBAO
    void setHBAOParameter(float intensity, float radius, float blurSharpness);

    //! Renders the ambient occlusion
    void drawAO(int width, int height);
    void drawPostProcessing(int width, int height);

    virtual void 
    drawOSDMessage(float width, float height) {}

    void prepareHbaoData(int width, int height);
    void drawLinearDepth(int width, int height, int sampleIdx);
    void drawHbaoBlur(int width, int height, int sampleIdx);
    void drawHbaoClassic(int width, int height, int sampleIdx);

    //! Clean water buffer to get started rendering water particles
    void beginWaterRendering(int width, int height);
    //! Global water rendering
    void endWaterRendering();


    //! Draws ths boundingBox
    void drawBoundingBox(float width, float height);

    void getDrawTicksCount(int fromIndex, int toIndex, int axisIndex, vec3f* positions, vec3f* vertices, vec3f objectScale, int *pOneTickCount, float *pOneTickUnit, bool calcFromCenter = false);
    void drawTicksBoundingAxis(int fromIndex, int toIndex, int dirFromIndex, int dirToIndex, int axisIndex, vec3f* vertices, float root3Length, vec3f objectScale,
                               int commonTickCount, float commonTickUnit, bool calcFromCenter = false, bool hideTick = false);
    void drawCenterTicksBoundingAxis(vec3f* vertices, float root3Length, vec3f objectScale);
    void drawText2DBoundingAxis(int fromIndex, int toIndex, int dirFromIndex, int dirToIndex, int axisIndex, int dirAxisIndex, vec3f* positions, vec3f* vertices,
            eulerf drawAngle, float root3Length, vec3f objectScale, int commonTickCount, float commonTickUnit, double stringScale, QColor stringColor, bool calcFromCenter,
                                bool drawInWorldUnits, matrix44f transform);


    virtual void blitScene(int width, int height);

    void bindScreenFrameBuffer();

    virtual void drawManipulator(NESpatial* object, bool pick=false){ }

    //! Draws the main world axes
    virtual void drawAxes();

    //! Printing 3D text into the scene can be accomplished by using this
    void glPrint3D( QString text, float x, float y, float z, float size /*= 0.2*/, Color32 color /*= Color32(255,255,255,255)*/ );

    virtual GLuint defaultFramebuffer(){ return m_offlineFinalFBO;}


public:

    //!< Check if moving camera or manipulating object
    bool isSceneDynamic(){return dynamicQuality();}

    bool backgroundGradient() { return m_backgroundGradient;}
    void setBackgroundGradient(bool arg) { m_backgroundGradient = arg;}
    BackgroundType backgroundType(){ return m_backgroundType;}
    void setBackgroundType(BackgroundType type) { m_backgroundType = type;}
    QColor backgroundGradColor(){ return m_backgroundGradColor;}
    void setBackgroundGradColor(QColor color) { m_backgroundGradColor = color; }

    QColor backgroundRealColor() { return m_backgroundRealColor;}
    void setBackgroundRealColor( QColor color) { m_backgroundRealColor = color; }

    EnvironmentType environmentType(){ return m_environmentType;}
    void setEnvironmentType( EnvironmentType type) { m_environmentType = type; }

    bool EnvironmentTypeCube(){ return ( m_environmentType == EnvironmentType::Cubemap ); }
    bool EnvironmentTypeSphere(){ return( m_environmentType == EnvironmentType::SphereMap ); }
    bool EnvironmentTypeOn(){ return m_environmentType != EnvironmentType::NoEnvironment; }

    bool environmentRender() {return m_environmentRender;}
    void setEnvironmentRender( bool bRender ) { m_environmentRender = bRender;}

    double envRotation(){ return m_envRotation;}
    void setEnvRotation(double envRot) { m_envRotation = envRot; }

    GlobalTransparency transparencyType(){ return m_transparencyType;}
    void setTransparencyType(GlobalTransparency ttype) { m_transparencyType = ttype; }


    //! Quality profile
    QualityProfile qualityProfile(){return m_QualityProfile;}
    void setQualityProfile(QualityProfile profile) { m_QualityProfile = profile; }

    //!< Check if Transparency is enabled in current quality profile
    bool checkTransparencyQualityProfile(){ return (m_QualityProfile == QualityProfile::HighQuality); }

    bool dynamicQuality(){ return m_DynamicQuality;}
    void setDynamicQuality(bool quality){ m_DynamicQuality = quality;}

    bool isDepthPeeling(){return m_transparencyType == DepthPeelingTransparency;}
    int depthPeelingIterations(){ return m_depthPeelingIterations;}
    void setDepthPeelingIterations(int iter) { m_depthPeelingIterations = iter; }

    bool drawGrid(){ return m_DrawGrid; }
    void setDrawGrid(bool bgrid) { m_DrawGrid = bgrid;}
    bool overlayGrid(){return m_OverlayGrid;}
    void setOverlayGrid(bool overgrid) { m_OverlayGrid = overgrid;}

    double axisThickness() { return m_AxisThickness; }
    void setAxisThickness(double arg) { m_AxisThickness = arg; }
    double gridThickness(){ return m_GridThickness;}
    void setGridThickness(double arg) { m_GridThickness = arg; }
    double subGridThickness() { return m_SubGridThickness; }
    void setSubGridThickness(double arg) { m_SubGridThickness = arg; }

    QColor gridColor(){ return m_GridColor;}
    void setGridColor(QColor color) {m_GridColor = color;}
    QColor subGridColor(){ return m_SubGridColor;}
    void setSubGridColor(QColor color) {m_SubGridColor = color;}

    bool ssao(){ return m_ssao;}
    void setSsao(bool val) { m_ssao = val;}

    HbaoType type(){ return m_hbaoType;}
    void setType(HbaoType type) { m_hbaoType = type;}

    //!< Check if AO is enabled in current quality profile
    bool checkAOQualityProfile() { return (m_QualityProfile == QualityProfile::MedQuality || m_QualityProfile == QualityProfile::HighQuality); }

    double intensity(){return m_intensity;}
    void setIntensity(double val) {m_intensity = val;}

    double radius() {return m_radius;}
    void setRadius(double val) { m_radius = val;}

    double blurSharpness() {return m_blurSharpness;}
    void setBlurSharpness(double val) { m_blurSharpness = val;}

    AntialiasingType aaType(){ return m_aaType;}
    void setAaType(AntialiasingType type) { m_aaType = type;}
    bool isMsaa(){return m_aaType == Msaa;}
    bool isFxaa(){return m_aaType == Fxaa;}
    bool isCsaa(){ return m_aaType == Csaa;}
    bool isBfaa(){return m_aaType == Bfaa;}

    bool showOsd(){return m_ShowOsd;}
    void setShowOsd(bool val) { m_ShowOsd = val;}

    bool showFPS() {return m_bShowFPS;}
    void setShowFPS(bool val) { m_bShowFPS = val;}

    QSizeF gridSize(){return m_GridSize;}
    QSize cellCount(){return m_CellCount;}
    QSizeF cellSize(){return m_CellSize;}
    bool addGridLevel(){return m_addGridLevel;}
    QSize cellDivision(){return m_CellDivision;}
    bool gridMeasurements(){
        return m_gridMeasurements;
    }
    void setGridSize(QSizeF val)
    {
        if (val.width() <= 0.0 || val.height() <= 0.0)
            return;

        m_GridSize = val;
        computeCellSize();
    }
    void setCellCount(QSize val)
    {
        m_CellCount = val;
        computeCellSize();
    }
    void setAddGridLevel(bool level){
        m_addGridLevel = level;
    }
    void setCellDivision(QSize val)
    {
        m_CellDivision = val;
    }
    void setgridMeasurements(bool value){
        m_gridMeasurements = value;
    }

    bool drawRenderArea() const { return m_DrawRenderArea; }
    void setDrawRenderArea(bool val) { m_DrawRenderArea = val; }

    const QSize& GlSize() const { return m_GlSize; }
    void setGlSize(const QSize& val) { m_GlSize = val; }

    bool viewAxes(){ return m_ViewAxes;}
    void setViewAxes(bool val) { m_ViewAxes = val;}

    bool showGizmo(){ return m_ShowGizmo;}
    void setShowGizmo(bool val) { m_ShowGizmo = val;}


    vec3f snapUnit(){ return m_SnapUnit;}
    void setSnapUnit(vec3f val) { m_SnapUnit = val;}

    //!< Check if Anti-aliasing is enabled in current quality profile
    bool checkAAQualityProfile(){ return (m_QualityProfile == QualityProfile::MedQuality || m_QualityProfile == QualityProfile::HighQuality); }

    friend class NEMaterial;

    int getWidth(){return m_width;}
    int getHeight(){return m_height;}
    int getGLWidth(){return m_glWidth;}
    int getGLHeight(){return m_glHeight;}

    void updateEnvironmentUniforms();

    // Transparency
    int getOITType(){return m_TransparencyStage;}
    int getOITMaxNodes(){return m_LinkedListMaxNodes;}

    // Water Rendering related
    FluidRenderer& getFluidRenderer(){return m_fluidRenderer;}

    // Sampled Backup buffer
    void backupFrameBuffer(int width, int height, bool bSceneBuffer);
    void bindSceneFrameBuffer();

    uint getSceneFBO(){return m_sceneFBO;}
    int getSceneColorTexture(){return m_sceneColorTexture;}
    int getSceneDepthTexture(){return m_sceneDepthStencilTexture;}

    //! Sets the camera
    void setCamera(QObject* pCamera);
    //! Gets the current camera of the widget
    NECamera* camera(){ return m_pCamera; }


    QColor backgroundColor() { return m_BackgroundColor;}
    void setBackgroundColor(QColor color) {m_BackgroundColor = color;}

    void setBackgroundTexture( FilePath arg )
    {
        m_backgroundTexture = arg;
        loadBackgroundTexture( arg.path() );
    }
    FilePath backgroundTexture() const{
        return m_backgroundTexture;
    }

private:
    void computeCellSize();
};

struct AVStream;
struct AVCodecContext;
struct AVFrame;
struct AVOutputFormat;
struct AVFormatContext;
struct AVCodec;
struct AVDictionary;
struct AVPacket;
struct AVRational;
// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class NECEXP_NERENDERING FFMpegMovieMerger
{
public:
    FFMpegMovieMerger();
    ~FFMpegMovieMerger();


public:

    float m_width;
    float m_height;

    float m_frameRate;

    bool m_bMerge;

    GLubyte *m_totalFrameBuffer;

    OutputStream m_oStream;
    QString m_fileName;

    AVOutputFormat *m_oFormat;
    AVFormatContext *m_oContext;
    AVCodec *m_videoCodec;

    int initializeFfmpeg();
    void finalizeFfmpeg() ;
    /* Add an output stream. */
    void add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);
    void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
    AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);

    /*
     * encode one video frame and send it to the muxer
     * return 1 when encoding is finished, 0 otherwise
     */
    void fillMergedFrame(QRect layout, GLubyte *framebuffer);
    int write_video_frame(AVFormatContext *oc, OutputStream *ost, void *framebuffer);
    AVFrame *get_video_frame(OutputStream *ost, void *framebuffer);

    void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt);
    int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);
    void close_stream(AVFormatContext *oc, OutputStream *ost);

};

class NECEXP_NERENDERING NEGLOfflineRenderer : public QObject, public IGLSurface
{
    Q_OBJECT
public:
    NEGLOfflineRenderer(QObject* camera, float width, float height, QString &filename, float framerate, float speed);
    NEGLOfflineRenderer(QRect layout, QString &filename, float framerate, float speed, FFMpegMovieMerger *recorder, int index);
    ~NEGLOfflineRenderer();

    bool isOneFrame(){return m_bOneFrame;}
    void renderOneFrame();

    void copyPropertiesFromWidget(IGLSurface *widget);

signals:
    void finished();
    void frameReady(void *fbuffer);

public slots:
    void update();
    void processFrame(GLubyte *fbuffer);

private:
    int m_index;

    float m_frameRate;
    float m_speedFactor;
    GLubyte *m_frameBuffer;
    bool m_bOneFrame;
    bool m_bMerge;

    float m_FrameRatio;

    QRect m_layout;

    FFMpegMovieMerger *m_Recorder;
};
