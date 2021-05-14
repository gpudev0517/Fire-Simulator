#pragma once

#include "Utility/Singleton.h"

class NECEXP_NEUIUTILITIES NEGUIStyle : public QObject, public Singleton<NEGUIStyle>
{
	Q_OBJECT


public:
	const QBrush* GrayFontBrush() const	{ return &m_GrayFontBrush;}
	const QBrush* windowBrush() const { return &m_WindowBrush; }
    const QBrush* baseBrush() const { return &m_BaseBrush; }
	const QBrush* RealGrayFontBrush() const { return &m_RealGrayFontBrush; }
	const QBrush* NormalFontBrush() const { return &m_NormalFontBrush;}
	const QFont* NormalFont() const { return &m_NormalFont; }
	const QFont* BoldFont() const { return &m_BoldFont; }
	const QFont* ItalicFont() const { return &m_ItalicFont; }
	const QFont* BoldItalicFont() const { return &m_BoldItalicFont; }

	const QFont* MonoNormalFont() const { return &m_MonoNormalFont; }
	const QFont* MonoBoldFont() const { return &m_MonoBoldFont; }
	const QFont* MonoItalicFont() const { return &m_MonoItalicFont; }
	const QFont* MonoBoldItalicFont() const { return &m_MonoBoldItalicFont; }
	bool darkTheme() { return m_DarkTheme; }

	const QFont* ItalicInactiveFont() const { return &m_ItalicInactiveFont; }

    const QFont* BoldSmallFont() const { return &m_BoldSmallFont; }
    const QFont* NormalSmallFont() const { return &m_NormalSmallFont; }
    const QFont* NormalSmallerFont() const { return &m_NormalSmallerFont; }

    const bool& DarkTheme() const;

public slots:
    void reloadStyleSheet();
    void reloadAndApplyStyleSheet();

    void init();
    void initThemes();
    void SetDarkTheme(bool val);

private:
    QPalette m_DarkPalette;
	QPalette m_LightPalette;
	QString m_LightStyleSheet;
	QString m_DarkStyleSheet;

	//! Application fonts
    static QFont m_NormalFont;
    static QFont m_NormalSmallFont;
    static QFont m_NormalSmallerFont;
    static QFont m_BoldFont;
    static QFont m_BoldSmallFont;
    static QFont m_ItalicFont;
    static QFont m_ItalicInactiveFont;
    static QFont m_BoldItalicFont;
    static QFont m_MonoNormalFont;
    static QFont m_MonoItalicFont;
    static QFont m_MonoBoldFont;
    static QFont m_MonoBoldItalicFont;

    static QBrush m_GrayFontBrush;
    static QBrush m_RealGrayFontBrush;
    static QBrush m_NormalFontBrush;
    static QBrush m_WindowBrush;
    static QBrush m_BaseBrush;

	bool m_DarkTheme;
};


#define NEGUISTYLE NEGUIStyle::Instance()
