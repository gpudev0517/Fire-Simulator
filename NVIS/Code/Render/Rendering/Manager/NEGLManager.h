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
//test
NE_FWD_DECL_5(class, NEMatrixStack4x4, NEIndexedTriMesh, NEVertexBuffer, NEIndexBuffer, NEGLResourceManager)
NE_FWD_DECL_6(class, NEGLResourceManager, NECamera, NEVolume, NESpatial, NE_PRIVATE(NEGLManager), QOpenGLShaderProgram)
NE_FWD_DECL_1(struct, ShaderMetaInfo)
NE_FWD_DECL_1(class, NEGLRenderer)

NE_FWD_DECL_5(class, NEDirectionalLight, NESpotLight, NESpotLight, NEPointLight, NEClipPlane )

class NEIndexedTriMesh;
class NEGLRenderableMesh;
class NEMaterial;
class NECamera;
class NELight;
class NEMatrixStack4x4;
class NEVertexBuffer;
class NESpatial;
class IGLSurface;


//! The GL manager provides some utility functions for OpenGL interaction.
/*!
    It wraps some of the important functionality of OpenGL so as to provide an interface to the
    programmer; that is not only more convenient, but also portable to the later versions
    of OpenGL.
*/

class NECEXP_NERENDERING NEGLManager final : public QObject
{
    Q_OBJECT


public:

    static NEGLManager& Instance();
    enum NEGUIItemSize { Smallest, Smaller, Small, Normal, Large, Larger, Largest };

    NEGLManager();
    ~NEGLManager();

//    static QOpenGLContext*& staticShareContext();
//    static QOffscreenSurface*& staticOffscreenSurface();

    static QOpenGLFunctions*& glFunc();
    static QOpenGLFunctions_3_3_Compatibility*& glFunc33();
    static QOpenGLFunctions_4_1_Compatibility*& glFunc41();
    static QOpenGLFunctions_4_3_Compatibility*& glFunc43();
    static QOpenGLExtension_EXT_direct_state_access*& glExtDirectStateAccess();
    static QOpenGLExtension_NV_texture_multisample*& glExtNVTextureMultisample();

    void init();
    void deInit();

    //! Sets-up the blending function for OpenGL.
    void setBlendFunc(uint blend);

    //! Sets different OpenGL fill modes.
    void setFillMode(uint fill);

    //! Sets OpenGL shading mode.
    void setShadeMode(NEShadeMode shade);

    //! Gets the OpenGL shading mode.
    NEShadeMode shadeMode();

    //! Sets OpenGL depth test function.
    void setDepthTest(uint func);

    //! Gets the OpenGL depth test function.
    uint depthTest();

    //! Drawing Methods which call the Renderer's call
    //!
    //! Draws points (distance attenuated as necessary)
    void drawParticles(NEVertexBuffer* vertexBuffer, int count); //!< This will be removed later
    void drawParticles(IGLSurface* surface, NEVertexBuffer* vertexBuffer, int count);
    void drawParticles(GLenum format, GLsizei stride, const GLvoid* pointer, int start, int count);


    //! Utility functions for drawing specific particle/mesh modes - example: water surface etc
    virtual void beginParticleDraw_Thickness(IGLSurface* surface, float radius, float thicknessParticleScale ) ;
    virtual void beginParticleDraw_Depth(IGLSurface* surface, float depthParticleScale) ;

    void drawParticleBlurDepth(IGLSurface* surface,
                               float radius, float smoothingRadius, float maxRadialSamples, float depthEdgetFalloff, float depthBlurFalloff);

    virtual void drawParticleDisplay(IGLSurface* surface, NEMaterial* material, float alpha) ;

    virtual void drawWaterMesh(IGLSurface* surface, const NEIndexedTriMesh *mesh, matrix44f modelMatrix);
    virtual void drawWaterMesh_CP(IGLSurface* surface, const NEIndexedTriMesh *mesh, matrix44f modelMatrix, bool useVertexColor);


    //! Draws volume
    void drawVolume(NEVolume* volume);
    //! Draw the given primitive using the information supplied inside the vertexBuffer and the indexBuffer using glDrawElements
    void draw(NEPrimType primType, NEVertexBuffer* vertexBuffer, NEIndexBuffer* indexBuffer);
    //! Draw the given primitive using the information supplied inside the vertexBuffer using glDrawArrays
    void draw(NEPrimType primType, NEVertexBuffer* vertexBuffer);
    void draw(NEPrimType primType, NEVertexBuffer* vertexBuffer, GLuint first, GLuint count);

