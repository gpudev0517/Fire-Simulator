#include "FZoneItem.h"
#include "FZone.h"
#include "Mesh/NESpatialMesh.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Core/Utility/mathUtil.h"

class FZoneItemPrivate
{
public:
    FZoneItemPrivate(FZoneItem* s) : S(s)
    {
    }

    /* Fonctions */
    double interpValue(const std::vector<std::pair<double, double>>& timeValuePairs, double time);

    /* Properties */
    FZoneItem::FItemType m_ItemType;
    QString m_PlantItemType;
    QString m_Issues;
    NEFailureCriteria m_Failure;
    bool m_Failed;
    double m_FailedTime;
    QColor m_FailedColor;
    QString m_Desc;
    uint m_ConnectorID;
    NESurfaceMaterial m_Material;
    uint m_CompartmentID;
    vec3f  m_GhostPosition;
    bool m_ShowGhostPosition;

    /* Other members */
    QString m_ZoneItemName;
    QString m_Model3DInfo;
    // Simulation output data
    std::vector<std::pair<double, double>> m_TimeSurfTempPairs;
    double m_MinSurfTemp;
    double m_MaxSurfTemp;

private:
    FZoneItem* S;
};

//NE_PROP_DEF(FilePath, FZoneItem, GeoFile, FilePath(FilePath::kRefeFileType))
NE_PROP_DEF(FZoneItem::FItemType, FZoneItem, ItemType, FZoneItem::Component)
NE_PROP_DEF(uint, FZoneItem, ConnectorID, 0)
NE_PROP_DEF(QString, FZoneItem, Desc, QString(""))
NE_PROP_DEF(QString, FZoneItem, Issues, QString(""))
NE_PROP_DEF(NEFailureCriteria, FZoneItem, Failure, NEFailureCriteria(""))
NE_PROP_DEF(QColor, FZoneItem, FailedColor, QColor(255, 0, 0))
//NE_PROP_DEF(QString, FZoneItem, PlantItemType, QString(""))
NE_PROP_DEF(NESurfaceMaterial, FZoneItem, Material, NESurfaceMaterial(""))
NE_PROP_DEF(uint, FZoneItem, CompartmentID, 0)
NE_PROP_DEF(bool, FZoneItem, ShowGhostPosition, false)

static void initProperties()
{
    //NE_INIT_PROP(FZoneItem, GeoFile);
    NE_INIT_PROP(FZoneItem, ItemType);
    NE_INIT_PROP(FZoneItem, ConnectorID);
    NE_INIT_PROP(FZoneItem, Desc);
    NE_INIT_PROP(FZoneItem, Issues);
    NE_INIT_PROP(FZoneItem, Failure);
    NE_INIT_PROP(FZoneItem, FailedColor);
    //NE_INIT_PROP(FZoneItem, PlantItemType );
    NE_INIT_PROP(FZoneItem, Material);
    NE_INIT_PROP(FZoneItem, CompartmentID);
    NE_INIT_PROP(FZoneItem, ShowGhostPosition);
}

FZoneItem::FZoneItem(NENodeFactory* factory) : NEZoneItem(factory)
{
    P = new FZoneItemPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);

    P->m_ItemType = NE_DEFAULT(ItemType);
    P->m_ConnectorID = NE_DEFAULT(ConnectorID);
    P->m_Failure = NE_DEFAULT(Failure);;
    P->m_FailedColor = NE_DEFAULT(FailedColor);
    P->m_Desc = NE_DEFAULT(Desc);
    P->m_Issues = NE_DEFAULT(Issues);
    P->m_Material = NE_DEFAULT(Material);
    P->m_CompartmentID = NE_DEFAULT(CompartmentID);
    P->m_ShowGhostPosition = NE_DEFAULT(ShowGhostPosition);

    P->m_GhostPosition = vec3f(0.0, 0.0, 0.0);

    SetSpatialMeshType(NESpatialMeshType::None);
    SetPivot(vec3f(-0.5, -0.5, -0.5));
    SetColor( QColor( 0, 0, 255 )) ;
    SetSolidWireframeColor( QColor( 0, 0, 0, 0 ) );
    SetDrawSolidWireframe( true );

    setOutputs( outputs() | NE::kIOTriangleMesh );

    resetFailed();
}

FZoneItem::~FZoneItem()
{
    delete P;
}

matrix44f FZoneItem::ghostTransform()
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

