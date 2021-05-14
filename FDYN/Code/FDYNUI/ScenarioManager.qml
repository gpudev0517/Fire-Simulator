import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.4
import "Components"
import "."

Rectangle {
    id:scenarioManager
    /*
    signal pItemChanged(string pItem)
    signal buoChanged(real buoVal)
    signal fuelChanged(real fuelVal)
    signal dampChanged(real dampVal)
    signal uncompChanged(bool uncompVal)
    signal qualityChanged(real qualityVal)
    signal solverChanged(string solverVal)
    signal turbChanged(real turbVal)
    */

    property int searchBarWidth: 30
    property string title: "SCENARIO"
    property var currentTreeVal
    color: Global.lightGrey
    width: 150
    height: 300

    FToolButton {
        x: 8
        y: 11
        width: 20
        height: 20
        source: "img/search.png"

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: {
                if(!txtSearch.visible)
                    txtSearch.forceActiveFocus();
                txtSearch.visible = !txtSearch.visible
                combo_scena.visible = !combo_scena.visible

                txtSearch.width = scenarioManager.width - 80 - txtSearch.width
                txtSearch.text = ""
                txtSearch.sigTextChanged(txtSearch.text)
            }
        }
    }

    FCombo {
        id : combo_scena
        objectName: "combo_scena"
        width: parent.width - 80;
        height: 25
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter;
        model: ["", "", "", "", ""]
        bkColor: Global.darkGrey
        fontBold: false
        fontItalic: true
        emptyString: ""
        visible: true
    }


    TextField {
        id: txtSearch
        objectName: "txtSearch"
        x: 40
        y: 6
        width: 0
        height: 30
        font.family: "Segoe UI"
        font.pointSize: 10
        color: "#fff"
        selectByMouse: true
        placeholderText: "Search..."
        visible: false

        background: Rectangle {
            implicitWidth: searchBarWidth
            implicitHeight: 32
            color: Global.darkGrey
            border.color: "#ffffff"
            radius: 10
        }

//        Behavior on width {
//            NumberAnimation{ duration: 300 }
//        }

//        Behavior on visible {
//            NumberAnimation{ duration: 100 }
//        }

        signal sigTextChanged(var searchItem)
        signal sigEditingFinished(var searchText)

        onTextChanged: {
            sigTextChanged(text)
        }

        Keys.onReleased: {
            if(event.key === Qt.Key_Escape){
                if(visible){
                    visible = !visible
                    combo_scena.visible = !combo_scena.visible

                    width = scenarioManager.width - 80 - width
                    text = ""
                    sigTextChanged(text)
                    //console.log("escape pressed.")
                }



            }
            else if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return){
                //console.log("Enter pressed.")
                sigEditingFinished(text)
            }
        }

    }

    FSearchCombo {
        id: combo_list
        objectName: "combo_list"
        width: 35
        height: 20
        flat: true
        bkColor: Global.lightGrey
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right;
        anchors.rightMargin: 0
        emptyString: "..."
        model: ["...", "..."];
        fontSize: 12
        fontBold : true;
        fontItalic : false;
        indicator: Canvas {
            width: 0
            height: 0
        }

        popup.x: -100
        popup.width: this.width + 100

    }

    FTree {
        id: treeview
        anchors.topMargin: 40;
    }

}
