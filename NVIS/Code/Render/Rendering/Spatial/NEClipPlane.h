#pragma once

#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Spatial/NESpatial.h"
#include "Utility/NEFrameInfo.h"
#include "Utility/NEAABB.h"

class NEClipPlanePrivate;
class NEIndexedTriMesh;

//! This class will be used to clip .
class NEClipPlane: public NESpatial
{
    Q_OBJECT
    NE_CLASS("Clipping Plane")

public:
    NE_PROP_RW(bool, ClipInvert, "Invert", "Inverts ths clip plane.", ClipInvertVisible,)

public:

    NEClipPlane(NENodeFactory *factory);
    ~NEClipPlane();
    uint init(uint initMode = NE::kInitOnCreate);

    virtual void render();
    virtual void renderPickable();
    virtual void renderPicked();
    virtual NESpatialType spatialType(){return NESpatialType::kClip;}
    virtual void updateUniforms(QOpenGLShaderProgram* program, int cPlanID, bool isActive);

    vec3f PlaneNormal();
    bool isProcess();
    void setProcess(bool bProcess);

    NEIndexedTriMesh& mesh();


public slots:
    //! Resets all properties totheir default values
    virtual void resetProperties();
    virtual uint SetScale(const vec3f& Scale);
    virtual uint SetScaleClip(const vec3f& Scale);

protected:
    virtual void createMesh();
    virtual bool scaleVisible() {return false;}
    virtual bool colorVisible() {return false;}
    virtual bool renderModeVisible() {return false;}
    virtual bool solidParamVisible() {return false;}
    virtual bool receiveShadowVisible() {return false;}
    virtual bool occludeShadowsVisible() {return false;}
    virtual bool ClipInvertVisible() {return true;}

    NEIndexedTriMesh m_TriMesh;
    vec3f m_normal;

private:
    NE_DECL_PRIVATE(NEClipPlane)

};



// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NEClipPlane, "NEClipPlane")


#include "Base/NENodeFactory.h"

class NEClipPlaneFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEClipPlaneFactory(NEManager* m);
    ~NEClipPlaneFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();

private:
    NEObject* createInstance();

protected:

};
