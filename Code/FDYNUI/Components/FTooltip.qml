import QtQuick 2.2
import QtQuick.Window 2.1
import QtQml 2.2
import QtQuick.Layouts 1.1
//import org.shotcut.qml 1.0 as Shotcut

Item {
    property alias text: toolTipText.text
    property alias isVisible: toolTipMouseArea.enabled
    property alias cursorShape: toolTipMouseArea.cursorShape
    property alias containsMouse: toolTipMouseArea.containsMouse

    anchors.fill: parent

    MouseArea {
        id: toolTipMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: {
            toolTipWindow.beginDisplay()

        }
        onExited: {
            toolTipWindow.endDisplay()
        }
        onEnabledChanged: {
            if (!enabled) toolTipWindow.close()
        }
    }

    Window {
        id: toolTipWindow
        width: toolTipRect.width + 2
        height: toolTipRect.height + 2
        visible: false
        flags: Qt.ToolTip

        function beginDisplay() {
            if (!toolTipWindow.visible) {
                tipTimer.interval = 500
                tipTimer.running = true
            } else {
                tipTimer.running = false
            }
        }

        function endDisplay() {
            if (toolTipWindow.visible) {
                tipTimer.interval = 500
                tipTimer.running = true
            } else {
                tipTimer.running = false
            }
        }

        Timer {
            id: tipTimer

            onTriggered: {
                if (!toolTipWindow.visible) {
                    var cursorPoint = mousePosition.cursorPos()
                    toolTipWindow.x = cursorPoint.x
                    toolTipWindow.y = cursorPoint.y + 15
                }
                if (toolTipWindow.visible) {
                    toolTipWindow.close()
                }
                else if (toolTipMouseArea.enabled) {
                    toolTipWindow.show()
                }
            }
        }

        Rectangle {
            id: toolTipRect
            anchors.centerIn: parent
            implicitWidth: Math.min(350, toolTipText.implicitWidth + 4)
            height: toolTipText.contentHeight + 4
            Text {
                id: toolTipText
                anchors.fill: parent
                anchors.margins: 2
                clip: false
                wrapMode: Text.WordWrap
            }
        }
    }
}
