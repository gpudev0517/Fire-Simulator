pragma Singleton

import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Window 2.3

QtObject {
    property color darkGrey: "#282629"
    property color moreDarkGrey: "#1d1c1e"
    property color lightGrey: "#343235"
    property color white: "#ffffff"
    property color lightWhite: "#aaa8ab"
    property color darkBlack: "#0e0c0f"
    property color darkBlue: "#2c53d7"
    property color blueBlue: "#2c53d7"
    property color satRed: "#ff0000"
    property color darkGreen: "#87CEFA"

    property int min_height: 32

    property int app_width: Window.width
}
