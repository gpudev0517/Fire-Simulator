#include <QJsonObject>
#include "FZone.h"
#include "Mesh/NESpatialMesh.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include <QImage>
#include "FScenario.h"
#include "FZoneItem.h"
#include "FVent.h"
#include "Resource/Mesh/NEMeshContainer.h"
#include "FZoneCompartment.h"
#include "uripdf/mupdfimp.h"

class FZonePrivate
{
public:
    FZonePrivate(FZone* s) : S(s)
    {
    }

    /* Fonctions */
    void computeCompBBSize();
    void computeCompBBCenter();
    void computeCompBBOuter();
    void updateChildGhostPositions();
    double interpValue(const std::vector<std::pair<double, double>>& timeValuePairs, double time);

    /* Properties */
    uint m_ConnectorID;
    QString m_Issues;
    bool m_ShowCompBB;
    vec3f m_CompBBSize;
    vec3f m_CompBBCenter;

    /* Other members */
    QString m_ZoneName;
    QList< QString > m_Scenarios;
    QList< QString > m_FranxItems;
    QString m_Model3DInfo;
    // Inner and outer compartment axis-aligned bounding boxes
    AABB m_CompAABB;
    AABB m_OuterCompAABB;
    // Ceiling/floor/wall thicknesses and materials
    bool   m_ShowFloorPlan;
    double m_FloorElevation;
    FilePath m_FloorPlan;
    int m_FloorPlanTransparency;
    double m_CeilingThickness;
    double m_FloorThickness;
    double m_WallThickness;
    double m_XMin;
    double m_XMax;
    double m_XSize;
    double m_ZMin;
    double m_ZMax;
    double m_ZSize;
    FZone::FSwapDegree m_Swap;
    bool m_Revert;
    FZone::NELineType m_LineType;
    NESurfaceMaterial m_CeilingMaterial;
    NESurfaceMaterial m_FloorMaterial;
    NESurfaceMaterial m_WallMaterial;
    double m_WallLeakRatio;
    double m_FloorLeakRatio;

    // Simulation output data
    std::vector<std::pair<double, double>> m_TimeLayerHeightPairs;
    std::vector<std::pair<double, double>> m_TimeLowerLayerTempPairs;
    std::vector<std::pair<double, double>> m_TimeHigherLayerTempPairs;
    std::vector<std::pair<double, double>> m_TimeCeilingSurfTempPairs;
    std::vector<std::pair<double, double>> m_TimeFloorSurfTempPairs;
    std::vector<std::pair<double, double>> m_TimeLowerWallSurfTempPairs;
    std::vector<std::pair<double, double>> m_TimeHigherWallSurfTempPairs;
    double m_MinLayerTemp;
    double m_MaxLayerTemp;
    double m_MinSurfTemp;
    double m_MaxSurfTemp;

    QImage m_textureImage;
    bool m_renderImage = false;
    double m_prevXSize = 10.0f;
    double m_prevZSize = 10.0f;
private:
    FZone* S;
};

NE_PROP_DEF(FZone::NELineType, FZone, LineType, FZone::NELineType::Dot)
NE_PROP_DEF(bool, FZone, ShowFloorPlan, true)
NE_PROP_DEF(double, FZone, FloorElevation, 0.0)
NE_PROP_DEF(FilePath, FZone, FloorPlan, FilePath(""))
NE_PROP_DEF(int, FZone, FloorPlanTransparency, 255)
NE_PROP_DEF(double, FZone, XMin, 0.0)
NE_PROP_DEF(double, FZone, XMax, 10.0)
NE_PROP_DEF(double, FZone, XSize, 10.0)
NE_PROP_DEF(double, FZone, ZMin, 0.0)
NE_PROP_DEF(double, FZone, ZMax, 10.0)
NE_PROP_DEF(double, FZone, ZSize, 10.0)
NE_PROP_DEF(FZone::FSwapDegree, FZone, Swap, FZone::FSwapDegree::Original)
NE_PROP_DEF(bool, FZone, Revert, false)
NE_PROP_DEF(double, FZone, CeilingThickness, 0.0)
NE_PROP_DEF(double, FZone, FloorThickness, 0.0)
NE_PROP_DEF(double, FZone, WallThickness, 0.0)
NE_PROP_DEF(NESurfaceMaterial, FZone, CeilingMaterial, NESurfaceMaterial(""))
NE_PROP_DEF(NESurfaceMaterial, FZone, FloorMaterial, NESurfaceMaterial(""))
NE_PROP_DEF(NESurfaceMaterial, FZone, WallMaterial, NESurfaceMaterial(""))

NE_PROP_DEF(double, FZone, WallLeakRatio, 0.0)
NE_PROP_DEF(double, FZone, FloorLeakRatio, 0.0)

