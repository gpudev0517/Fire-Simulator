import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {
    id: pSlider
    property string sliderName : "Sample"
    property double sliderValue : 0.0
    property int singleStep : 1
    property int maxValue : 100
    property int minValue : -100

    width: parent.width - 15
    height: 30

    signal sigSliderValueChanged(var valSlider)

    Text {
        color: "#f6f4f7"
        text: sliderName
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
//        value: sliderValue

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
//            sigSliderValueChanged(slider_main.value)
//        }
//    }

    SpinBox {
        y: 5
        id: spinbox
        from: minValue
        to: maxValue
        stepSize: singleStep
        value: sliderValue
        editable: true
        font.pixelSize: 15

        property int decimals: 0
        //width: 100
        width: parent.width/2
        height: 20
        anchors.right: parent.right
        anchors.rightMargin: 0

        validator: DoubleValidator {
            bottom: Math.min(spinbox.from, spinbox.to)
            top:  Math.max(spinbox.from, spinbox.to)
        }

        textFromValue: function(value, locale) {
            return Number(value /*/ 100*/).toLocaleString(locale, 'f', spinbox.decimals)
        }

        valueFromText: function(text, locale) {
            return Number.fromLocaleString(locale, text) /** 100*/
        }

        onValueChanged: {
            sigSliderValueChanged(spinbox.value)
        }

        contentItem: TextInput {
            id: contentText
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            clip: true
            text: spinbox.textFromValue(spinbox.value, spinbox.locale)
            selectionColor: "#004a80"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#ffffff"
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            selectByMouse: true

            validator: DoubleValidator {
                bottom: Math.min(spinbox.from, spinbox.to)
                top:  Math.max(spinbox.from, spinbox.to)
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

        background: Rectangle {
            color: Global.moreDarkGrey
            radius: 10
        }

    }

}
