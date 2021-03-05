import QtQuick 2.0
import "."

Rectangle {

    color: Global.darkGrey
    width: 200
    height: 300

//    TabWidget {
//        width: parent.width
//        height: parent.height
//        current: 1

//        Rectangle {
//            property string title: ""
//            color: Global.lightGrey
//            height: parent.height
//            anchors.fill: parent
//        }

        Rectangle {
            property string title: "MESSAGE"
            color: Global.lightGrey
            height: parent.height
            anchors.fill: parent

        }

//        Rectangle {
//            property string title: ""
//            color: Global.lightGrey
//            height: parent.height
//            anchors.fill: parent
//        }
//    }
}