NE_PROP_DEF(uint, FZone, ConnectorID, 0)
NE_PROP_DEF(QString, FZone, Issues, QString(""))
NE_PROP_DEF(bool, FZone, ShowCompBB, false)

static void initProperties()
{
    NE_INIT_PROP(FZone, LineType);
    NE_INIT_PROP(FZone, ConnectorID );
    NE_INIT_PROP(FZone, Issues);
    NE_INIT_PROP(FZone, ShowCompBB);
    NE_INIT_PROP(FZone, ShowFloorPlan);
    NE_INIT_PROP(FZone, FloorElevation);
    NE_INIT_PROP(FZone, FloorPlan);
    NE_INIT_PROP(FZone, FloorPlanTransparency);
    NE_INIT_PROP(FZone, XMin);
    NE_INIT_PROP(FZone, XMax);
    NE_INIT_PROP(FZone, XSize);
    NE_INIT_PROP(FZone, ZMin);
    NE_INIT_PROP(FZone, ZMax);
    NE_INIT_PROP(FZone, ZSize);
    NE_INIT_PROP(FZone, Swap);
    NE_INIT_PROP(FZone, Revert);
    NE_INIT_PROP(FZone, CeilingThickness);
    NE_INIT_PROP(FZone, FloorThickness);
    NE_INIT_PROP(FZone, WallThickness);
    NE_INIT_PROP(FZone, CeilingMaterial);
    NE_INIT_PROP(FZone, FloorMaterial);
    NE_INIT_PROP(FZone, WallMaterial);
    NE_INIT_PROP(FZone, WallLeakRatio);
    NE_INIT_PROP(FZone, FloorLeakRatio);
}

FZone::FZone(NENodeFactory* factory) : NEZone(factory)
{
    P = new FZonePrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);
    P->m_ConnectorID = NE_DEFAULT(ConnectorID);
    P->m_Issues = NE_DEFAULT(Issues);
    P->m_ShowCompBB = NE_DEFAULT(ShowCompBB);
    P->m_FloorPlan = NE_DEFAULT(FloorPlan);
    P->m_FloorElevation = NE_DEFAULT(FloorElevation);
    P->m_FloorPlan = NE_DEFAULT(FloorPlan);
    P->m_FloorPlanTransparency = NE_DEFAULT(FloorPlanTransparency);
    P->m_XMin = NE_DEFAULT(XMin);
    P->m_XMax = NE_DEFAULT(XMax);
    P->m_XSize = NE_DEFAULT(XSize);
    P->m_ZMin = NE_DEFAULT(ZMin);
    P->m_ZMax = NE_DEFAULT(ZMax);
    P->m_ZSize = NE_DEFAULT(ZSize);
    P->m_Swap = NE_DEFAULT(Swap);
    P->m_Revert = NE_DEFAULT(Revert);
    P->m_CeilingThickness = NE_DEFAULT(CeilingThickness);
    P->m_FloorThickness = NE_DEFAULT(FloorThickness);
    P->m_WallThickness = NE_DEFAULT(WallThickness);
    P->m_CeilingMaterial = NE_DEFAULT(CeilingMaterial);
    P->m_WallMaterial = NE_DEFAULT(WallMaterial);
    P->m_FloorMaterial = NE_DEFAULT(FloorMaterial);
    P->m_WallLeakRatio = NE_DEFAULT(WallLeakRatio);
    P->m_FloorLeakRatio = NE_DEFAULT(FloorLeakRatio);

    setOutputs( outputs() | NE::kIOTriangleMesh );
}

FZone::~FZone()
{
    delete P;
}

uint FZone::init(uint initMode)
{
    int ret = NEZone::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    if(zoneName().isEmpty())
        setZoneName(objectName());

    P->m_WallThickness = P->m_WallMaterial.thickness();
    P->m_CeilingThickness = P->m_CeilingMaterial.thickness();
    P->m_FloorThickness = P->m_FloorMaterial.thickness();

    P->m_MinLayerTemp = -DBL_MAX;
    P->m_MaxLayerTemp = DBL_MAX;
    P->m_MinSurfTemp = -DBL_MAX;
    P->m_MaxSurfTemp = DBL_MAX;

    computeCompBB();

    return NE::kReturnSuccess;
}

