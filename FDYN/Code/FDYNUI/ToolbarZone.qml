import QtQuick 2.0

Row {
    id: main_zone_row
    x: 2
    y: 2
    width: rect_main_zone.width - 4;
    height: 37

    Rectangle {
        id: rect_main_1
        width: main_zone_row.width / 3;
        height: main_zone_row.height
        color: darkGrey
        z: -1

        Image {
            x: 5
            y: 3
            width: 24
            height: 24
            source: "img/pointer.png"
        }

        Image {
            x: 40
            y: 3
            width: 24
            height: 24
            source: "img/move.png"
        }

        Image {
            x: 80
            y: 3
            width: 24
            height: 24
            source: "img/cycle.png"
        }

        Image {
            x: 120
            y: 3
            width: 24
            height: 24
            source: "img/unk.png"
        }
    }

    Rectangle {
        id: rect_main_2
        width: main_zone_row.width / 3;
        height: main_zone_row.height
        color: darkGrey
        z: -1

        Image {
            x: 40
            y: 4
            width: 20
            height: 20
            source: "img/rectangle.png"
        }

        Image {
            x: 80
            y: 4
            width: 20
            height: 20
            source: "img/ver.png"
        }

        Image {
            x: 125
            y: 4
            width: 20
            height: 20
            source: "img/hor.png"
        }

        Image {
            x: 170
            y: 4
            width: 20
            height: 20
            source: "img/cross.png"
        }
    }

    Rectangle {
        id: rect_main_3
        width: main_zone_row.width / 3;
        height: main_zone_row.height
        color: darkGrey
        z: -1

        Image {
            x: 190
            y: 3
            width: 24
            height: 24
            anchors.right: parent.right
            anchors.rightMargin: 41
            source: "img/3d.png"
        }

        Image {
            x: 223
            y: 3
            width: 24
            height: 24
            anchors.right: parent.right
            anchors.rightMargin: 8
            source: "img/3dfill.png"
        }
    }

}
