/****************************************************************************
**
** Copyright (c) 2012 Richard Steffen and/or its subsidiary(-ies).
** All rights reserved.
** Contact: rsteffen@messbild.de, rsteffen@uni-bonn.de
**
** NEColorRampEditor is free to use unter the terms of the LGPL 2.1 License in
** Free and Commercial Products.
****************************************************************************/

#include "NEColorRampEditor.h"

#define MAX_COLORRAMP_SIZE 32
#define RAMP_EDITOR_SIZE 42
#define RAMP_SLIDE_SIZE 16

// -----------------------------------------------------------
// NEColorRampEditor -----------------------------------------
// -----------------------------------------------------------
NEColorRampEditor::NEColorRampEditor(QWidget* parent, int orientation) : QWidget(parent),
    ori_(orientation),
    activeSlider_(-1), slideUpdate_(false),
    bspace_(8),
    visText_(false), textColor_(Qt::white), textAcc_(2)
{
    if (ori_==Qt::Horizontal)
        setMinimumSize(70,RAMP_EDITOR_SIZE);
    else
        setMinimumSize(RAMP_EDITOR_SIZE,70);

    setContentsMargins(0,0,0,0);
    if (ori_==Qt::Horizontal)
        setLayout(new QVBoxLayout());
    else
        setLayout(new QHBoxLayout());
    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->setContentsMargins(0,0,0,0);

    rampwid_ = new NERampWidget();
    rampwid_->rampeditor_ = this;
    rampwid_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    rampwid_->setContentsMargins(0,0,0,0);

    layout()->addWidget(rampwid_);

    slidewid_ = new NESlidersWidget();
    slidewid_->rampeditor_ = this;
    if (ori_==Qt::Horizontal)
    {
        slidewid_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        slidewid_->setFixedHeight(RAMP_SLIDE_SIZE);
    }
    else
    {
        slidewid_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        slidewid_->setFixedWidth(RAMP_SLIDE_SIZE);
    }
    slidewid_->setContentsMargins(0,0,0,0);
    layout()->addWidget(slidewid_);

    textwid_ = new NESliderTextWidget();
    textwid_->rampeditor_ = this;
    if (ori_==Qt::Horizontal)
    {
        textwid_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        textwid_->setFixedHeight(10);
    }
    else
    {
        textwid_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        textwid_->setFixedWidth(10);
    }
    layout()->addWidget(textwid_);
    textwid_->setVisible(false);

    // init sliders
    QVector<QPair<qreal, QColor> > ramp;
    ramp.push_back(QPair<qreal, QColor>(0.0,QColor(0,0,255,0)));
    ramp.push_back(QPair<qreal, QColor>(0.3,QColor(0,255,0,0)));
    ramp.push_back(QPair<qreal, QColor>(0.6,QColor(255,255,0,0)));
    ramp.push_back(QPair<qreal, QColor>(1.0,QColor(255,0,0,0)));
    setRamp(ramp);

}
// -----------------------------------------------------------
NEColorRampEditor::~NEColorRampEditor()
{
    for (int i=0; i<sliders_.size(); i++) delete(sliders_[i]);
}
// -----------------------------------------------------------
int NEColorRampEditor::getSliderNum()
{
    return sliders_.size();
}

// -----------------------------------------------------------
void NEColorRampEditor::setSlideUpdate(bool val)
{
    slideUpdate_ = val;
}

// -----------------------------------------------------------
bool NEColorRampEditor::colorRampSort(const QPair<qreal, QColor> &a1, const QPair<qreal, QColor> &a2)
{
    return a1.first < a2.first;
}

bool NEColorRampEditor::SliderSort(const NEColorRampEditorSlider* a1, const NEColorRampEditorSlider* a2)
{
    return a1->val < a2->val;
}

