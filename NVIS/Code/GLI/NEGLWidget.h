#pragma once

#include "Utility/NEExternalProperties.h"
#include "Utility/FilePath.h"
#include "PropertyManager/NEPropertyObject.h"

#include "Rendering/Spatial/NELight.h"
#include "Rendering/Manager/NEIGLSurface.h"

#include "NEGLWidget_p.h"

class NECamera;
class NESpatial;
class NEOsd;
class NEOsd3D;
class NEMatrixStack4x4;
class NEVertexBuffer;
class NEGUIManager;
class NEGLWidget;
struct GLDockWidgetBundle;
struct FluidRenderer;

enum SnapedFace
{
    NONE = 0,
    NEGATIVE_X,
    POSITIVE_X,
    NEGATIVE_Y,
    POSITIVE_Y,
    NEGATIVE_Z,
    POSITIVE_Z
};

// FIXME: These should all converted to the new architecture
class NECEXP_NEGLI NEGLWidgetProperties : public NEPropertyObject
{
    Q_OBJECT

    Q_PROPERTY(QString camera READ camera WRITE setCamera)
    Q_CLASSINFO("name-camera", "Camera")
    Q_CLASSINFO("help-camera", "The camera to look through.")

	Q_PROPERTY(bool showOsd READ showOsd WRITE setShowOsd)
    Q_CLASSINFO("name-showOsd", "On-Screen-Display")
    Q_CLASSINFO("help-showOsd", "Should the On-Screen-Display be visible.")
    Q_PROPERTY(SubPropertyStart OsdSubProps READ subStart)
        Q_PROPERTY(QColor osdTextColor READ osdTextColor WRITE setOsdTextColor DESIGNABLE showOsd)
        Q_CLASSINFO("name-osdTextColor", "Text Color")
        Q_CLASSINFO("help-osdTextColor", "Color of the OSD characters,")
    Q_PROPERTY(SubPropertyEnd OsdSubProps_End READ subEnd)

    Q_PROPERTY(bool showFPS READ showFPS WRITE setShowFPS)
    Q_CLASSINFO("name-showFPS", "Display Frequency")
    Q_CLASSINFO("help-showFPS", "Switch ON/OFF if displaying FPS or not.")

    Q_PROPERTY(ManipSpace manipSpace READ manipSpace WRITE setManipSpace )
    Q_CLASSINFO("name-manipSpace", "Manipulator Space")
    Q_CLASSINFO("help-manipSpace", "Set the space for manipulation of objects")

    Q_PROPERTY(bool showGizmo READ showGizmo WRITE setShowGizmo)
    Q_CLASSINFO("name-showGizmo", "Show Gizmo")
    Q_CLASSINFO("help-showGizmo", "Should the show gizmo be visible.")


    Q_PROPERTY(vec3f snapUnit READ snapUnit WRITE setSnapUnit)
    Q_CLASSINFO("name-snapUnit", "Snap Unit")
    Q_CLASSINFO("help-snapUnit", "Should the snap unit be available.")

    Q_PROPERTY(bool viewAxes READ viewAxes WRITE setViewAxes)
    Q_CLASSINFO("name-viewAxes", "World Axes")
    Q_CLASSINFO("help-viewAxes", "Should the world axes be visible.")

    NE_PROP_RW(bool, DrawGrid, "World Grid", "Decide whether to make the world grid visible.", true, )
    Q_PROPERTY(SubPropertyStart GridSubProps READ subStart)

        NE_PROP_RW(vec3f, GridCenter, "Center", "Center of the world grid.", DrawGrid, )
        NE_PROP_RW(eulerf, GridOrientation, "Orientation", "Orientation of the world grid.", DrawGrid, )
        NE_PROP_RW(QSizeF, GridSize, "Grid Size", "Size of the world grid.", DrawGrid, )
        NE_PROP_RW(QSize, CellCount, "Cell Count", "Number of cells of the world grid.", DrawGrid, )
        NE_PROP_RW(QSizeF, CellSize, "Cell Size", "Cell size of the world grid.", DrawGrid, )