void FZone::createMesh()
{
//    NEIndexedTriMesh& triMesh = triangleMesh();

//    triMesh.resize();

//    NECTNPVertex v;

//    v.position = vec3f(-XSize()/2,  P->m_FloorElevation, -ZSize()/2);
//    v.normal = vec3f(0.0f, 1.0f, 0.0f);
//    switch (P->m_Swap) {
//        case FZone::FSwapDegree::Original:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//        case FZone::FSwapDegree::degree_90:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//        case FZone::FSwapDegree::degree_180:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//        case FZone::FSwapDegree::degree_270:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//    }
//    triMesh.appendVertex(v);
//    v.position = vec3f(-XSize()/2, P->m_FloorElevation, ZSize()/2);
//    switch (P->m_Swap) {
//        case FZone::FSwapDegree::Original:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//        case FZone::FSwapDegree::degree_90:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//        case FZone::FSwapDegree::degree_180:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//        case FZone::FSwapDegree::degree_270:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//    }
//    triMesh.appendVertex(v);
//    v.position = vec3f(XSize()/2, P->m_FloorElevation, ZSize()/2);
//    switch (P->m_Swap) {
//        case FZone::FSwapDegree::Original:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//        case FZone::FSwapDegree::degree_90:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//        case FZone::FSwapDegree::degree_180:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//        case FZone::FSwapDegree::degree_270:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//    }
//    triMesh.appendVertex(v);
//    v.position = vec3f(XSize()/2, P->m_FloorElevation, -ZSize()/2);
//    switch (P->m_Swap) {
//        case FZone::FSwapDegree::Original:
//            v.tCoord = vec2f(1.0f, 0.0f);
//            break;
//        case FZone::FSwapDegree::degree_90:
//            v.tCoord = vec2f(0.0f, 0.0f);
//            break;
//        case FZone::FSwapDegree::degree_180:
//            v.tCoord = vec2f(0.0f, 1.0f);
//            break;
//        case FZone::FSwapDegree::degree_270:
//            v.tCoord = vec2f(1.0f, 1.0f);
//            break;
//    }
//    triMesh.appendVertex(v);

//    //Add triangles now
//    NETriangleIndexed t1(1, 3, 0), t2(1, 2, 3);
//    triMesh.appendTriangle(t1);
//    triMesh.appendTriangle(t2);

//    if( NEBASE.guiMode() )
//        if(triMesh.numTriangles() > 0)
//            triMesh.updateDrawingBuffers(GL_DYNAMIC_DRAW);

//    m_normal = v.normal;
}

void FZone::loadImage()
{
    QString extension = FloorPlan().path().split(".").last();
    if(extension == "tif" || extension == "jpg" || extension == "png")
    {
        P->m_textureImage = QImage(FloorPlan());

        if (P->m_textureImage.width() >= P->m_textureImage.height() && P->m_textureImage.width() > 4096)
                P->m_textureImage = P->m_textureImage.scaledToWidth(4096);
        if (P->m_textureImage.width() < P->m_textureImage.height() && P->m_textureImage.height() > 4096)
                P->m_textureImage = P->m_textureImage.scaledToHeight(4096);

        P->m_textureImage = P->m_textureImage.convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < P->m_textureImage.height(); ++y) {
            for (int x = 0; x < P->m_textureImage.width(); ++x) {
                QColor col = P->m_textureImage.pixelColor(x, y);
                QColor newCol = QColor(col.red(), col.green(), col.blue(), FloorPlanTransparency());
                P->m_textureImage.setPixelColor(x,y, newCol);
            }
        }
        P->m_renderImage = true;
    }
    else if(extension == "pdf")
    {
        void* pdfInstance = pdf_app_init(0, 0);

        int nResult = pdf_app_loadfile(pdfInstance, (wchar_t*) FloorPlan().path().utf16(), "");
        if (nResult == 1) { // Load Success
            int w = 0, h = 0;
            pdf_app_origin_size(pdfInstance, 1, &w, &h);
            // Load Image of First Page.
            int comps = 4;
            unsigned char* imageBuffer = pdf_app_image_with_number_without_zoom(pdfInstance, 1, &w, &h, &comps);
            if (imageBuffer) {
                P->m_textureImage = QImage(imageBuffer, w, h, QImage::Format_RGB32);
                if (P->m_textureImage.width() >= P->m_textureImage.height() && P->m_textureImage.width() > 4096)
                        P->m_textureImage = P->m_textureImage.scaledToWidth(4096);
                if (P->m_textureImage.width() < P->m_textureImage.height() && P->m_textureImage.height() > 4096)
                        P->m_textureImage = P->m_textureImage.scaledToHeight(4096);
                P->m_textureImage = P->m_textureImage.convertToFormat(QImage::Format_ARGB32);
                for (int y = 0; y < P->m_textureImage.height(); ++y) {
                    for (int x = 0; x < P->m_textureImage.width(); ++x) {
                        QColor col = P->m_textureImage.pixelColor(x, y);
                        QColor newCol = QColor(col.red(), col.green(), col.blue(), FloorPlanTransparency());
                        P->m_textureImage.setPixelColor(x,y, newCol);
                    }
                }
            }
            // Finalize
            pdf_app_finalize(pdfInstance);
        }
        P->m_renderImage = true;
    }
    else
        P->m_renderImage = false;

    if( ShowFloorPlan() == false )
        P->m_renderImage = false;

    UpdateUI();

    //P->m_textureImage.save("test.png");
}

