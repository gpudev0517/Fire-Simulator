import QtQuick 2.9
import QtQuick.Controls 2.4
import "../"

Item {
    property string stringName : "Sample"
    property string stringValue : "Value Sample"

    width: parent.width - 15
    height: 30

    signal sigStringValueChanged(var string)

    Text {
        width: parent.width / 3
        height: parent.height
        color: "#f6f4f7"
        text: stringName
        font.italic: false
        verticalAlignment: Text.AlignVCenter
        font.family: "Segoe UI"
        font.pixelSize: 11
    }

    Rectangle {
        id: rectangle
        y: 5
        //width: 130
        width: parent.width / 3 * 2
        height: 20
        radius: 12
        anchors.right: parent.right
        anchors.rightMargin: 0
        color: Global.moreDarkGrey
        TextInput {
            color: "#fff"
            text: stringValue
            anchors.fill: parent
            clip: true
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 5
            //fontSizeMode: Text.VerticalFit
            font.italic: false
            verticalAlignment: Text.AlignVCenter
            //horizontalAlignment: Text.AlignHCenter

            onEditingFinished: {
                stringValue = text
                sigStringValueChanged(stringValue)
            }

//            onTextChanged: {
//                stringValue = text
//                sigStringValueChanged(stringValue)
//            }

        }
    }
}
