#include "NEGUIStyle.h"
#include <QStyleFactory>


NEUITheme* UITheme;
QPalette UIPalette;
QPalette UIPaletteAlt;

QFont NEGUIStyle::m_NormalFont;
QFont NEGUIStyle::m_NormalSmallFont;
QFont NEGUIStyle::m_NormalSmallerFont;
QFont NEGUIStyle::m_BoldFont;
QFont NEGUIStyle::m_BoldSmallFont;
QFont NEGUIStyle::m_ItalicFont;
QFont NEGUIStyle::m_ItalicInactiveFont;
QFont NEGUIStyle::m_BoldItalicFont;
QFont NEGUIStyle::m_MonoNormalFont;
QFont NEGUIStyle::m_MonoItalicFont;
QFont NEGUIStyle::m_MonoBoldFont;
QFont NEGUIStyle::m_MonoBoldItalicFont;

QBrush NEGUIStyle::m_GrayFontBrush;
QBrush NEGUIStyle::m_RealGrayFontBrush;
QBrush NEGUIStyle::m_NormalFontBrush;
QBrush NEGUIStyle::m_WindowBrush;
QBrush NEGUIStyle::m_BaseBrush;

void NEGUIStyle::reloadStyleSheet()
{
    QFile file("System/Styles/dark.css");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printf("Cannot load System/Styles/dark.css");
        return;
    }

    m_DarkStyleSheet = file.readAll();

}

void NEGUIStyle::reloadAndApplyStyleSheet()
{
    reloadStyleSheet();

    if(m_DarkTheme)
        qApp->setStyleSheet(m_DarkStyleSheet);
    else
        qApp->setStyleSheet(m_LightStyleSheet);
}

void NEGUIStyle::init()
{
    initThemes();
    SetDarkTheme(true);
}