bool FZone::renderImage()
{
    return P->m_renderImage;
}

NEIndexedTriMesh&  FZone::mesh()
{
    return m_TriMesh;
}

void FZone::resetProperties()
{
    NEZone::resetProperties();

    ResetConnectorID();
    ResetIssues();
    ResetShowCompBB();
    ResetFloorElevation();
    ResetFloorPlan();
    ResetFloorPlanTransparency();
    ResetXMin();
    ResetXMax();
    ResetXSize();
    ResetZMin();
    ResetZMax();
    ResetZSize();
    ResetSwap();
    ResetRevert();
    ResetCeilingThickness();
    ResetFloorThickness();
    ResetWallThickness();
    ResetCeilingMaterial();
    ResetFloorMaterial();
    ResetWallMaterial();

    P->m_WallThickness = P->m_WallMaterial.thickness();
    P->m_CeilingThickness = P->m_CeilingMaterial.thickness();
    P->m_FloorThickness = P->m_FloorMaterial.thickness();

    computeCompBB();
}

const uint& FZone::ConnectorID() const
{
    return P->m_ConnectorID;
}

uint FZone::SetConnectorID( const uint& pId )
{
    NE_PROP_SET_EVAL(ConnectorID, P, pId);

    return NE::kReturnSuccess;
}

const QString& FZone::Issues() const
{
    return P->m_Issues;
}

uint FZone::SetIssues( const QString& issues)
{
    NE_PROP_SET_EVAL(Issues, P, issues);

    return NE::kReturnSuccess;
}

const bool& FZone::ShowCompBB() const
{
    return P->m_ShowCompBB;
}

uint FZone::SetShowCompBB(const bool& val)
{
    NE_PROP_SET_EVAL(ShowCompBB, P, val);

    computeCompBB();
    return NE::kReturnSuccess;
}

const vec3f& FZone::CompBBSize() const
{
    return P->m_CompBBSize;
}

const vec3f& FZone::CompBBCenter() const
{
    return P->m_CompBBCenter;
}

const bool& FZone::ShowFloorPlan() const
{
    return P->m_ShowFloorPlan;
}

uint FZone::SetShowFloorPlan(const bool& val)
{
    NE_PROP_SET_EVAL(ShowFloorPlan, P, val);
    if( val == true )
    {
        createMesh();
        loadImage();
    }
    else
    {
        std::vector<NETriangleIndexed>& tris = triangleMesh().triangles();
        tris.clear();

        std::vector<NECTNPVertex>& verts = triangleMesh().vertices();
        verts.clear();
        P->m_textureImage = QImage();
        P->m_renderImage = false;
    }
    return NE::kReturnSuccess;
}

uint FZone::SetFloorElevation( const double& val )
{
    NE_PROP_SET_EVAL(FloorElevation, P, val);
    createMesh();
    return NE::kReturnSuccess;
}

uint FZone::SetFloorPlan( const FilePath& val )
{
    NE_PROP_SET_EVAL(FloorPlan, P, val);
    if( ShowFloorPlan() == true )
    {
        createMesh();
        loadImage();
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

uint FZone::SetFloorPlanTransparency( const int& val )
{
    int _val;
    if (val > 255)
        _val = 255;
    else if (val < 0)
        _val = 0;
    else
        _val = val;
    NE_PROP_SET_EVAL(FloorPlanTransparency, P, _val);
    createMesh();
    loadImage();
    return NE::kReturnSuccess;
}

uint FZone::SetXMin( const double& val )
{
    double _val = qBound((double)0, val, XMax());
    if (_val == XMin())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(XMin, P, _val);
    return NE::kReturnSuccess;
}

uint FZone::SetXMax( const double& val )
{
    double _val = qMax(XMin(), val);
    if (_val == XMax())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(XMax, P, _val);
    return NE::kReturnSuccess;
}

uint FZone::SetXSize( const double& val )
{
    double _val = qMax(0.001, val);
    if (_val == XSize())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(XSize, P, _val);
    createMesh();
    return NE::kReturnSuccess;
}

uint FZone::SetZMin( const double& val )
{
    double _val = qBound((double)0, val, ZMax());
    if (_val == ZMin())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZMin, P, _val);
    return NE::kReturnSuccess;
}

uint FZone::SetZMax( const double& val )
{
    double _val = qMax(ZMin(), val);
    if (_val == ZMax())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZMax, P, _val);
    return NE::kReturnSuccess;
}

uint FZone::SetZSize( const double& val )
{
    double _val = qMax(0.001, val);
    if (_val == ZSize())
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZSize, P, _val);
    createMesh();
    return NE::kReturnSuccess;
}

