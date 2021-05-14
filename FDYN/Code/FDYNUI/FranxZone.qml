import QtQuick 2.11
import QtQuick.Controls 2.4
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
            property string title: "FRANX"
            color: Global.lightGrey
            height: parent.height;
            anchors.fill: parent

            ScrollView {
                id: scrollView
                anchors.fill: parent
                clip: true

                Rectangle {
                    id: rectangle
                    implicitHeight: 300
                    implicitWidth: parent.width
                    color: Global.lightGrey
                }
            }

        }

//        Rectangle {
//            property string title: ""
//            color: Global.lightGrey
//            height: parent.height
//            anchors.fill: parent
//        }
//    }
}
