#pragma once

#include "NENode.h"

class NEMaterial;

enum NEManipulator
{
	kManipNone = 0,
	kManipTranslate,
	kManipRotate,
	kManipScale
};

NE_FWD_DECL_2(class, NEIndexedTriMesh, NE_PRIVATE(NESpatial))

//! Protected interface of NESpatial
#define INTERFACE_NESpatial_Protected(_Terminal_)\
    virtual void render() _Terminal_\
    virtual void renderPicked() _Terminal_\
    virtual void renderPickable() _Terminal_

//! Public slot interface of NESpatial
#define INTERFACE_NESpatial_Public_Slots(_Terminal_)\
    virtual uint SetPosition(const vec3f& val) _Terminal_\
    virtual uint SetScale(const vec3f& val) _Terminal_\
    virtual uint SetOrientation(const eulerf& rotation) _Terminal_

class NECEXP_CORE NESpatial : public NENode
{
    Q_OBJECT
    NE_CLASS("3D Properties")

    NE_DECL_PRIVATE(NESpatial)
    NE_FRIENDS_3(class, NESpatialRenderer, NEVisWidget, NECamera)

public:
    enum NECenterCalculation { PolyhedralMass = 0, Centroid = 1, BoundingBoxCenter = 2};
    enum NEDisplayMode{ Self, BoundingBox};
    enum NERenderMode{ SmoothShaded, FlatShaded, Wireframe,  QuadWireframe, QuadWireframeOnShaded, QuadWireframeHidden, QuadWireframeOccluded, Hidden, Occluded, Invisible };
    enum NESpatialType { kRigid = 0, kFluid, kOSD, kClip, kGizmo, kGVDB };
    enum NEBoundingBoxType { AxisAlignedBoundingBox, OrientedBoundingBox};

    NE_PROP_3D_RW(vec3f, float, Position, "Position", "Position of the node", positionVisible, virtual)
    NE_PROP_RW(eulerf, Orientation, "Orientation", "Orientation of the node", orientationVisible, virtual)
    NE_PROP_3D_RW(vec3f, float, Scale, "Scale", "Scale of the node", scaleVisible, virtual)
    NE_PROP_2D_RW(vec2f, float, Scale2D, "Scale", "Scale of the node", scale2DVisible, virtual)
    NE_PROP_RW(double, Radius, "Radius", "Radius of the node", radiusVisible, virtual)
    NE_PROP_3D_RW(vec3f, float, Pivot, "Pivot", "Pivot of the node", pivotVisible, virtual)
    NE_PROP_3D_R(vec3f, float, Translation, "Translation", "Translate vector", false)
    NE_PROP_3D_RW(vec3f, float, RotateTranslation, "Rotate Translation", "Translation to compensate in the rotate transform for the change in pivot", false, )
    NE_PROP_3D_RW(vec3f, float, ScaleTranslation, "Scale Translation", "Translation to compensate in the scale transform for the change in pivot", false, )
    NE_PROP_RW(NECenterCalculation, CenterMethod, "Center Calculation", "Method of calculating the center of the object; centroid works only for convex objects", true, virtual)
    NE_PROP_RW(bool, LocalizedTransforms, "Localize Transforms", "Transforms with respect to the center of the object", true, virtual)

    NE_PROP_RW(NEDisplayMode, DisplayMode, "Display Mode", "set if node is rendered or it's proxy.", displayModeVisible,)

    NE_PROP_RW(NERenderMode, RenderMode, "Render Mode", "Visibility of the node", renderModeVisible,)
    NE_PROP_RW(bool, DrawSolidWireframe, "Solid Wireframe", "Solid Wireframe from Nvidia's 2007 white paper, and upgraded contour based on this", solidParamVisible,)
    NE_SUB_PROP_START(DrawSolidWireframeParams, true)
        NE_PROP_RW(QColor, SolidWireframeColor, "Solid Wireframe Color", "Color of solid wireframe", true,)
        NE_PROP_RW(double, SolidWireframeThickness, "Thickness", "Thickness of line", true,)
        NE_PROP_RW(double, ContourThreshold, "Contour Threshold", "The edge threshold value for drawing a contour", true,)
    NE_SUB_PROP_END(DrawSolidWireframeParams)

    NE_PROP_RW(int, RenderPriority, "Render  Priority", "Used to sort nodes when rendering", renderPriorityVisible,)
    NE_PROP_RW(QColor, Color, "Color", "Color of the node", colorVisible,)
    NE_PROP_RW(bool, OccludeShadows, "Occlude Shadows", "Whether to occlude shadow or not", occludeShadowsVisible,)
    NE_PROP_RW(bool, ReceiveShadows, "Receive Shadows", "Whether to receive shadow or not", receiveShadowVisible,)
    NE_PROP_RW(bool, ExternalRender, "External Render", "Controls whether the node should be exported for external rendering.", externalRenderVisible,)
    NE_PROP_RW(bool, ShowBoundingBox, "Show Bounding Box", "Show/Hide Bounding Box.", true,)
    NE_PROP_RW(NEBoundingBoxType, BoundingBoxType, "Bounding Box Type", "Set type of Bounding Box of the spatial object.", boundingBoxVisible,)
    NE_SUB_PROP_START(BoundingBoxParams, true)
        NE_PROP_RW(bool, ZeroCenterGrid, "Grid From Center", "Calculate grid value from center of the spatial object", zeroCenterGridVisible,)
        NE_PROP_RW(double, StringScale, "String Scale", "Axis Text Scale.(0.01 ~ 100)", boundingBoxVisible,)
        NE_PROP_RW(bool, NumbersFromWorldUnits, "World Units", "Draw Numbers in World Units", true,)
        NE_PROP_RW(bool, ShowBoundingBoxCenter, "Show Bounding Box Center", "Display the location of the Center of Bounding Box", true,)
        NE_PROP_RW(QColor, XAxisColor, "X Axis Color", "Color of the X Axis.", boundingBoxVisible,)
        NE_PROP_RW(QColor, YAxisColor, "Y Axis Color", "Color of the Y Axis.", boundingBoxVisible,)
        NE_PROP_RW(QColor, ZAxisColor, "Z Axis Color", "Color of the Z Axis.", boundingBoxVisible,)
    NE_SUB_PROP_END(BoundingBoxParams)
    NE_PROP_3D_R(vec3f, float, BoundingBoxPos, "Bounding Box Center", "Center of Bounding Box", boundingBoxSizeVisible)
    NE_PROP_3D_R(vec3f, float, BoundingBoxSize, "Bounding Box Size", "Size of Bounding Box", boundingBoxSizeVisible)

