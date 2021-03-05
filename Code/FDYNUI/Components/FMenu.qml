import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Menu {

    padding: 2

    delegate: MenuItem {
        implicitWidth: 200
        implicitHeight: 24

        contentItem: Text {
            font.family: "Segoe UI";
            font.pixelSize: 12;
            text: parent.text
            opacity: enabled ? 1.0 : 0.3
            color: parent.highlighted ? "#ffffff" : Global.lightWhite
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 40
            opacity: enabled ? 1 : 0.3
            color: parent.highlighted ? Global.darkBlack : Global.darkGrey
        }
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 40
        color: Global.lightGrey
        border.color: Global.darkBlack
    }
}
