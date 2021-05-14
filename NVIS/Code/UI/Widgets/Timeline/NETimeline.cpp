#include "NETimeline.h"
#include "NETimelineWidget.h"
#include "Managers/NEGUIManager.h"
#include "Base/NESceneManager.h"
#include "Spatial/NESpatial.h"

NETimeline::NETimeline( QWidget* parent /*= 0*/ ) : QWidget(parent)
{
    setupUi(this);


    //Set the input range of the timeline
    currentFrameLineEdit->setValidator(new QIntValidator(0, 999999, currentFrameLineEdit));
    currentFrameLineEdit->insert(0);
    currentFrameLineEdit->setAlignment(Qt::AlignRight);

    //make connections
    connect(playButton, &QAbstractButton::toggled, this, &NETimeline::play);
    connect(playButton, &QToolButton::toggled, &NEGUI, &NEGUIManager::playToggled);
    connect(nextFrameButton, &QAbstractButton::clicked, this, &NETimeline::incrementFrame);
    connect(previousFrameButton, &QAbstractButton::clicked, this, &NETimeline::decrementFrame);
    connect(simulateButton, &QAbstractButton::toggled, this, &NETimeline::simulateButtonPressed);
    connect(repeatButton, &QAbstractButton::toggled, this, &NETimeline::repeatButtonPressed);
    connect(gotoStartButton, &QAbstractButton::clicked, this, &NETimeline::goToStartFrame);
    connect(gotoEndButton, &QAbstractButton::clicked, this, &NETimeline::goToEndFrame);
    connect(currentFrameLineEdit, &QLineEdit::editingFinished, this, &NETimeline::setCurrentFrameFromTimeline);

    m_recordingProgress = new QProgressBar(this);
    m_recordingProgress->setHidden(true);

}

NETimeline::~NETimeline()
{

}

double NETimeline::playbackStartFrame() const
{
    return playbackStartSpinBox->value();
}

double NETimeline::playbackEndFrame() const
{
    return playbackEndSpinBox->value();
}

void NETimeline::setOfflineRecordingMode(bool b)
{
    m_locker.lock();
    m_recordingProgress->setValue(0);
    m_recordingProgress->setHidden(!b);
    m_recordingProgress->setGeometry(this->rect());
    m_locker.unlock();
}

void NETimeline::enablePlaybackUI(bool b)
{	
    //simulate button is toggled
    //disable buttons for going back in time

    previousFrameButton->setEnabled( !b );
    gotoStartButton->setEnabled( !b );

    //also disable go to end
    gotoEndButton->setEnabled( !b );

    //disable the frame edit
    currentFrameLineEdit->setEnabled( !b );

}

void NETimeline::enableSimulationUI(bool b)
{
    //en-/disable simulation button
    simulateButton->setEnabled(!b);
}

void NETimeline::setCurrentFrame( double val )
{
	if(timelineWidget->checkFramesTimeline())
	{
		currentFrameLineEdit->setValidator(new QIntValidator(0, 999999, currentFrameLineEdit));
	}
	else
	{
		currentFrameLineEdit->setValidator(new QDoubleValidator(0, 999999, 2, currentFrameLineEdit));
	}
	
    currentFrameLineEdit->setText(QString::number(val));
    timelineWidget->setCurrentFrame(val);
    //NEGUI.updatePropertyEditor();
}

double NETimeline::currentFrame()
{
	if(timelineWidget->checkFramesTimeline())
		return currentFrameLineEdit->text().toInt();
	else
		return currentFrameLineEdit->text().toDouble();

}

void NETimeline::init()
{
    // Connect timeline's reset button to the the related function of the scene manager
    connect(resetButton, &QToolButton::clicked, &NESCENE, &NESceneManager::resetAndCleanScene);

    // Set min and max frame values
    maxFramesSpinBox->setValue( 1000 );
    playbackEndSpinBox->setValue( 1000 );
    connect(playbackStartSpinBox, (void(QDoubleSpinBox::*)(double))(&QDoubleSpinBox::valueChanged), this, &NETimeline::setPlaybackStartFrame);
    connect(playbackEndSpinBox, (void(QDoubleSpinBox::*)(double))(&QDoubleSpinBox::valueChanged), this, &NETimeline::setPlaybackEndFrame);
    connect(maxFramesSpinBox, (void(QDoubleSpinBox::*)(double))(&QDoubleSpinBox::valueChanged), this, &NETimeline::setSimulationEndFrame);
    connect(currentFrameLineEdit, &QLineEdit::textChanged, timelineWidget, (void(NETimelineWidget::*)(const QString &))(&NETimelineWidget::setCurrentFrame));
    setValueWithTimelineUnits();
}


/*!
        Triggered when the user changes the playback start spin box or the program changes it.
*/
void NETimeline::setPlaybackStartFrame(double start)
{
    // Set the minimum of the end box, s.t. it can not be smaller or equal than start
    playbackEndSpinBox->setMinimum(start+1);

    double value = start;
//     if(!timelineWidget->checkFramesTimeline())
//         value = start * NESCENE.RecordFrameRate();
    // Synchronize the spin box
    if(playbackStartSpinBox->value() != start)
        playbackStartSpinBox->setValue(start);
    timelineWidget->repaint();
	if( timelineWidget->checkFramesTimeline())
		NESCENE.SetPlaybackStartFrame(value);
	else
		NESCENE.SetPlaybackStartTime(value);
	NEGUI.updatePropertyEditor();
}