// -----------------------------------------------------------
QVector<QPair<qreal, QColor> > NEColorRampEditor::getRamp()
{

    // create output
    QVector<QPair<qreal, QColor> > ret;
    for (int i=0; i<sliders_.size(); i++)
    {
        QColor col = sliders_[i]->getColor();
        ret.push_back(QPair<qreal, QColor>(sliders_[i]->val,col));
    }
    // sort the slider list
    qSort(ret.begin(), ret.end(), NEColorRampEditor::colorRampSort);

    return ret;
}
// -----------------------------------------------------------
QVector<QRgb> NEColorRampEditor::getColorTable()
{
    // get ramp and normalize
    QVector<QPair<qreal, QColor> > ramp = getRamp();
    for (int i=0; i<ramp.size(); i++) ramp[i].first = (ramp[i].first - mi_)/(ma_-mi_);

    QVector<QRgb> ctable;
    int index = 0;
    for (int i = 0; i < 256; i++)
    {
        float val = 1.0f*i/255;
        if (val>ramp[index+1].first) index++;
        // linear interpolate color
        QColor cc;
        float d = val - ramp[index].first;
        float dr = ramp[index+1].second.redF() - ramp[index].second.redF();
        float dg = ramp[index+1].second.greenF() - ramp[index].second.greenF();
        float db = ramp[index+1].second.blueF() - ramp[index].second.blueF();
        qreal dd = ramp[index+1].first - ramp[index].first;
        float red = ramp[index].second.redF() +  d*dr/dd;
        float green = ramp[index].second.greenF() +  d*dg/dd;
        float blue = ramp[index].second.blueF() +  d*db/dd;
		if (red<0.0f) red=0;
		if (red>1.0f) red=1;
		if (green<0.0f) green=0;
		if (green>1.0f) green=1;
		if (blue<0.0f) blue=0;
		if (blue>1.0f) blue=1;
        cc.setRedF(red);
        cc.setGreenF(green);
        cc.setBlueF(blue);
        ctable.push_back(cc.rgb());
    }
    return ctable;
}
// -----------------------------------------------------------
void NEColorRampEditor::setRamp(QVector<QPair<qreal, QColor> > ramp)
{
    if (ramp.size()<2) return;

    // sort the slider list
    qSort(ramp.begin(), ramp.end(), NEColorRampEditor::colorRampSort);

    for (int i=0; i<sliders_.size(); i++) delete(sliders_[i]);
    sliders_.clear();

    // find min/max
    mi_=ramp.first().first;
    ma_=ramp.last().first;

    // create sliders
    for (int i=0; i<ramp.size(); i++)
    {
        NEColorRampEditorSlider* sl = new NEColorRampEditorSlider(ori_,ramp[i].second, slidewid_);
        sl->val = ramp[i].first;
        sliders_.push_back(sl);
        updatePos(sl);
        sl->show();
    }

    //emit rampChanged();
    slidewid_->update();
}

void NEColorRampEditor::setMappingTextVisualize(bool vis)
{
    visText_ = vis;
    textwid_->setVisible(visText_);
    update();
}

void NEColorRampEditor::setMappingTextColor(QColor col)
{
    textColor_ = col;
    update();
}

void NEColorRampEditor::setMappingTextAccuracy(int acc)
{
    textAcc_ = acc;
    update();
}

qreal NEColorRampEditor::updateValue(NEColorRampEditorSlider* sl)
{
    QRect crec = slidewid_->contentsRect();
    if (ori_==Qt::Horizontal)
    {
        crec.adjust(bspace_,0,-bspace_,0);
        sl->val = mi_ + (1.0*sl->pos().x()-bspace_)/crec.width()*(ma_-mi_);
    }
    else
    {
        crec.adjust(0,bspace_,0,-bspace_);
        sl->val = mi_ + (1.0*sl->pos().y()-bspace_)/crec.height()*(ma_-mi_);
    }

    qreal val = sl->val;
    QString txt = QString::number(val, 'f', textAcc_);
    sl->setToolTip(txt);

    return sl->val;
}

int NEColorRampEditor::updatePos(NEColorRampEditorSlider* sl)
{
    QRect crec = slidewid_->contentsRect();
    qreal pos;
    if (ori_==Qt::Horizontal)
    {
        crec.adjust(bspace_,0,-bspace_,0);
        pos = (sl->val- mi_)/(ma_-mi_)*crec.width();
        pos -= sl->width()/2;
        pos += bspace_;
        sl->move(pos,0);
    }
    else
    {
        crec.adjust(0, bspace_,0,-bspace_);
        pos = (sl->val- mi_)/(ma_-mi_)*crec.height();
        pos -= sl->height()/2;
        pos += bspace_;
        sl->move(0,pos);
    }

    qreal val = sl->val;
    QString txt = QString::number(val, 'f', textAcc_);
    sl->setToolTip(txt);

    return pos;
}

