import QtQuick 2.11
import QtQuick.Controls 2.4
import "../"

ComboBox {
    id: control
    property int fontSize : 9
    property bool fontBold : false
    property bool fontItalic : false
    property color bkColor : Global.darkGrey
    property string emptyString
    property bool widePopup : false
    editable: true
    focus: true

    onActivated: {
        comboValChanged(currentText)
    }

    signal comboValChanged(string solverVal)
//    onComboValChanged: {
//        currentIndex = find(solverVal);

//        if (currentIndex === -1)
//            currentIndex = 0

//    }

    // main combo text
    contentItem: Text {
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.left: parent.left
        anchors.rightMargin: 0
        anchors.right: parent.right
        text: control.displayText.length == 0 ? emptyString : control.displayText
        font.pointSize: fontSize
        font.italic: false
        font.bold: fontBold
        //color: popup.visible ? "#fff" : Global.lightWhite
        color: "#fff"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        height: parent.height
        //combo background
        color: bkColor
        radius: 10
    }

    indicator: Canvas {
        id: canvas
        //x: control.width - width - control.rightPadding
        x: control.width - width - 8
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() {
                canvas.requestPaint()
            }
        }

        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width / 2, height);
            ctx.closePath();
            ctx.fillStyle = Global.lightWhite
            ctx.fill();
        }
    }

    // popup menu
    delegate: ItemDelegate {
        id:itemDlgt
        width: popup.width
        height:25

        contentItem: Text {
            text: modelData
            color: parent.highlighted ? Global.darkBlue : Global.lightWhite
            font.pointSize: 10
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            color: Global.darkBlack
            height: 25
            radius: 10
        }

        highlighted: control.highlightedIndex === index
    }

    popup: Popup {
        x: widePopup === true ? - control.width * 0.5 : 0
        y: control.height
        width: widePopup === true ? control.width * 1.5 : control.width
        implicitHeight: contentItem.implicitHeight + 1
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight + 1
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            height: 25

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            radius: 10
            color: Global.darkBlack
        }
    }
}
