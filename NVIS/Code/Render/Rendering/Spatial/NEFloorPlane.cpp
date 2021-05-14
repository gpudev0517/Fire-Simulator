#include "NEFloorPlane.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Base/NEBase.h"

#include "uripdf/mupdfimp.h"


class NEFloorPlanePrivate
{
public:
    NEFloorPlanePrivate(NEFloorPlane* s);

    NESpatial::NESpatialType m_spatialType;
    bool m_isShow;
    NEIndexedTriMesh& m_TriMesh;
    vec3f m_normal;
    QImage m_TextureImage;
    double m_XSize;
    double m_ZSize;
    double m_XMin;
    double m_ZMin;
    double m_XMax;
    double m_ZMax;
    double m_FloorElevation;
    NEFloorPlane::NESwapDegree m_Swap;
    bool m_Flip;
    double m_FloorPlanTransparency;
    NEFloorPlane::NEFloorPlaneEffect m_FloorPlaneEffect;
    FilePath m_FloorPlan;
    bool m_ShowFloorPlan;

    void loadImage();
private:
    NEFloorPlane* S;
};

NE_PROP_DEF(bool, NEFloorPlane, ShowFloorPlan, true)
NE_PROP_DEF(double, NEFloorPlane, FloorElevation, 0.001)
NE_PROP_DEF(FilePath, NEFloorPlane, FloorPlan, FilePath(""))
NE_PROP_DEF(double, NEFloorPlane, FloorPlanTransparency, 1.0f)
NE_PROP_DEF(NEFloorPlane::NEFloorPlaneEffect, NEFloorPlane, FloorPlaneEffect, NEFloorPlane::NEFloorPlaneEffect::AmbientOcclusion)
NE_PROP_DEF(double, NEFloorPlane, XMin, 0.0)
NE_PROP_DEF(double, NEFloorPlane, XMax, 30.0)
NE_PROP_DEF(double, NEFloorPlane, XSize, 30.0)
NE_PROP_DEF(double, NEFloorPlane, ZMin, 0.0)
NE_PROP_DEF(double, NEFloorPlane, ZMax, 30.0)
NE_PROP_DEF(double, NEFloorPlane, ZSize, 30.0)
NE_PROP_DEF(NEFloorPlane::NESwapDegree, NEFloorPlane, Swap, NEFloorPlane::NESwapDegree::Original)
NE_PROP_DEF(bool, NEFloorPlane, Flip, false)

static void initProperties()
{
    NE_INIT_PROP(NEFloorPlane, ShowFloorPlan);
    NE_INIT_PROP(NEFloorPlane, FloorElevation);
    NE_INIT_PROP(NEFloorPlane, FloorPlan);
    NE_INIT_PROP(NEFloorPlane, FloorPlanTransparency);
    NE_INIT_PROP(NEFloorPlane, FloorPlaneEffect);
    NE_INIT_PROP(NEFloorPlane, XMin);
    NE_INIT_PROP(NEFloorPlane, XMax);
    NE_INIT_PROP(NEFloorPlane, XSize);
    NE_INIT_PROP(NEFloorPlane, ZMin);
    NE_INIT_PROP(NEFloorPlane, ZMax);
    NE_INIT_PROP(NEFloorPlane, ZSize);
    NE_INIT_PROP(NEFloorPlane, Swap);
    NE_INIT_PROP(NEFloorPlane, Flip);
}


NEFloorPlanePrivate::NEFloorPlanePrivate(NEFloorPlane* s) : m_TriMesh( s->triangleMesh() ), S(s)
{
    m_spatialType = NESpatial::NESpatialType::kRigid;
    m_isShow = true;
}

