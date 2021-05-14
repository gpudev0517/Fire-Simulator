import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import "Components"

Rectangle {
    property color lightGrey: "#343235"
    property string title: "COMPARTMENTS"
    color: lightGrey
    anchors.fill: parent
    //width: Window.width
    width: 300
    height: 300

    Rectangle {
        anchors.fill: parent;
        color: lightGrey

        Image {
            x: 8
            y: 11
            width: 20
            height: 20
            source: "img/search.png"
        }

        FCombo {
            id : combo_zone
            width: parent.width - 80;
            height: 25
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter;
            model: ["Zone 1", "System 1", "Scene 1"]
            bkColor: Global.darkGrey
            fontBold: false;
            fontItalic: true
            visible: false
        }

        FCombo {
            width: 35
            height: 20
            flat: true
            bkColor: Global.lightGrey
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.right: parent.right;
            anchors.rightMargin: 0
            model: ["...", "..."];
            fontSize: 12
            fontBold : true;
            fontItalic : false;
            indicator: Canvas {
                width: 0
                height: 0
            }
        }
    }
}
