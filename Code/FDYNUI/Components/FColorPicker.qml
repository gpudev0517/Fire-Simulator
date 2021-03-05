import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {
    property string colorPickerName : "Sample"
    property string colorPickerValue : "Value Sample"

    property color pickColor: "red"

//    property double red: 0
//    property double green: 0
//    property double blue: 255
//    property double alpha: 200


    width: parent.width - 15
    height: 30

    signal sigColorPickerButtonClicked()

    function requireRender()
    {

        canvas.requestPaint()
    }


    Text {
        width: parent.width / 3
        height: parent.height
        color: "#f6f4f7"
        text: colorPickerName
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


        Canvas {
                id: canvas
                x: 8
                y: 2
                width: parent.height - 4
                height: parent.height - 4
                contextType: "2d"
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset()
                    ctx.fillStyle = Qt.rgba(pickColor.r, pickColor.g, pickColor.b, pickColor.a)
                    ctx.fillRect(0, 0, width, height);
                    ctx.fillStyle = Qt.rgba(pickColor.r, pickColor.g, pickColor.b, 1)
                    ctx.fillRect(width/4, width/4, width/2, height/2);
                }
            }

        Text {
            id: colorMain
            color: "#fff"
            text: colorPickerValue
            clip: true
            anchors.fill: parent
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.right: parent.right
            anchors.rightMargin: 35
            font.italic: false
            verticalAlignment: Text.AlignVCenter

        }
        RoundButton {
            width : 30
            height: 16
            y: 2
            anchors.right: parent.right
            anchors.rightMargin: 2
            text: "..."
            onClicked: {
                sigColorPickerButtonClicked()
            }
        }
    }
}
