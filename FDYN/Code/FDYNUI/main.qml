import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import "."

ApplicationWindow {

    id: win_main
    property color darkGrey: "#282629"
    property color lightGrey: "#343235"
    property color lightWhite: "#aaa8ab"
    property color darkBlack: "#0e0c0f"
    property color darkBlue: "#2c53d7"

    property int min_height: 32

    visible: true
    visibility: "Maximized"
    width: 1280
    height: 768
    title: qsTr("Fire2019")

    Grid {
        width: parent.width; height: parent.height
        rows: 3;

        MenuZone { id: menu_zone; width: parent.width; height: min_height }

        Rectangle { id: gap_zone; width: parent.width; height: min_height * 1.5; color: lightGrey; }

        Grid {
            width: parent.width; height: parent.height - menu_zone.height - gap_zone.height;
            columns: 3;

            Grid {
                width: parent.width * 0.16; height: parent.height;
                rows: 2;

                Rectangle {
                    width: parent.width;
                    height: parent.height / 2;
                    color: lightGrey

                    TabWidget {
                        x: 2; y: 2
                        width: parent.width - 4; height: parent.height
                        current: 1

                        ZoneManager { }

                        ScenarioManager {
                            /*
                            onPItemChanged: property_editor.pItemChanged(pItem)
                            onBuoChanged: property_editor.buoChanged(buoVal)
                            onFuelChanged: property_editor.fuelChanged(fuelVal)
                            onDampChanged: property_editor.dampChanged(dampVal)
                            onUncompChanged: property_editor.uncompChanged(uncompVal)
                            onQualityChanged: property_editor.qualityChanged(qualityVal)
                            onSolverChanged: property_editor.solverChanged(solverVal)
                            onTurbChanged: property_editor.turbChanged(turbVal)
                            */
                        }
                    }
                }

                Rectangle {
                    width: parent.width;
                    height: parent.height /2 ;
                    color: lightGrey

                    TabWidget {
                        id: tab_settings
                        x: 2; y: 2
                        width: parent.width - 4; height: parent.height
                        current: 1

                        Rectangle {
                            property string title: ""
                            color: lightGrey
                            height: parent.height
                        }

                        PropertyEditor { id: property_editor }

                        Rectangle {
                            property string title: ""
                            color: lightGrey
                            height: parent.height
                        }
                    }

                }

            }

            Grid {
                width: parent.width * 0.68; height: parent.height;
                rows: 3;

                FToolbar { id: ftoolbar; width: parent.width; height: min_height; }
                FOpenGL { width: parent.width; height: parent.height - ftoolbar.height - ftimeline.height; }
                FTimeline { id: ftimeline; width: parent.width; height: min_height }
            }

            FranxZone { id: franx_zone; width: parent.width * 0.16; height: parent.height; }
        }

    }
}
