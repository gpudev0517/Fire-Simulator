#include "Physics/Spatial/Physical/Rigid/NERigidTopography.h"
#include "NEOSDRigidTopography.h"
#include "Base/NEBase.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Base/NEGUICallInterface.h"

class NEOSDRigidTopographyPrivate
{
public:
    NEOSDRigidTopographyPrivate(NEOSDRigidTopography* s) : S(s)
    {
    }
    int m_TickCount;
    QString m_TitleText;
    NEOSDRigidTopography::NEOSDOrientation m_OSDOrientation;

private:
    NEOSDRigidTopography* S;
};


NE_PROP_DEF(int, NEOSDRigidTopography, TickCount, 5)
NE_PROP_DEF(QString, NEOSDRigidTopography, TitleText, QString(""))
NE_PROP_DEF(NEOSDRigidTopography::NEOSDOrientation, NEOSDRigidTopography, OSDOrientation, NEOSDRigidTopography::NEOSDOrientation::Left)

static void initProperties()
{
    NE_INIT_PROP(NEOSDRigidTopography, TickCount);
    NE_INIT_PROP(NEOSDRigidTopography, TitleText);
    NE_INIT_PROP(NEOSDRigidTopography, OSDOrientation);
}

NEOSDRigidTopography::NEOSDRigidTopography(NENodeFactory* factory): NEOSDBase( factory )
{
    P = new NEOSDRigidTopographyPrivate(this);
    CALL_ONCE(initProperties);

    m_rigidTopography = NULL;
    P->m_TickCount = NE_DEFAULT(TickCount);
    P->m_TitleText = NE_DEFAULT(TitleText);
    P->m_OSDOrientation = NE_DEFAULT(OSDOrientation);

    setInputs( inputs()|NE::kIOTriangleMesh );

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

NEOSDRigidTopography::~NEOSDRigidTopography()
{
    if( NEBASE.guiMode() )
        delete m_ColorBarProgram;

    delete P;
}

bool NEOSDRigidTopography::addSource( NENode *node, NEConn &conn )
{
    switch( conn.type )
    {
    case NE::kIOTriangleMesh:
        {
            NERigidTopography* pTopography = qobject_cast< NERigidTopography* >( node );
            if (pTopography)
            {
                //check if connection already exists
                if (m_rigidTopography)
                {
                    for(const NEConn& eraseConn : sourceConnections())
                    {
                        if( eraseConn.type == NE::kIOTriangleMesh &&
                            qobject_cast<NERigidTopography*> (eraseConn.source))
                        {
                            eraseConn.target->disconnectFrom( eraseConn.source, NE::kIOTriangleMesh, eraseConn.sIndex, eraseConn.tIndex );
                        }
                    }
                }

                m_rigidTopography = pTopography;
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


void NEOSDRigidTopography::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    switch( type )
    {
    case NE::kIOTriangleMesh:
        m_rigidTopography = 0;
        if( NEBASE.guiMode() )
            NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
        break;
    default:
        return;
    }
    NENode::removeSource( node, type, sIndex, tIndex );
}

bool NEOSDRigidTopography::isManipulatable(NEManipulator manipType)
{
    if (manipType == kManipTranslate || manipType == kManipScale)
        return true;
    return false;
}

const NERigidTopography* NEOSDRigidTopography::rigidTopography() const
{
    return m_rigidTopography;
}

const int& NEOSDRigidTopography::TickCount() const
{
    return P->m_TickCount;
}

uint NEOSDRigidTopography::SetTickCount(const int& val)
{
    NE_PROP_SET_AND_EVAL(TickCount, P, val);
    return NE::kReturnSuccess;
}

const QString& NEOSDRigidTopography::TitleText() const
{
    return P->m_TitleText;
}

uint NEOSDRigidTopography::SetTitleText(const QString& val)
{
    NE_PROP_SET_AND_EVAL(TitleText, P, val);
    return NE::kReturnSuccess;
}

const NEOSDRigidTopography::NEOSDOrientation& NEOSDRigidTopography::OSDOrientation() const
{
    return P->m_OSDOrientation;
}

uint NEOSDRigidTopography::SetOSDOrientation(const NEOSDOrientation& val)
{
    NE_PROP_SET_EVAL(OSDOrientation, P, val);
    return NE::kReturnSuccess;
}

vec2f NEOSDRigidTopography::osdSize()
{
    if (!m_rigidTopography) return NEOSDBase::osdSize();
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

            float minVal = m_rigidTopography->MinColoringRange();
            float maxVal = m_rigidTopography->MaxColoringRange();

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

QOpenGLShaderProgram* NEOSDRigidTopography::getColorBarProgram()
{
    return m_ColorBarProgram;
}

void NEOSDRigidTopography::resetProperties()
{
    NEOSDBase::resetProperties();
    ResetTickCount();
    ResetTitleText();
    ResetOSDOrientation();
}


NEOSDRigidTopographyFactory::NEOSDRigidTopographyFactory(NEManager* m) : NENodeFactory(m) {}

NEOSDRigidTopographyFactory::~NEOSDRigidTopographyFactory() {}

QString NEOSDRigidTopographyFactory::nodeName() { return "Rigid Topography OSD"; }

uint NEOSDRigidTopographyFactory::nodeType(){ return NE::kObjectOSD;}

QString NEOSDRigidTopographyFactory::nodeInherits() { return "OSDs";}

QString NEOSDRigidTopographyFactory::objectName() { return "RigidTopographyOSD"; }

QString NEOSDRigidTopographyFactory::nodeVersion() { return "0.1"; }

QString NEOSDRigidTopographyFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOSDRigidTopographyFactory::nodeHelp() { return "Rigid Topography OSD"; }

NEObject*NEOSDRigidTopographyFactory::createInstance() { return new NEOSDRigidTopography(this); }