    Q_ENUMS(NECenterCalculation NEDisplayMode NERenderMode NEBoundingBoxType)

public slots:
    const matrix44f& transform() const;
    const matrix44f& localTransform() const;
    const matrix44f& rotTransTransform() const;
    const matrix44f& localRotTransTransform() const;

    // Compute and return the pivot transform matrix
    matrix44f getPivotTransform() const;
    // Compute and return the pivot inverse transform matrix
    matrix44f getPivotInvTransform() const;
    // Compute and return the rotate transform matrix
    matrix44f getRotateTransform() const;
    // Compute and return the scale transform matrix
    matrix44f getScaleTransform() const;
    // Compute and return the translate transform matrix
    matrix44f getTranslateTransform() const;

    const AABB& aabb() const;
    void setAABB(const AABB& val);

    const vec3f& center() const;
    const quatf& orientationQuat() const;
    virtual uint SetOrientation(const quatf& rotation);

    //
    //! Associated Geometry with the Spatial.
    //
    virtual NEIndexedTriMesh& triangleMesh();
    const NEIndexedTriMesh& triangleMesh() const;
    void setTriMesh(const NEIndexedTriMesh& val);

public:
    NESpatial(NENodeFactory *val);
    virtual ~NESpatial() ;

    //! Called during loading and also called after object is created
    virtual uint init(uint initMode = NE::kInitOnCreate) override;

    //!< Returns the manipulatable ability for each object
    virtual bool isManipulatable(NEManipulator manipType);

    //!< Returns the type of spatial object
    virtual NESpatialType spatialType();

    //! Notifies the renderer about whether transformation before rendering is required or not
    bool needsGLTransform();
    void setNeedsGLTransform(bool val);

    //! Updates the world transform of the node
    virtual void updateWorldTransform();

    //! Returns the world scale, that accounts for the scale of its parent node
    vec3f worldScale() const;

    //! Returns the colorid of the node, which is used for picking
    uint colorID();
    void getColorID4fv(float colorv[]);

    //! Resets all properties to their default values
    virtual void resetProperties() override;

    virtual void computeBoundingBox();
    uint setLocalizedTransforms(const bool& val);
    uint setCenterMethod(const NESpatial::NECenterCalculation& val);

    //!
    virtual void hideProperties(QString classname);

protected:
    //! Declares pure virtual members
    NE_BASE_PURE(NESpatial, Protected)

    //! Returns the clamped scale
    float clampScale(const float Scale, const float spacing);

    virtual void setCenter(const vec3f& center);
    virtual uint localizeTransforms();
    virtual uint relocalizeTransforms();
    virtual uint freezeTransforms();
    virtual uint computeCenter();

    //! Return true/false for making properties visible/invisible
    virtual bool positionVisible() {return true;}
    virtual bool orientationVisible() {return true;}
    virtual bool scaleVisible() {return true;}
    virtual bool scale2DVisible() {return false;}
    virtual bool radiusVisible() {return false;}
    virtual bool pivotVisible() {return true;}
    virtual bool displayModeVisible() {return true;}
    virtual bool renderModeVisible() {return true;}
    virtual bool renderPriorityVisible() {return true;}
    virtual bool externalRenderVisible() {return true;}
    virtual bool boundingBoxVisible() {return true;}
    virtual bool boundingBoxSizeVisible(){ return true; }
    virtual bool zeroCenterGridVisible() {return true;}
    virtual bool colorVisible() {return true;}
    virtual bool solidParamVisible() {return true;}
    virtual bool receiveShadowVisible() {return true;}
    virtual bool occludeShadowsVisible() {return true;}
	
public:
    //! Called during node graph evaluation
    virtual uint eval() override;

    void updateParentAABB();
    void updateChildrenTransform();
    virtual void updateTransform();

    uint SetLocalPositionFromWorld(const vec3f& worldPosition);
    uint SetLocalOrientationFromWorld(const quatf& worldRotation);
    uint SetLocalScaleFromWorld(const vec3f& worldScale);
    quatf getLocalOrientationWithParent(NESpatial* parent);
    quatf updateWorldOrientationtoLocal(NESpatial* spatial);
    vec3f getLocalScaleWithParent(NESpatial* parent) const;
    uint updateLocalToParentLocal();
    uint updateLocalWithOutParentLocal();
    uint SetScaleValue(const vec3f& scale);
    uint SetPositionValue(const vec3f& position);
    uint SetRotationValue(const eulerf& rotation);
    uint SetSmallAdjustmentLocalPositionFromWorld(const vec3f& worldPosition);
    uint SetSmallAdjustmentLocalScaleFromWorld(const vec3f& worldScale);
    uint SetSmallAdjustmentLocalOrientationFromWorld(const quatf& worldRotation);
};


// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NESpatial, "NESpatial")
