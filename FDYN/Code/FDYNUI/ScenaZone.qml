import QtQuick 2.0
import "."

Rectangle {
    height: 300
    width: 150
    color: Global.lightGrey

    TabWidget {
        width: parent.width;
        height: parent.height
        current: 0

        ScenarioManager {
        }

        ZoneManager{

        }

        Rectangle {
            property string title: ""
            color: Global.lightGrey
            height: parent.height
            anchors.fill: parent
        }

    }
}
