#include "FVent.h"
#include "FZone.h"

class FVentPrivate : public QObject
{
public:
    FVentPrivate(FVent* s) : S(s)
    {
    }

    /* Properties */
    QString m_Desc;
    QString m_Issues;
    FVent::VentGeometry m_VentGeometryType;
    NEDynamicCombo m_FirstCompartment;
    NEDynamicCombo m_SecondCompartment;
    NEVentCriteria m_Criterion;
    vec3f m_GhostPosition;
    bool m_ShowGhostPosition;

    /* Other members */
    QString m_Name;
    QString m_Model3DInfo;
    int m_FirstCompartmentId;
    int m_SecondCompartmentId;
    bool m_isSnapped;
    double m_snappedXmin;
    double m_snappedXmax;
    double m_snappedYmin;
    double m_snappedYmax;
    double m_snappedZmin;
    double m_snappedZmax;
    vec3f m_snappedPos;

private:
    FVent* S;
};

FVentFactory::FVentFactory(NEManager* m) : NENodeFactory(m)
{
}

FVentFactory::~FVentFactory()
{
}

QString FVentFactory::nodeName()
{
    return "Fire Vent";
}

uint FVentFactory::nodeType()
{
    return NE::kObjectPhysical;
}

QString FVentFactory::nodeInherits()
{
    return "Physical";
}

QString FVentFactory::objectName()
{
    return "FVent";
}

QString FVentFactory::nodeVersion()
{
    return "0.1";
}

QString FVentFactory::nodeVendor()
{
    return "F Dynamics";
}

QString FVentFactory::nodeHelp()
{
    return "Vent";
}

NEObject* FVentFactory::createInstance()
{
    return new FVent(this);
}

NE_PROP_DEF(QString, FVent, Desc, QString(""))
NE_PROP_DEF(QString, FVent, Issues, QString(""))
NE_PROP_DEF(FVent::VentGeometry, FVent, VentGeometryType, FVent::Rectangular)
NE_PROP_DEF(NEDynamicCombo, FVent, FirstCompartment, NEDynamicCombo("Outside"))
NE_PROP_DEF(NEDynamicCombo, FVent, SecondCompartment, NEDynamicCombo("Outside"))
NE_PROP_DEF(NEVentCriteria, FVent, Criterion, NEVentCriteria(""))
NE_PROP_DEF(bool, FVent, ShowGhostPosition, false)
NE_PROP_DEF(double, FVent, VentThickness, 0.1)


static void initProperties()
{
    NE_INIT_PROP(FVent, Desc);
    NE_INIT_PROP(FVent, Issues);
    NE_INIT_PROP(FVent, FirstCompartment);
    NE_INIT_PROP(FVent, SecondCompartment);
    NE_INIT_PROP(FVent, Criterion);
    NE_INIT_PROP(FVent, ShowGhostPosition);
    NE_INIT_PROP(FVent, VentGeometryType);
    NE_INIT_PROP(FVent, VentThickness);

}

