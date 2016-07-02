import QtQuick 2.4
import Ubuntu.Components 1.3

Canvas {
    id: root
    anchors.fill: parent
    property var pebble: null
    property var date: null
    property color baseColor: "black"

    readonly property var infoRowModel: model
    ListModel { id: model }

    function paintDurationText(ctx, centerX, y, durationInSecs, titleText) {
        ctx.font = "" + units.gu(4) + "px Ubuntu";
        var hours = Math.floor(durationInSecs / 60 / 60);
        var minutes = Math.floor(durationInSecs / 60 % 60);
        var hoursWidth = ctx.measureText(hours).width
        var minutesWidth = ctx.measureText(minutes).width;
        ctx.font = "" + units.gu(2) + "px Ubuntu";
        var hWidth = ctx.measureText("H ").width
        var mWidth = ctx.measureText("m").width
        var totalWidth = hoursWidth + hWidth + minutesWidth + mWidth

        ctx.font = "" + units.gu(4) + "px Ubuntu";
        ctx.beginPath();
        ctx.text(hours, centerX - totalWidth / 2, y)
        ctx.text(minutes, centerX - totalWidth / 2 + hoursWidth + hWidth , y)
        ctx.font = "" + units.gu(2) + "px Ubuntu";
        ctx.text("H ", centerX - totalWidth / 2 + hoursWidth, y)
        ctx.text("M", centerX + totalWidth / 2 -mWidth, y)

        ctx.font = "" + units.gu(1.5) + "px Ubuntu";
        var titleWidth = ctx.measureText(titleText).width
        ctx.text(titleText, centerX - titleWidth / 2, y - units.gu(4))
        ctx.fill();
        ctx.closePath();
    }

    function paintTimeText(ctx, centerX, y, timeDate, titleText) {
        ctx.beginPath();
        ctx.font = "" + units.gu(1.5) + "px Ubuntu";
        var titleWidth = ctx.measureText(titleText).width
        ctx.text(titleText, centerX - titleWidth / 2, y - units.gu(4))

        ctx.font = "" + units.gu(4) + "px Ubuntu";
        var timeText = pad(timeDate.getHours(), 2) + ":" + pad(timeDate.getMinutes(), 2);
        var timeTextWidth = ctx.measureText(timeText).width;
        ctx.text(timeText, centerX - timeTextWidth / 2, y);
        ctx.fill();
        ctx.closePath();
    }

    function paintDurationSummary(ctx, centerX, y, durationInSecs, titleText, bottomText, baseColor) {

        paintDurationText(ctx, centerX, y + units.gu(5.5), durationInSecs, titleText)

        ctx.beginPath();
        ctx.font = "" + units.gu(1.5) + "px Ubuntu";
        ctx.fillStyle = "yellow";
        var bottomSize = ctx.measureText(bottomText);
        roundRect(ctx, centerX - bottomSize.width / 2 - units.gu(.5), y + units.gu(6.5), bottomSize.width + units.gu(1), units.gu(2), units.dp(3), true, false)
        ctx.fill();
        ctx.closePath();
        ctx.restore();

        ctx.save();
        ctx.fillStyle = Qt.darker(baseColor, 1.2)

        ctx.beginPath();
        ctx.text(bottomText, centerX - bottomSize.width / 2, y + units.gu(8))
        ctx.fill();
        ctx.closePath();

        ctx.restore();

    }

    function paintCounterSummary(ctx, centerX, y, counter, titleText, bottomText, baseColor) {
        ctx.beginPath();
        ctx.font = "" + units.gu(1.5) + "px Ubuntu";
        ctx.fillStyle = Qt.lighter(baseColor);
        var titleSize = ctx.measureText(titleText);
        ctx.text(titleText, centerX - titleSize.width / 2, y + units.gu(1.5))
        ctx.fill();
        ctx.closePath();

        ctx.beginPath();
        ctx.font = "" + units.gu(4) + "px Ubuntu";
        ctx.fillStyle = "white"
        var textSize = ctx.measureText("" + counter);
        ctx.text("" + counter, centerX - textSize.width / 2, y + units.gu(5.5));
        ctx.fill();
        ctx.closePath();


        ctx.beginPath();
        ctx.font = "" + units.gu(1.5) + "px Ubuntu";
        ctx.fillStyle = "yellow";
        var bottomSize = ctx.measureText(bottomText);
        roundRect(ctx, centerX - bottomSize.width / 2 - units.gu(.5), y + units.gu(6.5), bottomSize.width + units.gu(1), units.gu(2), units.dp(3), true, false)
        ctx.fill();
        ctx.closePath();

        ctx.restore();

        ctx.save();
        ctx.fillStyle = Qt.darker(baseColor, 1.2)

        ctx.beginPath();
        ctx.text(bottomText, centerX - bottomSize.width / 2, y + units.gu(8))
        ctx.fill();
        ctx.closePath();

        ctx.restore();
    }

    function roundRect(ctx, x, y, width, height, radius, fill, stroke) {
        if (typeof stroke == 'undefined') {
            stroke = true;
        }
        if (typeof radius === 'undefined') {
            radius = 5;
        }
        if (typeof radius === 'number') {
            radius = {tl: radius, tr: radius, br: radius, bl: radius};
        } else {
            var defaultRadius = {tl: 0, tr: 0, br: 0, bl: 0};
            for (var side in defaultRadius) {
                radius[side] = radius[side] || defaultRadius[side];
            }
        }
        ctx.beginPath();
        ctx.moveTo(x + radius.tl, y);
        ctx.lineTo(x + width - radius.tr, y);
        ctx.quadraticCurveTo(x + width, y, x + width, y + radius.tr);
        ctx.lineTo(x + width, y + height - radius.br);
        ctx.quadraticCurveTo(x + width, y + height, x + width - radius.br, y + height);
        ctx.lineTo(x + radius.bl, y + height);
        ctx.quadraticCurveTo(x, y + height, x, y + height - radius.bl);
        ctx.lineTo(x, y + radius.tl);
        ctx.quadraticCurveTo(x, y, x + radius.tl, y);
        ctx.closePath();
        if (fill) {
            ctx.fill();
        }
        if (stroke) {
            ctx.stroke();
        }

    }

    function paintCurve(ctx, points) {
        // move to the first point
        if (points.length == 0) {
            return;
        }

        ctx.moveTo(points[0].x, points[0].y);
        if (points.length == 2) {
            ctx.lineTo(points[1].x, points[1].y);
            return;
        }

        var i
        for (i = 1; i < points.length - 2; i ++)
        {
            var xc = (points[i].x + points[i + 1].x) / 2;
            var yc = (points[i].y + points[i + 1].y) / 2;
            ctx.quadraticCurveTo(points[i].x, points[i].y, xc, yc);
        }
        // curve through the last two points
        ctx.quadraticCurveTo(points[i].x, points[i].y, points[i+1].x,points[i+1].y);
    }

    function paintList(ctx, list, slotWidth, maxValue, closePath) {
        if (list.length == 0) return;

        var point = {}
        var points = []
        point["x"] = 0;
        point["y"] = root.height - (list[0] * (root.height / 2) / (maxValue * 1.2))
        points.push(point);

        for (var i = 0; i < list.length; i++) {
            point = {}
            point["x"] = slotWidth * (i+1);
            // s : ms = x : height
            point["y"] = root.height - (list[i] * (root.height / 2) / (maxValue * 1.2))
            points.push(point)
            //                ctx.lineTo(point.x, point.y)
        }
        paintCurve(ctx, points)

        if (closePath) {
            ctx.lineTo(point.x, root.height)
            ctx.lineTo(0, root.height)
            ctx.fill();
        } else {
            ctx.stroke();
        }
    }

    function pad(num, size) {
        var s = num+"";
        while (s.length < size) s = "0" + s;
        return s;
    }
}
