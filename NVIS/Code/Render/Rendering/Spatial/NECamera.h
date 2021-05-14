#pragma once

#include "Spatial/NESpatial.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_2(class, NEIndexedTriMesh, NE_PRIVATE(NECamera))

//! The camera class provides a basic camera functionality.

/*!
    It's inherits NESpatial class and overrides some of its properties.
*/

class NECEXP_NERENDERING NECamera : public NESpatial
{
    Q_OBJECT
    NE_CLASS("General Camera Settings")

public:
    enum OrthographicType
    {
        Custom,
        Top, Bottom,
        Left, Right,
        Front, Back
    };

    NE_PROP_RW(bool, Ortho, "Orthographic Camera", "Sets the orthographic camera", true, )
    NE_SUB_PROP_START(OrthoTypeParams, true)
       NE_PROP_RW(OrthographicType, OrthoType, "Orthographic Camera Type", "Type of Orthographic Camera.", true, )
    NE_SUB_PROP_END(OrthoTypeParams)

    NE_PROP_RW(double, Fov, "Field of View"," Sets the field of view of the camera", true, )
    NE_PROP_RW(double, NearClip, "Near clipping plane", "Sets the distance of the near clipping plane to the camera", true, )
    NE_PROP_RW(double, FarClip, "Far clipping plane"," Sets the distance of the far clipping plane to the camera", true, )
    NE_PROP_3D_RW(vec3f, float, LookAt, "Look At", "Sets the point where the camera looks at", true, )
    NE_PROP_3D_R(vec3f, float, Up, "Up Vector", "Upward vector of the camera", true)
    NE_PROP_3D_R(vec3f, float, Side, "Side Vector", "Side vector of the camera", true)
    NE_PROP_3D_R(vec3f, float, Scale, "Scale", "Sets the scale of the camera", false)
    NE_PROP_3D_R(vec3f, float, Pivot, "Pivot", "Sets the pivot point of the camera", false)
    NE_PROP_RW(double, OrthoWidth, "Orthographic Width"," Sets the orthographic width of the camera", true, )
    NE_PROP_GROUP_START(FlipAxes, "Flip Axes", "", true)
    NE_PROP_RW(bool, FlipSide, "Flip Side", "Flips the camera side vector", true, )
    NE_PROP_RW(bool, FlipUp, "Flip Up", "Flips the camera up vector", true, )
    NE_PROP_RW(bool, FlipLook, "Flip Look", "Flips the camera look vector", true, )
    NE_PROP_GROUP_END(FlipAxes)

    Q_ENUMS(OrthographicType)

public:
    NE_FWD_DECL_3(friend class, NEGLManager, NESceneManager, NE_PRIVATE(NESceneManager))
    NECamera(NENodeFactory* factory);
    ~NECamera();
    uint init(uint initMode = NE::kInitOnCreate);
    virtual void propertyTweak(QtPropertyBase* p);
    //! Returns the current projection matrix.
    const matrix44f& projectionMatrix() const;

    const NEIndexedTriMesh* mesh() const;
    void SetMesh(NEIndexedTriMesh* mesh);

    void computeProjection(int width = 0, int height = 0);

public slots:
    bool fixedOrthographic() const;
    bool customOrthographic() const;
    bool perspective() const;

    //! Returns the normalized look vector
    const vec3f& lookNormalized() const;
    //! Sets the position of the camera. but also constraints the direction to the lookAt
    uint SetPosition(const vec3f& val);
    //! Set the camera position and recalculate the look-at position to keep the same look vector
    uint SetPositionZoom(const vec3f& val);
    //! Translate the camera
    uint Translate(const vec3f& val);
    //! Returns true if the camera is assigned as the current viewer.
    bool isViewer() const;
    //! Assign the camera as the current viewer.
    void setViewer(bool flag);
    //! Returns a reference to the pure rotation matrix
    const matrix33f& pureRotation() const;
    //! Returns a reference to the normalized up std::vector
    const vec3f& up() const;
    //! Returns a reference to the normalized side std::vector (right handed)
    const vec3f& side() const;
    //! Returns a reference to the normalized look at std::vector
    const vec3f& look() const;
    //! Returns the current view matrix.
    const matrix44f& viewMatrix() const;
    virtual void addKeyframe( uint currentFrame, const QStringList& propertyNames, NE::NECurveType type );

    void resetFixedOrtho();
    void resetCustomOrtho();
    void resetPersp();

    uint setOrthoValues();

    void resetCamera();

protected:
    NE_DERIVED(NESpatial, Protected)
    virtual void initializeGUIActions();


private:
    NE_DECL_PRIVATE(NECamera)
};

//! The factory class for instantiating cameras
class NECameraFactory : public NENodeFactory
{
public:
    NECameraFactory(NEManager* m);
    virtual ~NECameraFactory();

    NE_DERIVED(NENodeFactory, Public)
    const NEIndexedTriMesh* mesh();

private:
    NEIndexedTriMesh* m_pMesh;
};

Q_DECLARE_METATYPE(NECamera*)