uint FZone::SetSwap( const FZone::FSwapDegree& val )
{
    NE_PROP_SET_EVAL(Swap, P, val);
    createMesh();
    return NE::kReturnSuccess;
}

uint FZone::SetRevert( const bool& val )
{
    NE_PROP_SET_EVAL(Revert, P, val);
    return NE::kReturnSuccess;
}

uint FZone::SetCeilingThickness (const double& val )
{
    NE_PROP_SET_EVAL(CeilingThickness, P, val);

    P->computeCompBBOuter();

    return NE::kReturnSuccess;
}

uint FZone::SetFloorThickness( const double& val )
{
    NE_PROP_SET_EVAL(FloorThickness, P, val);

    P->computeCompBBOuter();

    return NE::kReturnSuccess;
}

uint FZone::SetWallThickness( const double& val )
{
    NE_PROP_SET_EVAL(WallThickness, P, val);

    P->computeCompBBOuter();

    return NE::kReturnSuccess;
}


uint FZone::SetFloorLeakRatio( const double& val )
{
    NE_PROP_SET_EVAL(FloorLeakRatio, P, val);

    P->computeCompBBOuter();

    return NE::kReturnSuccess;
}

uint FZone::SetWallLeakRatio( const double& val )
{
    NE_PROP_SET_EVAL(WallLeakRatio, P, val);


    return NE::kReturnSuccess;
}

const double& FZone::FloorElevation() const
{
    return P->m_FloorElevation;
}

const FilePath& FZone::FloorPlan() const
{
    return P->m_FloorPlan;
}

const int& FZone::FloorPlanTransparency() const
{
    return P->m_FloorPlanTransparency;
}

const double& FZone::XMin() const
{
    return P->m_XMin;
}

const double& FZone::XMax() const
{
    return P->m_XMax;
}

const double& FZone::XSize() const
{
    return P->m_XSize;
}

const double& FZone::ZMin() const
{
    return P->m_ZMin;
}

const double& FZone::ZMax() const
{
    return P->m_ZMax;
}

const double& FZone::ZSize() const
{
    return P->m_ZSize;
}

const FZone::FSwapDegree& FZone::Swap() const
{
    return P->m_Swap;
}

const bool& FZone::Revert() const
{
    return P->m_Revert;
}

const double& FZone::CeilingThickness() const
{
    return P->m_CeilingThickness;
}

const double& FZone::FloorThickness() const
{
    return P->m_FloorThickness;
}

const double& FZone::WallThickness() const
{
    return P->m_WallThickness;
}

const double& FZone::FloorLeakRatio() const
{
    return P->m_FloorLeakRatio;
}

const double& FZone::WallLeakRatio() const
{
    return P->m_WallLeakRatio;
}

const FZone::NELineType& FZone::LineType() const
{
    return P->m_LineType;
}

uint FZone::SetLineType(const FZone::NELineType& val){
    //NE_PROP_SET_EVAL(LineType, P, val);
    P->m_LineType = val;
    return NE::kReturnSuccess;
}

uint FZone::SetWallMaterial(const NESurfaceMaterial& val)
{
    NE_PROP_SET_EVAL(WallMaterial, P, val);
    SetWallThickness(P->m_WallMaterial.thickness());
    if (val.name() == FloorMaterial().name())
    {
        NE_PROP_SET_EVAL(FloorMaterial, P, val);
        SetFloorThickness(P->m_FloorMaterial.thickness());
    }
    if (val.name() == CeilingMaterial().name())
    {
        NE_PROP_SET_EVAL(CeilingMaterial, P, val);
        SetCeilingThickness(P->m_CeilingMaterial.thickness());
    }
    return NE::kReturnSuccess;
}

uint FZone::SetFloorMaterial( const NESurfaceMaterial& val)
{
    NE_PROP_SET_EVAL(FloorMaterial, P, val);;
    SetFloorThickness(P->m_FloorMaterial.thickness());
    if (val.name() == CeilingMaterial().name())
    {
        NE_PROP_SET_EVAL(CeilingMaterial, P, val);
        SetCeilingThickness(P->m_CeilingMaterial.thickness());
    }
    if (val.name() == WallMaterial().name())
    {
        NE_PROP_SET_EVAL(WallMaterial, P, val);
        SetWallThickness(P->m_WallMaterial.thickness());
    }
    return NE::kReturnSuccess;
}

uint FZone::SetCeilingMaterial(const NESurfaceMaterial& val)
{
    NE_PROP_SET_EVAL(CeilingMaterial, P, val);
    SetCeilingThickness(P->m_CeilingMaterial.thickness());
    if (val.name() == FloorMaterial().name())
    {
        NE_PROP_SET_EVAL(FloorMaterial, P, val);
        SetFloorThickness(P->m_FloorMaterial.thickness());
    }
    if (val.name() == WallMaterial().name())
    {
        NE_PROP_SET_EVAL(WallMaterial, P, val);
        SetWallThickness(P->m_WallMaterial.thickness());
    }
    return NE::kReturnSuccess;
}

