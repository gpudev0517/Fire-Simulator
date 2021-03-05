import QtQuick 2.7
import QtQuick.Controls 2.4
import "../"


Item {
    id: pSlider
    property string doubleSliderName : "Sample"
    property double doubleSliderValue : 0.0
    property double singleStep : 1.0
    property double maxValue : 100.0
    property double minValue : -100.0
    property int decimals: 2

    width: parent.width - 15
    height: 30

    signal sigDoubleSliderValueChanged(var valSlider)

    Text {
        id: title
        color: "#f6f4f7"
        text: doubleSliderName
        width: parent.width/2
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        //anchors.topMargin: 5
        font.italic: false
        verticalAlignment: Text.AlignVCenter
        font.family: "Segoe UI"
        //horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 11
    }

//    Slider {
//        id: slider_main
//        y: 12
//        height: 15
//        anchors.right: parent.right
//        anchors.rightMargin: 0
//        anchors.left: parent.left
//        anchors.leftMargin: 0
//        from: minValue
//        to: maxValue
//        live:false
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 0
//        stepSize: singleStep
//        value: doubleSliderValue


//        background: Rectangle {
//            y: parent.topPadding + parent.availableHeight / 2 - height / 2
//            width: parent.width
//            height: 2
//            radius: 1
//            color: Global.lightWhite
//        }

//        handle: Rectangle {
//            x: parent.visualPosition * (parent.width - width)
//            y: parent.topPadding + parent.availableHeight / 2 - height / 2
//            width: 12
//            height: 12
//            radius: 12
//            color: parent.pressed ? Global.darkBlue : Global.lightGrey
//            border.color: "#fff"
//        }

//        onValueChanged: {
//            doubleSliderValue = value
//            sigDoubleSliderValueChanged(value)
//        }
//    }

    SpinBox {

        property int factor: Math.pow(10, pSlider.decimals)

        y: 5
        id: spinbox
//        from: minValue * factor
//        to: maxValue * factor
//        stepSize: singleStep * factor
//        value: doubleSliderValue * factor
        from: minValue
        to: maxValue
//        from: -100
//        to: 100

        stepSize: singleStep
        value: 0
        editable: true
        font.pixelSize: 15


        //width: 100
        width: parent.width/2
        height: 20
        anchors.right: parent.right
        anchors.rightMargin: 0

        validator: DoubleValidator {
            bottom: Math.min(-1.8e+308, 1.8e+308)
            top:  Math.max(-1.8e+308, 1.8e+308)
        }


//        textFromValue: function(value, locale) {
//            return Number(value / factor).toLocaleString(locale, 'f', pSlider.decimals)
//        }

//        valueFromText: function(text, locale) {
//            return Math.round(Number.fromLocaleString(locale, text) * factor)
//        }

        onValueChanged: {
            value = 0
        }

        contentItem: TextInput {
            id: contentText
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            clip: true
            //text: spinbox.textFromValue(spinbox.value, spinbox.locale)
            text: Number(doubleSliderValue).toLocaleString(spinbox.locale, 'f', pSlider.decimals)
            selectionColor: "#004a80"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#ffffff"
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            selectByMouse: true
            validator: DoubleValidator {
                bottom: Math.min(-1.8e+308, 1.8e+308)
                top:  Math.max(-1.8e+308, 1.8e+308)
            }

            onEditingFinished:{
                sigDoubleSliderValueChanged(Number.fromLocaleString(spinbox.locale, text))
            }

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    contentText.forceActiveFocus()
                    contentText.selectAll()
                }
            }
        }

        up.indicator: Rectangle {
            x: parent.width - width
            height: parent.height
            implicitWidth: 20
            implicitHeight: 40
            color: spinbox.up.pressed ? Global.darkBlack : Global.moreDarkGrey
            radius: 10

            Text {
                text: "+"
                color: "#ffffff"
                anchors.fill: parent
                fontSizeMode: Text.Fit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        up.onPressedChanged: {
            contentText.enabled = false
            if (up.pressed === false)
                sigDoubleSliderValueChanged(doubleSliderValue + singleStep)
            contentText.enabled = true
        }

        down.indicator: Rectangle {
            x: 0
            height: parent.height
            implicitWidth: 20
            implicitHeight: 40
            color: spinbox.down.pressed ? Global.darkBlack : Global.moreDarkGrey
            radius: 10

            Text {
                text: "-"
                color: "#ffffff"
                anchors.fill: parent
                fontSizeMode: Text.Fit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        down.onPressedChanged: {
            contentText.enabled = false
            if (down.pressed === false)
                sigDoubleSliderValueChanged(doubleSliderValue - singleStep)
            contentText.enabled = true
        }

        background: Rectangle {
            color: Global.moreDarkGrey
            radius: 10
        }

    }

}
