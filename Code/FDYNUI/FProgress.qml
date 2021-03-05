import QtQuick 2.0
import QtQuick.Window 2.11
import QtQuick.Controls 2.11
import "."
import "Components"

Rectangle{
    color: Global.lightGrey
    width: 300
    height: 150

    Grid {
        rows: 3;
        width: parent.width
        height: parent.height

        Text {
            id: topLabel
            objectName: "topLabel"
            text: qsTr("Please Wait...")
            font.bold: true
            font.pointSize: 12
            width: parent.width
            height: parent.height / 3
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        BusyIndicator{
            id: progressBar
            objectName: "progressBar"
            running: true;
            width: parent.width
            height: parent.height / 3

        }

        Text {
            id: bottomLabel
            objectName: "bottomLabel"
            //text: qsTr("Loading...")
            width: parent.width
            height: parent.height / 3
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

    }
}



