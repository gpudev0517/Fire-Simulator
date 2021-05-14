import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as QC14
import QtQuick.Controls.Styles 1.4 as QCS14

QC14.TreeView {
    id: treeView
    model: tModel
    anchors.fill: parent
    alternatingRowColors: false
    headerVisible: false
    frameVisible: false

    readonly property color cellBackgroundColor: lightGrey
    readonly property color cellRowColor: "#ffffff"
    readonly property color cellCurrentRowColor: darkBlue
    readonly property color cellPressedColor: lightWhite

    QC14.TableViewColumn {
        role: "name"
        title: "Name"
        width: treeView.width
    }

    style: QCS14.TreeViewStyle {
//        activateItemOnSingleClick: true
        backgroundColor: cellBackgroundColor
        itemDelegate: Item {
            width: 150
            height: 50
            Rectangle {
                anchors.fill: parent
                color: styleData.row % 2 ? darkGrey: lightGrey

                Image {
                    x: 5
                    y: 7
                    width: 12
                    height: 12
                    source: styleData.value[1] !== undefined ? "img/" + styleData.value[1] : "img/none.png"
                }

                Text {
                    x: 25
                    y: 7
                    text: styleData.value[0] !== undefined ? styleData.value[0] : "Undefined"
                    color: styleData.selected ? cellCurrentRowColor : cellRowColor
                    //anchors.centerIn: parent
//                    onEditingFinished: {
//                        tModel.setData(styleData.index, text)
//                    }
                }

                Image {
                    x: 240
                    y: 7
                    width: 12
                    height: 12
                    source: styleData.value[2] !== undefined ? "img/" + styleData.value[2] : "img/none.png"
                }
            }
        }

        rowDelegate: Item {
            width: 200
            height: 24

            Rectangle {
                color: styleData.row % 2 ? darkGrey: lightGrey
                anchors.fill: parent
            }
        }

    }
}

