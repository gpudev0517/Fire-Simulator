import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.2
import FTreeModel 1.0
import FTreeView 1.0
import "../"

TreeView {
    id: treeView
    objectName: "treeview"
    anchors.fill: parent

    model: treeModel
    alternatingRowColors: false
    headerVisible: false
    frameVisible: false
    horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff
    selectionMode: SelectionMode.ExtendedSelection

    //property bool ctrlKey: true

    readonly property color cellBackgroundColor: Global.lightGrey
    readonly property color cellRowColor: "#ffffff"
    readonly property color cellCurrentRowColor: Global.darkBlue
    readonly property color cellPressedColor: Global.lightWhite
    readonly property color headerColor: Global.lightGrey

    signal sigItemExpanded(var idx)
    signal sigItemCollapsed(var idx)
    signal sigItemClicked(var idx)
    signal sigItemSelected(var idx)
    signal sigItemSelectionChanged(var selected, var deselected)
    signal sigItemSelectionAdded(var idx)
    signal sigCreateContextMenu()
    signal sigCurrentTextInput(var txtInput)
    signal sigItemRenamed(var idx, string newStr)

    function setCurrentItemFromCpp(idx){
            takeSelModel.select(idx, ItemSelectionModel.ClearAndSelect)
    }

    function clearCurrentItemFromCpp(){
        takeSelModel.clearSelection()
    }

    function setSelectItemFromCpp(idx){
        takeSelModel.select(idx, ItemSelectionModel.Select)
    }

    function itemAdded(){
        __listView.positionViewAtEnd();
    }


    Column{
        z:-1
        Repeater {
            //model: parent.height/20
            model: 50
            Rectangle {
                width: treeView.width; height: 20
                //width: 300; height: 20
                color: index % 2 ? Global.lightGrey : Global.darkGrey
            }
        }
    }

    TableViewColumn {
        id: nameColumn
        role: "name"
        title: "Name"
        width: treeView.width - typeColumn.width

    }

    TableViewColumn {
        id: typeColumn
        role: "type"
        title: "Type"
        width: 50
    }

    headerDelegate: Rectangle {
        height: nameHeader.implicitHeight * 1.3
        color: "#373737"
        anchors.topMargin: 2
        Rectangle {
            anchors.fill: parent
            color: headerColor

            // item name
            Text {
                id: nameHeader
                visible: styleData.column % 2  ? true : false
                width: parent.width
                text: "Type"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: cellRowColor
            }

            Text {
                id: typeHeader
                visible: styleData.column % 2 ? false : true
                width: parent.width
                text: "Name"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color : cellRowColor
            }

            Rectangle {
                height: 1
                border.color: Global.darkGrey
                color:  Global.darkGrey
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
            }
            Rectangle {
                height: 1
                border.color: Global.darkGrey
                color:  Global.darkGrey
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
            }
            Rectangle {
                width: 1
                border.color: Global.darkGrey
                color: Global.darkGrey
                anchors.bottom: parent.bottom
                anchors.top: parent.top
                anchors.right: parent.right
            }
        }
    }

    style: TreeViewStyle {
        activateItemOnSingleClick: true
        //backgroundColor: cellBackgroundColor
        backgroundColor: "transparent"

        branchDelegate: Rectangle {
            width: 16
            height: 16
            color: "transparent"
            Image {
                anchors.fill: parent
                anchors.margins: 4
//                width: 8; height: 8
                source: styleData.isExpanded ? "../img/arrow-down.png" : "../img/arrow-right.png"
            }
        }

        handle: Rectangle {
            implicitWidth: 8
            implicitHeight: 8
            radius: 90

            color: "darkgray"
        }
        scrollBarBackground: Rectangle {
            implicitWidth: 8
            implicitHeight: 8
            //color: "transparent"
            color: Global.lightGrey
        }
        decrementControl: Rectangle {
            implicitWidth: 8
            implicitHeight: 8
            //color: "transparent"
            color: Global.lightGrey
        }
        incrementControl: Rectangle {
            implicitWidth: 8
            implicitHeight: 8
            //color: "transparent"
            color: Global.lightGrey
        }
        corner: Rectangle {
            implicitWidth: 8
            implicitHeight: 8
            //color: "transparent"
            color: Global.lightGrey
        }
    }

    selection: ItemSelectionModel {
        id: takeSelModel
        model: treeModel

        onSelectionChanged: {
            sigItemSelectionChanged(selected, deselected);
        }
    }

    itemDelegate: Item {
        id: columnDel
        width: 300
        height: 20

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            acceptedButtons: Qt.RightButton
            onClicked: {
                columnRect.forceActiveFocus()
                if (mouse.button === Qt.RightButton){
                    sigCreateContextMenu();
                    sigCurrentTextInput(textEdit)
                }
            }

            ToolTip.visible: (mouseArea.containsMouse && styleData.value.tooltip !== undefined && styleData.value.tooltip !== "") ? true : false
            ToolTip.delay: 1000
            ToolTip.text: styleData.value.tooltip !== undefined ? styleData.value.tooltip : ""
        }

        Item {
            id: draggable
            anchors.fill: parent
            Drag.active: iconArea.drag.active
            Drag.hotSpot.x: 30
            Drag.hotSpot.y: 30
            Drag.mimeData: { "opengl": "" + styleData.index }
            Drag.dragType: Drag.Automatic
            Drag.imageSource: "../img/3dfill.png"
        }

        Rectangle {
            id: columnRect
            anchors.fill: parent
            color: styleData.selected ?  Global.darkBlue : ( styleData.row  % 2 ? Global.lightGrey : Global.darkGrey)



            Image {
                id: iconImage
                visible: styleData.column % 2 ? false : true
                x: 1
                y: 5
                height: 12
                width: styleData.value.icon !== undefined ? 12 : 0
                source: {
                    if( visible === true){
                        if (styleData.value.icon){
                            "../img/" + styleData.value.icon ;
                        }
                        else{
                            ""
                        }
                    }
                    else{
                        ""
                    }
                }

            }
            TextInput {
                id: textEdit
                visible: styleData.column % 2 ? false : true
                x: styleData.value.icon !== undefined ? 20 : 10
                y: 5
                focus: false
                selectByMouse: true
                enabled: true
                text: styleData.value.name !== undefined ? styleData.value.name : ""
                //color : cellCurrentRowColor
                color : cellRowColor

                Keys.onReleased:{
                    if((event.key === Qt.Key_Escape))
                    {
                        textEdit.focus = false
                    }
                }

                Component.onCompleted: {
                    if(styleData.value.font !== undefined)
                        textEdit.font = styleData.value.font;
                    if(styleData.value.flags & 0x03)  //selectable and editable
                    {
                        textEdit.enabled = true;
                    }
                    else{
                        textEdit.enabled = false;
                    }
                    textEdit.enabled = true;
                }

                function setActiveFocusFromCpp(){
                    textEdit.forceActiveFocus()
                    textEdit.selectAll()
                }

                onEditingFinished: {
                    textEdit.focus = false;
                    sigItemRenamed(styleData.index, text)
                }

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        if(styleData.value.flags & 0x03){ //selectable and editable
                            textEdit.forceActiveFocus()
                            textEdit.selectAll()
                        }
                    }

                    onClicked: {
                        if(mouse.modifiers === Qt.ControlModifier){
                            if(takeSelModel.isSelected(styleData.index))
                                takeSelModel.select(styleData.index, ItemSelectionModel.Deselect)
                            else
                                takeSelModel.select(styleData.index, ItemSelectionModel.Select)
                        }
                        else if(mouse.modifiers === Qt.ShiftModifier){
                            takeSelModel.select(styleData.index, ItemSelectionModel.Select)
                        }
                        else{
                            takeSelModel.select(styleData.index, ItemSelectionModel.ClearAndSelect)
                        }
                    }

                }
            }


            Image {
                id: icon3D
                visible: styleData.column % 2 ? true : false
                x: columnDel.width - 50
                y: 5
                height: 12
                //width: styleData.value.icon !== undefined ? 12 : 0
                width: 12
                source: {
                    if(visible === true){
                        if(styleData.value.icon){
                            //"../img/32/" + styleData.value.icon
                            "../img/" + styleData.value.icon
                        }
                        else{
                            ""
                        }
                    }
                    else{
                        ""
                    }
                }
            }
            Image {
                id: iconAlert
                visible: styleData.column % 2 ? true : false
                x: columnDel.width - 30
                y: 5
                height: 12
                width: 12
                source: {

                    if(visible === true)
                    {
                        if(styleData.value.icon2){
                            //"../img/32/" + styleData.value.icon
                            "../img/" + styleData.value.icon2
                        }
                        else{
                            ""
                        }
                    }
                    else{
                        ""
                    }

                    //"../img/alert.png"
                }
            }

            MouseArea {
                id: iconArea
                visible: styleData.column % 2 ? true : false
                anchors.fill: {
                    if (styleData.value.icon === "" || styleData.value.icon === undefined){
                        if (styleData.value.icon2 === "" || styleData.value.icon2 === undefined){
                            undefined
                        }
                        else{
                            iconAlert
                        }
                    }
                    else
                    {
                        icon3D
                    }
                }
                hoverEnabled: true
                drag.target: draggable
            }

            // item name
            Text {
                id: textDisplay
                //visible: styleData.column % 2  ? true : false
                visible: false
                x: styleData.value.icon !== undefined ? 20 : 10
                y: 5
                text: styleData.value.name !== undefined ? styleData.value.name : ""
                //color: styleData.selected? cellCurrentRowColor : cellRowColor
                color: cellRowColor

                Component.onCompleted: {
                    if(styleData.value.font !== undefined)
                        textDisplay.font = styleData.value.font;
                }
            }

        }

    }

    rowDelegate: Item {
        width: 200
        height: styleData.hasChildren ? 20 :20

        Rectangle {
            color: styleData.row  % 2 ? Global.lightGrey : Global.darkGrey
            anchors.fill: parent
        }

    }

