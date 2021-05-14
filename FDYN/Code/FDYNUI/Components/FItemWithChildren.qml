import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {
    property string containerName : "Sample"
    property string containerValue : "Value Sample"

    width: parent.width - 15
    height: 30

    Text {
        width: parent.width / 3
        height: parent.height
        color: "#f6f4f7"
        text: containerName
        font.italic: false
        verticalAlignment: Text.AlignVCenter
        font.family: "Segoe UI"
        font.pixelSize: 11
        elide: Text.ElideRight
        clip: true
    }

    Rectangle {
        id: rectangle
        y: 5
        //width: 130
        width: parent.width / 3 * 2
        height: 20
        radius: 12
        anchors.right: parent.right
        anchors.rightMargin: 10
        color: Global.moreDarkGrey
        Text {
            color: "#fff"
            anchors.fill: parent
            text: containerValue
            clip: true
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.horizontalCenter: parent.horizontalCenter
            fontSizeMode: Text.VerticalFit
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }
    }
}