// -----------------------------------------------------------
void NEColorRampEditor::setSliderColor(int index, QColor col)
{
    if (index<0 || index>=sliders_.size()) return;
    sliders_[index]->setColor(col);
    emit rampChanged();
}

// -----------------------------------------------------------
void NEColorRampEditor::resizeEvent (QResizeEvent* e)
{
    for (int i=0; i<sliders_.size(); i++)
    {
        NEColorRampEditorSlider* sl = sliders_[i];
        updatePos(sl);
    }
}
// -----------------------------------------------------------
void NEColorRampEditor::mousePressEvent(QMouseEvent* e)
{
    if (sliders_.size() >= MAX_COLORRAMP_SIZE)
        return;

    if (e->button()== Qt::LeftButton)
    {
        QRect crec = contentsRect();
        if (ori_==Qt::Horizontal)
        {
            crec.adjust(bspace_,0,-bspace_,0);
            if (crec.contains(e->pos(), true )) // test mouse is in ramp
            {
                NEColorRampEditorSlider* sl = new NEColorRampEditorSlider(ori_, Qt::white, slidewid_);
                sliders_.push_back(sl);
                sl->move(e->pos().x(),0);
                updateValue(sl);
                sl->show();
                qSort(sliders_.begin(), sliders_.end(), NEColorRampEditor::SliderSort);
                updateRamp();
            }
        }
        else
        {
            crec.adjust(0,bspace_,0,-bspace_);
            if (crec.contains(e->pos(), true )) // test mouse is in ramp
            {
                NEColorRampEditorSlider* sl = new NEColorRampEditorSlider(ori_, Qt::white, slidewid_);
                sliders_.push_back(sl);
                sl->move(0,e->pos().y());
                updateValue(sl);
                sl->show();
                qSort(sliders_.begin(), sliders_.end(), NEColorRampEditor::SliderSort);
                updateRamp();
            }
        }
    }
}

void NEColorRampEditor::updateRamp()
{
    rampwid_->update();
    if (textwid_->isVisible()) textwid_->update();
    emit rampChanged();
}


// -----------------------------------------------------------
// NERampWidget ----------------------------------------------
// -----------------------------------------------------------

NERampWidget::NERampWidget(QWidget* parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    setContentsMargins(0,0,0,0);
    setMinimumHeight(RAMP_EDITOR_SIZE);
    setMinimumWidth(RAMP_EDITOR_SIZE);
}

void NERampWidget::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setPen(Qt::black);

    QLinearGradient grad;
    QColor slideColor;
    QRect crec = contentsRect();
    if (rampeditor_->ori_==Qt::Horizontal)
    {
        crec.adjust(rampeditor_->bspace_ - 1, 0, -rampeditor_->bspace_,-RAMP_SLIDE_SIZE - 1);
        grad = QLinearGradient( 0, 0, crec.width()-1, 0);
    }
    else
    {
        crec.adjust(0, rampeditor_->bspace_ - 1, -RAMP_SLIDE_SIZE - 1, -rampeditor_->bspace_);
        grad = QLinearGradient( 0, 0, 0, crec.height()-1);
    }

    for (int i=0; i<rampeditor_->sliders_.size(); i++)
    {
        qreal nval = (rampeditor_->sliders_[i]->val - rampeditor_->mi_)/(rampeditor_->ma_-rampeditor_->mi_);
        slideColor = rampeditor_->sliders_[i]->getColor();
        slideColor.setAlpha(255);
        grad.setColorAt(nval, slideColor);
    }

    painter.fillRect( crec, grad);
    painter.drawRect(crec);

    QWidget::paintEvent(e);
}

// -----------------------------------------------------------
// NESlidersWidget -------------------------------------------
// -----------------------------------------------------------
NESlidersWidget::NESlidersWidget(QWidget* parent) : QWidget(parent),
    activeSlider_(-1)
{
    setContentsMargins(0,0,0,0);
    setMinimumHeight(RAMP_SLIDE_SIZE);
    setMinimumWidth(RAMP_SLIDE_SIZE);
    deltaOfPos = 0;
}

