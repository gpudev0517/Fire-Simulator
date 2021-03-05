import QtQuick 2.11
import "Components"
import "."

Row {
    Rectangle {
        width: parent.width / 3; height: parent.height; color: Global.darkGrey;

        Grid {
            x: 5; y: 5; columns: 5; spacing: 10
            FToolButton { id: btnCursor; objectName:"btnCursor"; width: 18; height: 24; source: "img/32/gl_select_01.png"; checkable:true}
            FToolButton { id: btnTranslate; objectName:"btnTranslate"; width: 24; height: 24; source: "img/32/gl_move_01.png"; checkable:true }
            FToolButton { id: btnRotate; objectName:"btnRotate"; width: 24; height: 24; source: "img/32/gl_rotate_01.png"; checkable:true }
            FToolButton { id: btnScale; objectName:"btnScale"; width: 24; height: 24; source: "img/32/gl_scale_01.png"; checkable:true }
            FToolButton { id: btnResetCamera; objectName:"btnResetCamera"; width: 24; height: 24; source: "img/32/cam32.png"; checkable:false }
        }
    }

    Rectangle {
        width: parent.width / 3; height: parent.height; color: Global.darkGrey;

        Grid {
            x: 100; y: 5; anchors.horizontalCenter: parent.horizontalCenter; columns: 4; spacing: 10
            FToolButton { id: btnSinglView; objectName: "btnSinglView"; width: 24; height: 24; source: "img/32/gl_single_view_01.png" }
            FToolButton { id: btnHorizontalSplit; objectName: "btnHorizontalSplit"; width: 24; height: 24; source: "img/32/gl_split_vertical_view_01.png" }
            FToolButton { id: btnVerticalSplit; objectName: "btnVerticalSplit"; width: 24; height: 24; source: "img/32/gl_split_horizontal_view_01.png" }
            FToolButton { id: btnGridSplit; objectName: "btnGridSplit"; width: 24; height: 24; source: "img/32/gl_quad_view_01.png" }
        }

    }

    Rectangle {
        width: parent.width / 3; height: parent.height; color: Global.darkGrey;

        Grid {
            x: 5; y: 5; anchors.right: parent.right; anchors.rightMargin: 0; columns: 4; spacing: 10
            FToolButton { id:btnWireFrame; objectName: "btnWireFrame"; width: 24; height: 24; source: "img/32/gl_wireframe_01.png"; checkable:false }
            FToolButton { id:btnShading; objectName: "btnShading"; width: 24; height: 24; source: "img/32/gl_shaded_01.png"; checkable:false }
        }

    }

}
