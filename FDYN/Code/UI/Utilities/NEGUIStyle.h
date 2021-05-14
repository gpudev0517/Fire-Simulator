#pragma once

#include "Utility/Singleton.h"

class NEGUIStyle : public QObject, public Singleton<NEGUIStyle>
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
	QFont m_NormalFont;
    QFont m_NormalSmallFont;
    QFont m_NormalSmallerFont;
	QFont m_BoldFont;
    QFont m_BoldSmallFont;
	QFont m_ItalicFont;
	QFont m_ItalicInactiveFont;
	QFont m_BoldItalicFont;
	QFont m_MonoNormalFont;
	QFont m_MonoItalicFont;
	QFont m_MonoBoldFont;
	QFont m_MonoBoldItalicFont;

	QBrush m_GrayFontBrush;
	QBrush m_RealGrayFontBrush;
	QBrush m_NormalFontBrush;
	QBrush m_WindowBrush;
    QBrush m_BaseBrush;

	bool m_DarkTheme;
};


#define NEGUISTYLE NEGUIStyle::Instance()
