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
    property string typedString: ""
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


    signal sigSearchTextChanged(string searchText)

    Keys.onReleased: {
        if( (event.key >= Qt.Key_A && event.key <= Qt.Key_Z) || (event.key >= Qt.Key_0 && event.key <= Qt.Key_9) || event.key === Qt.Key_Underscore || event.key === Qt.Key_Minus){
            typedString += event.text
            
        }
        else if(event.key === Qt.Key_Escape || event.key === Qt.Key_Backspace){
            typedString = ""
        }
    }

    onTypedStringChanged: {
        sigSearchTextChanged(typedString)
    }

    // main combo text
    contentItem: Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: emptyString
        font.pointSize: fontSize
        font.italic: fontItalic
        font.bold: fontBold
        color: popup.visible ? "#fff" : Global.lightWhite
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment:  Text.AlignHCenter
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
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

//        Connections {
//            target: control
//            onPressedChanged: canvas.requestPaint()
//        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = Global.lightWhite
            context.fill();
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
        y: control.height
        width: control.width
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
