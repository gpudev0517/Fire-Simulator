
#include "NEClipPlane.h"

#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/NEGLRenderer.h"
#include "Base/NEBase.h"
#include "Rendering/Manager/NERenderManager.h"

class NEClipPlanePrivate
{
public:
    NEClipPlanePrivate(NEClipPlane* s);

    bool m_ClipInvert;
    bool m_bProcess;

    void initProperties();

private:
    NESpatial* S;
};

NE_PROP_DEF(bool, NEClipPlane, ClipInvert, false)

NEClipPlanePrivate::NEClipPlanePrivate(NEClipPlane* s) : S(s),
    m_ClipInvert(NE_DEFAULT(ClipInvert)), m_bProcess(true)
{
}

void NEClipPlanePrivate::initProperties()
{
    NE_INIT_PROP(NEClipPlane, ClipInvert);
    m_bProcess = true;
}

NEClipPlane::NEClipPlane( NENodeFactory * factory): NESpatial( factory )
{
    addNodeTypeToNodeGraphSceneType("ClipPlane", NENode::SceneType::Visualization );

	// Initializes the private members
    P = new NEClipPlanePrivate(this);
	// Initializes the properties, called once per class
    CALL_ONCE(P->initProperties);

    setInputs(inputs()|NE::kIOTriangleMesh);
//    setOutputs(outputs()|NE::kIOTriangleMesh);


}

NEClipPlane::~NEClipPlane()
{
	delete P;
}

const bool& NEClipPlane::ClipInvert() const
{
    return P->m_ClipInvert;
}


uint NEClipPlane::SetClipInvert(const bool& val)
{
    NE_PROP_SET_AND_EVAL(ClipInvert, P, val);

    return NE::kReturnSuccess;
}

bool NEClipPlane::isProcess()
{
    return P->m_bProcess;
}

void NEClipPlane::setProcess(bool bProcess)
{
    P->m_bProcess = bProcess;
}

uint NEClipPlane::init(uint initMode)
{
    if(!NESpatial::init(initMode))
        return NE::kReturnFail;

    createMesh();

    return NE::kReturnSuccess;
}

void NEClipPlane::createMesh()
{
    m_TriMesh.resize();

    NECTNPVertex v;

    v.position = vec3f(-0.5f, 0.0f, -0.5f);
    v.normal = vec3f(0.0f, 1.0f, 0.0f);
    v.tCoord = vec2f(0.0f, 0.0f);
    v.color  = vec4f( 0.5f, 0.5f, 0.5f, 0.3f );
    m_TriMesh.appendVertex(v);
    v.position = vec3f(-0.5f, 0.0f, 0.5f);
    v.tCoord = vec2f(0.0f, 1.0f);
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f, 0.0f, 0.5f);
    v.tCoord = vec2f(1.0f, 1.0f);
    m_TriMesh.appendVertex(v);
    v.position = vec3f(0.5f, 0.0f, -0.5f);
    v.tCoord = vec2f(1.0f, 0.0f);
    m_TriMesh.appendVertex(v);

    //Add triangles now
    NETriangleIndexed t1(1, 3, 0), t2(1, 2, 3);
    m_TriMesh.appendTriangle(t1);
    m_TriMesh.appendTriangle(t2);


    if( NEBASE.guiMode() )
        if(m_TriMesh.numTriangles() > 0)
            m_TriMesh.updateDrawingBuffers(GL_DYNAMIC_DRAW);


    m_normal = v.normal;
}

void NEClipPlane::render()
{
}

void NEClipPlane::renderPickable()
{
}

void NEClipPlane::renderPicked()
{
}

NEIndexedTriMesh& NEClipPlane::mesh()
{
    return m_TriMesh;
}

void NEClipPlane::updateUniforms(QOpenGLShaderProgram* program, int cPlanID, bool isActive)
{
    vec3f normalDir = quaternionsFromEulerAngles(Orientation()).rotatedVector(PlaneNormal());
    vec4f planDir = vec4f(normalDir.x(), normalDir.y(), normalDir.z(), -vec3f::dotProduct(normalDir, Position()));
    if(ClipInvert())
        planDir = -planDir;
    QVector4D* dirQVec = reinterpret_cast<QVector4D*>(&planDir);
    program->setUniformValue(QString("ClipPlanes[" + QString::number(cPlanID) + "]").toUtf8().constData(), *dirQVec);
    GL->glUniform1i(GL->glGetUniformLocation(program->programId(),QString("isProcess[" + QString::number(cPlanID) + "]").toUtf8().constData()), isActive && isProcess());
}

vec3f NEClipPlane::PlaneNormal()
{
    return m_normal;
}

uint NEClipPlane::SetScale(const vec3f &Scale)
{
    return NE::kReturnFail;
}

uint NEClipPlane::SetScaleClip(const vec3f &Scale)
{
    if(NESpatial::SetScale(Scale) != NE::kReturnSuccess)
        return NE::kReturnFail;

    return NE::kReturnSuccess;
}

void NEClipPlane::resetProperties()
{
	NESpatial::resetProperties();
	ResetClipInvert();
}


NEClipPlaneFactory::NEClipPlaneFactory(NEManager* m) : NENodeFactory( m )
{

}

NEClipPlaneFactory::~NEClipPlaneFactory() {}

QString NEClipPlaneFactory::nodeName() { return "Clip Plane"; }

uint NEClipPlaneFactory::nodeType(){ return NE::kObjectGizmo;}

QString NEClipPlaneFactory::nodeInherits() { return "Visuals";}

QString NEClipPlaneFactory::objectName() { return "ClipPlane"; }

QString NEClipPlaneFactory::nodeVersion() { return "0.1"; }

QString NEClipPlaneFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEClipPlaneFactory::nodeHelp() { return "NEClipPlane"; }

NEObject*NEClipPlaneFactory::createInstance() { return new NEClipPlane(this); }