const NESurfaceMaterial& FZone::CeilingMaterial() const
{
    return P->m_CeilingMaterial;
}

const NESurfaceMaterial& FZone::FloorMaterial() const
{
    return P->m_FloorMaterial;
}

const NESurfaceMaterial& FZone::WallMaterial() const
{
    return P->m_WallMaterial;
}

QString FZone::zoneName()
{
    return P->m_ZoneName;
}

uint FZone::setZoneName(const QString zoneName)
{
    P->m_ZoneName = zoneName;

    return NE::kReturnSuccess;
}

QList<QString> &FZone::otherItems()
{
    return P->m_FranxItems;
}

QString FZone::model3DInfo()
{
    return P->m_Model3DInfo;
}

QImage FZone::textureImage()
{
    return P->m_textureImage;
}

void FZone::setModel3DInfo(QString model3dinfo)
{
    P->m_Model3DInfo = model3dinfo;
}

QString FZone::nodeDescription()
{
    return Desc();
}

void FZone::propertiesFromJSON(QString jsonStr)
{
    return;
}

QString FZone::propertiesToJSON()
{
    QJsonObject   jsonObject ;

    int connectorID = ConnectorID();
    int id = connectorID;

    jsonObject["name"] = objectName();
    jsonObject["desc"] = Desc();
    jsonObject["type"] = Type();
    jsonObject["connectorID"] = connectorID;
    //jsonObject["ID"]    = id;
    jsonObject["issues"] = Issues();

    //jsonObject["model3DInfo"] = getJSONFromProperties( zone );

    QJsonDocument jsonDoc( jsonObject );

    QString jsonStr( jsonDoc.toJson() );

    return jsonStr;
}

void FZone::updateThicknesses()
{
    P->m_WallThickness = P->m_WallMaterial.thickness();
    P->m_CeilingThickness = P->m_CeilingMaterial.thickness();
    P->m_FloorThickness = P->m_FloorMaterial.thickness();

    P->computeCompBBOuter();
}

const AABB& FZone::compAABB() const
{
    return P->m_CompAABB;
}

const AABB& FZone::outerCompAABB() const
{
    return P->m_OuterCompAABB;
}

void FZone::computeCompBB()
{
    QList<FZoneCompartment*> boundaryList = findChildren<FZoneCompartment*>();

    vec3f compMinPos, compMaxPos;
    if (boundaryList.size() == 0)
    {
        compMinPos = vec3f(0.0, 0.0, 0.0);
        compMaxPos = vec3f(0.0, 0.0, 0.0);
    }
    else
    {
        // Compute compartment bounding box based on underlying boundaries
        // Center is computed by weighting based on sizes of boundaries
        // Size is computed such that the volume equal the total volume of boundaries
        // while keeping the relative proportions of the three dimensions
        vec3f minPos = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
        vec3f maxPos = vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        vec3f sumBundarySize = vec3f(0.0, 0.0, 0.0);
        double compVolume = 0.0;
        vec3f compCenter = vec3f(0.0, 0.0, 0.0);

        foreach(FZoneCompartment* boundary, boundaryList)
        {
            vec3f boundaryCenter = boundary->BoundingBoxPos();

            vec3f boundaryMinPos = boundaryCenter-0.5*boundary->BoundingBoxSize();
            vec3f boundaryMaxPos = boundaryCenter+0.5*boundary->BoundingBoxSize();

            if (boundaryMinPos.x() < minPos.x())
                minPos.x() = boundaryMinPos.x();
            if (boundaryMinPos.y() < minPos.y())
                minPos.y() = boundaryMinPos.y();
            if (boundaryMinPos.z() < minPos.z())
                minPos.z() = boundaryMinPos.z();
            if (boundaryMaxPos.x() > maxPos.x())
                maxPos.x() = boundaryMaxPos.x();
            if (boundaryMaxPos.y() > maxPos.y())
                maxPos.y() = boundaryMaxPos.y();
            if (boundaryMaxPos.z() > maxPos.z())
                maxPos.z() = boundaryMaxPos.z();

            sumBundarySize += boundary->BoundingBoxSize();
            double boundaryVolume = boundary->BoundingBoxSize().x()*boundary->BoundingBoxSize().y()*boundary->BoundingBoxSize().z();
            compVolume += boundaryVolume;

            compCenter.x() += boundary->BoundingBoxSize().x()*boundaryCenter.x();
            compCenter.z() += boundary->BoundingBoxSize().z()*boundaryCenter.z();
        }

        compCenter.x() /= sumBundarySize.x();
        compCenter.z() /= sumBundarySize.z();

        vec3f size = maxPos-minPos;
        vec3f compSize;
        compSize.x() = std::cbrt((size.x()*size.x()/(size.y()*size.z()))*compVolume);
        compSize.y() = (size.y()/size.x())*compSize.x();
        compSize.z() = (size.z()/size.x())*compSize.x();

        compCenter.y() = minPos.y()+0.5*compSize.y();

        compMinPos = compCenter-0.5*compSize;
        compMaxPos = compCenter+0.5*compSize;
    }

    P->m_CompAABB = AABB(compMinPos, compMaxPos);

    P->computeCompBBSize();
    P->computeCompBBCenter();
    P->computeCompBBOuter();

    P->updateChildGhostPositions();
}

