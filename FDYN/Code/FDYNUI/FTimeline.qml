import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "Components"
import "."

Rectangle {
    id: ftimeline
    objectName: "ftimeline"
    width: Window.width
    height: Global.min_height
    color: Global.darkGrey
    property int timelineValue : 0
    property int timelineMinValue : 0
    property int timelineMaxValue : 1000
    property double frameRate: 50
    property int decimals: 2
    property bool isTime: true

    property variant failedTimeValues: []

    property double grooveStep : 50

    function setTimelineValueFromCpp(timelineVal){
        if(isTime)
            timelineValue = timelineVal * frameRate
        else
            timelineValue = timelineVal
    }

    function setFailedTimesFromCpp(_failedTimeValues){
        failedTimeValues = _failedTimeValues
        canvas.requestPaint()
    }

    signal sigTimeValueChanged();
    signal sigStartTimeValueChanged(var _startTime)
    signal sigEndTimeValueChanged(var _endTime)

    Grid {
        width: parent.width; height: parent.height;
        columns: 6;
        spacing: 10

        Rectangle {
            width: 50
            height: 30
            color: Global.darkGrey
            FTimeSpinBox {
                id : spinCurrentTime
                objectName: "spinCurrentTime"
                width: 40
                x:10; y: 7
                factor:frameRate
                decimals: decimals
                isDouble: isTime
                value: timelineValue
                showIndicators: false
                onValueChanged: {
                    if(timelineValue !== spinCurrentTime.value){
                        timelineValue = spinCurrentTime.value
                        sigTimeValueChanged()
                    }

                }
            }
        }

        Slider {
            id : slider_timeline
            minimumValue: timelineMinValue
            maximumValue: timelineMaxValue
            stepSize: grooveStep / 50
            tickmarksEnabled: true
            value: timelineValue
            width: parent.width - 400
            z: 1

            Text {
                id: currentText
                text: {
                    if(isTime)
                        parseFloat(slider_timeline.value / frameRate).toFixed(decimals)
                    else
                        slider_timeline.value
                }
                color: "#fff"
                font.bold: true
                y:10
                x: (slider_timeline.value - slider_timeline.minimumValue) * (slider_timeline.width / (slider_timeline.maximumValue - slider_timeline.minimumValue))
            }

            style: SliderStyle {
                handle: Rectangle {
                    width: 10
                    height: 30
                    color: Global.darkBlue
                    opacity: 0.8
                }

                groove: Rectangle {
                    y: 17
                    height: 8
                    width: parent.width
                    color: Global.darkBlue
                    opacity: 1.0
                }

                tickmarks: Repeater {
                    id: repeater
                    model: slider_timeline.stepSize > 0 ? 1 + (slider_timeline.maximumValue - slider_timeline.minimumValue) / grooveStep: 0

                    Item {
                        Text {
                            width : repeater.width / repeater.count
                            x: styleData.handleWidth / 2 + index * ((repeater.width - styleData.handleWidth) / (repeater.count-1)) - 5
                            height: 20
                            color: Global.lightWhite
                            font.pixelSize: 10
                            text: {
                                if(isTime)
                                    parseFloat((index*grooveStep + slider_timeline.minimumValue)/frameRate).toFixed(decimals)
                                else
                                    index*grooveStep + slider_timeline.minimumValue
                            }
//                            function getText() {
//                                if (index % grooveStep === 0) return ( index + slider_timeline.minimumValue )
//                                else return "";
//                            }
                        }
                        Rectangle {
                            color: Global.lightWhite
                            width: 1
                            height: 12
                            y: 23
                            x: styleData.handleWidth / 2 + index * ((repeater.width - styleData.handleWidth) / (repeater.count-1))
                        }
                    }
                }
            }
            Canvas {
                id: canvas
                x: 0
                y: 0
                width: parent.width
                height: parent.height
                contextType: "2d"
                onPaint: {
                    var ctx = getContext("2d")

                    ctx.reset();
                    for( var i = 0; i < failedTimeValues.length; i++){
                        var failedVar =  (failedTimeValues[i] - slider_timeline.minimumValue) * (slider_timeline.width / (slider_timeline.maximumValue - slider_timeline.minimumValue))
                        failedVar = (width - 10) / width * failedVar + 5
                        ctx.moveTo(failedVar - 5, height/2);
                        ctx.lineTo(failedVar + 5, height/2);
                        ctx.lineTo(failedVar, height-2);
                        ctx.closePath();
                        ctx.fillStyle = Qt.rgba(1, 0, 0, 1)
                        ctx.fill();
                    }
                }

            }


            onValueChanged: {
                if(timelineValue !== slider_timeline.value){
                    timelineValue = slider_timeline.value
                    sigTimeValueChanged()
                }

            }
        }

        Rectangle {
            width: 80
            height: 30
            color: Global.darkGrey
            FTimeSpinBox {
                id : spinStartTime
                objectName:  "spinStartTime"
                x:5; y: 7
                factor:frameRate
                decimals: decimals
                isDouble: isTime
                value: timelineMinValue
                fontColor: Global.lightWhite
                onValueChanged: {
                    grooveStep = (spinLastTime.value - spinStartTime.value) / 20;
                    timelineMinValue = spinStartTime.value
                    sigStartTimeValueChanged(spinStartTime.value)
                    canvas.requestPaint()
                }
            }
        }

        Rectangle {
            width: 70
            height: 30
            color: Global.darkGrey
            FTimeSpinBox {
                id : spinLastTime
                objectName: "spinLastTime"
                y: 7
                factor:frameRate
                decimals: decimals
                isDouble: isTime
                value: timelineMaxValue
                fontColor: Global.lightWhite
                onValueChanged: {
                    grooveStep = (spinLastTime.value - timelineMinValue) / 20;
                    timelineMaxValue = spinLastTime.value
                    sigEndTimeValueChanged(spinLastTime.value)
                    canvas.requestPaint()
                }
            }
        }

//        Text {
//            text: "S"
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignLeft
//            font.bold: true
//            width: 20
//            height: 20
//            color: Global.darkBlue
//        }

        Rectangle {
            width: 130
            height: parent.height; color: Global.darkGrey;

            Grid {
                y: 7; columns: 6; spacing: 7; leftPadding: 2; topPadding: 3; bottomPadding: 3; rightPadding: 2;
                FToolButton { id: btnSim; objectName:"btnSim"; width: 14; height: 14; source: "img/32/sim_button_none_01.png"; checkable:true; switchtype: true  }
                FToolButton { id: btnToStart; objectName:"btnToStart"; width: 14; height: 14; source: "img/32/playback_go_to_start_gray_01.png"; checkable:false; switchtype: false  }
                FToolButton { id: btnStepBackward; objectName:"btnStepBackward"; width: 14; height: 14; source: "img/32/playback_step_backward_gray_01.png"; checkable:false; switchtype: false  }
                //FToolButton { id: btnBackPlay; objectName:"btnBackPlay"; width: 14; height: 14; source: "img/32/playback_play_backward_gray_01.png"; checkable:true; switchtype: true  }
                FToolButton { id: btnPlay; objectName:"btnPlay"; width: 14; height: 14; source: "img/32/playback_play_gray_01.png"; checkable:true; switchtype: true }
                FToolButton { id: btnStepForward; objectName:"btnStepForward"; width: 14; height: 14; source: "img/32/playback_step_forward_gray_01.png"; checkable:false; switchtype: false  }
                FToolButton { id: btnToEnd; objectName:"btnToEnd"; width: 14; height: 14; source: "img/32/playback_go_to_end_gray_01.png"; checkable:false; switchtype: false  }
            }

        }
    }

}

/*##^##
Designer {
    D{i:0;width:1280}
}
##^##*/