uint FZoneItem::init(uint initMode)
{
    uint ret = NEZoneItem::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    if(zoneItemName().isEmpty())
        setZoneItemName(objectName());

    P->m_MinSurfTemp = -DBL_MAX;
    P->m_MaxSurfTemp = DBL_MAX;

    hideProperty("Failure", P->m_ItemType == Trays || P->m_ItemType == Conduits || P->m_ItemType == Cable || P->m_ItemType == OtherRaceways);
    hideProperty("FailedTime", !P->m_Failed);

    return NE::kReturnSuccess;
}

uint FZoneItem::setPlantItemType( const QString& plantItemType )
{
    P->m_PlantItemType = plantItemType;

    hideProperty("Failure", P->m_ItemType == Trays || P->m_ItemType == Conduits || P->m_ItemType == Cable || P->m_ItemType == OtherRaceways);
    hideProperty("FailedTime", !P->m_Failed);

    return NE::kReturnSuccess;
}

const QString& FZoneItem::PlantItemType() const
{
    return P->m_PlantItemType;
}

//QString toCamelCase(const QString& s)
//{
//    QStringList parts = s.split('_', QString::SkipEmptyParts);
//    for (int i=1; i<parts.size(); ++i)
//        parts[i].replace(0, 1, parts[i][0].toUpper());

//    return parts.join("");
//}

QString FZoneItem::categoryType()
{
    if(P->m_ItemType == FZoneItem::Component)
        return "Components";
    else if(P->m_ItemType == FZoneItem::Cable)
        return "Cables";
    else if(P->m_ItemType == FZoneItem::OtherRaceways)
        return "Other Raceways";
    else if(P->m_ItemType == FZoneItem::Sources)
        return "Sources";
    else if(P->m_ItemType == FZoneItem::Trays)
        return "Trays";
    else if(P->m_ItemType == FZoneItem::Conduits)
        return "Conduits";
    else if(P->m_ItemType == FZoneItem::Equipments)
        return "Equipments";
    else if(P->m_ItemType == FZoneItem::Physical)
        return "Physical Only";

    return "Unknowns";
}

QString FZoneItem::otherCategoryType()
{
    if(Type() == "foRaceway")
        return "Raceways";
    else if(Type() == "foCable")
            return "Cables";
    else if(Type() == "foComponent")
        return "Components";
    else if(Type() == "foBasicEvent")
        return "BasicEvents";
    else
        return "Unknown";
}

uint FZoneItem::SetPosition(const vec3f& val)
{
    uint ret = NEZoneItem::SetPosition(val);
    if (ret != NE::kReturnSuccess)
        return ret;

    computeGhostPosition();

    return NE::kReturnSuccess;
}

const QString& FZoneItem::zoneItemName() const
{
    return P->m_ZoneItemName;
}

uint FZoneItem::setZoneItemName(const QString& zoneItemName)
{
    P->m_ZoneItemName = zoneItemName;

    return NE::kReturnSuccess;
}

const FZoneItem::FItemType& FZoneItem::ItemType() const
{
    return P->m_ItemType;
}

uint FZoneItem::SetItemType(const FZoneItem::FItemType& val)
{
    NE_PROP_SET_EVAL(ItemType, P, val);

    if( P->m_ItemType == Trays || P->m_ItemType == Conduits || P->m_ItemType == Cable || P->m_ItemType == OtherRaceways )
    {
        SetSolidWireframeColor( QColor( 21, 0, 63, 0 ) );
        SetColor(  QColor( 85, 0, 255, 0 ) );
    }

    return NE::kReturnSuccess;
}

const uint& FZoneItem::ConnectorID() const
{
    return P->m_ConnectorID;
}

uint FZoneItem::SetConnectorID(const uint& val)
{
    NE_PROP_SET_EVAL(ConnectorID, P, val);

    return NE::kReturnSuccess;
}

const QString& FZoneItem::Desc() const
{
    return P->m_Desc;
}

uint FZoneItem::SetDesc(const QString& val)
{
    NE_PROP_SET_EVAL(Desc, P, val);

    return NE::kReturnSuccess;
}

const QString& FZoneItem::Issues() const
{
    return P->m_Issues;
}

uint FZoneItem::SetIssues(const QString& val)
{
    NE_PROP_SET_EVAL(Issues, P, val);

    return NE::kReturnSuccess;
}

