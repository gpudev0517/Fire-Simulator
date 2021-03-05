import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {

    property string materialSelectorName : "Sample"
    property string materialSelectorValue : "Value Sample"
    property var materialSelectorMaterial: undefined

    width: parent.width - 15;
    height: 30


    property variant comboData: ["Navier-Stokes", "Ultimate Solver", "Solver X"]
    property int currentIdx: 0

    signal sigCurrentIdxChanged(var curIdx)
    signal sigMaterialSelectorButtonClicked(var material)


    Text {
        width: parent.width / 3
        height: parent.height
        color: "#f6f4f7"
        text: materialSelectorName
        font.italic: false
        verticalAlignment: Text.AlignVCenter
        font.family: "Segoe UI"
        font.pixelSize: 11
    }

    FCombo {
        id: fcombo_solver;
        x : parent.width / 3
        y : 5
        //width: 140;
        width: parent.width / 3 * 2 - 30
        height: 20;

        leftPadding: 20
        rightPadding: 20
        bkColor: Global.moreDarkGrey
        model: comboData
        fontBold : false;
        fontItalic : false;
        fontSize: 8
        currentIndex: currentIdx
        widePopup: true
        onComboValChanged: {
            //currentIdx = currentIndex
            sigCurrentIdxChanged(currentIndex)
        }
    }

    Rectangle {
        id: rectangle
        y: 5
        //width: 150
        width: 30
        height: 20
        radius: 10
        clip: true
        anchors.right: parent.right
        anchors.rightMargin: 0
        color: Global.moreDarkGrey

        RoundButton {
            id: rbutton
            anchors.fill: parent
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            //text: materialSelectorValue
            text: "..."

            contentItem: Text {
                text: rbutton.text
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle{
                color:  rbutton.down ? Global.darkGrey : Global.moreDarkGrey
                radius: rectangle.radius
            }

            onClicked: {
                sigMaterialSelectorButtonClicked(materialSelectorMaterial)
            }
        }
    }
}