    //! For drawing the geometry without setting up any material (faster)
    void drawRaw(const NEIndexedTriMesh* mesh, IGLSurface* surface, bool pick, bool boundingbox);

    //! For drawing the volume without setting up any material (faster)
    void drawRaw(const NEVolume* volume, bool pick = false);
    void drawRaw(const AABB& aabb);

    //! Utility function for drawing indexed triangle meshes using flat shading
    void drawFlat(const NEIndexedTriMesh* mesh, IGLSurface* surface, bool boundingbox);

    //! Utility function for drawing meshes using toon / corner shading
    void drawSolidWireframe(const NEIndexedTriMesh *mesh, IGLSurface* surface, QColor solidColor, float thickness, float threshold, matrix44f &modelMatrix, bool bUseDash = false);
    void drawQuadWireframe(const NEIndexedTriMesh *mesh, IGLSurface* surface, QColor wireColor, matrix44f &modelMatrix, bool boundingbox, bool isHideBackfacing);


    //! Functions to draw background
    void drawQuadBackground(IGLSurface *surface, const QColor &bgColor1 , const QColor &bgColor2, bool bRadial = false);
    void drawQuadBackgroundTexture( IGLSurface *surface, QOpenGLTexture *texture);
    void drawPedestal(IGLSurface* surface, QColor &realColor, matrix44f &mvpMat, float radial);

    void drawSkybox(IGLSurface *surface, QOpenGLTexture *cubeMapTexture );
    void drawEnvironmentMap(IGLSurface *surface, GLuint envCubemap, float envRotation);

    //! Utility function for drawing indexed triangle meshes
    void draw(const NEIndexedTriMesh* mesh, bool boundingbox, bool pick, bool twoSided, bool isReceiveShadow, matrix44f modelMat); // This will be removed later after moving all rendering codes to UI, and use below function instead
    void draw(const NEIndexedTriMesh* mesh, IGLSurface* surface, bool boundingbox, bool pick, bool twoSided, bool isReceiveShadow, matrix44f modelMat, int isInactive = 0); // 0: normal, 1: filling stencil, 2 : drawing outline
    void drawWithDefaultMaterial( const NEIndexedTriMesh* mesh, IGLSurface* surface, bool twoSided, bool isReceiveShadow, matrix44f modelMatrix );

    void setLightAndClipPlanForTerrain(IGLSurface* surface, QOpenGLShaderProgram* topographyShader);
    void drawShadowMap(IGLSurface *surface, const NEIndexedTriMesh* mesh);

    //! For drawing the geometry with per-pixel motion vectors
    void drawInMotion(IGLSurface *surface, const NEIndexedTriMesh* mesh);

    void drawQuad();
    void drawRect();
    void drawCube();
    void drawSphere();

    //! For starting drawing globally lit objects using the default material
    void beginLitDraw(IGLSurface* surface, bool twoSided = false);
    //! For ending drawing globally lit objects
    void endLitDraw(IGLSurface *surface);
    //! For starting drawing objects using single color
    void beginUnLitDraw(IGLSurface *surface);
    //! For ending drawing objects using single color
    void endUnLitDraw(IGLSurface *surface);
    //! Begins drawing particles as point sprites
    void beginParticleDraw(float radius, bool lighting, bool picked, bool asSand = false, bool isPicking = false); // This will be removed and next function will be used later
    void beginParticleDraw(IGLSurface* surface, float radius, bool lighting, bool picked, bool asSand = false, bool isPicking = false, bool isPCMap = false);
    //! Ends drawing particles as point sprites
    void endParticleDraw(IGLSurface *surface);

    //! Begins drawing hair
    void beginHairDraw(IGLSurface* surface);
    //!Ends drawing hair
    void endHairDraw(IGLSurface *surface);

    //
    // Render individual objects
    //
    void renderSpatial(NESpatial* spatial, IGLSurface* surface);
    void renderSpatialOnlyDepth(NESpatial* spatial, IGLSurface* surface);
    void renderWater(NESpatial* spatial, IGLSurface* surface);
    void renderPicked(NESpatial* spatial, IGLSurface* surface);
    void renderPickable(NESpatial* spatial, IGLSurface* surface);
    void renderShadow(NESpatial* spatial, IGLSurface* surface);

    void renderScenePerFrame(IGLSurface *surface, float width, float height, float devPixelRatio);


