#include <QQuickView>
#include <QtQuickControls2>

#include "FDYNTimeline.h"
//#include "FDYNTimelineWidget.h"
#include "Managers/FDYNGUIManager.h"
#include "Base/NESceneManager.h"
#include "Spatial/NESpatial.h"

FDYNTimeline::FDYNTimeline( QWidget* parent /*= 0*/ ) : QQuickWidget(parent)
{
    setSource(QUrl("qrc:/FTimeline.qml"));
    setResizeMode(QQuickWidget::SizeViewToRootObject);

    QObject* btnPlay = rootObject()->findChild<QObject *>("btnPlay");
    QObject::connect(btnPlay, SIGNAL(onToggled(bool)), this, SLOT(play(bool)));
    QObject::connect(btnPlay, SIGNAL(onToggled(bool)), &FDYNGUI, SLOT(playToggled(bool)));

    QObject* btnSim = rootObject()->findChild<QObject *>("btnSim");
    QObject::connect(btnSim, SIGNAL(onToggled(bool)), this, SLOT(simulateButtonPressed(bool)));

    QObject* btnToStart = rootObject()->findChild<QObject *>("btnToStart");
    QObject::connect(btnToStart, SIGNAL(onButtonClicked()), this, SLOT(goToStartFrame()));

    QObject* btnStepBackward = rootObject()->findChild<QObject *>("btnStepBackward");
    QObject::connect(btnStepBackward, SIGNAL(onButtonClicked()), this, SLOT(decrementFrame()));

    QObject* btnToEnd = rootObject()->findChild<QObject *>("btnToEnd");
    QObject::connect(btnToEnd, SIGNAL(onButtonClicked()), this, SLOT(goToEndFrame()));

    QObject* btnStepForward = rootObject()->findChild<QObject *>("btnStepForward");
    QObject::connect(btnStepForward, SIGNAL(onButtonClicked()), this, SLOT(incrementFrame()));


    QObject* spinCurrentTime = rootObject()->findChild<QObject *>("spinCurrentTime");
    QObject::connect(spinCurrentTime, SIGNAL(sigEditingFinished()), this, SLOT(setCurrentFrameFromTimeline()));
    QObject::connect(rootObject(), SIGNAL(sigTimeValueChanged()), this, SLOT(setCurrentFrameFromTimeline()));

    m_recordingProgress = new QProgressBar(this);

    QString uiStyle =
            "QProgressBar::chunk {"
                "background-color: #2c53d7;"
                "width: 20px;}"
            "QProgressBar {"
                "background-color: #282629;"
                "border-radius: 5px;"
                "color: white;"
                "text-align: center;}"
    ;

    m_recordingProgress->setStyleSheet(uiStyle);

    m_recordingProgress->setHidden(true);

#if 0

    //Set the input range of the timeline
    currentFrameLineEdit->setValidator(new QIntValidator(0, 999999, currentFrameLineEdit));
    currentFrameLineEdit->insert(0);
    currentFrameLineEdit->setAlignment(Qt::AlignRight);

    //make connections
    connect(playButton, &QAbstractButton::toggled, this, &FDYNTimeline::play);
    connect(playButton, &QToolButton::toggled, &FDYNGUI, &FDYNGUIManager::playToggled);
    connect(nextFrameButton, &QAbstractButton::clicked, this, &FDYNTimeline::incrementFrame);
    connect(previousFrameButton, &QAbstractButton::clicked, this, &FDYNTimeline::decrementFrame);
    connect(simulateButton, &QAbstractButton::toggled, this, &FDYNTimeline::simulateButtonPressed);
    connect(repeatButton, &QAbstractButton::toggled, this, &FDYNTimeline::repeatButtonPressed);
    connect(gotoStartButton, &QAbstractButton::clicked, this, &FDYNTimeline::goToStartFrame);
    connect(gotoEndButton, &QAbstractButton::clicked, this, &FDYNTimeline::goToEndFrame);
    connect(currentFrameLineEdit, &QLineEdit::editingFinished, this, &FDYNTimeline::setCurrentFrameFromTimeline);

#endif
}