        NE_PROP_RW(bool, AddGridLevel, "Add Grid Level", "Decide whether to draw the grid as a two-level grid, with smaller cells and larger cells.", DrawGrid, )
        NE_PROP_RW(QSize, CellDivision, "Cell Division", "Number of smaller cells by larger cells.", showSubGridProperties, )
        NE_PROP_R(QSize, SmallCellCount, "Small-Cell Count", "Number of smaller cells of the world grid.", showSubGridProperties)
        NE_PROP_R(QSizeF, SmallCellSize, "Small-Cell Size", "Small-cell size of the world grid.", showSubGridProperties)

        NE_PROP_RW(double, AxisThickness, "Axis Thickness", "Thickness of the axis.", DrawGrid, )
        NE_PROP_RW(double, GridThickness, "Grid Thickness", "Thickness of the cell lines of the world grid.", DrawGrid, )
        NE_PROP_RW(double, SubGridThickness, "Sub-Grid Thickness", "Thickness of the smaller-cell lines of the world grid.", showSubGridProperties, )

        NE_PROP_RW(QColor, GridColor, "Grid Color", "Color of the world grid", DrawGrid, )
        NE_PROP_RW(QColor, SubGridColor, "Sub-Grid Color", "Color of the world sub-grid", showSubGridProperties, )

        Q_PROPERTY(bool gridMeasurements READ gridMeasurements WRITE setGridMeasurements DESIGNABLE DrawGrid)
        Q_CLASSINFO("name-gridMeasurements", "Grid Measurements")
        Q_CLASSINFO("help-gridMeasurements", "If this is ON then every large grid cell at its corner has a measurement unit in text.")

        Q_PROPERTY(bool overlayGrid READ overlayGrid WRITE setOverlayGrid DESIGNABLE DrawGrid)
        Q_CLASSINFO("name-overlayGrid", "Overlay")
        Q_CLASSINFO("help-overlayGrid", "Controls whether the grid should be overlaid over everything.")
    Q_PROPERTY(SubPropertyEnd GridSubProps_End READ subEnd)


    Q_PROPERTY(bool ssao READ ssao WRITE setSsao DESIGNABLE available430)
    Q_CLASSINFO( "name-ssao", "Ambient Occlusion" )
    Q_CLASSINFO( "help-ssao", "Screen Space Ambient Occlusion")
    Q_PROPERTY(SubPropertyStart SSAOSubProps READ subStart)
        Q_PROPERTY(HbaoType type READ type WRITE setType )
        Q_CLASSINFO("name-type", "Type")
        Q_CLASSINFO("help-type", "HBAO type")

        Q_PROPERTY(double intensity READ intensity WRITE setIntensity )
        Q_CLASSINFO("name-intensity", "Intensity")
        Q_CLASSINFO("help-intensity", "HBAO intensity")

        Q_PROPERTY(double radius READ radius WRITE setRadius )
        Q_CLASSINFO("name-radius", "Radius")
        Q_CLASSINFO("help-radius", "HBAO radius in world space")

        Q_PROPERTY(double blurSharpness READ blurSharpness WRITE setBlurSharpness )
        Q_CLASSINFO("name-blurSharpness", "BlurSharpness")
        Q_CLASSINFO("help-blurSharpness", "HBAO blurSharpness")

    Q_PROPERTY(SubPropertyEnd SSAOSubProps_End READ subEnd)

    Q_PROPERTY(AntialiasingType aaType READ aaType WRITE setAaType  DESIGNABLE available410)
    Q_CLASSINFO( "name-aaType", "Antialising" )
    Q_CLASSINFO( "help-aaType", "Anti-alising type")

    Q_PROPERTY(SubPropertyStart AaTypeSubProps READ subStart)
        Q_PROPERTY(MsaaSampling msaaSampling READ msaaSampling WRITE setMsaaSampling  DESIGNABLE isMsaa)
        Q_CLASSINFO("name-msaaSampling", "MSAA Samples")
        Q_CLASSINFO("help-msaaSampling", "Samples count of MSAA.")

        Q_PROPERTY(CsaaMode csaaMode READ csaaMode WRITE setCsaaMode  DESIGNABLE isCsaa)
        Q_CLASSINFO("name-csaaMode", "CSAA Modes")
        Q_CLASSINFO("help-csaaMode", "Modes of CSAA.")

