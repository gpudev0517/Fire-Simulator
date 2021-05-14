import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Window 2.4
import "."

Rectangle {
    width: Window.width
    height: Global.min_height;
    color: Global.darkGrey
    id: dock_bar

    property string dock_title : "Title"

    signal closeClicked();

    Text {
        x: 8
        height: Global.min_height
        color: "#fff"
        text: dock_title
        font.pointSize: 10
        verticalAlignment: Text.AlignVCenter
    }

    Image {
        width: 16
        height: 16
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 32
        source: "img/window_float_01.svg"
        MouseArea {
            anchors.fill: parent;
            onClicked: {
            }
        }
    }

    Image {
        objectName: "btnClose"
        width: 16
        height: 16
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 8
        source: "img/window_close_01.svg"

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                closeClicked();
            }
        }

    }

}
