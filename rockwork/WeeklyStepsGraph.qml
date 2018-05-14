import QtQuick 2.4
import Ubuntu.Components 1.3

HealthGraph {
    id: root

    onDateChanged: {
        print("should update steps graph", date)
        var startDate = new Date(date);
        startDate.setDate(startDate.getDate() - startDate.getDay() + 1);
        startDate.setHours(0);
        startDate.setMinutes(0);
        startDate.setSeconds(0);

        d.stepsList = []
        d.maxSteps = 0
        d.averageSteps = 0;
        var count = 0;
        for (var i = 0; i < 7; i++) {
            var blockStartDate = new Date(startDate);
            blockStartDate.setDate(startDate.getDate() + i);
            var blockEndDate = new Date(blockStartDate);
            blockEndDate.setDate(blockStartDate.getDate() + 1);

            print("fetching steps for day:", blockStartDate.toString())
            var blockSteps = 0;
            if (blockStartDate <= new Date()) {
                blockSteps = root.pebble.steps(blockStartDate, blockEndDate);
                d.averageSteps += blockSteps;
                count++;
            } else {
                blockSteps = -1;
            }
            d.stepsList.push(blockSteps);
            d.maxSteps = Math.max(d.maxSteps, blockSteps)
        }
        d.averageSteps /= count;
        d.maxSteps = Math.max(d.maxSteps, d.averageSteps)

        var today = new Date(root.date);
        today.setHours(0);
        today.setMinutes(0);
        today.setSeconds(0);
        var tomorrow = new Date(today);
        tomorrow.setDate(tomorrow.getDate() + 1)
        d.typicalSteps = root.pebble.averageSteps(today, tomorrow);
        d.maxSteps = Math.max(d.maxSteps, d.typicalSteps)

        root.infoRowModel.clear();
        root.infoRowModel.append({type: "distance", title: "Avg Distance", distance: "???"})
        root.infoRowModel.append({type: "calories", title: "Avg Calories", calories: "???"})
        root.infoRowModel.append({type: "activetime", title: "Avg Active", activeTime: "???"})

        requestPaint();
    }

    QtObject {
        id: d
        property var stepsList: []
        property int typicalSteps: 0
        property int averageSteps: 0
        property int maxSteps: 0
    }

    onPaint: {
        var ctx = getContext('2d');
        ctx.save();
        ctx.reset()

        var slotsWidth = root.width / 7;
        var dayNames = ["M", "T", "W", "T", "F", "S", "S"]

        ctx.beginPath();
        ctx.fillStyle = "yellow"
        // avg : ms = y : r.h
        var avgY = root.height - d.typicalSteps * (root.height / 2) / d.maxSteps + units.gu(.25)
        ctx.rect(0, avgY, root.width, units.gu(.5))
        ctx.fill();
        ctx.closePath();

        ctx.font = "bold " + units.gu(1.5) + "px Ubuntu";

        for (var i = 0; i < 7; i++) {
            ctx.beginPath();
            ctx.fillStyle = Qt.darker(root.baseColor, 1.5)
            var height = slotsWidth - units.gu(2)
            roundRect(ctx, slotsWidth * i + units.gu(1), root.height - height, slotsWidth - units.gu(2), height, units.dp(3), true, false)
            ctx.fill();
            ctx.closePath();

            if (d.stepsList[i] >= 0) {
                ctx.beginPath();
                ctx.fillStyle = Qt.lighter(root.baseColor, 1.2)
                // s : ms = h : r.h
                height = d.stepsList[i] * root.height / 2 / d.maxSteps
                roundRect(ctx, slotsWidth * i + units.gu(1), root.height - height, slotsWidth - units.gu(2), height, units.dp(3), true, false)
                ctx.fill();
                ctx.closePath();
            }
            ctx.beginPath();
            ctx.fillStyle = "white"
            var textWidth = ctx.measureText(dayNames[i]).width
            ctx.text(dayNames[i], slotsWidth * i + (slotsWidth - textWidth) / 2, root.height - units.gu(1))
            ctx.fill();
            ctx.closePath();
        }

        paintCounterSummary(ctx, root.width / 2, units.gu(2), d.averageSteps, i18n.tr("AVERAGE STEPS"), i18n.tr("TYPICAL %1").arg(d.typicalSteps), root.baseColor)
        ctx.restore();
    }
}

