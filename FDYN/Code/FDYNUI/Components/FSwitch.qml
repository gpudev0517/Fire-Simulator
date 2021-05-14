import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {
    property string switchName : "Uncompressable"
    property bool switchChecked : true

    width: parent.width - 15
    height: 30

    signal sigSwitchChanged(var checked)

    Text {
        color: "#f6f4f7"
        text: switchName
        anchors.fill: parent
        font.italic: false
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }

    Switch {
        id: switch_comp
        anchors.top: rectangle.top
        anchors.right: rectangle.left
        checked: switchChecked

        indicator: Rectangle {
            implicitWidth: 32
            implicitHeight: 6
            y: 7
            radius: 10
            color: Global.lightWhite
            border.color: Global.lightWhite

            Rectangle {
                x: switch_comp.checked ? parent.width - width : 0
                y: -3
                width: 12
                height: 12
                radius: 12
                color: Global.lightGrey
                border.color: Global.lightWhite
            }
        }
        onToggled: {
            switchChecked = checked;
            sigSwitchChanged(switchChecked)
        }
    }

    Rectangle {
        id: rectangle
        y: 5
        width: 50
        height: 20
        radius: 12
        anchors.right: parent.right
        anchors.rightMargin: 0
        color: Global.moreDarkGrey
        Text {
            color: "#fff"
            text: switch_comp.checked ? "ON" : "OFF"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            fontSizeMode: Text.VerticalFit
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

}