FDYNTimeline::~FDYNTimeline()
{

}

void FDYNTimeline::changeUnits(int val)
{
    rootObject()->setProperty("isTime", (bool)val);
    rootObject()->setProperty("frameRate", NESCENE.RecordFrameRate());
}

double FDYNTimeline::playbackStartFrame() const
{
    QObject* spinStartTime = rootObject()->findChild<QObject *>("spinStartTime");
    double val = spinStartTime->property("value").value<double>();
    return val;

}

double FDYNTimeline::playbackEndFrame() const
{
    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    double val = spinLastTime->property("value").value<double>();
    return val;
}

void FDYNTimeline::setOfflineRecordingMode(bool b)
{
    m_locker.lock();
    m_recordingProgress->setValue(0);
    m_recordingProgress->setHidden(!b);
    m_recordingProgress->setGeometry(this->rect());
    m_locker.unlock();
}

double FDYNTimeline::playbackStartTimeOrFrame() const
{
#if 0
    return playbackStartSpinBox->value();
#endif
    QObject* spinStartTime = rootObject()->findChild<QObject *>("spinStartTime");
    double val = spinStartTime->property("value").value<double>();
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds){
        val = val / NESCENE.RecordFrameRate();
    }
    return val;

}

double FDYNTimeline::playbackEndTimeOrFrame() const
{
#if 0
    return playbackEndSpinBox->value();
#endif
    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    double val = spinLastTime->property("value").value<double>();
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds){
        val = val / NESCENE.RecordFrameRate();
    }
    return val;
}

void FDYNTimeline::enablePlaybackUI(bool b)
{	
#if 0
    //simulate button is toggled
    //disable buttons for going back in time

    previousFrameButton->setEnabled( !b );
    gotoStartButton->setEnabled( !b );

    //also disable go to end
    gotoEndButton->setEnabled( !b );

    //disable the frame edit
    currentFrameLineEdit->setEnabled( !b );
#endif
    QObject* btnStepBackward = rootObject()->findChild<QObject *>("btnStepBackward");
    btnStepBackward->setProperty("enabled", !b);

    QObject* btnToStart = rootObject()->findChild<QObject *>("btnToStart");
    btnToStart->setProperty("enabled", !b);

    QObject* btnToEnd = rootObject()->findChild<QObject *>("btnToEnd");
    btnToEnd->setProperty("enabled", !b);

    QObject* spinCurrentTime = rootObject()->findChild<QObject *>("spinCurrentTime");
    spinCurrentTime->setProperty("enabled", !b);


}

void FDYNTimeline::enableSimulationUI(bool b)
{
#if 0
    //en-/disable simulation button
    simulateButton->setEnabled(!b);
#endif

    QObject* btnSim = rootObject()->findChild<QObject *>("btnSim");
    btnSim->setProperty("enabled", !b);

}

void FDYNTimeline::setCurrentFrame( double val )
{
#if 0
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
    //FDYNGUI.updatePropertyEditor();
#endif
    QObject *timelineView = rootObject();
    QVariant returnValue;
    QMetaObject::invokeMethod(timelineView, "setTimelineValueFromCpp", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, val));
}

double FDYNTimeline::currentFrame()
{
//    if(timelineWidget->checkFramesTimeline())
//        return currentFrameLineEdit->text().toInt();
//    else
//        return currentFrameLineEdit->text().toDouble();

    QObject* spinCurrentTime = rootObject()->findChild<QObject*>("spinCurrentTime");
    double val = spinCurrentTime->property("value").value<int>();
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds)
        return val / NESCENE.RecordFrameRate();
    else
        return val;
    //return 0.0;

}

void FDYNTimeline::init()
{
#if 0
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
#endif
    connect(rootObject(), SIGNAL(sigStartTimeValueChanged(QVariant)), this, SLOT(setPlaybackStartFrameFromQML(QVariant)));
    connect(rootObject(), SIGNAL(sigEndTimeValueChanged(QVariant)), this, SLOT(setPlaybackEndFrameFromQML(QVariant)));
    connect(rootObject(), SIGNAL(sigEndTimeValueChanged(QVariant)), this, SLOT(setSimulationEndFrameFromQML(QVariant)));
    setValueWithTimelineUnits();
}


