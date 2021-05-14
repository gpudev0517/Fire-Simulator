#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"
#include "NEOSDParticleFluid.h"
#include "Base/NEBase.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Base/NEGUICallInterface.h"

class NEOSDParticleFluidPrivate
{
public:
    NEOSDParticleFluidPrivate(NEOSDParticleFluid* s) : S(s)
    {
    }
    int m_TickCount;
    QString m_TitleText;
    NEOSDParticleFluid::NEOSDOrientation m_OSDOrientation;

private:
    NEOSDParticleFluid* S;
};


NE_PROP_DEF(int, NEOSDParticleFluid, TickCount, 5)
NE_PROP_DEF(QString, NEOSDParticleFluid, TitleText, QString(""))
NE_PROP_DEF(NEOSDParticleFluid::NEOSDOrientation, NEOSDParticleFluid, OSDOrientation, NEOSDParticleFluid::NEOSDOrientation::Left)

static void initProperties()
{
    NE_INIT_PROP(NEOSDParticleFluid, TickCount);
    NE_INIT_PROP(NEOSDParticleFluid, TitleText);
    NE_INIT_PROP(NEOSDParticleFluid, OSDOrientation);
}

NEOSDParticleFluid::NEOSDParticleFluid(NENodeFactory* factory): NEOSDBase( factory )
{
    P = new NEOSDParticleFluidPrivate(this);
    CALL_ONCE(initProperties);

    m_ParticleFluidSolver = NULL;
    P->m_TickCount = NE_DEFAULT(TickCount);
    P->m_TitleText = NE_DEFAULT(TitleText);
    P->m_OSDOrientation = NE_DEFAULT(OSDOrientation);

    setInputs( inputs()|NE::kIOParticles );

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

NEOSDParticleFluid::~NEOSDParticleFluid()
{
    if( NEBASE.guiMode() )
        delete m_ColorBarProgram;

    delete P;
}

bool NEOSDParticleFluid::addSource( NENode *node, NEConn &conn )
{
    switch( conn.type )
    {
    case NE::kIOParticles:
    {
        NEParticleFluidSolver* pfluidSolver = qobject_cast< NEParticleFluidSolver* >( node );
        if (pfluidSolver)
        {
            m_ParticleFluidSolver = pfluidSolver;
            if( NEBASE.guiMode() )
                NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
        }
    }
        break;
    default:
        return false;
    }
    return NENode::addSource( node, conn );
}


void NEOSDParticleFluid::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    switch( type )
    {
    case NE::kIOParticles:
        m_ParticleFluidSolver = 0;
        if( NEBASE.guiMode() )
            NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
        break;
    default:
        return;
    }
    NENode::removeSource( node, type, sIndex, tIndex );
}

bool NEOSDParticleFluid::isManipulatable(NEManipulator manipType)
{
    if (manipType == kManipTranslate || manipType == kManipScale)
        return true;
    return false;
}

const NEParticleFluidSolver* NEOSDParticleFluid::particleFluidSolver() const
{
    return m_ParticleFluidSolver;
}

const int& NEOSDParticleFluid::TickCount() const
{
    return P->m_TickCount;
}

uint NEOSDParticleFluid::SetTickCount(const int& val)
{
    NE_PROP_SET_AND_EVAL(TickCount, P, val);
    return NE::kReturnSuccess;
}

const QString& NEOSDParticleFluid::TitleText() const
{
    return P->m_TitleText;
}

uint NEOSDParticleFluid::SetTitleText(const QString& val)
{
    NE_PROP_SET_AND_EVAL(TitleText, P, val);
    return NE::kReturnSuccess;
}

const NEOSDParticleFluid::NEOSDOrientation& NEOSDParticleFluid::OSDOrientation() const
{
    return P->m_OSDOrientation;
}

uint NEOSDParticleFluid::SetOSDOrientation(const NEOSDOrientation& val)
{
    NE_PROP_SET_EVAL(OSDOrientation, P, val);
    return NE::kReturnSuccess;
}

vec2f NEOSDParticleFluid::osdSize()
{
    if (!m_ParticleFluidSolver) return NEOSDBase::osdSize();
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

            float minVal = m_ParticleFluidSolver->MinColoringRange();
            float maxVal = m_ParticleFluidSolver->MaxColoringRange();

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

QOpenGLShaderProgram* NEOSDParticleFluid::getColorBarProgram()
{
    return m_ColorBarProgram;
}

void NEOSDParticleFluid::resetProperties()
{
    NEOSDBase::resetProperties();
    ResetTickCount();
    ResetTitleText();
    ResetOSDOrientation();
}


NEOSDParticleFluidFactory::NEOSDParticleFluidFactory(NEManager* m) : NENodeFactory(m) {}

NEOSDParticleFluidFactory::~NEOSDParticleFluidFactory() {}

QString NEOSDParticleFluidFactory::nodeName() { return "Particle Fluid OSD"; }

uint NEOSDParticleFluidFactory::nodeType(){ return NE::kObjectOSD;}

QString NEOSDParticleFluidFactory::nodeInherits() { return "OSDs";}

QString NEOSDParticleFluidFactory::objectName() { return "ParticleFluidOSD"; }

QString NEOSDParticleFluidFactory::nodeVersion() { return "0.1"; }

QString NEOSDParticleFluidFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOSDParticleFluidFactory::nodeHelp() { return "Particle Fluid OSD"; }

NEObject*NEOSDParticleFluidFactory::createInstance() { return new NEOSDParticleFluid(this); }