void FZone::childAdded(QObject* child)
{
    NEZone::childAdded(child);

    if (qobject_cast<FZoneCompartment*>(child))
        computeCompBB();
}

void FZone::childRemoved(QObject *child)
{
    NEZone::childRemoved(child);

    if (qobject_cast<FZoneCompartment*>(child))
        computeCompBB();
}

double FZone::layerHeight(double time) const
{
    return P->interpValue(P->m_TimeLayerHeightPairs, time);
}

double FZone::lowerLayerTemp(double time) const
{
    return P->interpValue(P->m_TimeLowerLayerTempPairs, time);
}

double FZone::higherLayerTemp(double time) const
{
    return P->interpValue(P->m_TimeHigherLayerTempPairs, time);
}

double FZone::ceilingSurfTemp(double time) const
{
    return P->interpValue(P->m_TimeCeilingSurfTempPairs, time);
}

double FZone::floorSurfTemp(double time) const
{
    return P->interpValue(P->m_TimeFloorSurfTempPairs, time);
}

double FZone::lowerWallSurfTemp(double time) const
{
    return P->interpValue(P->m_TimeLowerWallSurfTempPairs, time);
}

double FZone::higherWallSurfTemp(double time) const
{
    return P->interpValue(P->m_TimeHigherWallSurfTempPairs, time);
}

double FZone::minLayerTemp() const
{
    return P->m_MinLayerTemp;
}

double FZone::maxLayerTemp() const
{
    return P->m_MaxLayerTemp;
}

double FZone::minSurfTemp() const
{
    return P->m_MinSurfTemp;
}

double FZone::maxSurfTemp() const
{
    return P->m_MaxSurfTemp;
}

void FZone::addLayerHeight(double time, double height)
{
    P->m_TimeLayerHeightPairs.push_back(std::pair<double, double>(time, height));
}

void FZone::addLowerLayerTemp(double time, double temperature)
{
    P->m_TimeLowerLayerTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinLayerTemp == -DBL_MAX)
        P->m_MinLayerTemp = temperature;
    else
        P->m_MinLayerTemp = std::min(P->m_MinLayerTemp, temperature);

    if (P->m_MaxLayerTemp == DBL_MAX)
        P->m_MaxLayerTemp = temperature;
    else
        P->m_MaxLayerTemp = std::max(P->m_MaxLayerTemp, temperature);
}

void FZone::addHigherLayerTemp(double time, double temperature)
{
    P->m_TimeHigherLayerTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinLayerTemp == -DBL_MAX)
        P->m_MinLayerTemp = temperature;
    else
        P->m_MinLayerTemp = std::min(P->m_MinLayerTemp, temperature);

    if (P->m_MaxLayerTemp == DBL_MAX)
        P->m_MaxLayerTemp = temperature;
    else
        P->m_MaxLayerTemp = std::max(P->m_MaxLayerTemp, temperature);
}

void FZone::addCeilingSurfTemp(double time, double temperature)
{
    P->m_TimeCeilingSurfTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinSurfTemp == -DBL_MAX)
        P->m_MinSurfTemp = temperature;
    else
        P->m_MinSurfTemp = std::min(P->m_MinSurfTemp, temperature);

    if (P->m_MaxSurfTemp == DBL_MAX)
        P->m_MaxSurfTemp = temperature;
    else
        P->m_MaxSurfTemp = std::max(P->m_MaxSurfTemp, temperature);
}

void FZone::addFloorSurfTemp(double time, double temperature)
{
    P->m_TimeFloorSurfTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinSurfTemp == -DBL_MAX)
        P->m_MinSurfTemp = temperature;
    else
        P->m_MinSurfTemp = std::min(P->m_MinSurfTemp, temperature);

    if (P->m_MaxSurfTemp == DBL_MAX)
        P->m_MaxSurfTemp = temperature;
    else
        P->m_MaxSurfTemp = std::max(P->m_MaxSurfTemp, temperature);
}