        Q_PROPERTY(BfaaMode bfaaMode READ bfaaMode WRITE setBfaaMode  DESIGNABLE isBfaa)
        Q_CLASSINFO("name-bfaaMode", "Bfaa Modes")
        Q_CLASSINFO("help-bfaaMode", "Multiplier of BFAA.")
    Q_PROPERTY(SubPropertyEnd AaTypeSubProps_End READ subEnd)

    Q_PROPERTY(GlobalTransparency transparencyType READ transparencyType WRITE setTransparencyType )
    Q_CLASSINFO( "name-transparencyType", "Transparency" )
    Q_CLASSINFO( "help-transparencyType", "Transparency Type")
    Q_PROPERTY(SubPropertyStart TransparencyTypeSubProps READ subStart)
        Q_PROPERTY(int depthPeelingIterations READ depthPeelingIterations WRITE setDepthPeelingIterations  DESIGNABLE isDepthPeeling)
        Q_CLASSINFO("name-depthPeelingIterations", "Peeling Iters")
        Q_CLASSINFO("help-depthPeelingIterations", "Depth peeling iteration count")
    Q_PROPERTY(SubPropertyEnd TransparencyTypeSubProps_End READ subEnd)

    Q_PROPERTY(BackgroundType backgroundType READ backgroundType WRITE setBackgroundType )
	Q_CLASSINFO( "name-backgroundType", "Background Type" )
    Q_CLASSINFO( "help-backgroundType", "The mode of the background - whether flat/shaded/textured")