void NEFloorPlanePrivate::loadImage()
{
    QString extension = m_FloorPlan.path().split(".").last();
    if(extension == "tif" || extension == "jpg" || extension == "png")
    {
        m_TextureImage = QImage(S->FloorPlan());

        if (m_TextureImage.width() >= m_TextureImage.height() && m_TextureImage.width() > 4096)
                m_TextureImage = m_TextureImage.scaledToWidth(4096);
        if (m_TextureImage.width() < m_TextureImage.height() && m_TextureImage.height() > 4096)
                m_TextureImage = m_TextureImage.scaledToHeight(4096);

        m_TextureImage = m_TextureImage.convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < m_TextureImage.height(); ++y)
        {
            for (int x = 0; x < m_TextureImage.width(); ++x)
            {
                QColor col = m_TextureImage.pixelColor(x, y);
                QColor newCol = QColor(col.red(), col.green(), col.blue(),
                                       (float)col.alpha() * (float)S->FloorPlanTransparency());
                m_TextureImage.setPixelColor(x,y, newCol);
            }
        }
    }
    else if(extension == "pdf")
    {
        void* pdfInstance = pdf_app_init(0, 0);

        int nResult = pdf_app_loadfile(pdfInstance, (wchar_t*) S->FloorPlan().path().utf16(), "");
        if (nResult == 1)
        { // Load Success
            int w = 0, h = 0;
            pdf_app_origin_size(pdfInstance, 1, &w, &h);
            // Load Image of First Page.
            int comps = 4;
            unsigned char* imageBuffer = pdf_app_image_with_number_without_zoom(pdfInstance, 1, &w, &h, &comps);
            if (imageBuffer) {
                m_TextureImage = QImage(imageBuffer, w, h, QImage::Format_RGB32);
                if (m_TextureImage.width() >= m_TextureImage.height() && m_TextureImage.width() > 4096)
                        m_TextureImage = m_TextureImage.scaledToWidth(4096);
                if (m_TextureImage.width() < m_TextureImage.height() && m_TextureImage.height() > 4096)
                        m_TextureImage = m_TextureImage.scaledToHeight(4096);
                m_TextureImage = m_TextureImage.convertToFormat(QImage::Format_ARGB32);
                for (int y = 0; y < m_TextureImage.height(); ++y)
                {
                    for (int x = 0; x < m_TextureImage.width(); ++x)
                    {
                        QColor col = m_TextureImage.pixelColor(x, y);
                        QColor newCol = QColor(col.red(), col.green(), col.blue(),
                                               (float)col.alpha() * (float)S->FloorPlanTransparency());
                        m_TextureImage.setPixelColor(x,y, newCol);
                    }
                }
            }
            // Finalize
            pdf_app_finalize(pdfInstance);
        }
    }
    else
    {
       m_TextureImage = QImage();
    }

    UpdateUI();

    //P->m_textureImage.save("test.png");
}


NEFloorPlane::NEFloorPlane( NENodeFactory * factory): NESpatial( factory )
{
    // Initializes the private members
    P = new NEFloorPlanePrivate(this);
    CALL_ONCE(initProperties);

    P->m_FloorPlan = NE_DEFAULT(FloorPlan);
    P->m_FloorElevation = NE_DEFAULT(FloorElevation);
    P->m_FloorPlan = NE_DEFAULT(FloorPlan);
    P->m_FloorPlanTransparency = NE_DEFAULT(FloorPlanTransparency);
    P->m_FloorPlaneEffect = NE_DEFAULT(FloorPlaneEffect);
    P->m_XMin = NE_DEFAULT(XMin);
    P->m_XMax = NE_DEFAULT(XMax);
    P->m_XSize = NE_DEFAULT(XSize);
    P->m_ZMin = NE_DEFAULT(ZMin);
    P->m_ZMax = NE_DEFAULT(ZMax);
    P->m_ZSize = NE_DEFAULT(ZSize);
    P->m_Swap = NE_DEFAULT(Swap);
    P->m_Flip = NE_DEFAULT(Flip);
    setOutputs( outputs()|NE::kIOGizmo );
}

NEFloorPlane::~NEFloorPlane()
{
    delete P;
}

uint NEFloorPlane::init(uint initMode)
{
    if(!NESpatial::init(initMode))
        return NE::kReturnFail;

    SetOccludeShadows(false);


    return NE::kReturnSuccess;
}