FVent::FVent(NENodeFactory* factory) : NEZoneItem(factory)
{
    P = new FVentPrivate(this);

    CALL_ONCE(initProperties);

    P->m_Desc = NE_DEFAULT(Desc);
    P->m_Issues = NE_DEFAULT(Issues);
    P->m_VentGeometryType = NE_DEFAULT(VentGeometryType);
    P->m_FirstCompartment = NE_DEFAULT(FirstCompartment);
    P->m_SecondCompartment = NE_DEFAULT(SecondCompartment);
    P->m_Criterion = NE_DEFAULT(Criterion);
    P->m_ShowGhostPosition = NE_DEFAULT(ShowGhostPosition);

    P->m_FirstCompartmentId = 0;
    P->m_SecondCompartmentId = 0;
    P->m_GhostPosition = vec3f(0.0, 0.0, 0.0);
    P->m_isSnapped = false;
    P->m_snappedXmin = 0;
    P->m_snappedXmax = 0;
    P->m_snappedYmin = 0;
    P->m_snappedYmax = 0;
    P->m_snappedZmin = 0;
    P->m_snappedZmax = 0;
    SetColor( QColor( 255, 255, 116 ) );
    SetScale(vec3f(1.0, 1.0, 1.0));
    SetThickness(0.1);
    SetSpatialMeshType(NESpatialMeshType::None);

    addNodeTypeToNodeGraphSceneType("Physical", NENode::SceneType::Simulation);

    connect(this, SIGNAL(PositionXChanged(float)), this, SLOT(setSnappedAsFalse()));
    connect(this, SIGNAL(PositionYChanged(float)), this, SLOT(setSnappedAsFalse()));
    connect(this, SIGNAL(PositionZChanged(float)), this, SLOT(setSnappedAsFalse()));
    connect(this, SIGNAL(OrientationChanged(eulerf)), this, SLOT(setSnappedAsFalse()));
    connect(this, SIGNAL(UnitScaleChanged(double)), this, SLOT(setSnappedAsFalse()));
    connect(this, SIGNAL(ThicknessChanged(double)), this, SLOT(adjustSnappedVentPosition()));

    hideProperty( "DirVec", false );
    setOutputs(outputs() | NE::kIOTriangleMesh);
}

FVent::~FVent()
{
    delete P;
}

uint FVent::init(uint initMode)
{
    uint ret = NEZoneItem::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    if (VentGeometryType() == FVent::Rectangular)
        SetSpatialMeshType(NESpatialMeshType::Cuboid);
    else if (VentGeometryType() == FVent::Circular)
        SetSpatialMeshType(NESpatialMeshType::Cylinder);

    SetOccludeShadows( false );
    SetReceiveShadows( false );

    return NE::kReturnSuccess;
}

void FVent::resetProperties()
{
    NEZoneItem::resetProperties();

    ResetDesc();
    ResetIssues();
    ResetVentGeometryType();
    ResetFirstCompartment();
    ResetSecondCompartment();
    ResetShowGhostPosition();

    computeGhostPosition();
}

matrix44f FVent::ghostTransform()
{
    matrix44f worldTransform, worldRotTransTransform;
    worldTransform.setToIdentity();
    worldRotTransTransform.setToIdentity();

    matrix44f pivotMatrix, invPivotMatrix;
    pivotMatrix.setToIdentity();
    pivotMatrix.translate( Pivot() );
    invPivotMatrix.setToIdentity();
    invPivotMatrix.translate( -Pivot() );

    //
    // Translate in world space
    //
    matrix44f translateMatrix;
    translateMatrix.setToIdentity();
    translateMatrix.translate( P->m_GhostPosition );

    //
    // Rotations in world space
    //
    quatf quatX, quatY, quatZ;
    quatX = quatX.fromAxisAndAngle( 1,0,0, Orientation().phi() );
    quatY = quatY.fromAxisAndAngle( 0,1,0, Orientation().theta() );
    quatZ = quatZ.fromAxisAndAngle( 0,0,1, Orientation().psi() );
    quatf orientationQuat = quatZ*quatY*quatX;

    matrix44f rotationMatrix;
    rotationMatrix.setToIdentity();
    rotationMatrix.rotate( orientationQuat );

    //
    // Scale in Pivot Space - so transform into pivot space
    //
    matrix44f scaleMatrix;
    scaleMatrix.setToIdentity();
    scaleMatrix.scale( Scale() );

    pivotMatrix.setToIdentity();
    pivotMatrix.translate( Pivot() );


    //
    // Combine translate/rotate/scale in pivot space
    // Order: First Rotate, then scale, then translate
    //
    matrix44f wRotTransTransform = translateMatrix * rotationMatrix;
    matrix44f wTransform = wRotTransTransform * scaleMatrix;


    matrix44f localRotTransTransform = pivotMatrix * wRotTransTransform * pivotMatrix.inverted();
    matrix44f localTransform = pivotMatrix * wTransform * pivotMatrix.inverted();

    //
    // updateTransformWithParent();
    //
    if(NESpatialGroup* node = qobject_cast<NESpatialGroup*>(parent()))
    {
        worldTransform = node->transform() * localTransform;
        worldRotTransTransform = node->rotTransTransform() * localRotTransTransform;
    }
    else
    {
        worldTransform = localTransform;
        worldRotTransTransform = localRotTransTransform;
    }

    return worldTransform;

}