    Q_PROPERTY(SubPropertyStart BackgroundColorSubProps READ subStart)
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor  DESIGNABLE backgroundColorType)
        Q_CLASSINFO("name-backgroundColor", "Background Color")
        Q_CLASSINFO("help-backgroundColor", "Color of the background.")

        Q_PROPERTY(bool backgroundGradient READ backgroundGradient WRITE setBackgroundGradient  DESIGNABLE backgroundColorType)
        Q_CLASSINFO("name-backgroundGradient", "Background Gradient")
        Q_CLASSINFO("help-backgroundGradient", "Enable/Disable gradient fill background")

        Q_PROPERTY(QColor backgroundGradColor READ backgroundGradColor WRITE setBackgroundGradColor  DESIGNABLE backgroundColorType)
        Q_CLASSINFO("name-backgroundGradColor", "Background Gradient Color")
        Q_CLASSINFO("help-backgroundGradColor", "Other Color of the background.")

        Q_PROPERTY(QColor backgroundRealColor READ backgroundRealColor WRITE setBackgroundRealColor  DESIGNABLE backgroundRealType)
        Q_CLASSINFO("name-backgroundRealColor", "Background Real Color")
        Q_CLASSINFO("help-backgroundRealColor", "Other Color of the background.")
    Q_PROPERTY(SubPropertyEnd BackgroundColorSubProps_End READ subEnd)

    //Q_PROPERTY(SubPropertyStart BackgroundTexSubProps READ subStart )
        Q_PROPERTY(FilePath backgroundTexture READ backgroundTexture WRITE setBackgroundTexture  DESIGNABLE backgroundTextureType )
        Q_CLASSINFO( "name-backgroundTexture", "Background Texture File" )
        Q_CLASSINFO( "help-backgroundTexture", "Background texture (PNG/JPG/TIF) file")
    //Q_PROPERTY(SubPropertyEnd BackgroundTexSubProps READ subEnd)

    Q_PROPERTY(EnvironmentType environmentType READ environmentType WRITE setEnvironmentType)
    Q_CLASSINFO( "name-environmentType", "Environment Type" )
    Q_CLASSINFO( "help-environmentType", "The mode of the environment - whether cubemap/equirectangular sphere")

    Q_PROPERTY(FilePath environmentMap READ environmentMap WRITE setEnvironmentMap DESIGNABLE EnvironmentTypeSphere )
    Q_CLASSINFO( "name-environmentMap", "Spherical Environment Map" )
    Q_CLASSINFO( "help-environmentMap", "Spherical Environment Map(PNG/JPG/TIF/HDR) file")

    Q_PROPERTY(bool environmentRender READ environmentRender WRITE setEnvironmentRender DESIGNABLE EnvironmentTypeOn )
    Q_CLASSINFO( "name-environmentRender", "Show Environment Map" )
    Q_CLASSINFO( "help-environmentRender", "Show Environment Map in background")

    Q_PROPERTY(double envRotation READ envRotation WRITE setEnvRotation DESIGNABLE EnvironmentTypeSphere )
    Q_CLASSINFO( "name-envRotation", "Env Rotation" )
    Q_CLASSINFO( "help-envRotation", "Spherical environment rotation in degrees")


    Q_PROPERTY(FilePath rightEnv READ rightEnv WRITE setRightEnv DESIGNABLE EnvironmentTypeCube )
	Q_CLASSINFO( "name-rightEnv", "Right Env" )
    Q_CLASSINFO( "help-rightEnv", "Right side environment(PNG/TIF) file")

    Q_PROPERTY(FilePath leftEnv READ leftEnv WRITE setLeftEnv DESIGNABLE EnvironmentTypeCube )
	Q_CLASSINFO( "name-leftEnv", "Left Env" )
    Q_CLASSINFO( "help-leftEnv", "Left side environment(PNG/TIF) file")

    Q_PROPERTY(FilePath topEnv READ topEnv WRITE setTopEnv DESIGNABLE EnvironmentTypeCube )
	Q_CLASSINFO( "name-topEnv", "Top Env" )
    Q_CLASSINFO( "help-topEnv", "Top side environment(PNG/TIF) file")

    Q_PROPERTY(FilePath bottomEnv READ bottomEnv WRITE setBottomEnv DESIGNABLE EnvironmentTypeCube )
	Q_CLASSINFO( "name-bottomEnv", "Bottom Env" )
    Q_CLASSINFO( "help-bottomEnv", "Bottom side environment(PNG/TIF) file")

    Q_PROPERTY(FilePath frontEnv READ frontEnv WRITE setFrontEnv DESIGNABLE EnvironmentTypeCube )
	Q_CLASSINFO( "name-frontEnv", "Front Env" )
    Q_CLASSINFO( "help-frontEnv", "Front side environment(PNG/TIF) file")

    Q_PROPERTY(FilePath backEnv READ backEnv WRITE setBackEnv DESIGNABLE EnvironmentTypeCube )
	Q_CLASSINFO( "name-backEnv", "Back Env" )
    Q_CLASSINFO( "help-backEnv", "Back side environment(PNG/TIF) file")

    Q_PROPERTY(QualityProfile qualityProfile READ qualityProfile WRITE setQualityProfile)
    Q_CLASSINFO("name-qualityProfile", "Quality Profile")
    Q_CLASSINFO("help-qualityProfile", "An option in the view to switch Low Quality/High Quality")

    Q_PROPERTY(bool drawRenderArea READ drawRenderArea WRITE setDrawRenderArea)
    Q_CLASSINFO("name-drawRenderArea", "Draw Render Area")
    Q_CLASSINFO("help-drawRenderArea", "Should the external render area be marked with a rectangular box?")

    Q_PROPERTY(bool saveGLFrames READ saveGLFrames WRITE setSaveGLFrames)
    Q_CLASSINFO("name-saveGLFrames", "Save Frames")
    Q_CLASSINFO("help-saveGLFrames", "Should the rendered frames be saved when an animation is in progress?")
    Q_PROPERTY(QSize savedFrameSize READ imageSizeForSaving WRITE setImageSizeForSaving)
    Q_CLASSINFO("name-savedFrameSize", "Resolution")
    Q_CLASSINFO("help-savedFrameSize", "Resolution of the saved frames.")


    friend class NEGLWidget;

    Q_ENUMS( HbaoType )
    Q_ENUMS( BackgroundType )
    Q_ENUMS( EnvironmentType )
    Q_ENUMS( ManipSpace )
    Q_ENUMS( AntialiasingType )
    Q_ENUMS( MsaaSampling )
    Q_ENUMS( FxaaProfile )
    Q_ENUMS( CsaaMode )
    Q_ENUMS( BfaaMode )
    Q_ENUMS( GlobalTransparency )
    Q_ENUMS( QualityProfile )

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
        //Bfaa = 5
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

    NEGLWidgetProperties(NEGLWidget *val );
	~NEGLWidgetProperties();

    bool gridMeasurements();

	bool overlayGrid() const;
	QString camera() const;
	int clipPlaneCount() const;
	const vec4f& clipPlane2() const;
	const vec4f& clipPlane3() const;
	const vec4f& clipPlane4() const;
	const vec4f& clipPlane5() const;
	const vec4f& clipPlane6() const;
	bool backgroundGradient() const;
    double cellLineThickness() const;
    double subGridThickness() const;
    double axisThickness() const;
	BackgroundType backgroundType() const;
	bool backgroundColorType() const;
	bool backgroundTextureType() const;
    bool backgroundRealType() const;
	FilePath backgroundTexture() const;

    EnvironmentType environmentType() const;
    bool EnvironmentTypeCube() const;
    bool EnvironmentTypeSphere() const;
    bool EnvironmentTypeOn() const;
    bool environmentRender() const;

	bool available430() const;
	bool available410() const;
    bool ssao() const;
    HbaoType type() const;
    double intensity() const;
	double radius() const;
	double blurSharpness() const;
    AntialiasingType aaType() const;
	bool isMsaa() const;
    bool isFxaa() const;
    bool isCsaa() const;
    bool isBfaa() const;
    MsaaSampling msaaSampling() const;
    FxaaProfile fxaaProfile() const;
    CsaaMode csaaMode() const;
    BfaaMode bfaaMode() const;
    GlobalTransparency transparencyType() const;
    bool isDepthPeeling() const;
    int depthPeelingIterations() const;
	ManipSpace manipSpace() const;
	FilePath rightEnv() const;
	FilePath leftEnv() const;
	FilePath topEnv() const;
	FilePath frontEnv() const;
	FilePath backEnv() const;
	FilePath bottomEnv() const;

    QColor backgroundGradColor() const;
    QColor backgroundRealColor() const;

    void setBackgroundGradColor(QColor backgroundGradColor);
    void setBackgroundRealColor(QColor backgroundRealcolor);


    FilePath environmentMap() const
    {
        return m_environmentMap;
    }
    double envRotation() const;

