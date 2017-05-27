import QtQuick 2.5

Canvas {
    id:canvas
    width: 200
    height: 200

    property real value
    property color foreground: "#59c30b"
    property color background: "#192324"
    property string myFont

    Text {
        id:testo
        text: value.toFixed(0)
        color:"white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
        font.pointSize: 40
    }

    onValueChanged: requestPaint();

    function drawEllipse(context) {

        var angle = 2 * Math.PI * (value/100) - Math.PI/2;

        // Disegna il background
        context.beginPath();
        context.fillStyle = background;
        context.fillRect(0, 0, canvas.width, canvas.height);

        // Disegna un cerchio (di colore scuro)
        context.beginPath();
        context.fillStyle = "#293237"
        context.strokeStyle = "#293237"
        context.arc(canvas.width/2, canvas.height/2, 50, 0, 2*Math.PI, true)
        context.lineTo(canvas.width/2, canvas.height/2);
        context.fill();

        // Disegna il valore
        var gradient = context.createConicalGradient(canvas.width/2, canvas.height/2,Math.PI/2);
        gradient.addColorStop(0.3, foreground)
        gradient.addColorStop(0.95, Qt.lighter(foreground,0.5))

        context.beginPath();
        context.fillStyle = gradient
        context.arc(canvas.width/2, canvas.height/2, 50, -Math.PI/2, angle, false)
        context.lineTo(canvas.width/2, canvas.height/2);
        context.fill();

        // Cancella l'area interna con un cerchio
        context.beginPath();
        context.fillStyle = background
        context.arc(canvas.width/2, canvas.height/2, 50-5, -Math.PI/2, 3/2*Math.PI, false)
        context.fill();
    }

    onPaint: {
        var ctx = canvas.getContext("2d");
        drawEllipse(ctx);
    }
}