void FVent::copyValues(const FVent *vent)
{
    SetID(vent->ID());
    SetType(vent->Type());
    setOriginalName(vent->originalName());
    setItemType(const_cast<FVent*>(vent)->itemType());

    P->m_Desc = vent->Desc();
    P->m_Issues = vent->Issues();
    P->m_VentGeometryType = vent->VentGeometryType();
    P->m_FirstCompartment = vent->FirstCompartment();
    P->m_SecondCompartment = vent->SecondCompartment();
    P->m_Criterion = vent->Criterion();
    P->m_GhostPosition = vent->GhostPosition();
    P->m_ShowGhostPosition = vent->ShowGhostPosition();
    SetVentThickness(vent->VentThickness());

    P->m_Name = vent->name();
    P->m_Model3DInfo = vent->model3DInfo();
    P->m_FirstCompartmentId = vent->firstCompartmentId();
    P->m_SecondCompartmentId = vent->secondCompartmentId();
    P->m_isSnapped = const_cast<FVent*>(vent)->isSnapped();
    setSnappedPosition(const_cast<FVent*>(vent)->getSnappedPosition());
    setSnappedFaceSize(const_cast<FVent*>(vent)->getSnappedFaceSize());
}

const QString& FVent::Desc() const
{
    return P->m_Desc;
}

uint FVent::SetDesc(const QString& val)
{
    NE_PROP_SET_EVAL(Desc, P, val);

    return NE::kReturnSuccess;
}

const QString& FVent::Issues() const
{
    return P->m_Issues;
}

uint FVent::SetIssues(const QString& val)
{
    NE_PROP_SET_EVAL(Issues, P, val);

    return NE::kReturnSuccess;
}

const FVent::VentGeometry& FVent::VentGeometryType() const
{
    return P->m_VentGeometryType;
}

uint FVent::SetVentGeometryType(const FVent::VentGeometry& val)
{
    NE_PROP_SET_EVAL(VentGeometryType, P, val);

    if (VentGeometryType() == FVent::Rectangular)
        SetSpatialMeshType(NESpatialMeshType::Cuboid);
    else if (VentGeometryType() == FVent::Circular)
        SetSpatialMeshType(NESpatialMeshType::Cylinder);

    return NE::kReturnSuccess;
}

const NEDynamicCombo& FVent::FirstCompartment() const
{
    return P->m_FirstCompartment;
}

uint FVent::SetFirstCompartment(const NEDynamicCombo& val)
{
    NE_PROP_SET_EVAL(FirstCompartment, P, val);

    return NE::kReturnSuccess;
}

const NEDynamicCombo& FVent::SecondCompartment() const
{
    return P->m_SecondCompartment;
}

uint FVent::SetSecondCompartment(const NEDynamicCombo& val)
{
    NE_PROP_SET_EVAL(SecondCompartment, P, val);

    return NE::kReturnSuccess;
}

const NEVentCriteria& FVent::Criterion() const
{
    return P->m_Criterion;
}

uint FVent::SetCriterion(const NEVentCriteria &val)
{
    NE_PROP_SET_EVAL(Criterion, P, val);

    return NE::kReturnSuccess;
}

const vec3f& FVent::GhostPosition() const
{
    return P->m_GhostPosition;
}

const bool& FVent::ShowGhostPosition() const
{
    return P->m_ShowGhostPosition;
}

uint FVent::SetShowGhostPosition(const bool& val)
{
    NE_PROP_SET_EVAL(ShowGhostPosition, P, val);

    return NE::kReturnSuccess;
}

const double& FVent::VentThickness() const
{
    return Thickness();
}

uint FVent::SetVentThickness(const double& val)
{
    SetThickness( val );

    return NE::kReturnSuccess;
}

const QString& FVent::name() const
{
    return P->m_Name;
}