public slots:
	QColor backgroundColor() const;
	void setBackgroundColor(QColor val);
    //! Returns whether drawing of view axes is on or not
	bool viewAxes() const;
	void setViewAxes(bool val);
    //! show gizmo
    bool showGizmo() const;
    void setShowGizmo(bool val);
    //! snap unit
    vec3f snapUnit() const;
    void setSnapUnit(vec3f val);
    //! Returns whether clipping is on or not
	bool clipping() const;
	void setClipping(bool val);
    //! Returns the clipping plane coefficients
	vec4f clipPlane1();
    void setClipPlane1(const vec4f& val);
    //! Returns whether the OSD is being shown
	bool showOsd() const;
    //! Shows/hides the OSD
	void setShowOsd(bool val);
    //! OSD text color
	QColor osdTextColor() const;
	void setOsdTextColor(QColor val);

    bool showFPS() const;
    void setShowFPS(bool val);

    //! Quality profile
    QualityProfile qualityProfile() const;
    void setQualityProfile(QualityProfile val);
    bool checkAOQualityProfile() const; //!< Check if AO is enabled in current quality profile
    bool checkShadowQualityProfile() const; //!< Check if shadowing is enabled in current quality profile
    bool checkAAQualityProfile() const; //!< Check if Anti-aliasing is enabled in current quality profile
    bool checkTransparencyQualityProfile() const; //!< Check if Transparency is enabled in current quality profile
    //! Dynamic Quality
    bool dynamicQuality() const;
    void setDynamicQuality(bool val);

    //! Returns whether the external render area is drawn or not (should be moved to each camera separately in the future)
	bool drawRenderArea() const;
	void setDrawRenderArea(bool val);
    //! Flag that control saving of GL frames for the gl widget
	bool saveGLFrames() const;
    void setSaveGLFrames(bool val);


    //! Image size of saved OpenGL frames
	NEImageSize imageSizeForSaving() const;
    void setImageSizeForSaving(NEImageSize s);
    void setGridMeasurements(bool arg);

    void resizeTimeout();
    void setOverlayGrid(bool arg);
    bool setCamera(QString arg);
    void setClipPlaneCount(int arg);
    void setClipPlane2(const vec4f &arg);
    void setClipPlane3(const vec4f &arg);
    void setClipPlane4(const vec4f &arg);
    void setClipPlane5(const vec4f &arg);
    void setClipPlane6(const vec4f &arg);
	void setBackgroundGradient(bool arg);
	void setBackgroundType(BackgroundType arg);
    void setBackgroundTexture(FilePath arg);
    void setEnvironmentType(EnvironmentType arg);
    void setEnvironmentRender(bool arg);

    void setSsao(bool ssao);
    void setType(HbaoType type);
    void setIntensity(double intensity);
	void setRadius(double radius);
	void setBlurSharpness(double blurSharpness);
    void setAaType(AntialiasingType aaType);
    void setMsaaSampling(MsaaSampling msaaSamples);
    void setFxaaProfile(FxaaProfile fxaaProfile);
    void setCsaaMode(CsaaMode csaaMode);
    void setBfaaMode(BfaaMode bfaaMode);
    void setTransparencyType(GlobalTransparency transparencyType);
    void setDepthPeelingIterations(int iters);
	void setManipSpace(ManipSpace manipSpace);
    void setRightEnv(FilePath rightEnv);
    void setLeftEnv(FilePath leftEnv);
    void setTopEnv(FilePath topEnv);
    void setBottomEnv(FilePath bottomEnv);
    void setFrontEnv(FilePath frontEnv);
    void setBackEnv(FilePath backEnv);
    void setEnvironmentMap(FilePath environmentMap);
    void setEnvRotation(double envRotation);