void NETimeline::setPlaybackEndFrame(double end)
{
    //set the maximum of the start box, s.t. it can not be larger or equal than end
    playbackStartSpinBox->setMaximum(end-1);
    //synchronize the spin box
    double value = end;
//     if(!timelineWidget->checkFramesTimeline())
//         value = end * NESCENE.RecordFrameRate();
    if(playbackEndSpinBox->value() != end)
        playbackEndSpinBox->setValue(end);
    timelineWidget->repaint();
	if( timelineWidget->checkFramesTimeline())
		NESCENE.SetPlaybackEndFrame(value);
	else
		NESCENE.SetPlaybackEndTime(value);

	NEGUI.updatePropertyEditor();
}

void NETimeline::setSimulationEndFrame(double end)
{
    double value = end;
//     if(!timelineWidget->checkFramesTimeline())
//         value = end * NESCENE.RecordFrameRate();
    if(maxFramesSpinBox->value() != end)
        maxFramesSpinBox->setValue(end);
    timelineWidget->repaint();
	if( timelineWidget->checkFramesTimeline())
		NESCENE.SetSimulationEndFrame(value);
	else
		NESCENE.SetSimulationEndTime(value);

	NEGUI.updatePropertyEditor();
}

/*!
*/
void NETimeline::play(bool b)
{
    //NEGUI.enablePlaybackWidgets(b);
    //enable and disable simulation related buttons
    enableSimulationUI(b);
    //start or stop the timer which triggers the scene update

    //NESCENE.startSimulation();

    NESCENE.startUpdateTimer(b);
}

void NETimeline::simulateButtonPressed(bool b)
{
    if( NESCENE.isUpdating() )
    {
        simulateButton->setEnabled( true );
    }
    enablePlaybackUI(b);
    //change simulation flag
    NESCENE.setSolveMode(b);
}

void NETimeline::repeatButtonPressed(bool b)
{
    NESCENE.setRepeatFlag(b);
}

/*!
        This method is only called if the current frame is changed by the program.
        For synchronizing with the user interface.
*/
void NETimeline::setCurrentFrameFromTimeline()
{
    double val = currentFrame();
    timelineWidget->setCurrentFrame(val);
    if(!timelineWidget->checkFramesTimeline())
        val = val * NESCENE.RecordFrameRate();

    NESCENE.SetCurrentFrame(val);
    NEGUI.updatePropertyEditor();
}

void NETimeline::incrementFrame()
{	
    NESCENE.incrementFrame();
    NEGUI.updatePropertyEditor();
}

void NETimeline::decrementFrame()
{
    NESCENE.decrementFrame();
    NEGUI.updatePropertyEditor();
}

void NETimeline::goToStartFrame()
{
    //
    if(NESCENE.solveMode())
        simulateButton->setChecked(false);

    int frameNr;/* = playbackStartSpinBox->value();	*/

    const NEFrameInfo& sceneFrameInfo = NESCENE.FrameInfo();
    frameNr = sceneFrameInfo.firstFrame();

    NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode());
    if(spatial)
        frameNr = spatial->FrameInfo().firstFrame();

    if(frameNr < 0)
        frameNr = 0;

    if(playbackStartFrame() > frameNr)
        frameNr = playbackStartFrame();

    setCurrentFrame(frameNr);
    setCurrentFrameFromTimeline();
}

void NETimeline::goToEndFrame()
{
    int frameNr; /*= playbackEndSpinBox->value();*/

    const NEFrameInfo& sceneFrameInfo = NESCENE.FrameInfo();
    frameNr = sceneFrameInfo.lastFrame();

    NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode());
    if(spatial)
        frameNr = spatial->FrameInfo().lastFrame();

    if(playbackEndFrame() < frameNr)
        frameNr = playbackEndFrame();

    setCurrentFrame(frameNr);
    setCurrentFrameFromTimeline();
}

void NETimeline::setValueWithTimelineUnits()
{
    if( timelineWidget->checkFramesTimeline() )
    {
        setPlaybackStartFrame( NESCENE.PlaybackStartFrame() );
        setPlaybackEndFrame( NESCENE.PlaybackEndFrame() );
		setSimulationEndFrame(NESCENE.SimulationEndFrame());
        setCurrentFrame( NESCENE.CurrentFrame() );
		playbackStartSpinBox->setDecimals(0);
		playbackEndSpinBox->setDecimals(0);
		maxFramesSpinBox->setDecimals(0);
		repaint();
    }
    else
    {
		setPlaybackStartFrame(NESCENE.PlaybackStartTime());
		setPlaybackEndFrame(NESCENE.PlaybackEndTime());
        setSimulationEndFrame(NESCENE.SimulationEndTime());

        //
        // Since timeline is set to seconds we need to change the
        // current frame value which now in this case will represent seconds.
        //
        setCurrentFrame( NESCENE.CurrentFrame() / NESCENE.RecordFrameRate());

        playbackStartSpinBox->setDecimals( 2 );
        playbackEndSpinBox->setDecimals( 2 );
        maxFramesSpinBox->setDecimals( 2 );
		repaint();
    }
}
