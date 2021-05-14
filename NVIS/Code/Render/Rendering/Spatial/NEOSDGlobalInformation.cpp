#include "Base/NESceneManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "NEOSDGlobalInformation.h"


// Installs properties, their default values, and generates property reset functions
// Properties should always be passed with their names capitilized

class NEOSDGlobalInformationPrivate
{
public:
    NEOSDGlobalInformationPrivate(NEOSDGlobalInformation* s);

    QString m_AlternativeText;

private:
    NEOSDBase* S;
};


NE_PROP_DEF(QString, NEOSDGlobalInformation, AlternativeText, "")

static void initProperties()
{
    NE_INIT_PROP(NEOSDGlobalInformation, AlternativeText);
}

NEOSDGlobalInformationPrivate::NEOSDGlobalInformationPrivate(NEOSDGlobalInformation* s) : S(s)
{
}

NEOSDGlobalInformation::NEOSDGlobalInformation(NENodeFactory* factory): NEOSDBase( factory )
{
    P = new NEOSDGlobalInformationPrivate(this);
    CALL_ONCE(initProperties);

    SetPosition(vec3f(0.5f, 0.95f, 0.0f));
}

NEOSDGlobalInformation::~NEOSDGlobalInformation()
{
    delete P;
}

vec2f NEOSDGlobalInformation::osdSize()
{
    QStringList osdMessage;
    osdMessage << OsdText();

    uint widthU = 0;

    for (int i = osdMessage.size() - 1; i >= 0; i--)
    {
        QString text = osdMessage[i];
        uint texLen = (float)text.length() * fontWidth() + fontWidth();
        widthU = MAX(widthU, texLen);
    }

    return vec2f(widthU, fontHeight() * osdMessage.size());
}

QString NEOSDGlobalInformation::OsdText()
{
    if (AlternativeText() == "")
        return QString("Time: %1 [s]").arg(NESCENE.ElapsedTime(), 10, 'f', 4);
    return AlternativeText();
}

const QString& NEOSDGlobalInformation::AlternativeText() const
{
    return P->m_AlternativeText;
}

uint NEOSDGlobalInformation::SetAlternativeText(const QString& val)
{
    NE_PROP_SET_AND_EVAL(AlternativeText, P, val);
    return NE::kReturnSuccess;
}

void NEOSDGlobalInformation::resetProperties()
{
    NEOSDBase::resetProperties();
    ResetAlternativeText();
}

NEOSDGlobalInformationFactory::NEOSDGlobalInformationFactory(NEManager* m) : NENodeFactory(m) {}

NEOSDGlobalInformationFactory::~NEOSDGlobalInformationFactory() {}

QString NEOSDGlobalInformationFactory::nodeName() { return "Global OSD"; }

uint NEOSDGlobalInformationFactory::nodeType(){ return NE::kObjectOSD;}

QString NEOSDGlobalInformationFactory::nodeInherits() { return "OSDs";}

QString NEOSDGlobalInformationFactory::objectName() { return "GlobalOSD"; }

QString NEOSDGlobalInformationFactory::nodeVersion() { return "0.1"; }

QString NEOSDGlobalInformationFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOSDGlobalInformationFactory::nodeHelp() { return "Global OSD"; }

NEObject*NEOSDGlobalInformationFactory::createInstance() { return new NEOSDGlobalInformation(this); }