void FZone::addLowerWallSurfTemp(double time, double temperature)
{
    P->m_TimeLowerWallSurfTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinSurfTemp == -DBL_MAX)
        P->m_MinSurfTemp = temperature;
    else
        P->m_MinSurfTemp = std::min(P->m_MinSurfTemp, temperature);

    if (P->m_MaxSurfTemp == DBL_MAX)
        P->m_MaxSurfTemp = temperature;
    else
        P->m_MaxSurfTemp = std::max(P->m_MaxSurfTemp, temperature);
}

void FZone::addHigherWallSurfTemp(double time, double temperature)
{
    P->m_TimeHigherWallSurfTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinSurfTemp == -DBL_MAX)
        P->m_MinSurfTemp = temperature;
    else
        P->m_MinSurfTemp = std::min(P->m_MinSurfTemp, temperature);

    if (P->m_MaxSurfTemp == DBL_MAX)
        P->m_MaxSurfTemp = temperature;
    else
        P->m_MaxSurfTemp = std::max(P->m_MaxSurfTemp, temperature);
}

void FZone::clearSimData()
{
    P->m_TimeLayerHeightPairs.clear();
    P->m_TimeLowerLayerTempPairs.clear();
    P->m_TimeHigherLayerTempPairs.clear();
    P->m_TimeCeilingSurfTempPairs.clear();
    P->m_TimeFloorSurfTempPairs.clear();
    P->m_TimeLowerWallSurfTempPairs.clear();
    P->m_TimeHigherWallSurfTempPairs.clear();

    P->m_MinLayerTemp = -DBL_MAX;
    P->m_MaxLayerTemp = DBL_MAX;
    P->m_MinSurfTemp = -DBL_MAX;
    P->m_MaxSurfTemp = DBL_MAX;
}

void FZonePrivate::computeCompBBSize()
{
    m_CompBBSize = m_CompAABB.maxPos()-m_CompAABB.minPos();
}

void FZonePrivate::computeCompBBCenter()
{
    m_CompBBCenter = 0.5*(m_CompAABB.minPos()+m_CompAABB.maxPos());
}

void FZonePrivate::computeCompBBOuter()
{
    vec3f outerCompMinPos = m_CompAABB.minPos()-vec3f(m_WallThickness, m_FloorThickness, m_WallThickness);
    vec3f outerCompMaxPos = m_CompAABB.maxPos()+vec3f(m_WallThickness, m_CeilingThickness, m_WallThickness);
    m_OuterCompAABB = AABB(outerCompMinPos, outerCompMaxPos);
}

void FZonePrivate::updateChildGhostPositions()
{
    QObjectList childList = S->children();
    while (childList.size() > 0)
    {
        QObjectList nextChildList;
        for (QObject* c : childList)
        {
            if (FZoneItem* item = qobject_cast<FZoneItem*>(c))
                item->computeGhostPosition(S);
            if (FVent* vent = qobject_cast<FVent*>(c))
                vent->computeGhostPosition(S);

            nextChildList.append(c->children());
        }

        childList = nextChildList;
    }
}

double FZonePrivate::interpValue(const std::vector<std::pair<double, double>>& timeValuePairs, double time)
{
    if (timeValuePairs.size() == 0)
        return 0.0;
    else if (timeValuePairs.size() == 1)
        return timeValuePairs.back().second;

    std::pair<double, double> prevPair(-DBL_MAX, 0.0);
    std::pair<double, double> nextPair(DBL_MAX, 0.0);
    for (const std::pair<double, double>& pair : timeValuePairs)
    {
        if (pair.first == time)
            return pair.second;

        if (pair.first < time && pair.first > prevPair.first)
            prevPair = pair;
        else if (pair.first > time && pair.first < nextPair.first)
            nextPair = pair;
    }

    if (prevPair.first == -DBL_MAX)
        return nextPair.second;
    else if (nextPair.first == DBL_MAX)
        return prevPair.second;

    return ((time-prevPair.first)*prevPair.second+(nextPair.first-time)*nextPair.second)/(nextPair.first-prevPair.first);
}

QList<QString> &FZone::scenarios()
{
    return P->m_Scenarios;
}

FZoneFactory::FZoneFactory(NEManager* m) : NENodeFactory(m)
{
}

FZoneFactory::~FZoneFactory()
{
}

QString FZoneFactory::nodeName()
{
    return "Fire Zone";
}

QStringList FZoneFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("FZone");

    return aNames;
}

uint FZoneFactory::nodeType()
{
    return NE::kObjectGroup;
}

QString FZoneFactory::nodeInherits()
{
    return "Groups";
}

QString FZoneFactory::objectName()
{
    return "FZone";
}

QString FZoneFactory::nodeVersion()
{
    return "0.1";
}

QString FZoneFactory::nodeVendor()
{
    return "FRI3D";
}

QString FZoneFactory::nodeHelp()
{
    return "Describing a Fire Zone";
}

NEObject* FZoneFactory::createInstance()
{
    return new FZone(this);
}
