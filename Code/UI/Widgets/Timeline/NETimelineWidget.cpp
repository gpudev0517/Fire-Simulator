#include <QPainter>
#include <QPaintEvent>
#include <QScreen>

#include "NETimelineWidget.h"
#include "Widgets/Timeline/NETimeline.h"
#include "Base/NESceneManager.h"
#include "Managers/NEGUIManager.h"
#include "Managers/NEGUIManager_p.h"
#include "Core/Spatial/NESpatial.h"

class NETimelineWidgetPrivate
{
public:
    NETimelineWidgetPrivate(NETimelineWidget* s) :
        S(s),
        m_currentFrame(0),
        m_mousePressed(false),
        m_Spacing(0.1f),
        m_keyframeMarks(QList<uint>()),
        blue(0, 130, 200, 255),
        green(40, 100, 60, 255),
        orange(203, 92, 0, 255),
        lightRed(205, 92, 92, 255),
        lines(QColor(0, 0, 0)),
        cursor(QColor(255, 128, 0)),
        redPen(QColor(255, 0, 0, 255)),
        m_TimelineUnits(NETimelineWidget::Frames)

    {
        painter = new QPainter();
        lines.setWidth(1);
        cursor.setWidth(1);
        m_TimelineUnits = NETimelineWidget::Frames;
    }

    QPen lines;
    QPen cursor;
    QPen redPen;
    const QColor blue;
    const QColor green;
    const QColor orange;
    const QColor lightRed;
    //! The parent timeline
    NETimeline* m_pTimeline;
    //! The current frame shown in the timeline
    double m_currentFrame;
    //! Last simulation frame
    double m_SimLastFrame;
    //! Keeps whether mouse is currently pressed
    mutable bool m_mousePressed;
    //! Keeps the spacing of the markers in the timeline
    float m_Spacing;
    //! Keyframe marks are kept here
    QList<uint> m_keyframeMarks;
    NETimelineWidget::NETimelineUnits m_TimelineUnits = NETimelineWidget::Frames;
    QPainter* painter;
private:
    NETimelineWidget* S;
};

NETimelineWidget::NETimelineWidget(QWidget* parent) :
    QWidget(parent),
    P(new NETimelineWidgetPrivate(this))
{
    P->m_pTimeline = qobject_cast<NETimeline*>(parent);
}

NETimelineWidget::~NETimelineWidget()
{
    delete P->painter;
    delete P;
}

void NETimelineWidget::changeUnits(NETimelineWidget::NETimelineUnits val)
{
    P->m_TimelineUnits = val;
    repaint();
}

bool NETimelineWidget::checkFramesTimeline()
{
    return P->m_TimelineUnits == NETimelineWidget::Frames;
}

#define MIN_DIFF_TIME 0.01
#define MIN_DIFF_FRAME 1

