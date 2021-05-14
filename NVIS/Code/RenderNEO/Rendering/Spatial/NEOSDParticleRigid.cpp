#include "Physics/Spatial/Physical/Rigid/NEParticleRigid.h"
#include "NEOSDParticleRigid.h"
#include "Base/NEBase.h"
#include "Base/NEGUICallInterface.h"

class NEOSDParticleRigidPrivate
{
public:
    NEOSDParticleRigidPrivate(NEOSDParticleRigid* s) : S(s)
    {
    }
    int m_TickCount;
    QString m_TitleText;
    NEOSDParticleRigid::NEOSDOrientation m_OSDOrientation;

private:
    NEOSDParticleRigid* S;
};


NE_PROP_DEF(int, NEOSDParticleRigid, TickCount, 5)
NE_PROP_DEF(QString, NEOSDParticleRigid, TitleText, QString(""))
NE_PROP_DEF(NEOSDParticleRigid::NEOSDOrientation, NEOSDParticleRigid, OSDOrientation, NEOSDParticleRigid::NEOSDOrientation::Left)

static void initProperties()
{
    NE_INIT_PROP(NEOSDParticleRigid, TickCount);
    NE_INIT_PROP(NEOSDParticleRigid, TitleText);
    NE_INIT_PROP(NEOSDParticleRigid, OSDOrientation);
}


NEOSDParticleRigid::NEOSDParticleRigid(NENodeFactory* factory): NEOSDBase( factory )
{

    P = new NEOSDParticleRigidPrivate(this);
    CALL_ONCE(initProperties);

    m_particleRigid = NULL;
    P->m_TickCount = NE_DEFAULT(TickCount);
    P->m_TitleText = NE_DEFAULT(TitleText);
    P->m_OSDOrientation = NE_DEFAULT(OSDOrientation);

    setInputs( inputs()|NE::kIORigid );

    if( NEBASE.guiMode() )
    {
        m_ColorBarProgram = new QOpenGLShaderProgram();
        QString vertexPath = "System/Shaders/OSD/colorbar.vert";
        QString fragmentPath = "System/Shaders/OSD/colorbar.frag";
        QString coloringFragmentPath = "System/Shaders/Default/coloring.frag";
        if(!m_ColorBarProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath))
        {
            qWarning() << vertexPath <<  " " << m_ColorBarProgram->log();
            std::cerr << vertexPath.toStdString() << " " << m_ColorBarProgram->log().toStdString() << std::endl;
        }
        if(!m_ColorBarProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath))
        {
            qWarning() <<  fragmentPath << " " << m_ColorBarProgram->log() ;
            std::cerr << fragmentPath.toStdString() << " " << m_ColorBarProgram->log().toStdString() << std::endl;
        }
        if(!m_ColorBarProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, coloringFragmentPath))
        {
            qWarning() <<  coloringFragmentPath << " " << m_ColorBarProgram->log() ;
            std::cerr << coloringFragmentPath.toStdString() << " " << m_ColorBarProgram->log().toStdString() << std::endl;
        }
    }
}

NEOSDParticleRigid::~NEOSDParticleRigid()
{
    if( NEBASE.guiMode() )
        delete m_ColorBarProgram;

    delete P;
}

bool NEOSDParticleRigid::addSource( NENode *node, NEConn &conn )
{
    switch( conn.type )
    {
    case NE::kIORigid:
        if( NEParticleRigid* prigid = qobject_cast< NEParticleRigid* >( node ) )
	    {
            m_particleRigid = prigid;
            if( NEBASE.guiMode() )
                NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
	    }
	    break;
    }
    return NENode::addSource( node, conn );
}


void NEOSDParticleRigid::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    switch( type )
    {
    case NE::kIORigid:
        m_particleRigid = 0;
        if( NEBASE.guiMode() )
            NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
        break;
    default:
        return;
    }
    NENode::removeSource( node, type, sIndex, tIndex );
}
bool NEOSDParticleRigid::isManipulatable(NEManipulator manipType)
{
    if (manipType == kManipTranslate || manipType == kManipScale)
        return true;
    return false;
}

const NEParticleRigid* NEOSDParticleRigid::particleRigid() const
{
    return m_particleRigid;
}

const int& NEOSDParticleRigid::TickCount() const
{
    return P->m_TickCount;
}