// -----------------------------------------------------------
void NESlidersWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button()== Qt::LeftButton)
    {
        if (rampeditor_->sliders_.size()<2) return;
        for (int i=1; i<rampeditor_->sliders_.size()-1; i++)
        {
            QRect srec = rampeditor_->sliders_[i]->geometry();
            if (srec.contains(e->pos(), true ))
            {
                activeSlider_ = i;
                if (rampeditor_->ori_==Qt::Horizontal)
                    deltaOfPos = e->pos().x() - rampeditor_->sliders_[activeSlider_]->pos().x() - RAMP_SLIDE_SIZE/2;
                else
                    deltaOfPos = e->pos().y() - rampeditor_->sliders_[activeSlider_]->pos().y() - RAMP_SLIDE_SIZE/2;
                break;
            }
        }
    }
    else if (e->button()== Qt::RightButton)
    {
        if (rampeditor_->sliders_.size()<2) return;
        for (int i=1; i<rampeditor_->sliders_.size()-1; i++)
        {
            QRect srec = rampeditor_->sliders_[i]->geometry();
            if (srec.contains(e->pos(), true ))
            {
                activeSlider_ = i;
                delete(rampeditor_->sliders_[activeSlider_]);
                rampeditor_->sliders_.removeAt(activeSlider_);
                activeSlider_ = -1;
                rampeditor_->updateRamp();
                break;
            }
        }
    }
}
// -----------------------------------------------------------
void NESlidersWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (activeSlider_>=0)
    {
        QRect crec = contentsRect();

        qreal pos;
        if (rampeditor_->ori_==Qt::Horizontal)
        {
            crec.adjust(rampeditor_->bspace_,0,-rampeditor_->bspace_,0);
//            pos = 1.0*(e->pos().x()-rampeditor_->bspace_)/(crec.width());
            pos = (qreal)(e->pos().x()-rampeditor_->bspace_ - deltaOfPos)/(qreal)crec.width();
        }
        else
        {
            crec.adjust(0,rampeditor_->bspace_,0,-rampeditor_->bspace_);
//            pos = 1.0*(e->pos().y()-rampeditor_->bspace_)/(crec.height());
            pos = (qreal)(e->pos().y()-rampeditor_->bspace_ - deltaOfPos)/(qreal)crec.height();
        }

        if (pos<0.0 || pos>1.0)
        {
            delete(rampeditor_->sliders_[activeSlider_]);
            rampeditor_->sliders_.removeAt(activeSlider_);
            activeSlider_ = -1;
            rampeditor_->updateRamp();
        }
        else
        {
            if (rampeditor_->ori_==Qt::Horizontal)
                rampeditor_->sliders_[activeSlider_]->move(e->pos().x() - deltaOfPos - RAMP_SLIDE_SIZE/2, 0);
            else
                rampeditor_->sliders_[activeSlider_]->move(0,e->pos().y() - deltaOfPos - RAMP_SLIDE_SIZE/2);

            rampeditor_->updateValue(rampeditor_->sliders_[activeSlider_]);
//            qSort(rampeditor_->sliders_.begin(), rampeditor_->sliders_.end(), NEColorRampEditor::SliderSort);
//            if (rampeditor_->slideUpdate_) rampeditor_->updateRamp();
        }
        update();
    }
}
// -----------------------------------------------------------
void NESlidersWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button()== Qt::LeftButton)
    {
        qSort(rampeditor_->sliders_.begin(), rampeditor_->sliders_.end(), NEColorRampEditor::SliderSort);
        activeSlider_ = -1;
        rampeditor_->updateRamp();
    }
}
// -----------------------------------------------------------
void NESlidersWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (e->button()== Qt::LeftButton)
    {
        int index = -1;
        for (int i=0; i<rampeditor_->sliders_.size(); i++)
        {
            QRect srec = rampeditor_->sliders_[i]->geometry();
            if (srec.contains(e->pos(), true ))
            {
                index = i;
                break;
            }
        }
        if (index>=0)
        {
            QRgb oldRgba = rampeditor_->sliders_[index]->getColor().rgba();
            QColor newColor = QColorDialog::getColor(QColor::fromRgba(oldRgba), this, "", QColorDialog::ShowAlphaChannel);
            QRgb newRgba = newColor.rgba();
            if (newColor.isValid() && newRgba != oldRgba) {
                rampeditor_->sliders_[index]->setColor(newColor);
                rampeditor_->updateRamp();
            }
        }
    }
}