void NEFloorPlane::createMesh()
{
    P->m_TriMesh.resize();

    NECTNPVertex v;


    v.position = vec3f(-XSize()/2,  FloorElevation(), -ZSize()/2);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    switch (P->m_Swap)
    {
        case 0:
            v.tCoord = vec2f(0.0f, 0.0f);
            break;
        case 1:
            v.tCoord = vec2f(0.0f, 1.0f);
            break;
        case 2:
            v.tCoord = vec2f(1.0f, 1.0f);
            break;
        case 3:
            v.tCoord = vec2f(1.0f, 0.0f);
            break;
    }
    P->m_TriMesh.appendVertex(v);
    v.position = vec3f(-XSize()/2, FloorElevation(), ZSize()/2);
    switch (P->m_Swap)
    {
        case 0:
            v.tCoord = vec2f(0.0f, 1.0f);
            break;
        case 1:
            v.tCoord = vec2f(1.0f, 1.0f);
            break;
        case 2:
            v.tCoord = vec2f(1.0f, 0.0f);
            break;
        case 3:
            v.tCoord = vec2f(0.0f, 0.0f);
            break;
    }
    P->m_TriMesh.appendVertex(v);
    v.position = vec3f(XSize()/2, FloorElevation(), ZSize()/2);
    switch (P->m_Swap)
    {
        case 0:
            v.tCoord = vec2f(1.0f, 1.0f);
            break;
        case 1:
            v.tCoord = vec2f(1.0f, 0.0f);
            break;
        case 2:
            v.tCoord = vec2f(0.0f, 0.0f);
            break;
        case 3:
            v.tCoord = vec2f(0.0f, 1.0f);
            break;
    }
    P->m_TriMesh.appendVertex(v);
    v.position = vec3f(XSize()/2, FloorElevation(), -ZSize()/2);
    switch (P->m_Swap)
    {
        case 0:
            v.tCoord = vec2f(1.0f, 0.0f);
            break;
        case 1:
            v.tCoord = vec2f(0.0f, 0.0f);
            break;
        case 2:
            v.tCoord = vec2f(0.0f, 1.0f);
            break;
        case 3:
            v.tCoord = vec2f(1.0f, 1.0f);
            break;
    }
    P->m_TriMesh.appendVertex(v);

    //Add triangles now
    NETriangleIndexed t1(1, 3, 0), t2(1, 2, 3);
    P->m_TriMesh.appendTriangle(t1);
    P->m_TriMesh.appendTriangle(t2);

    if( NEBASE.guiMode() )
        if(P->m_TriMesh.numTriangles() > 0)
            P->m_TriMesh.updateDrawingBuffers(GL_DYNAMIC_DRAW);

    P->m_normal = v.normal;
}

NEIndexedTriMesh& NEFloorPlane::mesh()
{
    return P->m_TriMesh;
}

const double& NEFloorPlane::XMin() const
{
    return P->m_XMin;
}

const double& NEFloorPlane::XMax() const
{
    return P->m_XMax;
}

const double& NEFloorPlane::XSize() const
{
    return P->m_XSize;
}

const double& NEFloorPlane::ZMin() const
{
    return P->m_ZMin;
}

const double& NEFloorPlane::ZMax() const
{
    return P->m_ZMax;
}

const double& NEFloorPlane::ZSize() const
{
    return P->m_ZSize;
}

const NEFloorPlane::NESwapDegree& NEFloorPlane::Swap() const
{
    return P->m_Swap;
}

const bool& NEFloorPlane::Flip() const
{
    return P->m_Flip;
}

bool NEFloorPlane::renderModeVisible() {return false;}

bool NEFloorPlane::solidParamVisible() {return false;}

bool NEFloorPlane::receiveShadowVisible() {return true;}

bool NEFloorPlane::occludeShadowsVisible() {return true;}

bool NEFloorPlane::transparencyVisible()
{
    return FloorPlaneEffect() == NEFloorPlane::Transparency;
}


QImage NEFloorPlane::TextureImage()
{
    return P->m_TextureImage;
}



const double& NEFloorPlane::FloorElevation() const
{
    return P->m_FloorElevation;
}

const FilePath& NEFloorPlane::FloorPlan() const
{
    return P->m_FloorPlan;
}

const double& NEFloorPlane::FloorPlanTransparency() const
{
    return P->m_FloorPlanTransparency;
}

const NEFloorPlane::NEFloorPlaneEffect &NEFloorPlane::FloorPlaneEffect() const
{
    return P->m_FloorPlaneEffect;
}

const bool& NEFloorPlane::ShowFloorPlan() const
{
    return P->m_ShowFloorPlan;
}

