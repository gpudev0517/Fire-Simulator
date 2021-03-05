import QtQuick 2.0
import QtQuick.Window 2.11
import QtQuick.Controls 2.0
import "."
import "Components"

Grid {
    width: Window.width
    height: Window.height
    rows: 3;



    FToolbar { id: ftoolbar; objectName: "ftoolbar"; width: parent.width; height: Global.min_height; }

    Rectangle {
        color: Global.lightGrey;
        width: parent.width;
        height: parent.height - ftoolbar.height/* - ftimeline.height*/;

        DropArea {
            id: dropAreaOpenGL
            objectName:"dropAreaOpenGL"
            anchors.fill: parent
            keys: ["opengl"]

            signal sigDropped(var x, var y, var dropIndex)

            onEntered: {
//                if (!acceptDropCB.checked) {
//                    drag.accepted = false
//                    rejectAnimation.start()
//                }
            }
            onDropped: {
                if (drop.getDataAsString("opengl") !== "") {
                    if (drop.proposedAction == Qt.MoveAction || drop.proposedAction == Qt.CopyAction) {
                        //console.log("onDropped is called " + drop.x +", " + drop.y + ", " + drop.getDataAsString("opengl"))
                        sigDropped(drop.x, drop.y, drop.getDataAsString("opengl"))
                        drop.acceptProposedAction()
                    }
                }
            }
        }
    }

    //FTimeline { id: ftimeline; width: parent.width; height: Global.min_height }
}

