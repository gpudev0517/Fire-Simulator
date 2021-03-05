import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4

Row {
    id: row_timeline
    x: 2;
    y: 641;
    width: parent.width;
    height: 45;
    anchors.bottom: parent.bottom;
    anchors.bottomMargin: 0

    Rectangle {
        id: rect_main_timeline
        width: row_timeline.width
        height: row_timeline.height
        color: darkGrey

        Slider {
            id : slider_timeline
            minimumValue: 0
            maximumValue: 120
            stepSize: 1
            tickmarksEnabled: true

            width: parent.width * 0.8

            style: SliderStyle {
                handle: Rectangle {
                    width: 10
                    height: 30
                    color: darkBlue
                    opacity: 0.6
                }
                groove: Rectangle {
                    y: 18
                    height: 5
                    width: parent.width
                    color: darkBlue
                    opacity: 1.0
                }

                tickmarks: Repeater {
                    id: repeater
                    model: control.stepSize > 0 ? 1 + (control.maximumValue - control.minimumValue) / control.stepSize : 0

                    Item {
                        Text {
                            y: 5
                            width : repeater.width / repeater.count
                            x: width * index
                            height: 30
                            color: lightWhite
                            font.pixelSize: 12
                            text: getText()

                            function getText() {
                                if (index %5 === 0) return index
                                else return "";
                            }
                        }
                        Rectangle {
                            color: lightWhite
                            width: index %5 === 0 ? 1 : 0
                            height: 7
                            y: repeater.height
                            x: styleData.handleWidth / 2 + index * ((repeater.width - styleData.handleWidth) / (repeater.count-1))
                        }
                    }
                }
            }
        }

        Text {
            width: parent.width * 0.8 + 40
            height: 30
            color: "#f6f4f7"
            text: slider_timeline.value
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.bold: true
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            width: parent.width * 0.8 + 100
            height: 30
            color: lightWhite
            text: slider_timeline.minimumValue + " - " + slider_timeline.maximumValue
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.bold: true
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            width: parent.width * 0.8 + 150
            height: 30
            color: darkBlue
            text: "SIM"
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.bold: true
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }
    }
}
