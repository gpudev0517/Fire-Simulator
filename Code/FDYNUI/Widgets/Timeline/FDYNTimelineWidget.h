#pragma once

class FDYNTimeline;
class FDYNFrameInfo;
class FDYNTimelineWidgetPrivate;

/*!
  This draws the timeline widget onto NETimeline
*/

class FDYNTimelineWidget : public QWidget
{
    Q_OBJECT
public:
    friend class FDYNTimeline;
    enum NETimelineUnits { Frames, Seconds };

    FDYNTimelineWidget(QWidget* parent);
    ~FDYNTimelineWidget();
    void changeUnits(NETimelineUnits val);

    bool checkFramesTimeline();
    //!
    double playbackStartFrame() const;
    //!
    double playbackEndFrame() const;

public slots:
    //!
    void setPlaybackStartFrame(double frame);
    //!
    void setPlaybackEndFrame(double frame);
    //!
    void setCurrentFrame(double frame);
    //! For setting current frame
    void setCurrentFrame(const QString &frame);
    //! For setting keyframe mars
    void setKeyframeMarks(QList<uint> marks);

protected:
    //! Paint event handler
    void paintEvent(QPaintEvent* event);
    //! Mouse press event handler
    void mousePressEvent(QMouseEvent* event);
    //! Mouse release event handler
    void mouseReleaseEvent(QMouseEvent* event);
    //! Mouse move event handler
    void mouseMoveEvent(QMouseEvent* event);
    //! Computes consecutive bar positions
    bool computeBarPositions(const NEFrameInfo& frameInfo, float& barStartPos, float& barEndPos);

private:
    NE_DECL_PRIVATE(FDYNTimelineWidget)
};
