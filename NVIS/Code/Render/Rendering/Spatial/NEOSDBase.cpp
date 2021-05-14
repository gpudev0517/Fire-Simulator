#include "Base/NEBase.h"

#include "NEOSDBase.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NERenderManager.h"

// Installs properties, their default values, and generates property reset functions
// Properties should always be passed with their names capitilized

class NEOSDBasePrivate
{
public:
    NEOSDBasePrivate(NEOSDBase* s);

    FTTextureFont* textureFont;             // Pointer to 2D OpenGL font drawing object
    FTOutlineFont* textureOutlineFont;
    NEOSDBase::NEFontName m_FontName;
    int m_FontSize;
    QColor m_FontColor;
    QColor m_OutlineColor;
    NEOSDBase::NEDisplayNotation m_DisplayNotation;
    int m_FloatTypePrecision;
    int m_TotalNumberLength;
    bool m_TextOutline;
private:
    NEOSDBase* S;
};


NE_PROP_DEF(NEOSDBase::NEFontName, NEOSDBase, FontName, NEOSDBase::Consola)
NE_PROP_DEF(int, NEOSDBase, FontSize, 20)
NE_PROP_DEF(QColor, NEOSDBase, FontColor, QColor(255, 255, 255))
NE_PROP_DEF(NEOSDBase::NEDisplayNotation, NEOSDBase, DisplayNotation, NEOSDBase::FloatingPoint)
NE_PROP_DEF(int, NEOSDBase, FloatTypePrecision, 4)
NE_PROP_DEF(int, NEOSDBase, TotalNumberLength, 10)
NE_PROP_DEF(bool, NEOSDBase, TextOutline, false)
NE_PROP_DEF(QColor, NEOSDBase, OutlineColor, QColor(0, 0, 0))

static void initProperties()
{
    NE_INIT_PROP(NEOSDBase, FontName);
    NE_INIT_PROP(NEOSDBase, FontSize);
    NE_INIT_PROP(NEOSDBase, FontColor);
    NE_INIT_PROP(NEOSDBase, DisplayNotation);
    NE_INIT_PROP(NEOSDBase, FloatTypePrecision);
    NE_INIT_PROP(NEOSDBase, TotalNumberLength);
    NE_INIT_PROP(NEOSDBase, OutlineColor);
    NE_INIT_PROP(NEOSDBase, TextOutline);
}

NEOSDBasePrivate::NEOSDBasePrivate(NEOSDBase* s) : S(s)
{
    textureFont = NULL;
    textureOutlineFont = NULL;
}

NEOSDBase::NEOSDBase( NENodeFactory * factory): NESpatial( factory )
{
    addNodeTypeToNodeGraphSceneType("OSDs", NENode::SceneType::Visualization );

    P = new NEOSDBasePrivate(this);
    CALL_ONCE(initProperties);

    P->m_FontName = NE_DEFAULT(FontName);
    P->m_FontSize = NE_DEFAULT(FontSize);
    P->m_FontColor = NE_DEFAULT(FontColor);
    P->m_DisplayNotation = NE_DEFAULT(DisplayNotation);
    P->m_FloatTypePrecision = NE_DEFAULT(FloatTypePrecision);
    P->m_TotalNumberLength = NE_DEFAULT(TotalNumberLength);
    P->m_OutlineColor = NE_DEFAULT(OutlineColor);

    initFont();
    SetPosition(vec3f(0.5f, 0.5f, 0.0f));
}

NEOSDBase::~NEOSDBase()
{
    deInitFont();
    delete P;
}

void NEOSDBase::render()
{
}

void NEOSDBase::renderPickable()
{
}

void NEOSDBase::renderPicked()
{
}

bool NEOSDBase::isManipulatable(NEManipulator manipType)
{
    if (manipType == NEManipulator::kManipTranslate)
        return true;
    return false;
}

void NEOSDBase::glPrint(QString name, int x, int y, QColor colorval)
{
    Color32 color(colorval.red(), colorval.green(), colorval.blue(), 255);
    GL->glColor3ubv((GLubyte*)&color);
    P->textureFont->Render(name.toUtf8(), -1, FTPoint(x, y));
}