signals:
	void showOsdChanged(bool);
    void showFPSChanged(bool);
	void viewAxesChanged(bool);
	void clippingChanged(bool);
    void dynamicQualityChanged(bool);
	void drawRenderAreaChanged(bool);
	void saveGLFramesChanged(bool);

private:

    //! The clipping plane flag and equation
    bool m_Clipping;
    //! OpenGL clipping plane coefficients are stored here
//    vec4f m_ClipPlane;
    vec4f m_ClipPlane[6];
    //! Color of the text of the OSD
    QColor m_OsdTextColor;
    //! Flag that controls saving of OpenGL frames
    bool m_SaveGLFrames;
    //! Flag that keeps the state of saving motionmap
    bool m_SaveMotionMap;

    NEGLWidget* m_pGLWidget;

    bool m_DrawGrid;
    vec3f m_GridCenter;
    eulerf m_GridOrientation;
    QSizeF m_GridSize;
    QSize m_CellCount;
    QSizeF m_CellSize;
    bool m_AddGridLevel;
    QSize m_CellDivision;
    QSize m_SmallCellCount;
    QSizeF m_SmallCellSize;
    double m_AxisThickness;
    double m_GridThickness;
    double m_SubGridThickness;
    QColor m_GridColor;
    QColor m_SubGridColor;

    QString m_Camera;
    int m_ClipPlaneCount;

    void computeCellCount();
    void computeCellSize();
    void computeSmallCellCount();
    void computeSmallCellSize();

    bool showSubGridProperties()
    { return DrawGrid() && AddGridLevel(); }

	bool clip1Enabled();
	bool clip2Enabled();
	bool clip3Enabled();
	bool clip4Enabled();
	bool clip5Enabled();
	bool clip6Enabled();

    MsaaSampling m_msaaSamples;
    FxaaProfile m_fxaaProfile;
    CsaaMode m_csaaMode;
    BfaaMode m_bfaaMode;
    bool isSupportCsaa();
    ManipSpace m_manipSpace;

    FilePath m_rightEnv;
    FilePath m_leftEnv;
    FilePath m_topEnv;
    FilePath m_bottomEnv;
    FilePath m_frontEnv;
    FilePath m_backEnv;

    std::vector< QString > m_envList;

    FilePath m_environmentMap;

};

//#define prop() static_cast<NEGLWidgetProperties*>(propertyObject())

class QOpenGLContext;
class QOffscreenSurface;


class NECEXP_NEGLI NEGLWidget : public QOpenGLWidget, public NEExternalProperties<NEGLWidgetProperties>, public IGLSurface
{
	Q_OBJECT

    friend class NEGLWidgetProperties;
    friend class NEGUIManager;
    friend class NEGUIManagerPrivate;


    //For FDYN project.
    friend class FDYNGUIManager;
    friend class FDYNGUIManagerPrivate;
    friend class FDYNMainWindow;

public:

