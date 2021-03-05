import QtQuick 2.12
import QtQuick.Controls 1.4 as QC1

Rectangle {
    property string title: "SCENARIO"
    id: rect_scenario
    color: lightGrey
    height: left_zone.height / 2
    anchors.fill: parent


    Rectangle {
        anchors.fill: parent;
        color: lightGrey

        Tree { }
/*
        Text {
            x: 72
            width: 108
            height: 32
            color: "#f6f4f7"
            text: qsTr("Selected_Scenario")
            anchors.horizontalCenter: parent.horizontalCenter
            font.italic: true
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            font.family: "Arial"
            lineHeight: 1.1
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Text {
            x: 225
            width: 27
            height: 32
            color: "#f6f4f7"
            text: qsTr("...")
            anchors.right: parent.right
            anchors.rightMargin: 0
            font.italic: false
            verticalAlignment: Text.AlignTop
            font.bold: true
            font.family: "Arial"
            lineHeight: 1
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 20
        }

        Rectangle {
            y: 31
            width: rect_scenario.width
            height: 23
            color: darkGrey

            Image {
                x: 13
                y: 5
                width: 10
                height: 9
                source: "img/down.png"
            }

            Image {
                x: 31
                y: 7
                width: 10
                height: 10
                source: "img/ring.png"
            }

            Text {
                height: 23
                color: "#f6f4f7"
                text: qsTr("Main_Grp")
                anchors.left: parent.left
                anchors.leftMargin: 47
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }

        }

        Rectangle {
            y: 54
            width: rect_scenario.width
            height: 23
            color: lightGrey

            Image {
                x: 49
                y: 7
                width: 10
                height: 10
                source: "img/ring.png"
            }

            Text {
                y: 0
                height: 23
                color: "#f6f4f7"
                text: qsTr("Child_Grp")
                anchors.left: parent.left
                anchors.leftMargin: 68
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }

            Image {
                x: 234
                y: 7
                width: 10
                height: 10
                anchors.right: parent.right
                anchors.rightMargin: 8
                source: "img/alert.png"
            }
        }

        Rectangle {
            y: 77
            width: rect_scenario.width
            height: 23
            color: darkGrey

            Image {
                x: 29
                y: 7
                width: 10
                height: 10
                source: "img/right.png"
            }

            Image {
                x: 49
                y: 7
                width: 10
                height: 10
                source: "img/ring.png"
            }

            Text {
                y: 0
                height: 23
                color: "#f6f4f7"
                text: qsTr("Child_Grp2")
                anchors.left: parent.left
                anchors.leftMargin: 68
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }
        }

        Rectangle {
            id: rectangle2
            x: 0
            y: 100
            width: rect_scenario.width
            height: 23
            color: lightGrey

            Image {
                x: 31
                y: 3
                width: 10
                height: 10
                source: "img/down.png"
            }

            Image {
                x: 49
                y: 7
                width: 10
                height: 10
                source: "img/ring.png"
            }

            Text {
                y: 0
                height: 23
                color: "#f6f4f7"
                text: qsTr("Auc_Grp_04")
                anchors.left: parent.left
                anchors.leftMargin: 68
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }
        }

        Rectangle {
            x: 0
            y: 123
            width: rect_scenario.width
            height: 23
            color: darkGrey

            Image {
                x: 69
                y: 7
                width: 10
                height: 10
                source: "img/circle.png"
            }

            Text {
                y: 0
                height: 23
                color: "#2c53d7"
                text: qsTr("Auc_SubGrp_01")
                anchors.left: parent.left
                anchors.leftMargin: 85
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }
        }

        Rectangle {
            x: 0
            y: 146
            width: rect_scenario.width
            height: 23
            color: lightGrey

            Image {
                x: 49
                y: 5
                width: 10
                height: 10
                source: "img/down.png"
            }

            Image {
                x: 69
                y: 7
                width: 10
                height: 10
                source: "img/ring.png"
            }

            Text {
                y: 0
                height: 23
                color: "#f6f4f7"
                text: qsTr("Test_Geo_03")
                anchors.left: parent.left
                anchors.leftMargin: 85
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }
        }

        Rectangle {
            x: 0
            y: 169
            width: rect_scenario.width
            height: 23
            color: darkGrey

            Image {
                x: 86
                y: 7
                width: 10
                height: 10
                source: "img/circle.png"
            }

            Text {
                y: 0
                height: 23
                color: "#f6f4f7"
                text: qsTr("Auc_Grp_04")
                anchors.left: parent.left
                anchors.leftMargin: 106
                renderType: Text.NativeRendering
                fontSizeMode: Text.Fit
                anchors.right: parent.right
                anchors.rightMargin: 0
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                font.family: "Arial"
                lineHeight: 1
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: 12
            }
        }
    }
*/
    }
}
