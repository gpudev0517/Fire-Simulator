#pragma once
#include <QQuickWidget>
#include <QProgressBar>

class FDYNGUIManager;


class FDYNTimeline : public QQuickWidget
{
    Q_OBJECT

public:
    friend class FDYNGUIManager;
    friend class FDYNGUIManagerPrivate;
    FDYNTimeline(QWidget* parent = 0);
    ~FDYNTimeline();
    //NETimelineWidget* getTimelineWidget(){ return timelineWidget; }

    QProgressBar *m_recordingProgress;

    void changeUnits(int val); //0 : Frames, 1: Seconds

    double playbackStartFrame() const;
    double playbackEndFrame() const;

public slots:
    //!
    double playbackStartTimeOrFrame() const;
    //!
    double playbackEndTimeOrFrame() const;
    //!
    void setPlaybackStartFrameFromQML(QVariant start);
    //!
    void setPlaybackEndFrameFromQML(QVariant end);
    //!
    void setSimulationEndFrameFromQML(QVariant end);
    //!
    //void setSimulatedMax(int start, int end);

    //!
    void setPlaybackStartFrame(double start);
    //!
    void setPlaybackEndFrame(double end);
    //!
    void setSimulationEndFrame(double end);

    //! Enables/Disables all playback user interface buttons
    void enablePlaybackUI(bool b);
    //! Enables/Disables all simulation user interface buttons
    void enableSimulationUI(bool b);

    void setOfflineRecordingMode(bool b);

    //! This method is called whenever the user clicks the play button.
    void play(bool b);
    //! This method is called whenever the user clicks the simulate button.
    void simulateButtonPressed(bool b);
    //! This method is called whenever the user clicks the repeat button.
    void repeatButtonPressed(bool b);

    //! Sets the given frame as the current frame in all related widgets
    void setCurrentFrame(double val);
    //! Jumps to the previous frame
    void decrementFrame();
    //! Jumps to the next frame
    void incrementFrame();
    //! Jumps to the first frame
    void goToStartFrame();

    void goToEndFrame();
    //! Sets the current frame using the frame number in the timeline
    void setCurrentFrameFromTimeline();

    double currentFrame();

    void setValueWithTimelineUnits();
    //private:
    //void setCurrentFrame(int val);


private:
    void init();
    QMutex m_locker;

};
