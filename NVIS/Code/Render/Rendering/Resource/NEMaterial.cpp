#include "Base/NEBase.h"
#include "Spatial/NESpatial.h"
#include "Mesh/NEMeshConstructor.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Resource/NEMaterial.h"



class NEMaterialPrivate
{
public:
    NEMaterialPrivate(NEMaterial* s) : S(s)
    {

    }

    //! Returns on if val = true else returns off as a QString
    QString stateFromBool(const bool val) const;

    bool m_PerPixel;

    QColor m_Color; //diffuse color
    QColor m_Ambient;
    QColor m_Specular;
    QColor m_Ambiance;

    double m_KAmbient;
    double m_KDiffuse;
    double m_KSpecular;


    QOpenGLShaderProgram* m_pShaderProgram;
    QOpenGLShader *m_pDefaultFS;



private:
    NEMaterial* S;

};

NE_PROP_DEF(bool, NEMaterial, PerPixel, true)
NE_PROP_DEF(QColor, NEMaterial, Color, QColor(255, 255, 255, 255))
NE_PROP_DEF(double, NEMaterial, KAmbient, 0.1)
NE_PROP_DEF(double, NEMaterial, KDiffuse, 1.0)
NE_PROP_DEF(double, NEMaterial, KSpecular, 0.5)
NE_PROP_DEF(QColor, NEMaterial, Ambient, QColor(128, 128, 128, 255))
NE_PROP_DEF(QColor, NEMaterial, Specular, QColor(255, 255, 255, 255))
NE_PROP_DEF(QColor, NEMaterial, Ambiance, QColor(85, 85, 85, 255))


static void initProperties()
{
    NE_INIT_PROP(NEMaterial, PerPixel);
    NE_INIT_PROP(NEMaterial, Color);
    NE_INIT_PROP(NEMaterial, KAmbient);
    NE_INIT_PROP(NEMaterial, KDiffuse);
    NE_INIT_PROP(NEMaterial, KSpecular);
    NE_INIT_PROP(NEMaterial, Ambient);
    NE_INIT_PROP(NEMaterial, Specular);
    NE_INIT_PROP(NEMaterial, Ambiance);
}


NEMaterial::NEMaterial(NENodeFactory* factory) : NEResource(factory)
{
    addNodeTypeToNodeGraphSceneType("Materials", NENode::SceneType::Visualization );

    P = new NEMaterialPrivate(this);
    CALL_ONCE(initProperties);

    P->m_PerPixel = NE_DEFAULT(PerPixel);
    P->m_Color = NE_DEFAULT(Color);
    P->m_KAmbient = NE_DEFAULT(KAmbient);
    P->m_KDiffuse = NE_DEFAULT(KDiffuse);
    P->m_KSpecular = NE_DEFAULT(KSpecular);
    P->m_Ambient = NE_DEFAULT(Ambient);
    P->m_Specular = NE_DEFAULT(Specular);
    P->m_Ambiance = NE_DEFAULT(Ambiance);

    P->m_pShaderProgram = 0;
    P->m_pDefaultFS = 0;
    setInputs(inputs() | NE::kIOTriangleMesh);
}

NEMaterial::~NEMaterial()
{
    if (P){
        if(P->m_pShaderProgram){
            P->m_pShaderProgram->removeAllShaders();
            P->m_pShaderProgram->release();
            delete P->m_pShaderProgram;
            P->m_pShaderProgram = 0;

            delete P->m_pDefaultFS;
            P->m_pDefaultFS = 0;
        }
        delete P;
        P = NULL;
    }

}

void NEMaterial::updateUniforms()
{
    if( NEBASE.guiMode() )
    {
        P->m_pShaderProgram->setUniformValue("kAmbient", (float)KAmbient());
        P->m_pShaderProgram->setUniformValue("kDiffuse", (float)KDiffuse());
        P->m_pShaderProgram->setUniformValue("kSpecular", (float)KSpecular());
        P->m_pShaderProgram->setUniformValue("Color", Color());
        P->m_pShaderProgram->setUniformValue("specColor", Specular());
    }
}

NEMaterial::MaterialType NEMaterial::materialType()
{
    return NEMaterial::Default;
}

const bool& NEMaterial::PerPixel() const
{
    return P->m_PerPixel;
}

uint NEMaterial::SetPerPixel(const bool& val)
{
    NE_PROP_SET_AND_EVAL(PerPixel, P, val);
    return NE::kReturnSuccess;
}

const QColor& NEMaterial::Color() const
{
    return P->m_Color;
}

uint NEMaterial::SetColor(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(Color, P, val);
    return NE::kReturnSuccess;
}

const QColor& NEMaterial::Ambient() const
{
    return P->m_Ambient;
}

uint NEMaterial::SetAmbient(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(Ambient, P, val);
    return NE::kReturnSuccess;
}

const QColor& NEMaterial::Specular() const
{
    return P->m_Specular;
}

uint NEMaterial::SetSpecular(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(Specular, P, val);
    return NE::kReturnSuccess;
}

