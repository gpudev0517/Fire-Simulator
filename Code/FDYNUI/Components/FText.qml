import QtQuick 2.0

import QtQuick 2.11
import QtQuick.Controls 2.4
import "../"

TextInput {
    font.family: "Segoe UI"
    font.pointSize: 12
    color: "#fff"

    selectByMouse: true

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.IBeamCursor
        acceptedButtons: Qt.NoButton
    }

}