void NETimelineWidget::paintEvent(QPaintEvent* event)
{
    event->accept();

    double endFrame = playbackEndFrame();
    double startFrame = playbackStartFrame();

	double frameDiff = (endFrame - startFrame);

	
    if(frameDiff <= 0)
        return;

    P->m_Spacing = ((float)(this->width() - 40)) / ((float)frameDiff);
    double drawEvery = MIN_DIFF_FRAME;
	if (!checkFramesTimeline())
	{
		drawEvery = MIN_DIFF_TIME;
	}
    float framePos;


    QScreen *screen = QGuiApplication::primaryScreen();
    qreal dotsPerInch = screen->logicalDotsPerInch();

    float maxSpacingWidth = 25.0f;
    if(frameDiff > 10000)
        maxSpacingWidth += log10(frameDiff - 10000) * 2;

    maxSpacingWidth *= dotsPerInch / 96.0f;

    if(P->m_Spacing > 0.0f)
		while (P->m_Spacing * ((float)drawEvery) < maxSpacingWidth)
		{
			if (checkFramesTimeline())
				drawEvery += MIN_DIFF_FRAME;
			else
				drawEvery += MIN_DIFF_TIME;

			//++drawEvery;
		}
			
            

    P->painter->begin(this);

    // draw range of maximum already simulated frames
    float barStartPos;
    float barEndPos;

    const NEFrameInfo& sceneFrameInfo = NESCENE.FrameInfo();
    if(computeBarPositions(sceneFrameInfo, barStartPos, barEndPos))
    {
        QRectF minSim(barStartPos, this->height() / 2, barEndPos - barStartPos, this->height() / 2 - 3);
        P->painter->fillRect(minSim, P->blue);
    }

    if(NESpatial* currentObject = qobject_cast<NESpatial*> (NESCENE.selectedObject()))
    {
        const NEFrameInfo& currentObjectFrameInfo = currentObject->FrameInfo();
        if(computeBarPositions(currentObjectFrameInfo, barStartPos, barEndPos))
        {
            QRectF minSim(barStartPos, 3, barEndPos - barStartPos, this->height() / 2 - 3);
            P->painter->fillRect(minSim, P->orange);
        }
    }

    QString s;
    // draw scale
    for(double i = 0; i + drawEvery <= frameDiff; i += drawEvery)
    {
        framePos = ((float)(i)) / ((float)frameDiff);
        framePos *= ((float)(this->width() - 40));
        framePos += 20.0f;
        P->painter->drawLine(QPoint(framePos, 15), QPoint(framePos, this->height() - 5));
        switch (P->m_TimelineUnits)
        {
        case Frames:
            P->painter->drawText(QRectF(framePos - ((P->m_Spacing * drawEvery) / 2.0f), 3.0f, P->m_Spacing * drawEvery, 12.0f), Qt::AlignCenter, tr("%1").arg(i + startFrame));
            break;

        case Seconds:
            s = s.sprintf("%.2f", (float)i+ startFrame);
            P->painter->drawText(QRectF(framePos - ((P->m_Spacing * drawEvery) / 2.0f), 3.0f, P->m_Spacing * drawEvery, 12.0f), Qt::AlignCenter, s);
            break;

        default:
            break;
        }


    }
    P->painter->drawLine(QPoint(this->width() - 20, 15), QPoint(this->width() - 20, this->height() - 5));
    switch (P->m_TimelineUnits)
    {
    case Frames:
        P->painter->drawText(QRectF(this->width() - 20 - ((P->m_Spacing * drawEvery) / 2.0f), 3.0f, P->m_Spacing * drawEvery, 12.0f), Qt::AlignCenter, tr("%1").arg(endFrame));
        break;

    case Seconds:
        s = s.sprintf("%.1f", (float)endFrame);
        P->painter->drawText(QRectF(this->width() - 20 - ((P->m_Spacing * drawEvery) / 2.0f), 3.0f, P->m_Spacing * drawEvery, 12.0f), Qt::AlignCenter, s);
        break;

    default:
        break;
    }




    // save current pen and set new pen
    const QPen& currentPen = P->painter->pen();

    P->painter->setPen(P->redPen);
    // draw keyframe marks
    for(int i = startFrame; i <= endFrame; i++)
    {
        if(P->m_keyframeMarks.contains(i))
        {
            framePos = ((float)(i - startFrame)) / ((float)frameDiff);
            framePos *= ((float)(this->width() - 40));
            framePos += 20.0f;
            P->painter->drawLine(QPoint(framePos, 14), QPoint(framePos, this->height() - 4));
        }
    }
    P->painter->setPen(currentPen);

    // draw cursor
    if(P->m_currentFrame >= startFrame && P->m_currentFrame <= endFrame)
    {
        P->painter->setPen(P->cursor);
        framePos = ((float)(P->m_currentFrame - startFrame)) / ((float)frameDiff);
        framePos *= ((float)(this->width() - 40));
        framePos += 20.0f;
        QRectF frameCursor(framePos - 10.0f, 3.0f, 20.0f, this->height() - 6.0f);
        P->painter->drawRect(frameCursor);
        P->painter->fillRect(frameCursor, QColor(255, 255, 0, 64));
    }

    P->painter->end();
}

/*!
*/
double NETimelineWidget::playbackStartFrame() const
{
    if(P->m_pTimeline)
        return P->m_pTimeline->playbackStartFrame();
    else
        return 0;
}

/*!
*/
double NETimelineWidget::playbackEndFrame() const
{
    if(P->m_pTimeline)
        return P->m_pTimeline->playbackEndFrame();
    else
        return 0;
}

