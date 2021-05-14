#include "NEInteractiveMeasurementGizmo.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Base/NEBase.h"

class NEInteractiveMeasurementGizmoPrivate
{
public:
    NEInteractiveMeasurementGizmoPrivate(NEInteractiveMeasurementGizmo* s);

    vec3f m_Offset;                  /**< Offset of the node */
    double m_TextSize;                  /**< TextSize of Text */
    bool m_TextScaleByLength;           /**< Change Font Size By Length */

    void initProperties();

private:
    NESpatial* S;
};

NE_PROP_DEF_VEC3(vec3f, float, NEInteractiveMeasurementGizmo, Offset, vec3f(0.0f, 0.0f, 0.0f), P)
NE_PROP_DEF(bool, NEInteractiveMeasurementGizmo, TextScaleByLength, (false))
NE_PROP_DEF(double, NEInteractiveMeasurementGizmo, TextSize, 0.5)

NEInteractiveMeasurementGizmoPrivate::NEInteractiveMeasurementGizmoPrivate(NEInteractiveMeasurementGizmo* s)
: S(s), m_Offset(NE_DEFAULT(Offset)), m_TextSize(NE_DEFAULT(TextSize))
{
}

void NEInteractiveMeasurementGizmoPrivate::initProperties()
{
    NE_INIT_PROP(NEInteractiveMeasurementGizmo, Offset);
    NE_INIT_PROP(NEInteractiveMeasurementGizmo, TextScaleByLength);
    NE_INIT_PROP(NEInteractiveMeasurementGizmo, TextSize);
}

NEInteractiveMeasurementGizmo::NEInteractiveMeasurementGizmo( NENodeFactory * factory): NESpatial( factory )
{
	// Initializes the private members
    P = new NEInteractiveMeasurementGizmoPrivate(this);

    // Initializes the properties, called once per class
    CALL_ONCE(P->initProperties);

    setNeedsGLTransform(false);
    setInputs( inputs()|NE::kIOGizmo );
}

NEInteractiveMeasurementGizmo::~NEInteractiveMeasurementGizmo()
{
	delete P;
}

uint NEInteractiveMeasurementGizmo::init(uint initMode)
{
    if(!NESpatial::init(initMode))
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

void NEInteractiveMeasurementGizmo::render()
{
}

void NEInteractiveMeasurementGizmo::renderPickable()
{
}

void NEInteractiveMeasurementGizmo::renderPicked()
{
}

const vec3f& NEInteractiveMeasurementGizmo::Offset() const
{
    return P->m_Offset;
}

uint NEInteractiveMeasurementGizmo::SetOffset(const vec3f& offset)
{
    NE_PROP_SET_AND_EVAL_VEC3(Offset, P, offset);

    return NE::kReturnSuccess;
}

const double& NEInteractiveMeasurementGizmo::TextSize() const
{
    return P->m_TextSize;
}

uint NEInteractiveMeasurementGizmo::SetTextSize(const double& val)
{
    NE_PROP_SET_EVAL(TextSize, P, val);
    return NE::kReturnSuccess;
}

const bool& NEInteractiveMeasurementGizmo::TextScaleByLength() const
{
    return P->m_TextScaleByLength;
}

uint NEInteractiveMeasurementGizmo::SetTextScaleByLength(const bool& val)
{
    NE_PROP_SET_EVAL(TextScaleByLength, P, val);
    return NE::kReturnSuccess;
}

bool NEInteractiveMeasurementGizmo::addSource(NENode* node, NEConn& conn)
{
    QMap<uint,NEConn>& srcConns = sourceConnections();
    int cnt = srcConns.size();
    if(cnt > 1) {
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

void NEInteractiveMeasurementGizmo::resetProperties()
{
    NESpatial::resetProperties();
    ResetOffset();
}

void NEInteractiveMeasurementGizmo::propertyTweak(QtPropertyBase* p)
{
    hideProperty("Position", true);

    NESpatial::propertyTweak(p);
}

NEInteractiveMeasurementGizmoFactory::NEInteractiveMeasurementGizmoFactory(NEManager* m) : NENodeFactory( m )
{

}

NEInteractiveMeasurementGizmoFactory::~NEInteractiveMeasurementGizmoFactory() {}

QString NEInteractiveMeasurementGizmoFactory::nodeName() { return "Interactive Measurement Gizmo"; }

uint NEInteractiveMeasurementGizmoFactory::nodeType(){ return NE::kObjectGizmo;}

QString NEInteractiveMeasurementGizmoFactory::nodeInherits() { return "Visuals";}

QString NEInteractiveMeasurementGizmoFactory::objectName() { return "InteractiveMeasurementGizmo"; }

QString NEInteractiveMeasurementGizmoFactory::nodeVersion() { return "0.1"; }

QString NEInteractiveMeasurementGizmoFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEInteractiveMeasurementGizmoFactory::nodeHelp() { return "NEInteractiveMeasurementGizmo"; }

NEObject*NEInteractiveMeasurementGizmoFactory::createInstance() { return new NEInteractiveMeasurementGizmo(this); }