void FVent::setName(const QString& name)
{
    P->m_Name = name;
}

const QString& FVent::model3DInfo() const
{
    return P->m_Model3DInfo;
}

void FVent::setModel3DInfo(const QString& model3DInfo)
{
    P->m_Model3DInfo = model3DInfo;
}

int FVent::firstCompartmentId() const
{
    return P->m_FirstCompartmentId;
}

void FVent::setFirstCompartmentId(int id)
{
    P->m_FirstCompartmentId = id;
}

int FVent::secondCompartmentId() const
{
    return P->m_SecondCompartmentId;
}

void FVent::setSecondCompartmentId(int id)
{
    P->m_SecondCompartmentId = id;
}

void FVent::addCompartment(const QString& compName)
{
    P->m_FirstCompartment.add(compName);
    P->m_SecondCompartment.add(compName);
}

void FVent::removeCompartment(const QString& compName)
{
    P->m_FirstCompartment.remove(compName);
    P->m_SecondCompartment.remove(compName);
}

void FVent::resetCompartments()
{
    ResetFirstCompartment();
    ResetSecondCompartment();
}

void FVent::setFirstCompartmentFromName(const QString& compName, const QString& activeCompName)
{
    P->m_FirstCompartment.setCurIndexFromName(compName);
    if (P->m_FirstCompartment.curName() != activeCompName && P->m_SecondCompartment.curName() != activeCompName)
        P->m_SecondCompartment.setCurIndexFromName(activeCompName);
}

void FVent::setSecondCompartmentFromName(const QString& compName, const QString& activeCompName)
{
    P->m_SecondCompartment.setCurIndexFromName(compName);
    if (P->m_FirstCompartment.curName() != activeCompName && P->m_SecondCompartment.curName() != activeCompName)
        P->m_FirstCompartment.setCurIndexFromName(activeCompName);
}

QString FVent::categoryType()
{
    return "Simulation Items/Vents";
}

void FVent::computeGhostPosition(const FZone* comp)
{
    if (comp->compAABB().isInside(Position()))
        P->m_GhostPosition = Position();
    else
    {
        vec3f rayStart = Position();
        rayStart.y() = std::min(std::max(Position().y(), comp->compAABB().minPos().y()), comp->compAABB().maxPos().y());
        vec3f rayEnd = vec3f(comp->CompBBCenter().x(), rayStart.y(), comp->CompBBCenter().z());
        vec3f rayDirection = (rayEnd-rayStart).normalized();

        std::vector<std::pair<vec3f, vec3f>> planePointNormalPairs;
        planePointNormalPairs.push_back(std::pair<vec3f, vec3f>(comp->compAABB().minPos(), vec3f(0.0, -1.0, 0.0)));
        planePointNormalPairs.push_back(std::pair<vec3f, vec3f>(comp->compAABB().minPos(), vec3f(0.0, 0.0, -1.0)));
        planePointNormalPairs.push_back(std::pair<vec3f, vec3f>(comp->compAABB().minPos(), vec3f(-1.0, 0.0, 0.0)));
        planePointNormalPairs.push_back(std::pair<vec3f, vec3f>(comp->compAABB().maxPos(), vec3f(0.0, 1.0, 0.0)));
        planePointNormalPairs.push_back(std::pair<vec3f, vec3f>(comp->compAABB().maxPos(), vec3f(0.0, 0.0, 1.0)));
        planePointNormalPairs.push_back(std::pair<vec3f, vec3f>(comp->compAABB().maxPos(), vec3f(1.0, 0.0, 0.0)));

        std::pair<vec3f, double> interPointSquaredDistPair;
        interPointSquaredDistPair.second = DBL_MAX;
        for (std::pair<vec3f, vec3f>& planePointNormalPair : planePointNormalPairs)
        {
            vec3f interPoint = linePlaneInterPoint(rayStart, rayDirection, planePointNormalPair.first, planePointNormalPair.second);
            if (!std::isnan(interPoint.x()) && !std::isnan(interPoint.y()) && !std::isnan(interPoint.z()) && comp->compAABB().isInside(interPoint, FEps))
            {
                double squaredLength = (interPoint-rayStart).lengthSquared();
                if (squaredLength < interPointSquaredDistPair.second)
                    interPointSquaredDistPair = std::pair<vec3f, double>(interPoint, squaredLength);
            }
        }

        if (interPointSquaredDistPair.second == DBL_MAX)
            P->m_GhostPosition = Position();

        P->m_GhostPosition = interPointSquaredDistPair.first;
        if (!comp->compAABB().isInside(P->m_GhostPosition))
            P->m_GhostPosition += FEps*rayDirection;
    }
}

