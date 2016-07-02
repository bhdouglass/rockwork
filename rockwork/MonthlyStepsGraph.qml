import QtQuick 2.4
import Ubuntu.Components 1.3

HealthGraph {
    id: root

    onDateChanged: {

        var startDate = new Date(date);
        startDate.setHours(0);
        startDate.setMinutes(0);
        startDate.setSeconds(0);
        startDate.setDate(1);
        print("starting with", date, startDate)

        var monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dez"]

        d.stepsList = [];
        d.textList = []
        d.maxSteps = 0;
        var blockSteps = 0
        d.averageSteps = 0;
        var count = 0;

        for (var i = 0; i < 5; i++) {
            var blockStartTime = new Date(startDate);
            blockStartTime.setDate(i*7);
            var blockEndTime = new Date(startDate);
            blockEndTime.setDate((i+1)*7);
            if (blockStartTime > new Date()) break;

            print("have starttime", blockStartTime)

            blockSteps = root.pebble.steps(blockStartTime, blockEndTime) / 7;

            d.stepsList.push(blockSteps);
            d.averageSteps += blockSteps;
            count++;

            var textItem = {};
            textItem.startDate = monthNames[blockStartTime.getMonth()] + " " + blockStartTime.getDate()
            textItem.endDate = monthNames[blockEndTime.getMonth()] + " " + blockEndTime.getDate()
            d.textList.push(textItem)
            d.maxSteps = Math.max(d.maxSteps, blockSteps)
            print("blocksteps is", blockSteps)

        }
        d.averageSteps /= count;
        d.maxSteps = Math.max(d.maxSteps, d.averageSteps)

        var endDate = new Date(startDate)
        endDate.setMonth(startDate.getMonth()+1)
        d.typicalSteps = root.pebble.averageSteps(startDate, endDate);
        endDate.setDate(0)
        print("last day", endDate.getDate())
        d.typicalSteps /= endDate.getDate();
        d.averageSteps /= endDate.getDate();
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
        property var textList: []
        property int averageSteps: 0
        property int typicalSteps: 0
        property int maxSteps: 0

        property real slotWidth: root.width / 5
    }

    onPaint: {
        var ctx = getContext('2d');
        ctx.save();
        ctx.reset()

        ctx.lineWidth = 0

        ctx.beginPath();
        ctx.fillStyle = Qt.lighter(root.baseColor, 1.2)
        paintList(ctx, d.stepsList, d.slotWidth, d.maxSteps, true)
        ctx.closePath();

        ctx.beginPath();
        ctx.fillStyle = Qt.darker(root.baseColor, 1.2)
        for (var i = 1; i < 5; i++) {
            ctx.rect(d.slotWidth * i, 0, units.gu(.5), root.height)
        }
        ctx.fill();
        ctx.closePath();

        ctx.beginPath();
        ctx.fillStyle = "white"
        ctx.font = "bold " + units.gu(1.5) + "px Ubuntu";
        for (var i = 0; i < d.textList.length; i++) {
            var textWidth = ctx.measureText(d.textList[i].startDate).width
            ctx.text(d.textList[i].startDate, d.slotWidth * i + (d.slotWidth - textWidth) / 2, root.height - units.gu(3))
            textWidth = ctx.measureText(d.textList[i].endDate).width
            ctx.text(d.textList[i].endDate, d.slotWidth * i + (d.slotWidth - textWidth) / 2, root.height - units.gu(1))
        }
        ctx.fill();
        ctx.closePath();

        ctx.beginPath();
        ctx.fillStyle = "yellow";
        // y : h = steps : maxSteps
        var y = root.height - ((root.height / 2) * d.typicalSteps / d.maxSteps)
        ctx.rect(0, y, root.width, units.gu(.5))
        ctx.fill();
        ctx.closePath();

        paintCounterSummary(ctx, root.width / 2, units.gu(2), d.averageSteps, i18n.tr("AVERAGE STEPS"), i18n.tr("TYPICAL %1").arg(d.typicalSteps), root.baseColor)

        ctx.restore();
    }
}

