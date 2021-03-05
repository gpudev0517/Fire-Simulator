import QtQuick 2.15
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0

import "../"
import "./"

Item {
    property color overlayColor: Global.darkBlue
    property alias source: img.source
    property int imgY
    property bool checkable : false
    property bool checked: false
    property bool switchtype: false
    property var helptext: ""
    property var toolparent: parent
    property var shortcutkey

    Image {
        id: img
        height: parent.height
        width: parent.width
        y: imgY
        antialiasing: true
        smooth: true
        visible: false


    }

    signal onButtonClicked();
    signal onToggled(bool bCheck);
    signal onRightButtonClicked();

    Shortcut {
        sequence: shortcutkey
        onActivated: {
            onButtonClicked();

            if(checkable){
                if(switchtype)
                    checked = !checked;
                else
                    checked = true;
                onToggled(checked);
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton


        onClicked: {
            if (mouse.button == Qt.RightButton) {
                //console.log("Right Tool Button clicked.");
                onRightButtonClicked();
            }
            else {
                //console.log("Tool Button clicked.");
                onButtonClicked();

                //console.log("Tool Button clicked.");
                if(checkable){
                    if(switchtype)
                        checked = !checked;
                    else
                        checked = true;
                    onToggled(checked);
                }
            }
        }

        FTooltip {
            id: tip
            visible: text ? parent.containsMouse : false
            Component.onCompleted: parent.hoverEnabled = true
            text: helptext
        }
    }




    ColorOverlay {
        anchors.fill: img
        source: img
        antialiasing: true

        color:{
            if(checkable && checked)
                Global.darkBlue
            else if(mouseArea.containsMouse) {
                Global.white
            }
            else
                "transparent"
        }
    }
}