    enum NE3DTool
    {
        kSize = 5000,
        kLocation,
        kExtrude
    };

	//! Constructs an NEGLWidget
    NEGLWidget(QWidget *parent = 0);
	~NEGLWidget();


    void init();

    void reloadBuffer();
    void setMsaaSamples(int samples);
    void setCsaaMode(NEGLWidgetProperties::CsaaMode csaaMode);
    void setBfaaMultiplier(int multiplier);

    // FBO
    void bindFinalFrameBuffer();
    void bindPickFrameBuffer();

    void drawWaterMesh(const NEIndexedTriMesh *mesh, matrix44f modelMatrix);
    void drawWaterMesh_CP(const NEIndexedTriMesh *mesh, matrix44f modelMatrix, bool useVertexColor);

    virtual GLuint defaultFramebuffer(){ return defaultFramebufferObject();}

    virtual void drawOSDMessage(float width, float height);

    virtual bool isVisibleSurface() { return isVisible();}

    //! Creates some utility resources shared by all gl widgets
    static void initStaticData();
    //! Clears the allocated resources (called on program exit)
    static void clearStaticData();

signals:

    void addOSD(uint rank, const NEOsdMsg& msg);
    void addOSD(uint rank, const NEOsdMsg& msg, const NEObject* object);
    void drawOSD(IGLSurface *, int, int );

    void selectObject( QObject *node );
    void selectObjects(QList<NENode*>& objects);
    void setCameraUI(NECamera* cam);
    void setMenuContextMenuPolicySignal(int contextMenuPolicy);

    void updatePropertyEditorUI();
    void updateGLWindows();

    void splitGLDockHorizontally( NEGLWidget * );
    void splitGLDockVertically( NEGLWidget * );
    void splitGLDockToQuad( NEGLWidget * );

    void closeGLWidget( NEGLWidget * );
    void hideGLWidget( NEGLWidget * );

    void dropCalled(matrix44f viewMat, matrix44f projMat, double x, double y, double w, double h, void *tItem);
    void popupDescriptionAndName(const QPoint &pos, QWidget *w, NENode *selSpatial, QPainter *painter);

    void showPropertyShortCut(const QPoint &pos, QWidget *w, NENode *selSpatial);

public slots:
    //! Splits the widget into two horizontally
	void splitHorizontally();
    //! Splits the widget into two vertcally
	void splitVertically();
    //! Splits the widget into four widgets
	void splitToQuad();
    //! Closes the widget
    void close();
    void hide();

    void resetCamera();

    void saveGLFrameData();

    //! Return the center & AABB of selected object(s) if selected or center and AABB of All objects in scene if nothing is selected
    void getSelectedInfo( vec3f& centerSel, AABB& selObjectAABB );
    //! Frame the selected object(s) or All in the middle of the camera plane
    void frameSelectionOrtho();
    //! Frame the selected object(s) or All to fit into the camera frustrum
    void frameSelection();
    void resizeTimeout();

    void lockMainThread();
    void unLockMainThread();

    void slotDropped(QVariant vx, QVariant vy, QVariant vArea);
    void hideDescription();

protected:
	//! Called once before the first call to paintGL() or resizeGL().
	void initializeGL();
	//! The paint event handler of the widget
    void paintGL();
	//! Handles when the user presses a mouse button in the client area of the widget
	void mousePressEvent(QMouseEvent* event);
	//! Handles when the user releases a mouse button in the client area of the widget
	void mouseReleaseEvent(QMouseEvent* event);
	//! Handles when the user moves the mouse in the client area of the widget
	void mouseMoveEvent(QMouseEvent* event);
	//! Handles when the user presses a key while the widget is active
	void keyPressEvent(QKeyEvent* event);
	//! Handles when the user releases a key while the widget is active
	void keyReleaseEvent(QKeyEvent* event);
	//void resizeEvent(QResizeEvent* event);

    void resizeEvent(QResizeEvent* event);

    //! Called whenever the widget is resized.
    void resizeGL(int width, int height);

private:
    //! Returns the minimum size for the widget
    QSize minimumSizeHint() const;

    //! Recursively renders scene contents
    void renderSceneContentsRecursive(NESpatial *spatial);
	//! The picking function
    NENode *pick(int x, int y, int pixelRatio, bool multi = false, bool bForSelect = true, bool bDeselectedWhenSelected=false);
	//! Other widgets on top of OpenGL can be rendered here
	void render2DWidgets();

