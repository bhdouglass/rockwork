import QtQuick 2.4
import Ubuntu.Components 1.3

HealthGraph {
    id: root

    onDateChanged: {
        print("should update steps graph", date)

        var startTime = date;
        startTime.setHours(0);
        startTime.setMinutes(0);
        startTime.setSeconds(0);

        d.stepsList = [];
        d.trimmedStepsList = [];
        d.maxSteps = 0;
        var blockSteps = 0
        for (var i = 0; i < 24; i++) {
            var blockStartTime = new Date(startTime);
            blockStartTime.setHours(i);
            var blockEndTime = new Date(startTime);
            blockEndTime.setHours(i+1);
            if (blockStartTime > new Date()) break;

            print("fetching:", blockStartTime.toString(), "to", blockEndTime.toString())
            blockSteps += root.pebble.steps(blockStartTime, blockEndTime);
            d.stepsList.push(blockSteps);
            if (i >= d.startSlot) {
                d.trimmedStepsList.push(blockSteps);
            }
            d.maxSteps = Math.max(d.maxSteps, blockSteps)
        }

        d.stepsAverageList = [];
        d.trimmedStepsAverageList = [];
        blockSteps = 0;
        for (var i = 0; i < 24; i++) {
            var blockStartTime = new Date(startTime);
            blockStartTime.setHours(i);
            var blockEndTime = new Date(startTime);
            blockEndTime.setHours(i+1);

            blockSteps += root.pebble.averageSteps(blockStartTime, blockEndTime);
            d.stepsAverageList.push(blockSteps);
            if (i >= d.startSlot) {
                d.trimmedStepsAverageList.push(blockSteps)
            }
            d.maxSteps = Math.max(d.maxSteps, blockSteps)
        }


        root.infoRowModel.clear();
        root.infoRowModel.append({type: "distance", title: "Distance", distance: "???"})
        root.infoRowModel.append({type: "calories", title: "Calories", calories: "???"})
        root.infoRowModel.append({type: "activetime", title: "Active Time", activeTime: "???"})

        requestPaint();
    }

    QtObject {
        id: d
        property var stepsList: []
        property var trimmedStepsList: []
        property var stepsAverageList: []
        property var trimmedStepsAverageList: []
        property int maxSteps: 0

        property int startSlot: 8
        property real slotWidth: root.width / (24 - startSlot);


    }

    onPaint: {
        var ctx = getContext('2d');
        ctx.save();
        ctx.reset()

        ctx.font = "" + units.gu(1.5) + "px Ubuntu";
        ctx.lineWidth = 0

        var points = [];

        // Average steps
        ctx.beginPath();
        ctx.fillStyle = "yellow"
        paintList(ctx, d.trimmedStepsAverageList, d.slotWidth, d.maxSteps, true)
        ctx.closePath();


        // Steps
        ctx.beginPath();
        ctx.fillStyle = Qt.lighter(root.baseColor, 1.2)
        paintList(ctx, d.trimmedStepsList, d.slotWidth, d.maxSteps, true)
        ctx.closePath();

        ctx.beginPath();
        ctx.strokeStyle = "yellow"
        ctx.lineWidth = units.gu(.5)
        paintList(ctx, d.trimmedStepsAverageList, d.slotWidth, d.maxSteps, false)
        ctx.closePath();

        ctx.beginPath();
        ctx.font = "bold " + units.gu(1.5) + "px Ubuntu";
        ctx.fillStyle = root.baseColor;
        for (var i = 12; i < 24; i+=4) {
            ctx.text(i + ":00", (i - d.startSlot) * d.slotWidth, root.height - units.gu(.5));
            ctx.fill()
        }
        ctx.closePath();

        var today = new Date();
        var yesterday = new Date()
        yesterday.setDate(today.getDate() - 1)
        var text;
        if (today.getDate() == date.getDate() && today.getDay() == date.getDay()) {
            text = i18n.tr("TODAY'S STEPS");
        } else if (yesterday.getDate() == date.getDate() && yesterday.getDay() == date.getDay()) {
            text = i18n.tr("YESTERDAY'S STEPS")
        } else {
            text = i18n.tr("STEPS")
        }

        paintCounterSummary(ctx, root.width / 2, units.gu(2), d.stepsList[d.stepsList.length - 1], text, i18n.tr("TYPICAL"), root.baseColor)

        ctx.restore();
    }
}

