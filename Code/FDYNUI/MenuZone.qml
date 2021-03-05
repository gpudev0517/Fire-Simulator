import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Window 2.4
import "Components"
import "."

Grid {
    width: Window.width

    Rectangle {
        width: parent.width;
        height: Global.min_height;
        color: Global.lightGrey;

        /*
        MenuBar {
            height: Global.min_height
            FMenu {
                title: { text: qsTr("File"); }
                Action { text: qsTr("Open"); }
                FMenuSeparator { }
                Action { text: qsTr("Close"); onTriggered: close() }
            }

            FMenu {
                title: "Edit"
                Action { text: qsTr("Copy"); }
                Action { text: qsTr("Paste"); }
            }

            FMenu {
                title: "Window"
                Action { text: "Zone Inspector" }
                Action { text: "Scenario Inspector" }
                Action { text: "Scene Inspector" }
                Action { text: "System Inspector" }
                Action { text: "Franx Inspector" }
                Action { text: "Message Inspector" }
            }

            FMenu {
                title: "View"
                Action { text: "Layer" }
                Action { text: "Adjust" }
            }

            FMenu {
                title: "Render"
                Action { text: "Texture" }
                Action { text: "Adjust" }
            }

            FMenu {
                title: "Help"
                Action { text: "Index" }
                FMenuSeparator { }
                Action { text: "About Fire 2019" }
            }

            delegate: MenuBarItem {

                contentItem: Text {
                    font.family: "Arial";
                    font.pixelSize: 12;
                    text: parent.text
                    opacity: enabled ? 1.0 : 0.3
                    color: parent.highlighted ? "#ffffff" : "#eee"
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    implicitWidth: 50
                    implicitHeight: 32
                    opacity: enabled ? 1 : 0.5
                    color: parent.highlighted ? Global.darkGrey : Global.lightGrey
                }
            }

        }
        */
        FTitleBar {}

        FProgressBar { percentage: 60; batterytime: 240 }

    }

//    Rectangle {
//        id: gap_zone;
//        width: parent.width;
//        height: Global.min_height * 2;
//        color: Global.lightGrey;
//        border.color: Global.darkBlack;
//        border.width: 4
//    }
}
