#include "NEMeasurementItem.h"
#include "Base/NESceneManager.h"
#include "Base/NEBase.h"

#include <QJsonArray>
#include <QJsonDocument>

class NEMeasurementItemPrivate
{
public:
    NEMeasurementItemPrivate(NEMeasurementItem* s);

    vec3f m_Offset;                  /**< Offset of the node */
    double m_TextSize;                  /**< TextSize of Text */
    bool m_TextScaleByLength;           /**< Change Font Size By Length */
    NEMeasurementItem::NEMeasurementType m_MeasureType;
    NECamera::OrthographicType m_CameraType;

    void initProperties();

private:
    NESpatial* S;
};


NEMeasurementItemPrivate::NEMeasurementItemPrivate(NEMeasurementItem* s) : S(s)
{
}

NE_PROP_DEF_VEC3(vec3f, float, NEMeasurementItem, Offset, vec3f(0.0f, 0.0f, 0.0f), P)
NE_PROP_DEF(bool, NEMeasurementItem, TextScaleByLength, (false))
NE_PROP_DEF(double, NEMeasurementItem, TextSize, 1.0)
NE_PROP_DEF(NEMeasurementItem::NEMeasurementType, NEMeasurementItem, MeasureType, NEMeasurementItem::Point)
NE_PROP_DEF(NECamera::OrthographicType, NEMeasurementItem, CameraType, NECamera::Top)

void NEMeasurementItemPrivate::initProperties()
{
    NE_INIT_PROP(NEMeasurementItem, Offset);
    NE_INIT_PROP(NEMeasurementItem, TextScaleByLength);
    NE_INIT_PROP(NEMeasurementItem, TextSize);
    NE_INIT_PROP(NEMeasurementItem, MeasureType);
    NE_INIT_PROP(NEMeasurementItem, CameraType);
}



NEMeasurementItem::NEMeasurementItem( NENodeFactory * factory): NESpatial( factory )
{
    // Initializes the private members
    measureInfoOrigin.clear();
    P = new NEMeasurementItemPrivate(this);

    // Initializes the properties, called once per class
    CALL_ONCE(P->initProperties);
    SetTextSize( 1.0 );

    setInputs( inputs()|NE::kIOGizmo );

    setOutputs( outputs()|NE::kIOGizmo );
}

NEMeasurementItem::~NEMeasurementItem()
{
    delete P;
}

