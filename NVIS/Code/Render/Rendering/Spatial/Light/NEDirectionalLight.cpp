
#include "Rendering/Spatial/Light/NEDirectionalLight.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"
#include "Base/NEBase.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEIGLSurface.h"

#include "Rendering/Spatial/NECamera.h"

class NEDirectionalLightPrivate
{
public:

    NEDirectionalLightPrivate(NEDirectionalLight* s) : S(s)
    {

    }
    vec3f m_Direction;
    vec3f m_NormalizedDirection;
    NEIndexedTriMesh* m_Mesh;
private:
    NEDirectionalLight* S;
};

NE_PROP_DEF_VEC3(vec3f, float, NEDirectionalLight, Direction, vec3f(3, 2, 1), P)

static void initProperties()
{
    NE_INIT_PROP(NEDirectionalLight, Direction);
}

NEDirectionalLightFactory::NEDirectionalLightFactory(NEManager* m) : NENodeFactory(m)
{
    if( NEBASE.guiMode() )
    {
        m_pMesh = new NEIndexedTriMesh();
        m_pMesh->loadFromFile("System/Meshes/directionalLight.obj");
        //m_pMesh->createDrawingBuffers(GL_STATIC_DRAW);
        NERENDERMAN.GLManager()->createDrawingBuffers(m_pMesh, GL_STATIC_DRAW);

        NEIcon icon;
        icon.iconPath = QString(":/Neutrino/Resources/16/directionalLight16.png");
        icon.iconSize = QSize(16, 16);
        nodeIcon()->push_back(icon);

        icon.iconPath = QString("::/Neutrino/Resources/22/directionalLight22.png");
        icon.iconSize = QSize(22, 22);
        nodeIcon()->push_back(icon);

        icon.iconPath = QString("::/Neutrino/Resources/32/directionalLight32.png");
        icon.iconSize = QSize(32, 32);
        nodeIcon()->push_back(icon);

//        nodeIcon()->addFile(":/Neutrino/Resources/16/directionalLight16.png", QSize(16, 16));
//        nodeIcon()->addFile(":/Neutrino/Resources/22/directionalLight22.png", QSize(22, 22));
//        nodeIcon()->addFile(":/Neutrino/Resources/32/directionalLight32.png", QSize(32, 32));
    }
}

NEDirectionalLightFactory::~NEDirectionalLightFactory()
{
    if( NEBASE.guiMode() )
    {
        m_pMesh->clearDrawingBuffers();
        delete m_pMesh;
    }
}

QString NEDirectionalLightFactory::nodeName()
{
    return "Directional Light";
}

uint NEDirectionalLightFactory::nodeType()
{
    return NE::kObjectLight;
}

QString NEDirectionalLightFactory::nodeInherits()
{
    return "Lights";
}

QString NEDirectionalLightFactory::objectName()
{
    return "DirectionalLight";
}

QString NEDirectionalLightFactory::nodeVersion()
{
    return "0.1";
}

QString NEDirectionalLightFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NEDirectionalLightFactory::nodeHelp()
{
    return "Directional light";
}

NEObject* NEDirectionalLightFactory::createInstance()
{
    return new NEDirectionalLight(this);
}

const NEIndexedTriMesh* NEDirectionalLightFactory::mesh()
{
    return m_pMesh;
}

NEDirectionalLight::NEDirectionalLight(NENodeFactory* factory) : NELight(factory)
{
    P = new NEDirectionalLightPrivate(this);
    CALL_ONCE(initProperties);
    SetPosition(vec3f(3, 1, 1));
    SetDirection(NE_DEFAULT(Direction));
}

NEDirectionalLight::~NEDirectionalLight()
{
    delete P;
}

uint NEDirectionalLight::init(uint initMode)
{
    NESpatial::init(initMode);
    P->m_Mesh = const_cast<NEIndexedTriMesh*>(((NEDirectionalLightFactory&)factory()).mesh());
    return NE::kReturnSuccess;

}

void NEDirectionalLight::render()
{

}

void NEDirectionalLight::renderPicked()
{

}

void NEDirectionalLight::renderPickable()
{

}

const vec3f& NEDirectionalLight::Direction() const
{
    return P->m_Direction;
}

const NEIndexedTriMesh* NEDirectionalLight::mesh() const
{
    return P->m_Mesh;
}

void NEDirectionalLight::updateUniforms(IGLSurface* surface, QOpenGLShaderProgram* program, int lightID, bool isActive)
{
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].color").toUtf8().constData(), LightColor());
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].intensity").toUtf8().constData(), (float) LightIntensity());

    // Map light direction from world space to eye space before passing to the shader
    matrix44f cm = surface->camera()->viewMatrix();
    vec3f dirMapped = cm.mapVector(P->m_NormalizedDirection);
    QVector3D* dirQVec = reinterpret_cast<QVector3D*>(&dirMapped);
    // Texture 0 and 1 for standard material
    int shadowTexId = MAX_MATERIAL_TEXTURES + lightID;
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].isactive").toUtf8().constData(), isActive);
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].dir").toUtf8().constData(), *dirQVec);

    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].shadowMap").toUtf8().constData(), shadowTexId);
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( program->programId(), QString("DirLights[" + QString::number(lightID) + "].DepthBiasMVP").toUtf8().constData()),
                        1, GL_FALSE, getDepthBiasMVP().data() );
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].castingShadow").toUtf8().constData(), (int)ShadowMode());

    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].shadowColor").toUtf8().constData(), this->ShadowColor() );
    GL->glUniformMatrix4fv( GL->glGetUniformLocation( program->programId(), QString("DirLights[" + QString::number(lightID) + "].lightView").toUtf8().constData()),
                        1, GL_FALSE, getViewMatrix().data() );
    GL->glUniform2f(GL->glGetUniformLocation( program->programId(), QString("DirLights[" + QString::number(lightID) + "].lightRadiusUV").toUtf8().constData()),getLightRadiusUV().x(),getLightRadiusUV().y());
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].lightZNear").toUtf8().constData(), getNear());
    program->setUniformValue(QString("DirLights[" + QString::number(lightID) + "].lightZFar").toUtf8().constData(), getFar());

    GL->glActiveTexture(GL_TEXTURE0 + shadowTexId);
    bindShadowTexture();
}

uint NEDirectionalLight::SetDirection(const vec3f& val)
{
    if(P->m_Direction == val)
        return NE::kReturnSuccess;
    P->m_Direction = val;
    P->m_NormalizedDirection = P->m_Direction.normalized();
    NESpatial::SetOrientation(eulerAnglesFromDirection(-P->m_NormalizedDirection));
    return NE::kReturnSuccess;
}

uint NEDirectionalLight::SetOrientation(const eulerf& rotation)
{
    NESpatial::SetOrientation(rotation);
    P->m_NormalizedDirection = -directionFromEulerAngles(Orientation());
    P->m_Direction = P->m_Direction.length()*P->m_NormalizedDirection;
    return NE::kReturnSuccess;
}

void NEDirectionalLight::propertyTweak(QtPropertyBase* p)
{
    //hideProperty("Pivot", true);
    //hideProperty("Position", true);
    //hideProperty("Scale", true);

    NELight::propertyTweak(p);
}