uint NEOSDParticleRigid::SetTickCount(const int& val)
{
    NE_PROP_SET_AND_EVAL(TickCount, P, val);
    return NE::kReturnSuccess;
}

const QString& NEOSDParticleRigid::TitleText() const
{
    return P->m_TitleText;
}

uint NEOSDParticleRigid::SetTitleText(const QString& val)
{
    NE_PROP_SET_AND_EVAL(TitleText, P, val);
    return NE::kReturnSuccess;
}

const NEOSDParticleRigid::NEOSDOrientation& NEOSDParticleRigid::OSDOrientation() const
{
    return P->m_OSDOrientation;
}

uint NEOSDParticleRigid::SetOSDOrientation(const NEOSDOrientation& val)
{
    NE_PROP_SET_EVAL(OSDOrientation, P, val);
    return NE::kReturnSuccess;
}

vec2f NEOSDParticleRigid::osdSize()
{
    if (!m_particleRigid) return NEOSDBase::osdSize();
    vec2f rectSize;

    switch(OSDOrientation())
    {
    case NEOSDOrientation::Left:
    case NEOSDOrientation::Right:
        {
            int wRes = OSD_PARTICLE_COLORBAR_WIDTH * Scalex(); //!< Width of color bar
            int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * Scaley();
            int hTitle = fontHeight() * 2;
            int wMargin = OSD_PARTICLE_MEASURE_MARGIN * Scalex(); //!< Width of margin between color bar and measure
            int widthU = 0; //!< Maximize size of text width

            float minVal = m_particleRigid->MinColoringRange();
            float maxVal = m_particleRigid->MaxColoringRange();

            for (int i = 0; i <= TickCount(); i++)
            {
                float s = i / (float)TickCount();
                float val = minVal * (1-s) + maxVal * s;
                QString text;
                if( DisplayNotation() == NEOSDBase::Exponent )
                {
                    text = QString("%1").arg( (double)val, 2, 'E', 2 );
                }
                else if( DisplayNotation() == NEOSDBase::FloatingPoint )
                {
                    text = QString("%1").arg( (double)val, TotalNumberLength(), 'f', FloatTypePrecision() );
                }
                widthU = MAX(widthU, (float)text.length() * fontWidth() + fontWidth());
            }

            rectSize = vec2f(wRes + wMargin + widthU, hRes + fontHeight() + hTitle);
        }
        break;
    case NEOSDOrientation::Top:
    case NEOSDOrientation::Bottom:
        {
            int wRes = OSD_PARTICLE_COLORBAR_HEIGHT * Scalex(); //!< Width of color bar
            int hRes = OSD_PARTICLE_COLORBAR_WIDTH * Scaley();
            int hTitle = fontHeight() * 2;
            int hMargin = OSD_PARTICLE_MEASURE_MARGIN * Scaley(); //!< Height of margin between color bar and measure
            int widthU = 0; //!< Maximize size of text width

            rectSize = vec2f(wRes, hTitle + hRes + hMargin + fontHeight());
        }
        break;
    default:
        rectSize = NEOSDBase::osdSize();
        break;
    }

    return rectSize;
}

QOpenGLShaderProgram* NEOSDParticleRigid::getColorBarProgram()
{
    return m_ColorBarProgram;
}

void NEOSDParticleRigid::resetProperties()
{
    NEOSDBase::resetProperties();
    ResetTickCount();
    ResetTitleText();
    ResetOSDOrientation();
}


NEOSDParticleRigidFactory::NEOSDParticleRigidFactory(NEManager* m) : NENodeFactory(m) {}

NEOSDParticleRigidFactory::~NEOSDParticleRigidFactory() {}

QString NEOSDParticleRigidFactory::nodeName() { return "Particle Rigid OSD"; }

uint NEOSDParticleRigidFactory::nodeType(){ return NE::kObjectOSD;}

QString NEOSDParticleRigidFactory::nodeInherits() { return "OSDs";}

QString NEOSDParticleRigidFactory::objectName() { return "ParticleRigidOSD"; }

QString NEOSDParticleRigidFactory::nodeVersion() { return "0.1"; }

QString NEOSDParticleRigidFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOSDParticleRigidFactory::nodeHelp() { return "Particle Rigid OSD"; }

NEObject*NEOSDParticleRigidFactory::createInstance() { return new NEOSDParticleRigid(this); }
