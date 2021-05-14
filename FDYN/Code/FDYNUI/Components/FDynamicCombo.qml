import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {

    width: parent.width - 15;
    height: 30

    property string dynamicComboName : "Sample"
    property variant dynamicComboData: ["Navier-Stokes", "Ultimate Solver", "Solver X"]
    property int currentIdx: 0

    signal sigCurrentIdxChanged(var curIdx)


    Text {
        width: parent.width / 3
        height: parent.height
        color: "#f6f4f7"
        text: dynamicComboName
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
        width: parent.width / 3 * 2
        height: 20;

        leftPadding: 20
        rightPadding: 20
        bkColor: Global.moreDarkGrey
        model: dynamicComboData
        fontBold : false;
        fontItalic : false;
        fontSize: 8
        currentIndex: currentIdx
        widePopup: true
        onComboValChanged: {
            sigCurrentIdxChanged(currentIndex)
        }
    }
}
