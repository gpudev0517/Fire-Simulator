#include "NENullGizmo.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Base/NEBase.h"

class NENullGizmoPrivate
{
public:
    NENullGizmoPrivate(NENullGizmo* s);

private:
    NESpatial* S;
};

NENullGizmoPrivate::NENullGizmoPrivate(NENullGizmo* s) : S(s)
{
}


NENullGizmo::NENullGizmo( NENodeFactory * factory): NESpatial( factory )
{
	// Initializes the private members
    P = new NENullGizmoPrivate(this);
    setOutputs( outputs()|NE::kIOGizmo );
}

NENullGizmo::~NENullGizmo()
{
	delete P;
}

uint NENullGizmo::init(uint initMode)
{
    if(!NESpatial::init(initMode))
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

void NENullGizmo::render()
{
}

void NENullGizmo::renderPickable()
{
}

void NENullGizmo::renderPicked()
{
}


bool NENullGizmo::connectTo(NENode* target, uint type, uint sIndex, uint tIndex, int order)
{
    if(target == 0)
        return false;

    QMap<uint,NEConn>& tSrcConns = target->sourceConnections();
    int cnt = tSrcConns.size();

    if(cnt < 2) {
        order = cnt;
        if(cnt == 1) {
            int key = tSrcConns.begin().key();
            if(key == 0)
                order = 1;
            else // key == 1
                order = 0;
        }
    }
    else {
        order = 0;
    }

    return NENode::connectTo(target, type, sIndex, tIndex, order);
}

NENullGizmoFactory::NENullGizmoFactory(NEManager* m) : NENodeFactory( m )
{

}

NENullGizmoFactory::~NENullGizmoFactory() {}

QString NENullGizmoFactory::nodeName() { return "Null Gizmo"; }

uint NENullGizmoFactory::nodeType(){ return NE::kObjectGizmo;}

QString NENullGizmoFactory::nodeInherits() { return "Visuals";}

QString NENullGizmoFactory::objectName() { return "NullGizmo"; }

QString NENullGizmoFactory::nodeVersion() { return "0.1"; }

QString NENullGizmoFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NENullGizmoFactory::nodeHelp() { return "NENullGizmo"; }

NEObject*NENullGizmoFactory::createInstance() { return new NENullGizmo(this); }
