import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {
    property string filePathName : "Sample"
    property string filePathValue : "Value Sample"

    width: parent.width - 15
    height: 30

    signal sigFilePathValueChanged(var filePathString)
    signal sigFilePathButtonClicked(var filePathString)

    Text {
        width: parent.width / 3
        height: parent.height
        color: "#f6f4f7"
        text: filePathName
        font.italic: false
        verticalAlignment: Text.AlignVCenter
        font.family: "Segoe UI"
        font.pixelSize: 11
    }

    Rectangle {
        id: rectangle
        y: 5
        //width: 150
        width: parent.width / 3 * 2
        height: 20
        radius: 12
        clip: true
        anchors.right: parent.right
        anchors.rightMargin: 0
        color: Global.moreDarkGrey
        TextInput {
            id: pathMain
            color: "#fff"
            text: filePathValue
            clip: true
            selectByMouse: true
            anchors.fill: parent
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 35
            //fontSizeMode: Text.VerticalFit
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            //horizontalAlignment: Text.AlignHCenter

            onTextChanged: {
                sigFilePathValueChanged(text)
            }
        }
        RoundButton {
            width : 30
            height: 16
            y: 2
            anchors.right: parent.right
            anchors.rightMargin: 2
            text: "..."
            onClicked: {
                sigFilePathButtonClicked(pathMain.text)
            }
        }
    }
}