const NESurfaceMaterial& FZoneItem::Material() const
{
    return P->m_Material;
}

uint FZoneItem::SetMaterial(const NESurfaceMaterial& val)
{
    NE_PROP_SET_EVAL(Material, P, val);

    return NE::kReturnSuccess;
}

const uint& FZoneItem::CompartmentID() const
{
    return P->m_CompartmentID;
}

uint FZoneItem::SetCompartmentID(const uint& val)
{
    NE_PROP_SET_EVAL(CompartmentID, P, val);

    return NE::kReturnSuccess;
}

const vec3f& FZoneItem::GhostPosition() const
{
    return P->m_GhostPosition;
}

const bool& FZoneItem::ShowGhostPosition() const
{
    return P->m_ShowGhostPosition;
}

uint FZoneItem::SetShowGhostPosition(const bool& val)
{
    NE_PROP_SET_EVAL(ShowGhostPosition, P, val);

    return NE::kReturnSuccess;
}

const NEFailureCriteria& FZoneItem::Failure() const
{
    return P->m_Failure;
}

uint FZoneItem::SetFailure(const NEFailureCriteria& val)
{
    NE_PROP_SET_EVAL(Failure, P, val);

    return NE::kReturnSuccess;
}

const bool& FZoneItem::Failed() const
{
    return P->m_Failed;
}

const double& FZoneItem::FailedTime() const
{
    return P->m_FailedTime;
}

const QColor& FZoneItem::FailedColor() const
{
    return P->m_FailedColor;
}

uint FZoneItem::SetFailedColor(const QColor& val)
{
    NE_PROP_SET_EVAL(FailedColor, P, val);

    return NE::kReturnSuccess;
}

QString FZoneItem::nodeDescription()
{
    return Desc();
}

void FZoneItem::copyValues(const NEZoneItem *zoneitem)
{
    SetID(zoneitem->ID());
    setOriginalName( zoneitem->originalName() );
    SetType(zoneitem->Type());
    setItemType(const_cast<NEZoneItem*>(zoneitem)->itemType());
    // Some default settings

    SetColor( QColor( 0, 0, 255 ) );

    FZoneItem *fzoneitem = qobject_cast< FZoneItem *>( const_cast< NEZoneItem *>( zoneitem ) );
    if( !fzoneitem )
    {
        return;
    }


    SetDesc( fzoneitem->Desc() );
    SetIssues( fzoneitem->Issues() );

    setModel3DInfo( fzoneitem->model3DInfo() );
    SetItemType( fzoneitem->ItemType() );

}

int FZoneItem::itemType()
{
    return P->m_ItemType;
}

void FZoneItem::resetProperties()
{
    NEZoneItem::resetProperties();

    ResetItemType();
    ResetConnectorID();
    ResetFailure();
    ResetFailedColor();
    ResetDesc();
    ResetIssues();
    ResetMaterial();
    ResetCompartmentID();
    ResetShowGhostPosition();

    computeGhostPosition();

    hideProperty("Failure", P->m_ItemType == Trays || P->m_ItemType == Conduits || P->m_ItemType == Cable || P->m_ItemType == OtherRaceways);
    hideProperty("FailedTime", !P->m_Failed);
}

const QString& FZoneItem::model3DInfo() const
{
    return P->m_Model3DInfo;
}

void FZoneItem::setModel3DInfo(const QString& model3dinfo)
{
    P->m_Model3DInfo = model3dinfo;
}

uint FZoneItem::setFailed(double failedTime)
{
    if (failedTime < 0.0)
        return NE::kReturnFail;

    P->m_Failed = true;
    P->m_FailedTime = failedTime;

    hideProperty("FailedTime", !P->m_Failed);

    return NE::kReturnSuccess;
}

void FZoneItem::resetFailed()
{
    P->m_Failed = false;
    P->m_FailedTime = -1.0;

    hideProperty("FailedTime", !P->m_Failed);
}