// -----------------------------------------------------------
// NEColorRampEditorSlider -----------------------------------
// -----------------------------------------------------------
NEColorRampEditorSlider::NEColorRampEditorSlider(int orientation, QColor col, QWidget* parent) : QWidget(parent),
    ori_(orientation), color_(col)
{
    if (ori_==Qt::Horizontal)
        setFixedSize(RAMP_SLIDE_SIZE, RAMP_SLIDE_SIZE);
    else
        setFixedSize(RAMP_SLIDE_SIZE, RAMP_SLIDE_SIZE);
}
// -----------------------------------------------------------
void NEColorRampEditorSlider::setColor(QColor col)
{
    color_ = col;
}
// -----------------------------------------------------------
QColor NEColorRampEditorSlider::getColor()
{
    return color_;
}

// -----------------------------------------------------------
void NEColorRampEditorSlider::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    QColor slideColor(color_);
    slideColor.setAlpha(255);

    painter.setBrush(slideColor);
    if (ori_==Qt::Horizontal)
    {
        QRect rec(0,RAMP_SLIDE_SIZE*4/5,RAMP_SLIDE_SIZE-1,RAMP_SLIDE_SIZE);
        painter.drawRect(rec);
        QPolygon pp;
        pp << QPoint(0,RAMP_SLIDE_SIZE*4/5) << QPoint(RAMP_SLIDE_SIZE/2,0) << QPoint(RAMP_SLIDE_SIZE, RAMP_SLIDE_SIZE*4/5);
        painter.drawPolygon(pp, Qt::OddEvenFill);
    }
    else
    {
        QRect rec(RAMP_SLIDE_SIZE*4/5,0,RAMP_SLIDE_SIZE,RAMP_SLIDE_SIZE-1);
        painter.drawRect(rec);
        QPolygon pp;
        pp << QPoint(RAMP_SLIDE_SIZE*4/5,0) << QPoint(0,RAMP_SLIDE_SIZE/2) << QPoint(RAMP_SLIDE_SIZE*4/5,RAMP_SLIDE_SIZE);
        painter.drawPolygon(pp, Qt::OddEvenFill);
    }

}


// -----------------------------------------------------------
// NESliderTextWidget ----------------------------------------
// -----------------------------------------------------------
NESliderTextWidget::NESliderTextWidget(QWidget* parent) : QWidget(parent)
{
    setContentsMargins(0,0,0,0);
}

void NESliderTextWidget::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    QFont f = painter.font();
    f.setPixelSize(8);
    painter.setFont(f);

    painter.setPen(rampeditor_->textColor_);
    painter.setBrush(rampeditor_->textColor_);

    QFontMetrics fm(f);

    // adjust size for vertical
    if (rampeditor_->ori_==Qt::Vertical)
    {
        {
            QString txt1 = QString::number(rampeditor_->sliders_.first()->val, 'f', rampeditor_->textAcc_);
            QString txt2 = QString::number(rampeditor_->sliders_.last()->val, 'f', rampeditor_->textAcc_);
            int w = fm.width(txt1) + 2;
            if (w>this->width()) setFixedWidth(w);
            w = fm.width(txt2) + 2;
            if (w>this->width()) setFixedWidth(w);
        }
        // draw the text for vertical orientation
        for (int i=0; i<rampeditor_->sliders_.size(); i++)
        {
            int pos = rampeditor_->sliders_[i]->pos().y();
            qreal val = rampeditor_->sliders_[i]->val;
            QString txt = QString::number(val, 'f', rampeditor_->textAcc_);
            painter.drawText(2, pos + rampeditor_->sliders_[i]->height(), txt);
        }
    }
    else // draw the text for horizontal orientation
    {
        for (int i=0; i<rampeditor_->sliders_.size(); i++)
        {
            int pos = rampeditor_->sliders_[i]->pos().x();
            qreal val = rampeditor_->sliders_[i]->val;
            QString txt = QString::number(val, 'f', rampeditor_->textAcc_);
            if ((pos+fm.width(txt))>width()) pos += -fm.width(txt) + rampeditor_->sliders_[i]->width();
            painter.drawText(pos, 2+fm.height()/2, txt);
        }
    }

    QWidget::paintEvent(e);
}

