#pragma once

#include <QProgressBar>
#include <QMutex>
#include "ui_NETimeline.h"

class NEGUIManager;

class NETimeline : public QWidget, public Ui::NETimeline
{
	Q_OBJECT

public:
	friend class NEGUIManager;
    friend class NEGUIManagerPrivate;
    NETimeline(QWidget* parent = 0);
	~NETimeline();
	//NETimelineWidget* getTimelineWidget(){ return timelineWidget; }

    QProgressBar *m_recordingProgress;

public slots:
	//!
	double playbackStartFrame() const;
	//!
	double playbackEndFrame() const;
	//!
	void setPlaybackStartFrame(double start);
	//!
	void setPlaybackEndFrame(double end);
	//!
	void setSimulationEndFrame(double end);
	//!
	//void setSimulatedMax(int start, int end);

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
