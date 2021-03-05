import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.2
import FPropertyModel 1.0
import FPropertyView 1.0
import "../"


TreeView {
    id: propertyView
    objectName: "propertyview"
    anchors.fill: parent

    model: propertyModel
    alternatingRowColors: false
    headerVisible: false
    frameVisible: false
    horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff

    readonly property color cellBackgroundColor: Global.lightGrey
    readonly property color cellRowColor: "#ffffff"
    readonly property color cellCurrentRowColor: Global.darkBlue
    readonly property color cellPressedColor: Global.lightWhite
    readonly property color headerColor: Global.lightGrey


    readonly property int type_Group: 0
    readonly property int type_Slider: 1
    readonly property int type_Switch: 2
    readonly property int type_Combo: 3
    readonly property int type_WithChild: 4
    readonly property int type_FilePath: 5
    readonly property int type_ColorRamp: 6
    readonly property int type_NEBool: 7
    readonly property int type_String: 8
    readonly property int type_DoubleSlider: 9
    readonly property int type_ColorPicker: 10
    readonly property int type_SurfaceMaterialSelector: 11
    readonly property int type_FireMaterialSelector: 12
    readonly property int type_FailureCriteriaSelector: 13
    readonly property int type_DynamicCombo: 14
    readonly property int type_VentCriteriaSelector: 15
    readonly property int type_CableSelector: 16


    signal sigItemExpanded(var idx)
    signal sigItemCollapsed(var idx)
    signal sigItemClicked(var idx)
    signal sigItemSelected(var idx)
    signal sigItemSelectionChanged(var selected, var deselected)
    signal sigItemSelectionAdded(var idx)
    signal sigCreateContextMenu()
    signal sigCurrentTextInput(var txtInput)
    signal sigItemRenamed(var idx, string newStr)

    //signals for properties
    signal sigItemValueChanged(var index, var value, var type)
    signal sigDynamicComboItemValueChanged(var index, var value, var models, var type)
    signal sigOpenFilePathDialog(var index, var value, var type)
    signal sigColorPickerDialog(var index, var type)
    signal sigMaterialDialog(var index, var value, var type, var materialName)


    function setCurrentItemFromCpp(idx){
        takeSelModel.select(idx, ItemSelectionModel.ClearAndSelect)
    }

    function clearCurrentItemFromCpp(){
        takeSelModel.clearSelection()
    }

    function setSelectItemFromCpp(idx){
        takeSelModel.select(idx, ItemSelectionModel.Select)
    }

    function setFilePathFromCpp(strFilePath){

    }

    Column{
        z:-1
        Repeater {
            model: 50
            Rectangle {
                width: propertyView.width; height: 30
                color: index % 2 ? Global.lightGrey : Global.darkGrey
                //color: Global.lightGrey
            }
        }
    }

    TableViewColumn {
        id: nameColumn
        role: "name"
        title: "Name"
        width: propertyView.width/* - typeColumn.width*/

    }

//    QC14.TableViewColumn {
//        id: typeColumn
//        role: "type"
//        title: "Type"
//        width: 50
//    }

    style: TreeViewStyle {
        activateItemOnSingleClick: true
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
        model: propertyModel

        onSelectionChanged: {
            sigItemSelectionChanged(selected, deselected);
        }
    }

    itemDelegate: Item {
        id: columnDel
        width: 300
        height: 30

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            //drag.target: draggable

            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                columnRect.forceActiveFocus()
//                if(mouse.modifiers & Qt.ControlModifier){
//                    takeSelModel.select(styleData.index, ItemSelectionModel.Select)
//                }
//                else{
                    takeSelModel.select(styleData.index, ItemSelectionModel.ClearAndSelect)
                //}

                if (mouse.button === Qt.RightButton){
                    sigCreateContextMenu();
                    //sigCurrentTextInput(textEdit)
                }
            }
        }

        Rectangle {
            id: columnRect
            anchors.fill: parent
            color:  styleData.row  % 2 ? Global.lightGrey : Global.darkGrey

            FSlider {
                id: pSlider;
                visible: styleData.value && styleData.value.type === type_Slider ? true : false
                minValue: styleData.value && styleData.value.extra1 !== undefined && visible === true  ? styleData.value.extra1 : minValue
                maxValue: styleData.value && styleData.value.extra2 !== undefined && visible === true  ? styleData.value.extra2 : maxValue
                singleStep: styleData.value && styleData.value.extra3 !== undefined && visible === true  ? styleData.value.extra3 : singleStep
                sliderName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                sliderValue: styleData.value && styleData.value.value !== undefined && visible === true ? styleData.value.value : 0
                onSigSliderValueChanged: {
                    if(sliderValue !== valSlider)
                        sigItemValueChanged(styleData.index, valSlider, type_Slider)
                }

            }

            FDoubleSlider {
                id: pDoubleSlider;
                decimals: 5
                visible: styleData.value && styleData.value.type !== undefined && styleData.value.type === type_DoubleSlider ? true : false
                minValue: styleData.value && styleData.value.extra1 !== undefined && visible === true   ? styleData.value.extra1 : minValue
                maxValue: styleData.value && styleData.value.extra2 !== undefined && visible === true   ? styleData.value.extra2 : maxValue
                singleStep: styleData.value && styleData.value.extra3 !== undefined && visible === true  ? styleData.value.extra3 : singleStep
                doubleSliderName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                doubleSliderValue: styleData.value && styleData.value.value !== undefined && visible === true ? styleData.value.value : doubleSliderValue
                onSigDoubleSliderValueChanged: {
                    sigItemValueChanged(styleData.index, valSlider, type_DoubleSlider)
                }

            }
            FSwitch {
                id: pSwitch;
                visible: styleData.value && styleData.value.type === type_Switch ? true : false
                switchName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                switchChecked: styleData.value && styleData.value.value !== undefined && visible === true ? styleData.value.value : false
                onSigSwitchChanged: {
                    sigItemValueChanged(styleData.index, checked, type_Switch)
                }
            }
            PCombo {
                id:pCombo;
                visible: styleData.value && styleData.value.type === type_Combo ? true : false
                comboName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                comboData: styleData.value && styleData.value.extra !== undefined && visible === true ? styleData.value.extra : []
                currentIdx: styleData.value && styleData.value.value !== undefined && visible === true ? styleData.value.value : 0
                onSigCurrentIdxChanged: {
                    sigItemValueChanged(styleData.index, curIdx, type_Combo)
                }
            }
            FItemWithChildren {
                id: pParentItem
                visible: styleData.value && styleData.value.type === type_WithChild ? true : false
                containerName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                containerValue: styleData.value && styleData.value.value !== undefined && visible === true ? styleData.value.value : ""
            }
            FString {
                id: pString
                visible: styleData.value && styleData.value.type === type_String ? true : false
                stringName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                stringValue: styleData.value && styleData.value.value !== undefined && visible === true ? styleData.value.value : ""
                onSigStringValueChanged: {
                    sigItemValueChanged(styleData.index, string, type_String)
                }
            }
            FFilePath {
                id: pFilePath
                visible: styleData.value && styleData.value.type === type_FilePath ? true : false
                filePathName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                filePathValue: (styleData.value && styleData.value.value !== undefined && visible === true) ? styleData.value.value : ""
                onSigFilePathValueChanged: {
                    if(filePathValue !== filePathString)
                        sigItemValueChanged(styleData.index, filePathString, type_FilePath)
                }
                onSigFilePathButtonClicked: {
                    sigOpenFilePathDialog(styleData.index, filePathString, type_FilePath)
                }
            }

            FColorPicker {
                id: pColorPicker
                visible: styleData.value && styleData.value.type === type_ColorPicker ? true : false
                pickColor: styleData.value && styleData.value.extra !== undefined && visible === true ? styleData.value.extra : pickColor
                colorPickerName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                colorPickerValue: (styleData.value && styleData.value.value !== undefined && visible === true) ? styleData.value.value : ""
                onPickColorChanged: {
                    pColorPicker.requireRender();
                }

                onSigColorPickerButtonClicked: {
                    sigColorPickerDialog(styleData.index, type_ColorPicker)
                }
            }

            FMaterialSelector {
                id: pMaterialSelector
                visible: (styleData.value && (styleData.value.type === type_SurfaceMaterialSelector || styleData.value.type === type_CableSelector || styleData.value.type === type_FireMaterialSelector
                                              || styleData.value.type === type_FailureCriteriaSelector || styleData.value.type === type_VentCriteriaSelector )) ? true : false
                materialSelectorName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                //materialSelectorValue: (styleData.value && styleData.value.value !== undefined && visible === true) ? styleData.value.materialText : ""
                comboData: (styleData.value && styleData.value.materialList !== undefined && visible === true) ? styleData.value.materialList : []
                currentIdx: styleData.value && styleData.value.materialIndex !== undefined && visible === true ? styleData.value.materialIndex : 0

                onSigMaterialSelectorButtonClicked: {
                    sigMaterialDialog(styleData.index, styleData.value.value, styleData.value.type, styleData.value.name)
                }

                onSigCurrentIdxChanged: {
                    sigItemValueChanged(styleData.index, curIdx, styleData.value.type)
                }
            }

            FDynamicCombo {
                id: pDynamicCombo
                visible: (styleData.value && styleData.value.type === type_DynamicCombo ) ? true : false
                dynamicComboName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
                dynamicComboData: (styleData.value && styleData.value.extra !== undefined && visible === true) ? styleData.value.extra : []
                currentIdx: styleData.value && styleData.value.index !== undefined && visible === true ? styleData.value.index : 0

                onSigCurrentIdxChanged: {
                    sigDynamicComboItemValueChanged(styleData.index, curIdx, dynamicComboData, styleData.value.type)
                }
            }

            FLabel{
                id: pContainer
                visible: styleData.value && styleData.value.type === type_Group ? true : false
                labelName: styleData.value && styleData.value.name !== undefined && visible === true ? styleData.value.name : ""
            }
        }

    }

    rowDelegate: Item {
        width: propertyView.width
        height: 30

        Rectangle {
            color: styleData.row  % 2 ? Global.lightGrey : Global.darkGrey
            //color: Global.lightGrey
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
    }

    function itemCollapse(itemIndex) {
        if(isExpanded(itemIndex))
            collapse(itemIndex);
        //console.log("Got message:", itemIndex)
    }

    onCollapsed: {
        //console.log("Signal : Treeview Item onCollapsed Called");
        sigItemCollapsed(index)
    }

    onDoubleClicked: {
        //console.log("Signal : Treeview Item onDoubleClicked Called");
        isExpanded(index) ? collapse(index) : expand(index)
    }

}
