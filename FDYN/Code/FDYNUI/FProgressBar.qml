import QtQuick 2.0
import QtQuick.Controls 2.4
import "."

Rectangle {
    id: rect_status
    color: Global.lightGrey
    anchors.right: parent.right
    anchors.rightMargin: 15

    property int percentage;
    property int batterytime; // in minutes

    Text {
        width: 108
        height: Global.min_height
        color: "#f6f4f7"
        text: percentage
        anchors.right: parent.right
        anchors.rightMargin: 222
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        horizontalAlignment: Text.AlignRight
    }

    Text {
        x: 209
        width: 108
        height: Global.min_height
        color: "#99f6f4f7"
        text: "- ETA " + Math.round(( percentage / 100) * batterytime / 60)  + "H " + ( ( percentage / 100) * batterytime % 60 ) + "M"
        anchors.right: parent.right
        anchors.rightMargin: 110
        font.italic: true
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        horizontalAlignment: Text.AlignLeft
    }

    ProgressBar {
        id: progressBar
        x: 283
        y: 12
        value : percentage / 100
        width: 120
        height: 8
        anchors.right: parent.right
        anchors.rightMargin: 11
        transformOrigin: Item.Center

        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 6
            color: Global.lightWhite
            radius: 4
        }

        contentItem: Item {
            implicitWidth: 200
            implicitHeight: 4

            Rectangle {
                width: progressBar.visualPosition * parent.width
                height: parent.height
                radius: 3
                color: Global.darkBlue
            }
        }
    }
}