/*!
        Triggered when the user changes the playback start spin box or the program changes it.
*/
void FDYNTimeline::setPlaybackStartFrameFromQML(QVariant start)
{
#if 0
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
#endif
    double value = start.value<double>();

    // Set the minimum of the end box, s.t. it can not be smaller or equal than start
    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    spinLastTime->setProperty("from", value+1);

    // Synchronize the spin box
    QObject* spinStartTime = rootObject()->findChild<QObject *>("spinStartTime");
    if(spinStartTime->property("value") != start)
        spinStartTime->setProperty("value", start);

    if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames)
        NESCENE.SetPlaybackStartFrame(value);
    else
        NESCENE.SetPlaybackStartTime(value/NESCENE.RecordFrameRate());
    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::setPlaybackEndFrameFromQML(QVariant end)
{
#if 0
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
#endif
    double value = end.value<double>();

    //set the maximum of the start box, s.t. it can not be larger or equal than end
    QObject* spinStartTime = rootObject()->findChild<QObject *>("spinStartTime");
    spinStartTime->setProperty("to", value-1);

    //synchronize the spin box
    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    if(spinLastTime->property("value") != end)
        spinLastTime->setProperty("value", end);

    if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames)
        NESCENE.SetPlaybackEndFrame(value);
    else
        NESCENE.SetPlaybackEndTime(value/NESCENE.RecordFrameRate());

    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::setSimulationEndFrameFromQML(QVariant end)
{
#if 0
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
#endif
    double value = end.value<double>();

    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    if(spinLastTime->property("value") != end)
        spinLastTime->setProperty("value", end);

     if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames)
        NESCENE.SetSimulationEndFrame(value);
    else
        NESCENE.SetSimulationEndTime(value/NESCENE.RecordFrameRate());

    FDYNGUI.updatePropertyEditor();
}

/*!
        Triggered when the user changes the playback start spin box or the program changes it.
*/
void FDYNTimeline::setPlaybackStartFrame(double start)
{
#if 0
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
#endif
    double value = start;
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds)
        value = value * NESCENE.RecordFrameRate();

    // Set the minimum of the end box, s.t. it can not be smaller or equal than start
    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    spinLastTime->setProperty("from", value+1);

    // Synchronize the spin box
    QObject* spinStartTime = rootObject()->findChild<QObject *>("spinStartTime");
    if(spinStartTime->property("value") != value)
        spinStartTime->setProperty("value", value);

    if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames)
        NESCENE.SetPlaybackStartFrame(start);
    else
        NESCENE.SetPlaybackStartTime(start);
    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::setPlaybackEndFrame(double end)
{
#if 0
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
#endif
    double value = end;
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds)
        value = value * NESCENE.RecordFrameRate();

    //set the maximum of the start box, s.t. it can not be larger or equal than end
    QObject* spinStartTime = rootObject()->findChild<QObject *>("spinStartTime");
    spinStartTime->setProperty("to", value-1);

    //synchronize the spin box
    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    if(spinLastTime->property("value") != value)
        spinLastTime->setProperty("value", value);

    if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames)
        NESCENE.SetPlaybackEndFrame(end);
    else
        NESCENE.SetPlaybackEndTime(end);

    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::setSimulationEndFrame(double end)
{
#if 0
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
#endif
    double value = end;
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds)
        value = value * NESCENE.RecordFrameRate();

    QObject* spinLastTime = rootObject()->findChild<QObject *>("spinLastTime");
    if(spinLastTime->property("value") != value)
        spinLastTime->setProperty("value", value);

     if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames)
        NESCENE.SetSimulationEndFrame(end);
    else
        NESCENE.SetSimulationEndTime(end);

    FDYNGUI.updatePropertyEditor();
}

