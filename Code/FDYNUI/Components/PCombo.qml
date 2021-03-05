import QtQuick 2.0
import "../"

Item {
    width: parent.width - 15;
    height: 30
    //color: Global.lightGrey

    property string comboName: "Solver"
    property variant comboData: ["Navier-Stokes", "Ultimate Solver", "Solver X"]
    property int currentIdx: 0

    signal sigCurrentIdxChanged(var curIdx)

    Text {
        //y: 10
        color: "#f6f4f7"
        width: parent.width / 3
        height: parent.height
        text: comboName
        verticalAlignment: Text.AlignVCenter
        font.family: "Arial"
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 11
    }

    FCombo {
        id: fcombo_solver;
        y : 5
        //width: 140;
        width: parent.width / 3 * 2
        height: 20;
        anchors.right: parent.right
        anchors.rightMargin: 0
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
}