uint NEMeasurementItem::init(uint initMode)
{
    if(!NESpatial::init(initMode))
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

void NEMeasurementItem::render()
{
}

void NEMeasurementItem::renderPickable()
{
}

void NEMeasurementItem::renderPicked()
{
}

NESpatial::NESpatialType NEMeasurementItem::spatialType()
{
    return NESpatialType::kGizmo;
}


QString NEMeasurementItem::toJson()
{
    QJsonObject jsonObj;
    QJsonArray measureInfoOriginArr;
    QJsonArray measureEdgeInfoArr;

    if( measureInfoOrigin.size() >= 2 )
    {
        QJsonObject obj0, obj1;
        QString spatialName0 = measureInfoOrigin.at(0).first->objectName();
        QString spatialName1 = measureInfoOrigin.at(1).first->objectName();
        obj0[ spatialName0 ] = measureInfoOrigin.at(0).second;
        obj1[ spatialName1 ] = measureInfoOrigin.at(1).second;
        measureInfoOriginArr.append( obj0 );
        measureInfoOriginArr.append( obj1 );
        jsonObj[ "measureInfoOrigin" ] = measureInfoOriginArr;
    }
    if( measureEdgeInfo.size() >= 2 )
    {
        QJsonObject obj0, obj1;

        QString spatialName0 = measureEdgeInfo.at(0).first->objectName();
        QString spatialName1 = measureEdgeInfo.at(1).first->objectName();

        QJsonObject firstedge, secondedge;
        firstedge[ "first" ] = measureEdgeInfo.at(0).second.first.first;
        firstedge[ "second" ] = measureEdgeInfo.at(0).second.first.second;

        firstedge["posx"] = measureEdgeInfo.at(0).second.second.x();;
        firstedge["posy"] = measureEdgeInfo.at(0).second.second.y();;
        firstedge["posz"] = measureEdgeInfo.at(0).second.second.z();;

        secondedge[ "first" ] = measureEdgeInfo.at(1).second.first.first;
        secondedge[ "second" ] = measureEdgeInfo.at(1).second.first.second;
        secondedge["posx"] = measureEdgeInfo.at(1).second.second.x();
        secondedge["posy"] = measureEdgeInfo.at(1).second.second.y();
        secondedge["posz"] = measureEdgeInfo.at(1).second.second.z();

        obj0[ spatialName0 ] = firstedge;
        obj1[ spatialName1 ] = secondedge;

        measureEdgeInfoArr.append( obj0 );
        measureEdgeInfoArr.append( obj1 );
        jsonObj[ "measureEdgeInfo" ] = measureEdgeInfoArr;

    }
    QJsonObject measureInfoMovedObj;
    measureInfoMovedObj[ "dirx" ] = measureInfoMoved.first.x();
    measureInfoMovedObj[ "diry" ] = measureInfoMoved.first.y();
    measureInfoMovedObj[ "dirz" ] = measureInfoMoved.first.z();
    measureInfoMovedObj[ "amount" ] = measureInfoMoved.second;

    jsonObj[ "measureInfoMoved" ] = measureInfoMovedObj;

    QJsonDocument jdoc( jsonObj );
    return jdoc.toJson();
}


void NEMeasurementItem::fromJson(QString &obj)
{
    QJsonDocument jdoc = QJsonDocument::fromJson( obj.toUtf8() );
    QJsonObject jobj = jdoc.object();

    //std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin;
    //std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfo;
    //QPair<vec3f, float> measureInfoMoved;


    QJsonObject movedobj = jobj[ "measureInfoMoved" ].toObject();
    measureInfoMoved.first = vec3f( movedobj[ "dirx" ].toDouble(),
            movedobj[ "diry" ].toDouble(),
            movedobj[ "dirz" ].toDouble() );
    measureInfoMoved.second = movedobj["amount"].toDouble();


    if( jobj.keys().contains( "measureInfoOrigin" ) )
    {
        QJsonArray miarr = jobj["measureInfoOrigin"].toArray();
        for( int i = 0; i < miarr.size(); ++i )
        {
            QJsonObject jobj = miarr[i].toObject();
            QStringList spatialnames = jobj.keys();
            for( QJsonObject::iterator itr = jobj.begin(); itr != jobj.end(); ++itr )
            {
                QString spatialname = itr.key();
                NESpatialMesh* mesh = qobject_cast< NESpatialMesh *>( NESCENE.findChildNode( spatialname ) );
                if( mesh )
                {
                    QPair< NESpatialMesh *, int > pair;
                    pair.first = mesh;
                    pair.second =  itr.value().toInt();
                    measureInfoOrigin.push_back( pair );
                }
            }
        }
    }
    else if( jobj.keys().contains( "measureEdgeInfo" ) )
    {
        QJsonArray miarr = jobj["measureEdgeInfo"].toArray();

        for( int i = 0; i < miarr.size(); ++i )
        {
            QJsonObject jobj = miarr[i].toObject();
            QStringList spatialnames = jobj.keys();
            for( QJsonObject::iterator itr = jobj.begin(); itr != jobj.end(); ++itr )
            {
                QString spatialname = itr.key();
                NESpatialMesh* mesh = qobject_cast< NESpatialMesh *>( NESCENE.findChildNode( spatialname ) );
                if( mesh )
                {

                    QPair<int, int> pair3;
                    QJsonObject edgeobj = itr.value().toObject();
                    pair3.first =  edgeobj["first"].toInt();
                    pair3.second = edgeobj["second"].toInt();

                    QPair<QPair<int, int>, vec3f> pair2;
                    pair2.first = pair3;
                    pair2.second = vec3f( edgeobj["posx"].toDouble(), edgeobj["posy"].toDouble(), edgeobj["posz"].toDouble() );

                    QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>> pair;
                    pair.first = mesh;
                    pair.second =  pair2;
                    measureEdgeInfo.push_back( pair );
                }
            }
        }
    }
}

const NECamera::OrthographicType& NEMeasurementItem::CameraType() const
{
    return P->m_CameraType;
}

uint NEMeasurementItem::SetCameraType(const NECamera::OrthographicType& value)
{
    //NE_PROP_SET_AND_EVAL( CameraType, P, value );
    P->m_CameraType = value;

    return NE::kReturnSuccess;

}

bool NEMeasurementItem::renderModeVisible() {return false;}

bool NEMeasurementItem::solidParamVisible() {return false;}

bool NEMeasurementItem::receiveShadowVisible() {return false;}

bool NEMeasurementItem::occludeShadowsVisible() {return false;}

bool NEMeasurementItem::isMeasurementEdge() const
{
    return MeasureType() == NEMeasurementItem::Edge;
}

void NEMeasurementItem::setMeasurementEdge(bool value)
{
    if( value == true )
        SetMeasureType( NEMeasurementItem::Edge );
}


const vec3f& NEMeasurementItem::Offset() const
{
    return P->m_Offset;
}

uint NEMeasurementItem::SetOffset(const vec3f& offset)
{
    NE_PROP_SET_AND_EVAL_VEC3(Offset, P, offset);

    return NE::kReturnSuccess;
}

const double& NEMeasurementItem::TextSize() const
{
    return P->m_TextSize;
}

uint NEMeasurementItem::SetTextSize(const double& val)
{
    NE_PROP_SET_EVAL(TextSize, P, val);
    return NE::kReturnSuccess;
}

const bool& NEMeasurementItem::TextScaleByLength() const
{
    return P->m_TextScaleByLength;
}

uint NEMeasurementItem::SetTextScaleByLength(const bool& val)
{
    NE_PROP_SET_EVAL(TextScaleByLength, P, val);
    return NE::kReturnSuccess;
}

const NEMeasurementItem::NEMeasurementType& NEMeasurementItem::MeasureType() const
{
    return P->m_MeasureType;
}

uint NEMeasurementItem::SetMeasureType(const NEMeasurementItem::NEMeasurementType &val )
{
    NE_PROP_SET_EVAL(MeasureType, P, val);
    return NE::kReturnSuccess;
}

bool NEMeasurementItem::addSource(NENode* node, NEConn& conn)
{
    QMap<uint,NEConn>& srcConns = sourceConnections();
    int cnt = srcConns.size();
    if(cnt > 1)
    {
        NEConn oldSrcConn = srcConns[0];
        NENode* pOldSrc = oldSrcConn.source;
        disconnectFrom(pOldSrc, oldSrcConn.type, oldSrcConn.sIndex, oldSrcConn.tIndex);

        // change source node's outConn.order
        QMap<uint, NEConn>& sourceOutConns = node->connections();
        int key = MAX(conn.order, sourceOutConns.size()-1);
        NEConn& outConn = sourceOutConns[key];
        outConn.order = 0;
        conn.order = 0;
    }

    return NENode::addSource(node, conn);
}

void NEMeasurementItem::resetProperties()
{
    NESpatial::resetProperties();
    ResetOffset();
}

void NEMeasurementItem::propertyTweak(QtPropertyBase* p)
{
    hideProperty("Position", true);

    NESpatial::propertyTweak(p);
}

bool NEMeasurementItem::scaleVisible() {return false;}

bool NEMeasurementItem::pivotVisible() {return false;}


NEMeasurementItemFactory::NEMeasurementItemFactory(NEManager* m) : NENodeFactory( m )
{

}

NEMeasurementItemFactory::~NEMeasurementItemFactory() {}

QString NEMeasurementItemFactory::nodeName() { return "Measurement Item"; }

uint NEMeasurementItemFactory::nodeType(){ return NE::kObjectGizmo;}

QString NEMeasurementItemFactory::nodeInherits() { return "Visuals";}

QString NEMeasurementItemFactory::objectName() { return "NEMeasurementItem"; }

QString NEMeasurementItemFactory::nodeVersion() { return "0.1"; }

QString NEMeasurementItemFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEMeasurementItemFactory::nodeHelp() { return "NEMeasurementItem"; }

NEObject*NEMeasurementItemFactory::createInstance() { return new NEMeasurementItem(this); }
