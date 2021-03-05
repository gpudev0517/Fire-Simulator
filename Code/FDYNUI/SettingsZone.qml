import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle {
    property string title: "SETTINGS"
    id: rect_settings
    color: lightGrey
    height: left_bottom_rect.height
    anchors.fill: parent

    Rectangle {
        id: rectangle1
        width: tab_settings.width
        height: rect_settings.height
        color: lightGrey

        Text {
            x: 72
            width: 108
            height: 32
            color: "#f6f4f7"
            text: qsTr("Selected_Item")
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: true
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Text {
            y: 30
            width: 128
            height: 18
            color: "#f6f4f7"
            text: qsTr("Buoyancy")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Slider {
            id: sliderBuo
            y: 50
            height: 22
            stepSize: 0.01
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 14
            value: 0.8

            background: Rectangle {
                x: parent.leftPadding
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 2
                width: parent.availableWidth
                height: implicitHeight
                radius: 1
                color: lightWhite

                Rectangle {
                    width: parent.visualPosition * parent.width
                    height: parent.height
                    color: lightWhite
                    radius: 1
                }
            }

            handle: Rectangle {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 15
                implicitHeight: 15
                radius: 15
                color: parent.pressed ? darkBlue : lightGrey
                border.color: lightWhite
            }
        }

        Text {
            y: 30
            width: 54
            height: 18
            color: "#f6f4f7"
            text: sliderBuo.value
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            y: 70
            width: 128
            height: 17
            color: "#f6f4f7"
            text: qsTr("Fuel")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Slider {
            id: sliderFuel
            y: 90
            height: 22
            stepSize: 0.01
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 14
            value: 0.35

            background: Rectangle {
                x: parent.leftPadding
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 2
                width: parent.availableWidth
                height: implicitHeight
                radius: 1
                color: lightWhite

                Rectangle {
                    width: parent.visualPosition * parent.width
                    height: parent.height
                    color: lightWhite
                    radius: 1
                }
            }

            handle: Rectangle {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 15
                implicitHeight: 15
                radius: 15
                color: parent.pressed ? darkBlue : lightGrey
                border.color: lightWhite
            }
        }

        Text {
            y: 70
            width: 54
            height: 18
            color: "#f6f4f7"
            text: sliderFuel.value
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            y: 110
            width: 128
            height: 18
            color: "#f6f4f7"
            text: qsTr("Damp")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Slider {
            id: sliderDamp
            y: 130
            height: 22
            stepSize: 0.01
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 14
            value: 1.0

            background: Rectangle {
                x: parent.leftPadding
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 2
                width: parent.availableWidth
                height: implicitHeight
                radius: 1
                color: lightWhite

                Rectangle {
                    width: parent.visualPosition * parent.width
                    height: parent.height
                    color: lightWhite
                    radius: 1
                }
            }

            handle: Rectangle {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 15
                implicitHeight: 15
                radius: 15
                color: parent.pressed ? darkBlue : lightGrey
                border.color: lightWhite
            }
        }

        Text {
            y: 110
            width: 54
            height: 18
            color: "#f6f4f7"
            text: sliderDamp.value
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            y: 150
            width: 128
            height: 18
            color: "#f6f4f7"
            text: qsTr("Uncompressible")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Switch {
            id: switch_comp
            y: 170
            anchors.left: parent.left
            anchors.leftMargin: 14

            indicator: Rectangle {
                implicitWidth: 30
                implicitHeight: 10
                x: switch_comp.leftPadding
                radius: 10
                color: lightWhite
                border.color: lightWhite

                Rectangle {
                    x: switch_comp.checked ? parent.width - width : 0
                    y: -3
                    width: 16
                    height: 16
                    radius: 15
                    color: lightGrey
                    border.color: lightWhite
                }
            }

        }

        Text {
            y: 150
            width: 54
            height: 18
            color: "#f6f4f7"
            text: switch_comp.checked ? "ON" : "OFF"
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            y: 190
            width: 128
            height: 18
            color: "#f6f4f7"
            text: qsTr("Quality")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Slider {
            id: sliderQuality
            y: 210
            height: 22
            stepSize: 0.01
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 14
            value: 0.2

            background: Rectangle {
                x: parent.leftPadding
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 2
                width: parent.availableWidth
                height: implicitHeight
                radius: 1
                color: lightWhite

                Rectangle {
                    width: parent.visualPosition * parent.width
                    height: parent.height
                    color: lightWhite
                    radius: 1
                }
            }

            handle: Rectangle {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 15
                implicitHeight: 15
                radius: 15
                color: parent.pressed ? darkBlue : lightGrey
                border.color: lightWhite
            }
        }

        Text {
            y: 190
            width: 54
            height: 18
            color: "#f6f4f7"
            text: sliderQuality.value
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            y: 250
            width: 128
            height: 18
            color: "#f6f4f7"
            text: qsTr("Solver")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Image {
            x: 228
            y: 250
            width: 10
            height: 10
            anchors.right: parent.right
            anchors.rightMargin: 14
            source: "img/down.png"
        }

        Text {
            y: 250
            width: 22
            height: 18
            color: "#f6f4f7"
            text: "Navier-Stokes"
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

        Text {
            y: 290
            width: 128
            height: 18
            color: "#f6f4f7"
            text: qsTr("Turbulance")
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.horizontalCenterOffset: -48
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
        }

        Slider {
            id: sliderTur
            y: 310
            height: 22
            stepSize: 0.01
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 14
            value: 0.0

            background: Rectangle {
                x: parent.leftPadding
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 2
                width: parent.availableWidth
                height: implicitHeight
                radius: 1
                color: lightWhite

                Rectangle {
                    width: parent.visualPosition * parent.width
                    height: parent.height
                    color: lightWhite
                    radius: 1
                }
            }

            handle: Rectangle {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                y: parent.topPadding + parent.availableHeight / 2 - height / 2
                implicitWidth: 15
                implicitHeight: 15
                radius: 15
                color: parent.pressed ? darkBlue : lightGrey
                border.color: lightWhite
            }
        }

        Text {
            y: 290
            width: 54
            height: 18
            color: "#f6f4f7"
            text: sliderTur.value
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.horizontalCenterOffset: 85
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 12
        }

    }


}