    //! AO
    //! HBAO+
    struct AOParams
    {
        int width;
        int height;
        int samples;
        const float* matProj;
        float radius;
        float bias;
        float intensity;
        bool blur;
        float blurSharpness;
    };

#if USE_EXT_GFSDK_SSAO
    void initAOContext();
    void releaseAOContext();
    void drawAO(GLuint dstFB, GLuint srcDepthTex, AOParams params);
#endif

    //
    //! Other Utilities
    //
    void setLightsForShader(IGLSurface* surface, QOpenGLShaderProgram* shader);
    void setClipPlanesForShader(QOpenGLShaderProgram* shader);
    void setClipPlanesForSquare(bool isEnable);
    void setClipPlanesForSpatial(NESpatial* object);
    NEMaterial* getDefaultFluidMaterial();

    //! Updates all shader that use light sources
    void updateLightShaders();
    //!
    void updateClipShaders();


    //! Updates the shaders of all materials depending on the light sources
    void updateAllMaterialShaders();
    void updateMaterialShaders( NEMaterial* material);

    //! Updates all shaders including all material shaders
    void updateAllShaders();

    std::vector<NEDirectionalLight*> getDirectionLights();
    std::vector<NEPointLight*> getPointLights();
    std::vector<NESpotLight*> getSpotLights();
    std::vector<NEClipPlane*> getClipPlanes();


    // --------------------

    //! Prints 2D text onto screen (shouldn't be used directly, use NEOsd instead).
    void glPrint(QString name, int x, int y, Color32 color = Color32(255, 255, 255, 255));
    void glPrint3D(QString text, int len);

    //! Creates a vertex buffer on the GPU
    NEVertexBuffer* createVertexBuffer(NEVertexFormat format, uint numVertices, void* data = 0, uint usage = GL_DYNAMIC_DRAW);
    NEVertexBuffer* createVertexBuffer(NEVertexFormat format);
    //! Destroys the given vertex buffer
    void destroy(NEVertexBuffer* vb);
    //! Creates a vertex buffer on the GPU
    NEIndexBuffer* createIndexBuffer(uint numIndices, void* data = 0, uint usage = GL_DYNAMIC_DRAW);
    NEIndexBuffer* generateAdjacencyIndexBuffer(uint numIndices, void* data = 0, uint usage = GL_DYNAMIC_DRAW);
    NEVertexBuffer* generateUnindexedVBuffer(NEVertexFormat format, uint numIndices, void* indices, void* vertices, uint usage = GL_DYNAMIC_DRAW);

    NEVertexBuffer* generateBoundingBoxBuffer(NEVertexFormat formatShaded, NEVertexFormat formatWireframe, uint numVertices, void* vertices, uint usage, NEVertexBuffer*& outUnindexedBuffer);
    //! Destroys the given index buffer
    void destroy(NEIndexBuffer* ib);

    //! Binds the simple color shader
    void bindPositionShader(IGLSurface *surface);
    void bindLineStippleShader(IGLSurface *surface);
    void bindRectYFilterShader(IGLSurface *surface);
    void bindColorRampOSDShader(IGLSurface *surface);
    void bindFloorDrawingShader(IGLSurface *surface);
    void bindSolidWireframeShader(IGLSurface *surface);
    void bindQuadWireframeShader(IGLSurface *surface);
    void bindCPShader(IGLSurface *surface);
    void bindCPWaterShader(IGLSurface *surface);
    void bindCamLitShader(IGLSurface *surface);
    //!Binds OIT Shader
    void bindWeightedBlendedFinalShader(IGLSurface *surface);
    //!Binds Depth Peeling Blending Shader
    void bindDepthPeelingBlendShader(IGLSurface *surface);
    //!Binds Depth Peeling Final Shader
    void bindDepthPeelingFinalShader(IGLSurface *surface);
    //!Binds Linked List Final Shader
    void bindLinkedListFinalShader(IGLSurface *surface);
    //! Binds the simple texture-only shader
    void bindRectTextureShader(IGLSurface *surface);
    void bindGVDBComposeShader(IGLSurface *surface);
    //! Binds the shader for rendering particles with sphere look
    void bindParticleShader(IGLSurface *surface);
    //! Binds the AO shader for rendering particles with ambient occlusion
    void bindAOParticleShader(IGLSurface *surface);
    //! Binds the AO shader for rendering AO result texture with depth and transparency
    void bindAOResultShader(IGLSurface *surface);
    //! Binds the shader for rendering particles with sand look
    void bindSandShader(IGLSurface *surface);
    //! Binds the shader for rendering hair
    void bindHairShader(IGLSurface *surface);

