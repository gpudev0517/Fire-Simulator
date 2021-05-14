import QtQuick 2.2
import QtQuick.Window 2.11
import QtQuick.Controls 2.0
import "Components"
import "."


Column {

    objectName: "FCADModelingTool"

    function setBtnMeshObjSelect(toggled){
        btnMeshObjSelect.checked = toggled
    }

    function setBtnMeshFaceSelect(toggled){
        btnMeshFaceSelect.checked = toggled
    }

    function setBtnMeshEdgeSelect(toggled){
        btnMeshEdgeSelect.checked = toggled
    }

    function setBtnSnapToBoundary(toggled){
        btnSnapToBoundary.checked = toggled
        btnSnapToObj.checked = false
        btnSnapUnit.checked = false
    }

    function setBtnSnapToUnit(toggled){
        btnSnapUnit.checked = toggled
        btnSnapToObj.checked = false;
        btnSnapToBoundary.checked = false
    }

    function setBtnSnapToObj(toggled){
        btnSnapToObj.checked = toggled
        btnSnapToBoundary.checked = false
        btnSnapUnit.checked = false
    }

    function setBtnSnapToEdge(toggled){
        btnSnapToEdge.checked = toggled
        btnSnapToBoundary.checked = false
        btnSnapToObj.checked = false
    }

    function setBtnMeasure(toggled){
        btnMeasure.checked = toggled
        //btnMeasureEdge.checked = false
    }

    function setBtnMeasureEdge(toggled){
        btnMeasureEdge.checked = toggled
        //btnMeasure.checked = false
    }

    function deactivateCADTools(){
        btnMeasureEdge.checked = false;
        //btnMeasure.checked = false;
    }
    function activateCADTools(){
        btnMeasureEdge.checked = true;
        //btnMeasure.checked = true;
    }

    Rectangle {
        width: parent.width; height: Window.height*0.25; color: Global.darkGrey;

        Grid {
            x: 10; y: 50; columns: 1; rows: 3; spacing: 10
            FToolButton {
                id: btnMeshObjSelect;
                objectName:"btnMeshObjSelect";
                width: 24; height: 24; source: "img/32/gl_selectobject_tool_01.png";
                checkable:true;
                switchtype: true
                helptext: qsTr("Select Object (q)")
            }
            FToolButton {
                id: btnMeshFaceSelect;
                objectName:"btnMeshFaceSelect";
                width: 24; height: 24; source: "img/32/gl_selectface_tool_01.png";
                checkable:true;
                switchtype: true
                helptext: qsTr("Select Face (f)")
            }
            FToolButton {
                id: btnMeshEdgeSelect;
                objectName:"btnMeshEdgeSelect";
                width: 24; height: 24;
                source: "img/32/gl_selectedge_tool_01.png";
                checkable:true;
                switchtype: true
                helptext: qsTr("Select Edge (e)")
            }
        }
    }

    Rectangle {
        width: parent.width; height: Window.height*0.25; color: Global.darkGrey;

        Grid {
            x: 10; y: 50; columns: 1; rows: 5; spacing: 10

            FToolButton {
                id: btnSnapToBoundary;
                objectName:"btnSnapToBoundary";
                width: 24;
                height: 24;
                source: "img/32/gl_snaptoface_tool_01.png";
                checkable:true; switchtype: true;
                helptext: qsTr("Snap to Boundaries (b)")
                shortcutkey: "b"
            }
            FToolButton {
                id: btnSnapToObj;
                objectName:"btnSnapToObj";
                width: 24;
                height: 24;
                source: "img/32/gl_snapobject.png";
                checkable:true;
                switchtype: true;
                helptext: qsTr("Snap to Other Objects (o)")
                shortcutkey: "o"
            }
            FToolButton {
                id: btnSnapUnit;
                objectName:"btnSnapUnit";
                width: 24;
                height: 24;
                source: "img/32/gl_snapgrid.png";
                checkable:true;
                switchtype: true;
                helptext: qsTr("Snap to Units (u)")
                shortcutkey: "u"
            }
//            FToolButton {
//              id: btnSnapToEdge;
//              objectName:"btnSnapToEdge";
//              width: 24; height: 24;
//              source: "img/32/gl_snaptoedge_tool_01.png";
//              checkable:true;
//              switchtype: true
//            }
        }
    }

    Rectangle {
        width: parent.width; height: Window.height*0.15; color: Global.darkGrey;

        Grid {
            x: 10; y: 50; columns: 1; rows: 3; spacing: 10
//            FToolButton {
//                id: btnMeasure;
//                objectName:"btnMeasure";
//                width: 24;
//                height: 24;
//                source: "img/32/gl_measurevertex_tool_01.png";
//                checkable:true;
//                switchtype: true;
//                helptext: qsTr("Measure Between Vertices (v)")
//            }
            FToolButton {
                id: btnMeasureEdge;
                objectName:"btnMeasureEdge";
                width: 24;
                height: 24;
                source: "img/32/gl_measureedge_tool_01.png";
                checkable:true;
                switchtype: true;
                helptext: qsTr("Measure Between Edges (m)")
                shortcutkey: "m"
            }
        }
    }

    Rectangle {
        width: parent.width; height: Window.height*0.35; color: Global.darkGrey;

        Grid {
            x: 10; y: 50; columns: 1; rows: 6; spacing: 10
            FToolButton {
                id: btnMesExtrude;
                objectName:"btnMesExtrude";
                width: 24;
                height: 24;
                source: "img/32/gl_extrude_tool_01.png";
                checkable:false;
                switchtype: true
                helptext: qsTr("Extrude (Ctrl+E)")
            }
//            FToolButton {
//                id: btnMesEdgeOffset;
//                objectName:"btnMesEdgeOffset";
//                width: 24;
//                height: 24;
//                source: "img/32/gl_edgeoffset_tool_01.png";
//                checkable:false;
//                switchtype: true
//                helptext: qsTr("Edge Offset (Ctrl+O)")
//            }
            FToolButton {
                id: btnMesBridge;
                objectName:"btnMesBridge";
                width: 24;
                height: 24;
                source: "img/32/gl_bridge_tool_01.png";
                checkable:false;
                switchtype: true
                helptext: qsTr("Bridge Faces(Ctrl+B)")
            }
            FToolButton {
                id: btnMesEdgeLoop;
                objectName:"btnMesEdgeLoop";
                width: 24;
                height: 24;
                source: "img/32/gl_edgeloop_tool_01.png";
                checkable:false;
                switchtype: true
                helptext: qsTr("Edge Loop (Ctrl+L)")
            }
        }
    }


}

