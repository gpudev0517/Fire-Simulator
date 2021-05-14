import QtQuick 2.11
import QtQuick.Controls 2.4
import "../"

Rectangle{
    color: "transparent"
    width: 300
    height: 200

    Canvas {
        id: canvas
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        contextType: "2d"

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset();
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.lineTo(width, height * 0.7);
            ctx.lineTo(width * 0.5, height);
            ctx.lineTo(0, height * 0.7);
            ctx.closePath();

            context.fillStyle = "#0000008f"
            context.fill();
        }
    }
}