void NEGUIStyle::initThemes()
{

#ifdef NE_FOR_WINDOWS
    m_NormalFont = QFont("Segoe UI", 8);
    m_MonoNormalFont = QFont("Consolas",8);
#endif
#ifdef NE_FOR_MAC
    m_NormalFont = QApplication::font();
    m_MonoNormalFont = QFont("Monaco",10);
#endif
#ifdef NE_FOR_LINUX
    m_NormalFont = QApplication::font();
    m_MonoNormalFont = QFont("Monospace", 8);
#endif

    m_NormalSmallFont = m_NormalFont;
    m_NormalSmallerFont = m_NormalFont;
    m_ItalicFont = m_NormalFont;
    m_ItalicFont.setItalic(true);
    m_BoldFont = m_NormalFont;
    m_BoldFont.setBold(true);
    m_BoldSmallFont = m_BoldFont;

#ifdef NE_FOR_WINDOWS
    m_NormalSmallFont.setPointSize(8);
    m_NormalSmallerFont.setPointSize(8);
    m_BoldFont.setPointSize(8);
    m_BoldSmallFont.setPointSize(8);
    m_NormalFont.setPointSize(9);
#endif
#ifdef NE_FOR_MAC
    m_NormalSmallFont.setPointSize(10);
    m_NormalSmallerFont.setPointSize(10);
    m_BoldFont.setPointSize(10);
    m_BoldSmallFont.setPointSize(10);
    m_NormalFont.setPointSize(11);
#endif
#ifdef NE_FOR_LINUX
    m_NormalSmallFont.setPointSize(8);
    m_NormalSmallerFont.setPointSize(8);
    m_BoldFont.setPointSize(8);
    m_BoldSmallFont.setPointSize(8);
    m_NormalFont.setPointSize(9);
#endif

    qApp->setFont(m_NormalSmallFont);

    m_DarkTheme = false;

    m_NormalFontBrush = QBrush(Qt::black);
    m_GrayFontBrush = QBrush(Qt::darkGray);
    m_RealGrayFontBrush = QBrush(Qt::gray);
    m_WindowBrush = QGuiApplication::palette().color(QPalette::Window);
    m_BaseBrush = QGuiApplication::palette().color(QPalette::Base);
    m_ItalicInactiveFont = QApplication::font();
    m_ItalicInactiveFont.setItalic(true);

    // Backup the original light Fusion theme
    m_LightPalette = QGuiApplication::palette();
    m_LightPalette.setColor(QPalette::Window, QGuiApplication::palette().color(QPalette::Window));
    m_LightPalette.setColor(QPalette::WindowText, QGuiApplication::palette().color(QPalette::WindowText));
    m_LightPalette.setColor(QPalette::Base, QGuiApplication::palette().color(QPalette::Base));
    m_LightPalette.setColor(QPalette::AlternateBase, QGuiApplication::palette().color(QPalette::AlternateBase));
    m_LightPalette.setColor(QPalette::ToolTipBase, QGuiApplication::palette().color(QPalette::ToolTipBase));
    m_LightPalette.setColor(QPalette::ToolTipText, QGuiApplication::palette().color(QPalette::ToolTipText));
    m_LightPalette.setColor(QPalette::Text, QGuiApplication::palette().color(QPalette::Text));
    m_LightPalette.setColor(QPalette::Button, QGuiApplication::palette().color(QPalette::Button));
    m_LightPalette.setColor(QPalette::ButtonText, QGuiApplication::palette().color(QPalette::ButtonText));
    m_LightPalette.setColor(QPalette::BrightText, QGuiApplication::palette().color(QPalette::BrightText));
    m_LightPalette.setColor(QPalette::Link, QGuiApplication::palette().color(QPalette::Link));
    m_LightPalette.setColor(QPalette::Highlight, QGuiApplication::palette().color(QPalette::Highlight));
    m_LightPalette.setColor(QPalette::HighlightedText, QGuiApplication::palette().color(QPalette::HighlightedText));
    m_LightStyleSheet = qApp->styleSheet();

    // Define the dark theme
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Window, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::WindowText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Base, QColor(25,25,25));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::AlternateBase, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::ToolTipBase, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::ToolTipText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Text, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Button, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::ButtonText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::BrightText, Qt::red);
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Link, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::LinkVisited, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Highlight, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::HighlightedText, Qt::black);
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Light, Qt::black);
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Midlight, QColor(25,25,25));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Dark, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Mid, QColor(38,38,38));
    m_DarkPalette.setColor(QPalette::Normal, QPalette::Shadow, QColor(25,25,25));

    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Window, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::WindowText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Base, QColor(25,25,25));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::AlternateBase, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::ToolTipBase, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::ToolTipText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Text, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Button, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::BrightText, Qt::red);
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Link, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::LinkVisited, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::HighlightedText, Qt::black);
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Light, Qt::black);
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Midlight, QColor(25,25,25));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Dark, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Mid, QColor(38,38,38));
    m_DarkPalette.setColor(QPalette::Inactive, QPalette::Shadow, QColor(25,25,25));

    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Window, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(160,160,160));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Base, QColor(25,25,25));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::AlternateBase, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::ToolTipBase, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::ToolTipText, QColor(240,240,240));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(160, 160, 160));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(160,160,160));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::BrightText, Qt::red);
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Link, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::LinkVisited, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(42, 130, 218));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, Qt::black);
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Light, Qt::black);
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Midlight, QColor(25,25,25));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Dark, QColor(53,53,53));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Mid, QColor(38,38,38));
    m_DarkPalette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(25,25,25));

    reloadStyleSheet();
}

void NEGUIStyle::SetDarkTheme(bool val)
{
    if(val == m_DarkTheme)
        return;

    m_DarkTheme = val;

    if(m_DarkTheme)
    {
        //qApp->setStyle(m_DarkStyle);
        qApp->setPalette(m_DarkPalette);
        qApp->setStyleSheet(m_DarkStyleSheet);
        m_NormalFontBrush = QBrush(Qt::white);
        m_GrayFontBrush = QBrush(Qt::lightGray);
    }
    else
    {
        //qApp->setStyle(m_LightStyle);
        qApp->setPalette(m_LightPalette);
        qApp->setStyleSheet(m_LightStyleSheet);
        m_NormalFontBrush = QBrush(Qt::black);
        m_GrayFontBrush = QBrush(Qt::darkGray);
    }

    m_WindowBrush = QGuiApplication::palette().color(QPalette::Window);
    m_BaseBrush = QGuiApplication::palette().color(QPalette::Base);
}

const bool& NEGUIStyle::DarkTheme() const
{
    return m_DarkTheme;
}

