#if NE_VOLUME
//#include "Geometry/Volume/Physics/Spatial/NEVolume.h"
#include "Volume/Core/NEVolumeNode.h"
#endif
#include "Base/NEGUICallInterface.h"
#include "NEOSDVolume.h"

#include "Base/NEBase.h"
#include "Rendering/Manager/NEGLManager.h"


class NEOSDVolumePrivate
{
public:
    NEOSDVolumePrivate(NEOSDVolume* s) : S(s)
    {
    }
    int m_TickCount;
    QString m_TitleText;
    NEOSDVolume::NEOSDOrientation m_OSDOrientation;

private:
    NEOSDVolume* S;
};

NE_PROP_DEF(int, NEOSDVolume, TickCount, 5)
NE_PROP_DEF(QString, NEOSDVolume, TitleText, QString(""))
NE_PROP_DEF(NEOSDVolume::NEOSDOrientation, NEOSDVolume, OSDOrientation, NEOSDVolume::NEOSDOrientation::Left)

static void initProperties()
{
    NE_INIT_PROP(NEOSDVolume, TickCount);
    NE_INIT_PROP(NEOSDVolume, TitleText);
    NE_INIT_PROP(NEOSDVolume, OSDOrientation);
}

NEOSDVolume::NEOSDVolume( NENodeFactory *factory ): NEOSDBase( factory )
{
    P = new NEOSDVolumePrivate(this);
    CALL_ONCE(initProperties);
    m_volume = NULL;
    P->m_TickCount = NE_DEFAULT(TickCount);
    P->m_TitleText = NE_DEFAULT(TitleText);
    P->m_OSDOrientation = NE_DEFAULT(OSDOrientation);

    setInputs( inputs()|NE::kIOVolume );

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

NEOSDVolume::~NEOSDVolume()
{
    if( NEBASE.guiMode() )
        delete m_ColorBarProgram;
}

bool NEOSDVolume::addSource( NENode *node, NEConn &conn )
{
    switch( conn.type )
    {
    case NE::kIOVolume:
        if( NEVolumeNode* pVolume = qobject_cast< NEVolumeNode* >( node ) )
        {
            //check if connection already exists
            if (m_volume)
            {
                for(const NEConn& eraseConn : sourceConnections())
                {
                    if( eraseConn.type == NE::kIOVolume &&
                        (qobject_cast<NEVolumeNode*> (eraseConn.source) ) )
                    {
                        eraseConn.target->disconnectFrom( eraseConn.source, NE::kIOVolume, eraseConn.sIndex, eraseConn.tIndex );
                    }
                }
            }

            m_volume = pVolume;
            if( NEBASE.guiMode() )
                NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
        }
        break;
    default:
        return false;
    }
    return NENode::addSource( node, conn );
}


void NEOSDVolume::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    switch( type )
    {
    case NE::kIOVolume:
        m_volume = 0;
        if( NEBASE.guiMode() )
            NEBASE.getGUICallInterface()->updateGLWindowsFromBase();
        break;
    default:
        return;
    }
    NENode::removeSource( node, type, sIndex, tIndex );
}

bool NEOSDVolume::isManipulatable(NEManipulator manipType)
{
    if (manipType == kManipTranslate || manipType == kManipScale)
        return true;
    return false;
}

NESpatial* NEOSDVolume::Volume()
{
    return m_volume;
}

const int& NEOSDVolume::TickCount() const
{
    return P->m_TickCount;
}

uint NEOSDVolume::SetTickCount(const int& val)
{
    NE_PROP_SET_AND_EVAL(TickCount, P, val);
    return NE::kReturnSuccess;
}

const QString& NEOSDVolume::TitleText() const
{
    return P->m_TitleText;
}

QString NEOSDVolume::getTitleText()
{
    return TitleText();
}

uint NEOSDVolume::SetTitleText(const QString& val)
{
    NE_PROP_SET_AND_EVAL(TitleText, P, val);
    return NE::kReturnSuccess;
}

const NEOSDVolume::NEOSDOrientation& NEOSDVolume::OSDOrientation() const
{
    return P->m_OSDOrientation;
}

uint NEOSDVolume::SetOSDOrientation(const NEOSDOrientation& val)
{
    NE_PROP_SET_EVAL(OSDOrientation, P, val);
    return NE::kReturnSuccess;
}

vec2f NEOSDVolume::osdSize()
{
    if (!m_volume) return NEOSDBase::osdSize();
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

            float minVal = 0;//m_volume->MinColoringRange();
            float maxVal = 1;//m_volume->MaxColoringRange();

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

QOpenGLShaderProgram* NEOSDVolume::getColorBarProgram()
{
    return m_ColorBarProgram;
}

void NEOSDVolume::resetProperties()
{
    NEOSDBase::resetProperties();
    ResetTickCount();
    ResetTitleText();
    ResetOSDOrientation();
}

NEOSDVolumeFactory::NEOSDVolumeFactory(NEManager* m) : NENodeFactory(m) {}

NEOSDVolumeFactory::~NEOSDVolumeFactory() {}

QString NEOSDVolumeFactory::nodeName() { return "Volume OSD"; }

uint NEOSDVolumeFactory::nodeType(){ return NE::kObjectOSD;}

QString NEOSDVolumeFactory::nodeInherits() { return "OSDs";}

QString NEOSDVolumeFactory::objectName() { return "VolumeOSD"; }

QString NEOSDVolumeFactory::nodeVersion() { return "0.1"; }

QString NEOSDVolumeFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOSDVolumeFactory::nodeHelp() { return "Volume OSD"; }

NEObject*NEOSDVolumeFactory::createInstance() { return new NEOSDVolume(this); }