const QColor& NEMaterial::Ambiance() const
{
    return P->m_Ambiance;
}

uint NEMaterial::SetAmbiance(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(Ambiance, P, val);
    return NE::kReturnSuccess;
}

const double& NEMaterial::KAmbient() const
{
    return P->m_KAmbient;
}

uint NEMaterial::SetKAmbient(const double& val)
{
    NE_PROP_SET_AND_EVAL(KAmbient, P, val);
    return NE::kReturnSuccess;
}

const double& NEMaterial::KDiffuse() const
{
    return P->m_KDiffuse;
}

uint NEMaterial::SetKDiffuse(const double& val)
{
    NE_PROP_SET_AND_EVAL(KDiffuse, P, val);
    return NE::kReturnSuccess;
}

const double& NEMaterial::KSpecular() const
{
    return P->m_KSpecular;
}

uint NEMaterial::SetKSpecular(const double& val)
{
    NE_PROP_SET_AND_EVAL(KSpecular, P, val);
    return NE::kReturnSuccess;
}

QOpenGLShaderProgram* NEMaterial::shaderProgram() const
{
    return P->m_pShaderProgram;
}

QString NEMaterialPrivate::stateFromBool(const bool val) const
{
    if(val) return "on";
    else return "off";
}

void NEMaterial::setShaderProgram(QOpenGLShaderProgram* pShaderProgram)
{
    P->m_pShaderProgram = pShaderProgram;
}

void NEMaterial::setDefaultFS(QOpenGLShader *shader)
{
    P->m_pDefaultFS = shader;
}

QOpenGLShader* NEMaterial::defaultFS()
{
    return P->m_pDefaultFS;
}

bool NEMaterial::addSource(NENode *node, NENode::NEConn &conn)
{
    NESpatial* rigidSource = qobject_cast< NESpatial *>( node );

    if ( rigidSource )
    {
        const NEIndexedTriMesh& mesh = rigidSource->triangleMesh();
        if( NERENDERMAN.GLManager()->material(  const_cast< NEIndexedTriMesh*>( &mesh ) ) )
        {
            // Check if connection already exists
            for(const NEConn& eraseConn : node->connections())
            {
                if( eraseConn.type == NE::kIOTriangleMesh &&
                        eraseConn.target != this &&
                        qobject_cast<NEMaterial*> (eraseConn.target))
                {
                    eraseConn.target->disconnectFrom( eraseConn.source, NE::kIOTriangleMesh, eraseConn.sIndex, eraseConn.tIndex );
                }
            }
        }

        NERENDERMAN.GLManager()->setMaterial( const_cast< NEIndexedTriMesh*>( &mesh ), this );
        return NENode::addSource( node, conn );
    }

    NEMeshConstructor* meshConstSource = qobject_cast< NEMeshConstructor *>( node );

    if ( meshConstSource )
    {
        const NEIndexedTriMesh& mesh = meshConstSource->triangleMesh();
        if( NERENDERMAN.GLManager()->material(  const_cast< NEIndexedTriMesh*>( &mesh ) ) )
        {

            for(const NEConn& eraseConn : node->connections())
            {
                if( eraseConn.type == NE::kIOTriangleMesh &&
                        eraseConn.target != this &&
                        qobject_cast<NEMaterial*> (eraseConn.target))
                {
                    eraseConn.target->disconnectFrom( eraseConn.source, NE::kIOTriangleMesh, eraseConn.sIndex, eraseConn.tIndex );
                }
            }
        }
        //else
        //    std::cout << "Material: " << meshConstSource->objectName().toStdString() << " " << std::endl;
        NERENDERMAN.GLManager()->setMaterial( const_cast< NEIndexedTriMesh*>( &mesh ), this );
        if( NERENDERMAN.GLManager()->material(  const_cast< NEIndexedTriMesh*>( &mesh ) ) )
        {
            //std::cout << "Rigid Source has material sss  " << NERENDERMAN.GLManager()->material( const_cast< NEIndexedTriMesh*>( &mesh ) )->objectName().toStdString() << std::endl;
        }
        return NENode::addSource( node, conn );
    }

    return false;
}

void NEMaterial::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    //std::cout << "here " << std::endl;
    NESpatial* rigidSource = qobject_cast< NESpatial *>( node );
    if ( rigidSource )
    {
        const NEIndexedTriMesh& mesh = rigidSource->triangleMesh();
        NERENDERMAN.GLManager()->setMaterial( const_cast< NEIndexedTriMesh*>( &mesh ), NULL );
        NENode::removeSource( node, type, sIndex, tIndex );
    }
    NEMeshConstructor* meshConstSource = qobject_cast< NEMeshConstructor *>( node );
    if ( meshConstSource )
    {
        const NEIndexedTriMesh& mesh = meshConstSource->triangleMesh();
        NERENDERMAN.GLManager()->setMaterial( const_cast< NEIndexedTriMesh*>( &mesh ), NULL );
        NENode::removeSource( node, type, sIndex, tIndex );
    }

    return;
}

