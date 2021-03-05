import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.4
import "Components"
import "."

Rectangle {
    id : property_editor;
    objectName: "property_editor"
    width: 150
    height: 300
    color: Global.lightGrey

    //property string title: "SETTINGS"
    property string pItemName: ""

    Text {
        color: "#f6f4f7"
        width: parent.width
        height: 20
        text: property_editor.pItemName
        font.italic: true
        verticalAlignment: Text.AlignVCenter
        font.family: "Segoe UI"
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 10
    }

    PTree {
        anchors.topMargin: 10;
    }
}

