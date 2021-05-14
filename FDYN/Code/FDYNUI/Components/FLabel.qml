import QtQuick 2.0
import QtQuick.Controls 2.4
import "../"

Item {
    width: parent.width - 15;
    height: 30
    property string labelName: "Sample"

    Label{
        anchors.fill: parent
        text: labelName
        color: "#fff"
        verticalAlignment: Text.AlignVCenter
    }

}