void NETimelineWidget::setPlaybackStartFrame(double frame)
{
    //m_PlaybackStartFrame = frame;
    repaint();
}

void NETimelineWidget::setPlaybackEndFrame(double frame)
{
    //m_PlaybackEndFrame = frame;
    repaint();
}

void NETimelineWidget::setCurrentFrame(double frame)
{
    P->m_currentFrame = frame;
    if(P->m_currentFrame <= playbackEndFrame())
        repaint();
}

void NETimelineWidget::setCurrentFrame(const QString& frame)
{
	if (checkFramesTimeline())
		P->m_currentFrame = frame.toInt();
	else
		P->m_currentFrame = frame.toDouble();
    repaint();
}

void NETimelineWidget::setKeyframeMarks(QList<uint> marks)
{
    P->m_keyframeMarks = marks;
    repaint();
}

void NETimelineWidget::mousePressEvent(QMouseEvent* event)
{
    // return if left button not pressed
    if(event->button() != Qt::LeftButton)
        return;

    if(NESCENE.solveMode())
    {
        NEGUI.P->timeline()->playButton->setChecked(false);
        NEGUI.P->timeline()->simulateButton->setChecked(false);
    }
    int xPos = event->x();
    int yPos = event->y();

    // return if border of timeline is hit
    if(xPos < 20 || xPos > this->width() - 20 || yPos < 3 || yPos > this->height() - 3)
        return;

    P->m_mousePressed = true;
    P->m_SimLastFrame = P->m_currentFrame;
    double endFrame = playbackEndFrame();
    double startFrame = playbackStartFrame();
    double frameDiff = endFrame - startFrame;
    float frameNr = ((float)(xPos - 20)) / ((float)(this->width() - 40));
    frameNr *= frameDiff;
    frameNr += startFrame;

	if(checkFramesTimeline())
		P->m_currentFrame =  floor(frameNr + 0.5f);
	else
		P->m_currentFrame = floor(100.f * frameNr + 0.5f) / 100.f;
	if(checkFramesTimeline())
		P->m_pTimeline->currentFrameLineEdit->setText(tr("%1").arg(P->m_currentFrame));
	else {
		QString strCurrntFrame;
		strCurrntFrame = strCurrntFrame.sprintf("%.2f", P->m_currentFrame);
		P->m_pTimeline->currentFrameLineEdit->setText(strCurrntFrame);
	}
	
    switch (P->m_TimelineUnits)
    {
    case Frames:
        NESCENE.SetCurrentFrame(P->m_currentFrame);
        break;

    case Seconds:
        NESCENE.SetCurrentFrame(P->m_currentFrame * NESCENE.RecordFrameRate());
        break;

    default:
        break;
    }

    repaint();
}

void NETimelineWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() != Qt::LeftButton)
        return;

    P->m_mousePressed = false;
    switch (P->m_TimelineUnits)
    {
    case Frames:
        NESCENE.SetCurrentFrame(P->m_currentFrame);
        break;

    case Seconds:
        NESCENE.SetCurrentFrame(P->m_currentFrame * NESCENE.RecordFrameRate());
        break;

    default:
        break;
    }
    NEGUI.updatePropertyEditor();
	

    if (checkFramesTimeline())
		P->m_pTimeline->currentFrameLineEdit->setText(tr("%1").arg(P->m_currentFrame));
	else {
		QString strCurrntFrame;
		strCurrntFrame = strCurrntFrame.sprintf("%.2f", P->m_currentFrame);
		P->m_pTimeline->currentFrameLineEdit->setText(strCurrntFrame);
	}

}

void NETimelineWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(!P->m_mousePressed)
        return;

    int xPos = event->x();

    if(xPos < 20)
    {
        xPos = 20;
		
		if(checkFramesTimeline())
			P->m_pTimeline->currentFrameLineEdit->setText(tr("%1").arg(P->m_currentFrame));
		else {
			QString strCurrntFrame;
			strCurrntFrame = strCurrntFrame.sprintf("%.2f", P->m_currentFrame);
			P->m_pTimeline->currentFrameLineEdit->setText(strCurrntFrame);
		}
        switch (P->m_TimelineUnits)
        {
        case Frames:
            NESCENE.SetCurrentFrame(P->m_currentFrame);
            break;

        case Seconds:
            NESCENE.SetCurrentFrame(P->m_currentFrame * NESCENE.RecordFrameRate());
            break;

        default:
            break;
        }
        repaint();
    }
    else if(xPos > this->width() - 20)
    {
        xPos = this->width() - 20;
		
		if(checkFramesTimeline())
			P->m_pTimeline->currentFrameLineEdit->setText(tr("%1").arg(P->m_currentFrame));
		else {
			QString strCurrntFrame;
			strCurrntFrame = strCurrntFrame.sprintf("%.2f", P->m_currentFrame);
			P->m_pTimeline->currentFrameLineEdit->setText(strCurrntFrame);
		}
        switch (P->m_TimelineUnits)
        {
        case Frames:
            NESCENE.SetCurrentFrame(P->m_currentFrame);
            break;

        case Seconds:
            NESCENE.SetCurrentFrame(P->m_currentFrame * NESCENE.RecordFrameRate());
            break;

        default:
            break;
        }
        repaint();
    }


    int endFrame = playbackEndFrame();
    int startFrame = playbackStartFrame();

    int frameDiff = endFrame - startFrame;
    float frameNr = ((float)(xPos - 20)) / ((float)(this->width() - 40));
    frameNr *= frameDiff;
    frameNr += startFrame;

    //P->m_currentFrame =  floor(frameNr + 0.5f);
	if (checkFramesTimeline())
		P->m_currentFrame = floor(frameNr + 0.5f);
	else
		P->m_currentFrame = floor(100.f * frameNr + 0.5f) / 100.f;
	if(checkFramesTimeline())
		P->m_pTimeline->currentFrameLineEdit->setText(tr("%1").arg(P->m_currentFrame));
	else {
		QString strCurrntFrame;
		strCurrntFrame = strCurrntFrame.sprintf("%.2f", P->m_currentFrame);
		P->m_pTimeline->currentFrameLineEdit->setText(strCurrntFrame);
	}
    switch (P->m_TimelineUnits)
    {
    case Frames:
        NESCENE.SetCurrentFrame(P->m_currentFrame);
        break;

    case Seconds:
        NESCENE.SetCurrentFrame(P->m_currentFrame * NESCENE.RecordFrameRate());
        break;

    default:
        break;
    }
    repaint();
}


/*!
        Computes start and end positions in the timeline for minimum and maximum frame provided by frame info structure of the object or scene.
*/
bool NETimelineWidget::computeBarPositions(const NEFrameInfo& frameInfo, float& barStartPos, float& barEndPos)
{
    int endFrame = playbackEndFrame();
    int startFrame = playbackStartFrame();

    int firstFrame = frameInfo.firstFrame();
    int lastFrame = frameInfo.lastFrame();
    if(P->m_TimelineUnits == Seconds)
    {
        firstFrame = firstFrame / NESCENE.RecordFrameRate();
        lastFrame = lastFrame / NESCENE.RecordFrameRate();
    }

    if(!(firstFrame < startFrame && lastFrame < startFrame)
            || !(firstFrame > endFrame && lastFrame > endFrame))
    {
        // determine start of the bar
        if(firstFrame <= startFrame)
        {
            if(lastFrame >= startFrame)
            {
                // recording start is outside
                barStartPos = 10.0f;
            }
        }
        else
        {
            if(firstFrame <= endFrame)
            {
                // recording start is inside
                barStartPos = ((float)(firstFrame - startFrame));
                barStartPos *= P->m_Spacing;
                barStartPos += 20.0f;
            }
        }

        // determine end of the bar
        if(lastFrame >= endFrame)
        {
            // recording end is outside
            if(firstFrame <= endFrame)
            {
                barEndPos = this->width() - 10;
            }
        }
        else
        {
            // recording is inside
            if(lastFrame >= startFrame)
            {
                barEndPos = ((float)(lastFrame - startFrame));
                barEndPos *= P->m_Spacing;
                barEndPos += 20.0f;
            }
        }

        return true;
    }
    else
        return false;
}
