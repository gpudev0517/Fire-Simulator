/****************************************************************************
**
** Copyright (c) 2012 Richard Steffen and/or its subsidiary(-ies).
** All rights reserved.
** Contact: rsteffen@messbild.de, rsteffen@uni-bonn.de
**
** QColorRampEditor is free to use unter the terms of the LGPL 2.1 License in
** Free and Commercial Products.
****************************************************************************/

#ifndef __QCOLORRAMPEDITOR_H__
#define __QCOLORRAMPEDITOR_H__

using namespace std;

// -----------------------------------------------------------
// QColorRampEditor ------------------------------------------
// -----------------------------------------------------------
class NEColorRampEditorSlider;
class NERampWidget;
class NESlidersWidget;
class NESliderTextWidget;
class NECEXP_NEUIPROPERTIES NEColorRampEditor : public QWidget
{
    Q_OBJECT
public:

    /// Basic Constructor
    NEColorRampEditor(QWidget* parent=0, int orientation = Qt::Horizontal);

    /// Destructor
    virtual ~NEColorRampEditor();

    // define friends to access protected members
    friend class NERampWidget;
    friend class NESlidersWidget;
    friend class NESliderTextWidget;

    /// get the number of sliders
    int getSliderNum();

	/// set the update emitting when sliding
	void setSlideUpdate(bool val);

    /// return the Ramp definition
    QVector<QPair<qreal, QColor> > getRamp();

	/// return a 256 colortable from the ramp
	QVector<QRgb> getColorTable();

    /// set Ramp definition
    void setRamp(QVector<QPair<qreal, QColor> > ramp);

	/// set flag for visualize 
	void setMappingTextVisualize(bool);

	/// set the text color
	void setMappingTextColor(QColor);

	/// set the text color
	void setMappingTextAccuracy(int);

    /// get the value of a slider
    qreal updateValue(NEColorRampEditorSlider* sl);

    /// get the position
    int updatePos(NEColorRampEditorSlider* sl);

signals:

    /// signal that hide is changed
    void rampChanged();

public slots:

    /// set the color of a slider to zero
    void setSliderColor(int index, QColor col);

protected slots:

    /// resize required
    virtual void resizeEvent(QResizeEvent* e);

    /// detect a mouse is pressed
    virtual void mousePressEvent(QMouseEvent* e);    

    /// on update the ramp because sliders are changed
    virtual void updateRamp();

protected:

    /// sort the color ramp
    static bool colorRampSort(const QPair<qreal, QColor> &a1, const QPair<qreal, QColor> &a2);

    /// sort the slider list
    static bool SliderSort(const NEColorRampEditorSlider* a1, const NEColorRampEditorSlider* a2);

    /// all poses with its sliders
    QList<NEColorRampEditorSlider*> sliders_;

    /// the orientation
    int ori_;

    /// bound space
    int bspace_;

    /// min and max value from initialization
    qreal mi_, ma_;

    /// the widgets drawint the ramp, the sliders, the text
    NERampWidget* rampwid_;
    NESlidersWidget* slidewid_;
    NESliderTextWidget* textwid_;

    /// the index of the active slider
    int activeSlider_;

	/// flag to visualize the mapping
	bool visText_;

	/// color of the text
	QColor textColor_;

	/// the text accuracy
	int textAcc_;

	/// continous update?
	bool slideUpdate_;
};

// -----------------------------------------------------------
// NEColorRampEditorSlider ------------------------------------
// -----------------------------------------------------------
class NEColorRampEditorSlider : public QWidget
{
    Q_OBJECT
public:

    /// Constructor
    NEColorRampEditorSlider(int orientation = Qt::Horizontal, QColor col = Qt::black, QWidget* parent=0);

    /// set the color of the slider
    void setColor(QColor col);

    /// get the color
    QColor getColor();    

    /// the value
    qreal val;

protected slots:

    /// paint the widget
    virtual void paintEvent(QPaintEvent* event);

protected:

    /// the color of the slider
    QColor color_;

    /// the orientation
    int ori_;
};

class NERampWidget : public QWidget
{
public:
    NERampWidget(QWidget* parent=NULL);
    NEColorRampEditor* rampeditor_;

protected:
    void paintEvent(QPaintEvent* e);
};

class NESlidersWidget : public QWidget
{
    Q_OBJECT
public:
    /// Constructor
    NESlidersWidget(QWidget* parent=NULL);

    NEColorRampEditor* rampeditor_;

protected slots:

    /// detect a mouse is pressed
    virtual void mousePressEvent(QMouseEvent* e);

    /// detect a mouse is moved
    virtual void mouseMoveEvent(QMouseEvent* e);

    /// detect a mouse is released
    virtual void mouseReleaseEvent(QMouseEvent* e);

    /// detect a mouse is released
    virtual void mouseDoubleClickEvent(QMouseEvent* e);

protected:
    /// the active slider
    int activeSlider_;
    int deltaOfPos;
};

class NESliderTextWidget : public QWidget
{
public:
    NESliderTextWidget(QWidget* parent=NULL);

    NEColorRampEditor* rampeditor_;

protected:

    void paintEvent(QPaintEvent* e);
};

#endif