    //! Binds the shader for mapping color channels
    void bindColorMapperShader(IGLSurface *surface);

    //! Binds the Ambient Occlusion Shader
    void bindBgShader(IGLSurface *surface);
    void bindPedestalShader(IGLSurface *surface);
    void bindSimpleDepthShader(IGLSurface *surface);
    void bindShadowMappingShader(IGLSurface *surface);
    void bindSkyboxShader(IGLSurface *surface);
    void bindSkySphereShader(IGLSurface *surface);
    void bindFXAAShader(IGLSurface *surface);
    void bindFXAAShader(int level, IGLSurface *surface);

    void bindEdgeShader(IGLSurface *surface);

    //! Binds the edge detection shader for SMAA
    void bindSmaaEdgeShader(IGLSurface *surface);
    //! Binds the blend weights calculation for SMAA
    void bindSmaaBlendShader(IGLSurface *surface);
    //! Binds the neighborhood blending shader for SMAA
    void bindSmaaNeighborShader(IGLSurface *surface);

    void bindIblEquirectangularToCubemapShader(IGLSurface *surface);
    void bindIblIrradianceShader(IGLSurface *surface);
    void bindIblBackgroundShader(IGLSurface *surface);
    void bindIblPrefilterShader(IGLSurface *surface);
    void bindIblBrdfShader(IGLSurface *surface);

    //! Binds the shader that writes motion vectors
    void bindMotionShader(IGLSurface *surface);

    //! Synchronizes shaders that use lights with the light configuration of the scene, defined in NERenderer
    void lightsUpdated(QOpenGLShader* lightShader, QOpenGLShader* lightSpriteShader);

    //! Functions below get and set some useful data about the rendered OpenGL widget
    const matrix44f& normalMatrix();
    const matrix44f& modelViewMatrix();
    const matrix44f& worldMatrix();
    const matrix44f& viewMatrix();

    void normalMatrix( const matrix44f& nMatrix );
    void modelViewMatrix( const matrix44f& nmvMatrix );
    void worldMatrix( const matrix44f& wMatrix );
    void viewMatrix( const matrix44f& vMatrix );

    //! Updates common uniforms that are used for particle rendering
    void updateCommonParticleUniforms(IGLSurface* surface);
    //! Updates common uniforms that are used for normal rendering (ColorMesh)
    void updatePositionUniforms(bool isPicked = false); //!< This will be removed later
    void updatePositionUniforms(IGLSurface* surface, bool isPicked = false, int isInactive = 0);
    void updateLineStippleUniforms(IGLSurface* surface, float dashSize, float gapSize);
    void updateRectYFilterUniforms(IGLSurface* surface, QColor minColor, QColor maxColor, float upperValue, float lowerValue, float layerHeight);
    void updateColorRampOSDUniforms(IGLSurface* surface, QColor minColor, QColor maxColor);
    void updateFloorDrawingUniforms(IGLSurface* surface, float Xmin, float Xmax, float Zmin, float Zmax);
    void updateSolidWireframeUniforms(IGLSurface* surface, QColor color, float thickness, float threshold, bool bUseDash = false);
    void updateQuadWireframeUniforms(IGLSurface* surface, QColor color, bool hideBackface, bool swapUpAxes);
    void updateCPUniforms(IGLSurface *surface);
    void updateCPWaterUniforms(IGLSurface* surface, bool useVertexColor);
    void updateWaterUniforms(IGLSurface* surface);
    void setModelMatrixForPBR(IGLSurface *surface, matrix44f &modelMatrix);

    //! Deletes existing shader program for shaderInfo and creates a new one. Called by lightsUpdated()
    void recreateShader(ShaderMetaInfo* shaderInfo);
    //! Links the shader program for shader
//	void linkShader(QOpenGLShaderProgram* shader);
    void setOSDFontSize(NEGUIItemSize size);

    //
    // Vertex and Index buffers
    //
    std::shared_ptr<NEGLRenderableMesh> getOrCreateRenderableMesh(NEIndexedTriMesh *mesh);
    bool isBuffered( NEIndexedTriMesh *mesh );
    NEMaterial* material( NEIndexedTriMesh *mesh );
    void setMaterial(NEIndexedTriMesh *mesh , NEMaterial *material);