void FVent::computeGhostPosition()
{
    QObject* parent = this->parent();
    while (parent != nullptr && qobject_cast<FZone*>(parent) == nullptr)
        parent = parent->parent();

    if (FZone* zone = qobject_cast<FZone*>(parent))
        computeGhostPosition(zone);
}

void FVent::resetGhostPosition()
{
    P->m_GhostPosition = Position();
}

void FVent::setSnapped(bool m_snapped)
{
    P->m_isSnapped = m_snapped;
}

bool FVent::isSnapped()
{
    return P->m_isSnapped;
}

void FVent::setSnappedAsFalse()
{
    P->m_isSnapped = false;
}

void FVent::adjustSnappedVentPosition()
{
    double thick1 = this->Thickness() * this->UnitScale();
    double thick2 = this->oldThickness() * this->UnitScale();
    if(this->isSnapped() && abs(thick1 - thick2) > 1e-8)
    {
        vec3f move = getRotateTransform() * vec3f(0,(thick1 - thick2) / 2,0);
        this->adustFaceXMax(move.x());
        this->adustFaceXMin(move.x());
        this->adustFaceYMax(move.y());
        this->adustFaceYMin(move.y());
        this->adustFaceZMax(move.z());
        this->adustFaceZMin(move.z());
//        vec3f pos = (getRotateTransform().inverted() * getSnappedPosition()) + move;
//        pos = getRotateTransform() * pos;
        vec3f pos = Position() + move;
        this->SetPosition(pos);
        this->setOldThickness(this->Thickness() * this->UnitScale());
    }

}

double* FVent::getSnappedFaceSize()
{
    double* snappedSize = new double[6];
    snappedSize[0] = P->m_snappedXmin;
    snappedSize[1] = P->m_snappedXmax;
    snappedSize[2] = P->m_snappedYmin;
    snappedSize[3] = P->m_snappedYmax;
    snappedSize[4] = P->m_snappedZmin;
    snappedSize[5] = P->m_snappedZmax;

    return snappedSize;
}

void FVent::setSnappedFaceSize(double* snappedSize)
{
    P->m_snappedXmin = snappedSize[0];
    P->m_snappedXmax = snappedSize[1];
    P->m_snappedYmin = snappedSize[2];
    P->m_snappedYmax = snappedSize[3];
    P->m_snappedZmin = snappedSize[4];
    P->m_snappedZmax = snappedSize[5];
}

void FVent::adustFaceXMax(double m)
{
    P->m_snappedXmax += m;
}

void FVent::adustFaceYMax(double m)
{
    P->m_snappedYmax += m;
}

void FVent::adustFaceZMax(double m)
{
    P->m_snappedZmax += m;
}

void FVent::adustFaceXMin(double m)
{
    P->m_snappedXmin -= m;
}

void FVent::adustFaceYMin(double m)
{
    P->m_snappedYmin -= m;
}

void FVent::adustFaceZMin(double m)
{
    P->m_snappedZmin -= m;
}

void FVent::setSnappedPosition(vec3f pos)
{
    P->m_snappedPos = pos;
}

vec3f FVent::getSnappedPosition()
{
    return P->m_snappedPos;
}

uint FVent::SetPosition(const vec3f& val)
{
    uint ret = NEZoneItem::SetPosition(val);
    if (ret != NE::kReturnSuccess)
        return ret;

    computeGhostPosition();

    return NE::kReturnSuccess;
}