uint NEFloorPlane::SetShowFloorPlan(const bool& val)
{
    NE_PROP_SET_EVAL(ShowFloorPlan, P, val);

    if( val == true )
    {
        createMesh();
        P->loadImage();
    }
    else
    {
        std::vector<NETriangleIndexed>& tris = triangleMesh().triangles();
        tris.clear();

        std::vector<NECTNPVertex>& verts = triangleMesh().vertices();
        verts.clear();
        P->m_TextureImage = QImage();
    }
    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetFloorElevation( const double& val )
{
    NE_PROP_SET_EVAL(FloorElevation, P, val);

    createMesh();

    return NE::kReturnSuccess;
}


uint NEFloorPlane::SetFloorPlan( const FilePath& val )
{
    NE_PROP_SET_EVAL(FloorPlan, P, val);
    if( ShowFloorPlan())
    {
        createMesh();
        P->loadImage();
    }
    else
    {
        std::vector<NETriangleIndexed>& tris = triangleMesh().triangles();
        tris.clear();

        std::vector<NECTNPVertex>& verts = triangleMesh().vertices();
        verts.clear();
    }
    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetFloorPlanTransparency( const double& val )
{
    double _val;
    if (val > 1.0)
        _val = 1.0;
    else if (val < 0)
        _val = 0;
    else
        _val = val;
    NE_PROP_SET_EVAL(FloorPlanTransparency, P, _val);
    createMesh();
    P->loadImage();
    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetFloorPlaneEffect( const NEFloorPlane::NEFloorPlaneEffect& val )
{
    NE_PROP_SET_EVAL(FloorPlaneEffect, P, val);

    if( val == NEFloorPlane::NEFloorPlaneEffect::AmbientOcclusion)
    {
        setSpatialType( NESpatialType::kRigid);
    }
    else {
        setSpatialType(NESpatialType::kGizmo);
    }

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetXMin( const double& val )
{
    double _val = qBound((double)0, val, XMax());
    if (_val == XMin())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(XMin, P, _val);


    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetXMax( const double& val )
{
    double _val = qMax(XMin(), val);
    if (_val == XMax())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(XMax, P, _val);

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetXSize( const double& val )
{
    double _val = qMax(0.001, val);
    if (_val == XSize())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(XSize, P, _val);

    createMesh();

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetZMin( const double& val )
{
    double _val = qBound((double)0, val, ZMax());
    if (_val == ZMin())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZMin, P, _val);

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetZMax( const double& val )
{
    double _val = qMax(ZMin(), val);
    if (_val == ZMax())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZMax, P, _val);


    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetZSize( const double& val )
{
    double _val = qMax(0.001, val);
    if (_val == ZSize())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZSize, P, _val);

    createMesh();

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetSwap( const NEFloorPlane::NESwapDegree& val )
{
    NE_PROP_SET_EVAL(Swap, P, val);

    createMesh();

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetFlip( const bool& val )
{
    NE_PROP_SET_EVAL(Flip, P, val);

    return NE::kReturnSuccess;
}

uint NEFloorPlane::SetScale(const vec3f &Scale)
{
    //emit sendScaleFloorPlane( Scale );
    return NESpatial::SetScale( Scale );
}

uint NEFloorPlane::SetPosition(const vec3f &Pos)
{
    //emit sendScaleFloorPlane( Pos );
    return NESpatial::SetPosition( Pos );
}

void NEFloorPlane::render()
{
}

void NEFloorPlane::renderPickable()
{
}

void NEFloorPlane::renderPicked()
{
}

NESpatial::NESpatialType NEFloorPlane::spatialType()
{
    return P->m_spatialType;
}

void NEFloorPlane::setSpatialType(NESpatial::NESpatialType type)
{
    P->m_spatialType = type;
}

NEFloorPlaneFactory::NEFloorPlaneFactory(NEManager* m) : NENodeFactory( m )
{

}

NEFloorPlaneFactory::~NEFloorPlaneFactory() {}

QString NEFloorPlaneFactory::nodeName() { return "Floor Plane"; }

uint NEFloorPlaneFactory::nodeType(){ return NE::kObjectGizmo;}

QString NEFloorPlaneFactory::nodeInherits() { return "Visuals";}

QString NEFloorPlaneFactory::objectName() { return "NEFloorPlane"; }

QString NEFloorPlaneFactory::nodeVersion() { return "0.1"; }

QString NEFloorPlaneFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEFloorPlaneFactory::nodeHelp() { return "NEFloorPlane"; }

NEObject*NEFloorPlaneFactory::createInstance() { return new NEFloorPlane(this); }