    QMutex m_renderMutex;
    //
    //! Load background cubemap texture file
    //
    bool  loadCubeMapTexture( std::vector<QString>& faces );

    //
    //! Load sphere environment map (PNG/JPG/TIF/HDR) file
    //
    bool  loadEnvironmentMap( QString path );

	//! Draws the small look-at shape of the camera
	void drawCameraLookAt();

    //! Initializes new framebuffer object for motionVector
    void initFramebufferForMotionVector();
    //! Saves the rendered motion std::vector image
    void captureMotionVector();
    //! Renders motion vectors for the objects in the scene
    void renderMotionVectors();

	//! Processes mouse click picking
    NENode *processPickEvent(int x, int y, bool multi=false, bool bForSelect = true, bool bDeselectedWhenSelected = false );
	//! Processes mouse drag picking
    void processDragPickEvent(int pixelRatio);

	//! Draws the active object manipulator
    virtual void drawManipulator(NESpatial* object, bool pick=false);
	void drawTranslationManipulator(NESpatial* object, bool pick);
	void drawRotationManipulator(NESpatial* object, bool pick);
    void drawScaleManipulator(NESpatial* object, bool pick);

    void drawSizeManipulator(NESpatial* object, bool pick);
    void drawLocationManipulator(NESpatial* object, bool pick);

	//! Below functions cache some primitives to display lists for faster drawing
    static void cacheUnitCube();
	void draw2DWidgets( QPainter* painter );

    bool manipSelected(int x, int y);

    //! Below are shared between all gl widgets
    static bool m_ActiveManipOperation; //!< True if user is manipulating object
    static uint m_ActiveManipulator;
    static bool m_SnapToBoundary;
    static bool m_SnapToObj;
    static bool m_bSnapUnit;
    static bool m_SnapToEdge;
    static bool m_Measure;
    static bool m_MeasureEdge;
    static bool m_SnapModeChanged;

    //! Saves the current mode of the trackball, like translation, rotation..
	uint m_Tracking;
	//! Other Trackball related variables
	QPoint mousePosOld;
	QPoint mousePressPos;
	quatf camQuat;

    vec3f m_SelectedPos; // selected object's position when pressing mouse about moving gizmo.
    AABB m_SelectedAABB; // selected object's AABB when pressing mouse abount moving gizmo

    SnapedFace m_facedDirection; //for Snap in Scaling
    NESpatial *m_facedOtherSpatial; // selected object's scale when pressing mouse about scale gizmo.
    SnapedFace m_snapedFace;

	//! Pointers to some static utility geometry
    static GLuint DL_Cone;
	static GLuint DL_Cube;

	//! Objects below are for motion std::vector
	GLuint fbo; // framebuffer object
	GLuint depthbuffer; // renderbuffer object
	GLuint img; //texture object

    //! The context menu of the widget
    ///QMenu* m_pCamContextMenu;
	
    //! Selection state related
    bool m_bSelectionStarted;
	QPoint m_SelectionFirst;
	QPoint m_SelectionSecond;
    QRect m_SelectionArea;

    NENode *m_PopupSpatial;
    QPoint m_PopupPos;
    mutable int mouseOldX, mouseOldY;
    mutable vec3f clickLocalUp;     /**< The local cam up when mouse is clicked */
    QTimer* resizeTimer;            /**< A timer used for lazy resizing of the gl widget */
    QResizeEvent* lastResizeEvent;  /**< Keeps the last resize event */
    bool resizeFinished;            /**< Set when the resize is finished */
    GLDockWidgetBundle* m_pBundle;  /**< Pointer to the dock widget that contains this gl widget */

    IGizmo* m_moveGizmo;
    IGizmo* m_scaleGizmo;
    IGizmo* m_rotateGizmo;

    IGizmo* m_sizeGizmo; // for only zone compartment
    IGizmo* m_locationGizmo; // for only zone compartment.

    matrix44f m_gizmoTransform;
    float m_manipulatorRenderScale;

    double m_frameTime;

    static double m_SmallIncrementalAdjustment;

    //!< Check if moving camera or manipulating object
    virtual bool isSceneDynamic();
};