/*!
*/
void FDYNTimeline::play(bool b)
{
#if 0
    //NEGUI.enablePlaybackWidgets(b);
    //enable and disable simulation related buttons
    enableSimulationUI(b);
    //start or stop the timer which triggers the scene update

    //NESCENE.startSimulation();

    NESCENE.startUpdateTimer(b);
#endif

    //FDYNGUI.enablePlaybackWidgets(b);
    //enable and disable simulation related buttons
    enableSimulationUI(b);
    //start or stop the timer which triggers the scene update

    //NESCENE.startSimulation();

    NESCENE.startUpdateTimer(b);
}

void FDYNTimeline::simulateButtonPressed(bool b)
{
#if 0
    if( NESCENE.isUpdating() )
    {
        simulateButton->setEnabled( true );
    }
    enablePlaybackUI(b);
    //change simulation flag
    NESCENE.setSolveMode(b);
#endif

    if( NESCENE.isUpdating() )
    {
        QObject* btnSim = rootObject()->findChild<QObject *>("btnSim");
        btnSim->setProperty("enabled", true);
    }
    enablePlaybackUI(b);
    //change simulation flag
    NESCENE.setSolveMode(b);
}

void FDYNTimeline::repeatButtonPressed(bool b)
{
    NESCENE.setRepeatFlag(b);
}

/*!
        This method is only called if the current frame is changed by the program.
        For synchronizing with the user interface.
*/
void FDYNTimeline::setCurrentFrameFromTimeline()
{
#if 0
    double val = currentFrame();
    timelineWidget->setCurrentFrame(val);
    if(!timelineWidget->checkFramesTimeline())
        val = val * NESCENE.RecordFrameRate();

    NESCENE.SetCurrentFrame(val);
    NEGUI.updatePropertyEditor();
#endif

    double val = currentFrame();
    if(FDYNGUI.TimelineUnits() == FDYNGUIManager::Seconds)
        val = val * NESCENE.RecordFrameRate();

    NESCENE.SetCurrentFrame(val);
    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::incrementFrame()
{	
    NESCENE.incrementFrame();
    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::decrementFrame()
{
    NESCENE.decrementFrame();
    FDYNGUI.updatePropertyEditor();
}

void FDYNTimeline::goToStartFrame()
{
#if 0
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
#endif

    if(NESCENE.solveMode()){
        QObject* btnSim = rootObject()->findChild<QObject *>("btnSim");
        btnSim->setProperty("checked", false);
    }

    int frameNr;/* = playbackStartSpinBox->value();	*/

    const NEFrameInfo& sceneFrameInfo = NESCENE.FrameInfo();
    frameNr = sceneFrameInfo.firstFrame();

    NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode());
    if(spatial)
        frameNr = spatial->FrameInfo().firstFrame();

    if(frameNr < 0)
        frameNr = 0;

    if(playbackStartTimeOrFrame() > frameNr)
        frameNr = playbackStartTimeOrFrame();

    setCurrentFrame(frameNr);
    setCurrentFrameFromTimeline();
}

void FDYNTimeline::goToEndFrame()
{
#if 0
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
#endif
    int frameNr; /*= playbackEndSpinBox->value();*/

    const NEFrameInfo& sceneFrameInfo = NESCENE.FrameInfo();
    frameNr = sceneFrameInfo.lastFrame();

    NESpatial* spatial = qobject_cast<NESpatial*>(NESCENE.selectedNode());
    if(spatial)
        frameNr = spatial->FrameInfo().lastFrame();

    if(playbackEndTimeOrFrame() < frameNr)
        frameNr = playbackEndTimeOrFrame();

    setCurrentFrame(frameNr);
    setCurrentFrameFromTimeline();
}

void FDYNTimeline::setValueWithTimelineUnits()
{
#if 0
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
#endif
    if( FDYNGUI.TimelineUnits() == FDYNGUIManager::Frames )
    {
        setPlaybackStartFrame( NESCENE.PlaybackStartFrame() );
        setPlaybackEndFrame( NESCENE.PlaybackEndFrame() );
        setSimulationEndFrame(NESCENE.SimulationEndFrame());
        setCurrentFrame( NESCENE.CurrentFrame() );
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

        repaint();
    }

}