void FZoneItem::computeGhostPosition(const FZone* comp)
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

    /*// Create the AABB in world coord. system, since aabb() currently returns the AABB in local coord. system
    vec3f vertices[8];
    vertices[0] = aabb().minPos();
    vertices[6] = aabb().maxPos();
    vertices[1] = vec3f(aabb().minPos().x(),aabb().maxPos().y(),aabb().minPos().z());
    vertices[2] = vec3f(aabb().maxPos().x(),aabb().maxPos().y(),aabb().minPos().z());
    vertices[3] = vec3f(aabb().maxPos().x(),aabb().minPos().y(),aabb().minPos().z());
    vertices[4] = vec3f(aabb().minPos().x(),aabb().minPos().y(),aabb().maxPos().z());
    vertices[5] = vec3f(aabb().minPos().x(),aabb().maxPos().y(),aabb().maxPos().z());
    vertices[7] = vec3f(aabb().maxPos().x(),aabb().minPos().y(),aabb().maxPos().z());

    AABB worldAABB;
    for (int i = 0; i < 8; i++)
        worldAABB.insertPoint(this->transform()*vertices[i]);

    if (worldAABB.isInside(comp->compAABB()))
    {
//        qDebug() << "AABB is inside compartment AABB";
        P->m_GhostPosition = Position();
    }
    else
    {
//        qDebug() << "AABB is not inside compartment AABB";

        bool isInsideX = worldAABB.minPos().x() >= comp->compAABB().minPos().x() && worldAABB.maxPos().x() <= comp->compAABB().maxPos().x();
        bool isInsideZ = worldAABB.minPos().z() >= comp->compAABB().minPos().z() && worldAABB.maxPos().z() <= comp->compAABB().maxPos().z();

        if (isInsideX && isInsideZ)
        {
//            qDebug() << "AABB is within the x- and z- extents of compartment AABB";

            P->m_GhostPosition = Position();

            float sizeY = worldAABB.maxPos().y()-worldAABB.minPos().y();
            if (sizeY > comp->CompBBSize().y())
            {
                qDebug() << objectName() << "is too large to fit to the compartment";
                P->m_GhostPosition.y() = comp->CompBBCenter().y();
            }
            else if (worldAABB.maxPos().y() > comp->compAABB().maxPos().y())
                P->m_GhostPosition.y() = comp->compAABB().maxPos().y()-0.5*sizeY;
            else if (worldAABB.minPos().y() < comp->compAABB().minPos().y())
                P->m_GhostPosition.y() = comp->compAABB().minPos().y()+0.5*sizeY;
        }
        else
        {
//            qDebug() << "AABB is not within the x- and z- extents of compartment AABB";

            vec3f rayStart = Position();

            bool isInsideY = worldAABB.minPos().y() >= comp->compAABB().minPos().y() && worldAABB.maxPos().y() <= comp->compAABB().maxPos().y();
            if (!isInsideY)
            {
                float sizeY = worldAABB.maxPos().y()-worldAABB.minPos().y();
                if (sizeY > comp->CompBBSize().y())
                {
                    qDebug() << objectName() << "is too large to fit the compartment";
                    rayStart.y() = comp->CompBBCenter().y();
                }
                else if (worldAABB.maxPos().y() > comp->compAABB().maxPos().y())
                    rayStart.y() = worldAABB.maxPos().y()-0.5*sizeY;
                else if (worldAABB.minPos().y() < comp->compAABB().minPos().y())
                    rayStart.y() = worldAABB.minPos().y()+0.5*sizeY;
            }

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
            {
                qDebug() << "Failed to compute a ghost position for" << objectName();
                P->m_GhostPosition = Position();
            }
            else
            {
                vec3f interPoint = interPointSquaredDistPair.first;
                if (!comp->compAABB().isInside(interPoint))
                    interPoint += FEps*rayDirection;
                P->m_GhostPosition = interPoint;

                float sizeX = worldAABB.maxPos().x()-worldAABB.minPos().x();
                float sizeZ = worldAABB.maxPos().z()-worldAABB.minPos().z();
                if (sizeX > comp->CompBBSize().x() && sizeZ > comp->CompBBSize().z())
                {
                    qDebug() << objectName() << "is too large to fit the compartment";
                    P->m_GhostPosition.x() = comp->CompBBCenter().x();
                    P->m_GhostPosition.z() = comp->CompBBCenter().z();
                }
                else if (sizeX > comp->CompBBSize().x())
                {
                    qDebug() << objectName() << "is too large to fit the compartment";
                    P->m_GhostPosition.x() = comp->CompBBCenter().x();

                    if (P->m_GhostPosition.z()-0.5*sizeZ < comp->compAABB().minPos().z())
                        P->m_GhostPosition.z() = comp->compAABB().minPos().z()+0.5*sizeZ;
                    else if (P->m_GhostPosition.z()+0.5*sizeZ > comp->compAABB().maxPos().z())
                        P->m_GhostPosition.z() = comp->compAABB().maxPos().z()+0.5*sizeZ;
                }
                else if (sizeZ > comp->CompBBSize().z())
                {
                    qDebug() << objectName() << "is too large to fit the compartment";
                    P->m_GhostPosition.z() = comp->CompBBCenter().z();

                    if (P->m_GhostPosition.x()-0.5*sizeX < comp->compAABB().minPos().x())
                        P->m_GhostPosition.x() = comp->compAABB().minPos().x()+0.5*sizeX;
                    else if (P->m_GhostPosition.x()+0.5*sizeX > comp->compAABB().maxPos().x())
                        P->m_GhostPosition.x() = comp->compAABB().maxPos().x()+0.5*sizeX;
                }
                else
                {
                    if (P->m_GhostPosition.x()-0.5*sizeX < comp->compAABB().minPos().x())
                        P->m_GhostPosition.x() = comp->compAABB().minPos().x()+0.5*sizeX;
                    else if (P->m_GhostPosition.x()+0.5*sizeX > comp->compAABB().maxPos().x())
                        P->m_GhostPosition.x() = comp->compAABB().maxPos().x()+0.5*sizeX;

                    if (P->m_GhostPosition.z()-0.5*sizeZ < comp->compAABB().minPos().z())
                        P->m_GhostPosition.z() = comp->compAABB().minPos().z()+0.5*sizeZ;
                    else if (P->m_GhostPosition.z()+0.5*sizeZ > comp->compAABB().maxPos().z())
                        P->m_GhostPosition.z() = comp->compAABB().maxPos().z()+0.5*sizeZ;
                }
            }
        }
    }*/
}

