#pragma once

NE_FWD_DECL_2(class, FTPolygonFont, FTTextureFont)

class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFunctions;
class QOpenGLFunctions_3_3_Compatibility;
class QOpenGLFunctions_4_1_Compatibility;
class QOpenGLFunctions_4_3_Compatibility;
class QOpenGLExtension_EXT_direct_state_access;
class QOpenGLExtension_NV_texture_multisample;

struct FluidProgram
{
    ShaderMetaInfo* mPointThicknessProgram;
    ShaderMetaInfo* mEllipsoidDepthProgram;
    ShaderMetaInfo* mDepthBlurProgram;
    ShaderMetaInfo* mDiffuseProgram;
};

class GFSDK_SSAO_Context_GL;

class NEGLRenderer;

class NEGLManagerPrivate
{
public:
    NEGLManagerPrivate(NEGLManager* s);
    ~NEGLManagerPrivate();

    //! Disables the GPU buffers, necessary for working without buffer objects
    void disableGPUBuffers();
    //! Loads some utility shaders that will be used throughly in the software
    void loadDefaultShaders();

    NEMaterial* m_DefaultMaterial;
    NEMaterial* m_DefaultFluidMaterial;

#if USE_EXT_GFSDK_SSAO
    //! HBAO+
    GFSDK_SSAO_Context_GL* m_pAOContext = 0;
#endif


    //! Pointers to some commonly used shaders
    ShaderMetaInfo* m_ParticleShader;
    ShaderMetaInfo* m_AOParticleShader;
    ShaderMetaInfo* m_SandShader;
    ShaderMetaInfo* m_PositionShader;
    ShaderMetaInfo *m_SolidWireframeShader;
    ShaderMetaInfo *m_QuadWireframeShader;
    ShaderMetaInfo* m_LineStippleShader;
    ShaderMetaInfo* m_RectYFilterShader;
    ShaderMetaInfo* m_ColorRampOSDShader;
    ShaderMetaInfo* m_FloorDrawingShader;
    ShaderMetaInfo* m_CPShader;
    ShaderMetaInfo* m_CPWaterShader;
    ShaderMetaInfo* m_CamLitShader;
    ShaderMetaInfo* m_RectTextureShader;
    ShaderMetaInfo* m_GVDBComposeShader;
    ShaderMetaInfo* m_MotionShader;
    ShaderMetaInfo* m_HairShader;
    ShaderMetaInfo* m_ShaderColorMapper;
    ShaderMetaInfo* m_WeightedBlendedFinalShader;
    ShaderMetaInfo* m_DepthPeelingBlendShader;
    ShaderMetaInfo* m_DepthPeelingFinalShader;
    ShaderMetaInfo* m_LinkedListFinalShader;
    // Fluid
    FluidProgram    m_FluidProgram;
    // HBAO
    ShaderMetaInfo *m_AOGeomShader;
    ShaderMetaInfo *m_AODepthLinearizeShader;
    ShaderMetaInfo *m_AODepthLinearizeMSAAShader;
    ShaderMetaInfo *m_AOCalcShader;
    ShaderMetaInfo *m_HBAOBlurShader;
    ShaderMetaInfo *m_HBAOBlur2Shader;
    ShaderMetaInfo *m_AOResultShader;
    ShaderMetaInfo *m_EdgeShader;
    ShaderMetaInfo *m_bgShader;
    ShaderMetaInfo *m_pedestalShader;
    ShaderMetaInfo *m_simpleDepthShader;
    ShaderMetaInfo *m_shadowMappingShader;
    ShaderMetaInfo *m_skyboxShader;
    ShaderMetaInfo *m_skySphereShader;
    ShaderMetaInfo *m_fxaaShader;
    ShaderMetaInfo *m_fxaaLowShader;
    ShaderMetaInfo *m_fxaaMediumShader;
    ShaderMetaInfo *m_fxaaHighShader;
    ShaderMetaInfo *m_fxaaExtremeShader;

    ShaderMetaInfo *m_lineShader;

    // SMAA
    ShaderMetaInfo *m_smaaEdgeShader;
    ShaderMetaInfo *m_smaaBlendShader;
    ShaderMetaInfo *m_smaaNeighborShader;

    // IBL
    ShaderMetaInfo *m_iblEquirectangularToCubemapShader;
    ShaderMetaInfo *m_iblIrradianceShader;
    ShaderMetaInfo *m_iblBackgroundShader;
    ShaderMetaInfo *m_iblPrefilterShader;
    ShaderMetaInfo *m_iblBrdfShader;


    QOpenGLShader* m_PerPixelVS;
    QOpenGLShader* m_PerPixelFS;
    QOpenGLShader* m_PerPixelFlatVS;
    QOpenGLShader* m_PerPixelFlatFS;
    QOpenGLShader* m_FullScreenVS;
    QOpenGLShader* m_WaterDisplayFS;
    QOpenGLShader* m_LightsFS;
    QOpenGLShader* m_LightsSpritesFS;
    QOpenGLShader* m_ClipFS;
    QOpenGLShader* m_ColoringFS;
    QOpenGLShader* m_RigidParticleFS;

    //
    // Shading related
    //
    QString m_PerPixelVSCodes;
    QString m_PerPixelFlatVSCodes;
    QString m_FullScreenVSCodes;
    QString m_PerPixelFSCodes;
    QString m_PerPixelFlatFSCodes;
    QString m_WaterDisplayFSCodes;
    QString m_LightsFSCodes;
    QString m_RigidParticleCodes;
    QString m_LightsParticlesFSCodes;
    QString m_FoamCodes;
    QString m_ClipFSCodes;
    QString m_ColoringFSCodes;

    //
    // Lights
    //

    std::vector<NEDirectionalLight*> dirLights;
    std::vector<NEPointLight*> pointLights;
    std::vector<NESpotLight*> spotLights;

    std::vector<NEClipPlane*> clipPlanes;


    FTPolygonFont* polyFont;            /**< Pointer to 3D OpenGL font drawing object */
    FTTextureFont* textureFont;         /**< Pointer to 2D OpenGL font drawing object */

    uint m_OpenGLVersion;

    //! Variables below are set by each gl widget before its drawing occurs
    matrix44f m_NormalMatrix;
    matrix44f m_ModelViewMatrix;
    matrix44f m_WorldMatrix;
    matrix44f m_ViewMatrix;
    //! Initializes the GL manager.
    void NECEXP_NERENDERING init();
    //! De-initializes the GL manager
    void NECEXP_NERENDERING deInit();
    //! Create common object
    void createRenderObject();

    //! Cube object for environment
    NEVertexBuffer* bgCube;
    //! Sphere object for environment
    NEVertexBuffer* bgSphere;

    NEGLRenderer *m_pGLRenderer;

//    static QOpenGLContext *s_sharedContext;
//    static QOffscreenSurface *s_offscreenSurface;

    static QOpenGLFunctions *s_gl;
    static QOpenGLFunctions_3_3_Compatibility *s_gl33;
    static QOpenGLFunctions_4_1_Compatibility *s_gl41;
    static QOpenGLFunctions_4_3_Compatibility *s_gl43;
    static QOpenGLExtension_EXT_direct_state_access *s_glext_direct_state_access;
    static QOpenGLExtension_NV_texture_multisample *s_glext_nv_texture_multisample;

    private:
    NEGLManager* S;
};
