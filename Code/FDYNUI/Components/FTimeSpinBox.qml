import QtQuick 2.11
import QtQuick.Controls 2.0
import "../"

SpinBox {
    id: spinbox
    from: 0
    to: 999999999
    stepSize: 1
    editable: true
    font.pixelSize: 15
    width: 70
    height: 20

    property bool isDouble: false
    property int decimals: 2
    property int factor: 10
    property bool showIndicators: true
    property color fontColor: "#fff"

    signal sigEditingFinished();

    validator: DoubleValidator {
        bottom: Math.min(spinbox.from, spinbox.to)
        top:  Math.max(spinbox.from, spinbox.to)
    }

    textFromValue: function(value, locale) {
        if(isDouble)
            return parseFloat(value*1.0/factor).toFixed(decimals);
        else
            return value
    }

    valueFromText: function(text, locale){
        if(isDouble)
            return Number.fromLocaleString(locale, text)*factor;
        else
            return text
    }

    contentItem: TextInput {
        anchors.fill: parent
        anchors.leftMargin: showIndicators === true ? 20 : 0
        anchors.rightMargin: showIndicators === true ? 20 : 0
        clip: true
        text: textFromValue(value, locale)
        //anchors.verticalCenterOffset: 3
//        anchors.verticalCenter: parent.verticalCenter
//        anchors.horizontalCenter: parent.horizontalCenter
        selectionColor: "#004a80"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: fontColor
        inputMethodHints: Qt.ImhFormattedNumbersOnly
        selectByMouse: true
        onEditingFinished: {
            sigEditingFinished();
        }
    }

    up.indicator: Rectangle {
        visible: showIndicators

        x: parent.width - width
        width: showIndicators? 20: 0
        height: parent.height
        implicitWidth: 20
        implicitHeight: 40
        color: spinbox.up.pressed ? Global.lightGrey : Global.darkBlack
        radius: 10

        Text {
            text: "+"
            color: fontColor
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    down.indicator: Rectangle {
        visible: showIndicators

        x: 0
        width: showIndicators? 20: 0
        height: parent.height
        implicitWidth: 20
        implicitHeight: 40
        color: spinbox.down.pressed ? Global.lightGrey : Global.darkBlack
        radius: 10

        Text {
            text: "-"
            color: fontColor
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    background: Rectangle {
        color: Global.darkBlack
        radius: 10
    }

}