void FZoneItem::computeGhostPosition()
{
    QObject* parent = this->parent();
    while (parent != nullptr && qobject_cast<FZone*>(parent) == nullptr)
        parent = parent->parent();

    if (FZone* zone = qobject_cast<FZone*>(parent))
        computeGhostPosition(zone);
}

void FZoneItem::resetGhostPosition()
{
    P->m_GhostPosition = Position();
}

double FZoneItem::surfTemp(double time) const
{
    return P->interpValue(P->m_TimeSurfTempPairs, time);
}

double FZoneItem::minSurfTemp() const
{
    return P->m_MinSurfTemp;
}

double FZoneItem::maxSurfTemp() const
{
    return P->m_MaxSurfTemp;
}

void FZoneItem::addSurfTemp(double time, double temperature)
{
    P->m_TimeSurfTempPairs.push_back(std::pair<double, double>(time, temperature));

    if (P->m_MinSurfTemp == -DBL_MAX)
        P->m_MinSurfTemp = temperature;
    else
        P->m_MinSurfTemp = std::min(P->m_MinSurfTemp, temperature);

    if (P->m_MaxSurfTemp == DBL_MAX)
        P->m_MaxSurfTemp = temperature;
    else
        P->m_MaxSurfTemp = std::max(P->m_MaxSurfTemp, temperature);
}

void FZoneItem::clearSimData()
{
    P->m_TimeSurfTempPairs.clear();

    P->m_MinSurfTemp = -DBL_MAX;
    P->m_MaxSurfTemp = DBL_MAX;
}

double FZoneItemPrivate::interpValue(const std::vector<std::pair<double, double>>& timeValuePairs, double time)
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

FZoneItemFactory::FZoneItemFactory(NEManager* m) : NENodeFactory(m)
{

}

FZoneItemFactory::~FZoneItemFactory()
{

}

QString FZoneItemFactory::nodeName()
{
    return "Fire Plant Item";
}

QStringList FZoneItemFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("Fire Zone Item");

    return aNames;
}

uint FZoneItemFactory::nodeType()
{
    return NE::kObjectParticleRigid;
}

QString FZoneItemFactory::nodeInherits()
{
    return "Geometry";
}

QString FZoneItemFactory::objectName()
{
    return "FZoneItem";
}

QString FZoneItemFactory::nodeVersion()
{
    return "0.1";
}

QString FZoneItemFactory::nodeVendor()
{
    return "FRI3D";
}

QString FZoneItemFactory::nodeHelp()
{
    return "Describing a Fire Zone Item/Object";
}

NEObject* FZoneItemFactory::createInstance()
{
    return new FZoneItem(this);
}