    NEIndexBuffer* indexBuffer( const NEIndexedTriMesh* mesh );
    NEIndexBuffer* indexBufferWithAdjacency( const NEIndexedTriMesh* mesh );
    NEVertexBuffer* vertexBuffer( const NEIndexedTriMesh* mesh );
    NEVertexBuffer* unIndexedVBuffer( const NEIndexedTriMesh* mesh);
    NEVertexBuffer* boundingBoxBuffer( const NEIndexedTriMesh* mesh);
    NEVertexBuffer* unIndexedBoundingBoxBuffer( const NEIndexedTriMesh *mesh);

    ShaderMetaInfo* getPositionShader();
    ShaderMetaInfo* getLineStippleShader();
    ShaderMetaInfo* getRectYFilterShader();
    ShaderMetaInfo* getColorRampOSDShader();
    ShaderMetaInfo* getFloorDrawingShader();
    ShaderMetaInfo* getSolidWireframeShader();
    ShaderMetaInfo* getQuadWireframeShader();
    ShaderMetaInfo* getCPShader();
    ShaderMetaInfo* getCPWaterShader();

    ShaderMetaInfo* getAOGeomShader();
    ShaderMetaInfo* getAODepthLinearizeShader();
    ShaderMetaInfo* getAODepthLinearizeMSAAShader();
    ShaderMetaInfo* getAOCalcShader();
    ShaderMetaInfo* getHBAOBlurShader();
    ShaderMetaInfo* getHBAOBlur2Shader();
    ShaderMetaInfo* getBgShader();
    ShaderMetaInfo* getPedestalShader();

    ShaderMetaInfo* getFluidPointThicknessShader();
    ShaderMetaInfo* getFluidEllipsoidDepthProgram();
    ShaderMetaInfo* getFluidDepthBlurProgram();
    ShaderMetaInfo* getFluidDiffuseProgram();

    ShaderMetaInfo* getSkyboxShader();
    ShaderMetaInfo* getSkySphereShader();
    ShaderMetaInfo* hairShader();
    ShaderMetaInfo* edgeShader();

    ShaderMetaInfo* smaaEdgeShader();
    ShaderMetaInfo* smaaBlendShader();
    ShaderMetaInfo* smaaNeighborShader();

	uint OpenGLVersion();

protected:
    QMap< NEIndexedTriMesh*, std::shared_ptr< NEGLRenderableMesh > > m_RenderableMeshes;

public slots:
    void createDrawingBuffers( NEIndexedTriMesh* mesh, uint mode );
    void clearDrawingBuffers(NEIndexedTriMesh *mesh );
    void rehintDrawingBuffers( NEIndexedTriMesh* mesh, uint mode );
    void updateDrawingBuffers( NEIndexedTriMesh* mesh, uint mode );

    void calcNormalsMWA(NEIndexedTriMesh *mesh);
    void calcNormalsMWASEL(NEIndexedTriMesh *mesh);
    void calcNormalsMWE(NEIndexedTriMesh *mesh);

    NEGLRenderer* getGLRenderer();

private:
    friend class NERenderManager;

    NE_DECL_PRIVATE(NEGLManager)

    // Shader parsing
    QString getContent(QString filename);
    QString manualInclude(QString filename, QString prepend);

};

struct NECEXP_NERENDERING ShaderMetaInfo
{
    QOpenGLShaderProgram* mShader;
    QString mVertexPath;
    QString mGeometryPath;
    QString mFragPrepend;
    QString mFragmentPath;
    QStringList mSecondaryFragmentList;
    bool mUsesLights;

    QOpenGLShaderProgram* shaderProgram() const;

    ShaderMetaInfo();
    ShaderMetaInfo(QOpenGLShaderProgram* shader, QString vertexPath, QString fragmentPath, bool usesLights);
    ShaderMetaInfo(QOpenGLShaderProgram* shader, QString vertexPath, QString geometryPath, QString fragmentPath, bool usesLights);
    ~ShaderMetaInfo();
    void addFragPrepend(QString prepend);
    void addSecondaryFragShader(QString fragmentPath);
};

//#define GLCTX   NEGLManager::staticShareContext()
//#define GLSURF  NEGLManager::staticOffscreenSurface()

//#define GL      NEGLManager::glFunc()
#define GL    NEGLManager::glFunc33()
#define GL41    NEGLManager::glFunc41()
#define GL43    NEGLManager::glFunc43()
#define GLEXTDSA NEGLManager::glExtDirectStateAccess()
#define GLEXTNTM NEGLManager::glExtNVTextureMultisample()