//    // c++ integration
    onExpanded: {
        //console.log("Signal : Treeview Item onExpanded Called");
        sigItemExpanded(index)
    }

    function itemExpand(itemIndex) {
        if(!isExpanded(itemIndex))
            expand(itemIndex);
        //console.log("Got message:", itemIndex)
        //return "some return value"
    }

    function itemCollapse(itemIndex) {
        if(isExpanded(itemIndex))
            collapse(itemIndex);
        //console.log("Got message:", itemIndex)
        //return "some return value"
    }

    onCollapsed: {
        //console.log("Signal : Treeview Item onCollapsed Called");
        sigItemCollapsed(index)
    }

    onDoubleClicked: {
        //console.log("Signal : Treeview Item onDoubleClicked Called");
        isExpanded(index) ? collapse(index) : expand(index)
    }

//    onActivated: {
//        console.log("Signal : Treeview Item onActivated Called:" + index);
//        sigItemClicked(index)
//    }

    //    onClicked: {
    //        //console.log("Signal : Treeview Item onClicked Called" + index);

    //        takeSelModel.setCurrentIndex(index,0x0002)
    //        takeSelModel.select(index, 0x0002)
    //        sigItemClicked(index)

    //        if(treeView.ctrlKey === true)
    //        {
    //            //console.log("Signal : Treeview Item onCurrentAdded Called in selection model" + current + ", " + previous);
    //            sigItemSelectionAdded(current);
    //            //sigItemSelectionAdded(index);
    //        }
    //        else
    //        {
    //            //takeSelModel.clearSelection()
    //            //console.log("Signal : Treeview Item onCurrentChanged Called in selection model" + current + ", " + previous);
    //            sigItemSelectionChanged(current);
    //        }

    //    }

    Keys.onPressed:{
        //console.log("Signal : Treeview Item keypressed");
        if((event.key === Qt.Key_Control))
        {
            //console.log("Signal : Treeview ctrl true");
            ctrlKey = true;
        }
    }

    Keys.onReleased:{
        if((event.key === Qt.Key_Control))
        {
            //console.log("Signal : Treeview ctrl false");
            ctrlKey = false;
        }
    }

}