void NEOSDBase::initFont()
{
    QString fontName = "";
    switch(FontName())
    {
    case NEFontName::Consola:
        fontName = "System/Fonts/consola.ttf";
        break;
    case NEFontName::CourierNew:
        fontName = "System/Fonts/cour.ttf";
        break;
    case NEFontName::RobotaLight:
        fontName = "System/Fonts/Roboto-Light.ttf";
        break;
    case NEFontName::RobotaRegular:
        fontName = "System/Fonts/Roboto-Regular.ttf";
        break;
    case NEFontName::Monofonto:
        fontName = "System/Fonts/Monofonto.ttf";
        break;
    case NEFontName::Anonymous:
        fontName = "System/Fonts/Anonymous.ttf";
        break;
    case NEFontName::VaguelyFatal:
        fontName = "System/Fonts/Vaguely Fatal.ttf";
        break;
    case NEFontName::Monspace_CD_LT:
        fontName = "System/Fonts/nk57-monospace-cd-lt.ttf";
        break;
    case NEFontName::Monospace_CD_RG:
        fontName = "System/Fonts/nk57-monospace-cd-rg.ttf";
        break;
    case NEFontName::Monospace_NO_BK:
        fontName = "System/Fonts/nk57-monospace-no-bk.ttf";
        break;
    case NEFontName::Monospace_NO_RG:
        fontName = "System/Fonts/nk57-monospace-no-rg.ttf";
        break;
    case  NEFontName::Monospace_SC_LT:
        fontName = "System/Fonts/nk57-monospace-sc-lt.ttf";
        break;
    case NEFontName:: Monospace_SC_BK:
        fontName = "System/Fonts/nk57-monospace-sc-bk.ttf";
        break;
    case NEFontName:: Monospace_SC_RG:
        fontName = "System/Fonts/nk57-monospace-sc-rg.ttf";
        break;
    case NEFontName:: Monospace_SC_SB:
        fontName = "System/Fonts/nk57-monospace-sc-sb.ttf";
        break;
    case NEFontName:: Monospace_SE_BK:
        fontName = "System/Fonts/nk57-monospace-se-bk.ttf";
        break;
    case NEFontName:: Monospace_SE_LT:
        fontName = "System/Fonts/nk57-monospace-se-lt.ttf";
        break;
    case NEFontName:: Monospace_SE_RG:
        fontName = "System/Fonts/nk57-monospace-se-rg.ttf";
        break;
    case NEFontName::Minim:
        fontName = "System/Fonts/minim.ttf";
        break;

    }
    P->textureFont = new FTTextureFont(fontName.toStdString().c_str());
    P->textureFont->UseDisplayList(true);
    if(P->textureFont->Error())
    {
        Log() << "Cannot load the font";
        EndLog(NE::kLogError);
    }
    P->textureFont->FaceSize(FontSize());

    P->textureOutlineFont = new FTOutlineFont(fontName.toStdString().c_str());
    P->textureOutlineFont->UseDisplayList(true);
    P->textureOutlineFont->Outset(1.0);
    if(P->textureOutlineFont->Error())
    {
        Log() << "Cannot load the font";
        EndLog(NE::kLogError);
    }
    P->textureOutlineFont->FaceSize(FontSize());
}

void NEOSDBase::deInitFont()
{
    delete P->textureFont;
    delete P->textureOutlineFont;
}

FTTextureFont* NEOSDBase::getTextureFont()
{
    return P->textureFont;
}

FTOutlineFont* NEOSDBase::getTextureOutlineFont()
{
    if(TextOutline())
        return P->textureOutlineFont;
    else
        return NULL;
}

NESpatial::NESpatialType NEOSDBase::spatialType()
{
    return NESpatial::kOSD;
}

vec2f NEOSDBase::osdSize()
{
    return vec2f();
}

float   NEOSDBase::fontHeight()
{
    return FontSize();
}

float   NEOSDBase::fontWidth()
{
    return fontHeight() / 1.8;
}

int     NEOSDBase::leftMargin()
{
    return 5;
}

int     NEOSDBase::topMargin()
{
    return fontHeight() / 2;
}

QColor NEOSDBase::fontColor()
{
    return P->m_FontColor;
}

const NEOSDBase::NEFontName& NEOSDBase::FontName() const
{
    return P->m_FontName;
}

uint NEOSDBase::SetFontName(const NEOSDBase::NEFontName& val)
{
    NE_PROP_SET_AND_EVAL(FontName, P, val);
    deInitFont();
    initFont();
    return NE::kReturnSuccess;
}

const int& NEOSDBase::FontSize() const
{
    return P->m_FontSize;
}

const NEOSDBase::NEDisplayNotation& NEOSDBase::DisplayNotation() const
{
    return P->m_DisplayNotation;
}

uint NEOSDBase::SetDisplayNotation(const NEOSDBase::NEDisplayNotation& val)
{
    NE_PROP_SET_AND_EVAL(DisplayNotation, P, val);

    return NE::kReturnSuccess;
}

uint NEOSDBase::SetFontSize(const int& val)
{
    NE_PROP_SET_AND_EVAL(FontSize, P, val);
    deInitFont();
    initFont();
    return NE::kReturnSuccess;
}

const int& NEOSDBase::FloatTypePrecision() const
{
    return P->m_FloatTypePrecision;
}

uint NEOSDBase::SetFloatTypePrecision(const int& val)
{
    int setValue = MIN(4, MAX(0, val));
    NE_PROP_SET_AND_EVAL(FloatTypePrecision, P, setValue);

    return NE::kReturnSuccess;
}

const int& NEOSDBase::TotalNumberLength() const
{
    return P->m_TotalNumberLength;
}

uint NEOSDBase::SetTotalNumberLength(const int& val)
{
    int setValue = MIN(10, MAX(1, val));
    NE_PROP_SET_AND_EVAL(TotalNumberLength, P, setValue);

    return NE::kReturnSuccess;
}

const QColor& NEOSDBase::FontColor() const
{
    return P->m_FontColor;
}

uint NEOSDBase::SetFontColor(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(FontColor, P, val);
    return NE::kReturnSuccess;
}

const QColor& NEOSDBase::OutlineColor() const
{
    return P->m_OutlineColor;
}

uint NEOSDBase::SetOutlineColor(const QColor& val)
{
    NE_PROP_SET_AND_EVAL(OutlineColor, P, val);
    return NE::kReturnSuccess;
}

const bool& NEOSDBase::TextOutline() const
{
    return P->m_TextOutline;
}

uint NEOSDBase::SetTextOutline(const bool& val)
{
    NE_PROP_SET_AND_EVAL(TextOutline, P, val);
    return NE::kReturnSuccess;
}

void NEOSDBase::resetProperties()
{
    NESpatial::resetProperties();
    ResetFontName();
    ResetFontSize();
    ResetFontColor();
    ResetDisplayNotation();
    ResetFloatTypePrecision();
    ResetTotalNumberLength();
    ResetOutlineColor();
    ResetTextOutline();
}
