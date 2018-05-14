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

        d.sleepList = [];
        d.maxSleep = 0;
        d.averageSleep = 0;
        var count = 0;
        for (var i = 0; i < 7; i++) {
            var blockStartDate = new Date(startDate);
            blockStartDate.setDate(startDate.getDate() + i);

//            print("fetching sleep for day:", blockStartDate.toString())
            var sleepDuration = 0;
            var deepSleepDuration = 0;
            if (blockStartDate <= new Date()) {
                var result = root.pebble.sleepDataForDay(blockStartDate);
//                print("have result", result)
                for (var j = 0; j < result.length; j++) {
                    var entry = result[j];
                    if (entry.type == 0) {
                        sleepDuration += entry.duration;
                    } else {
                        deepSleepDuration += entry.duration;
                    }
                }
                d.averageSleep += sleepDuration;
                // Assume the user didn't wear the watch
                if (sleepDuration > 0) {
                    count++;
                }
            } else {
                sleepDuration = -1;
                deepSleepDuration = -1;
            }
            print("have sleep for day:", blockStartDate.toString(), sleepDuration, count)
            var sleepItem = {};
            sleepItem.sleepDuration = sleepDuration;
            sleepItem.deepSleepDuration = deepSleepDuration;
            d.sleepList.push(sleepItem);
            d.maxSleep = Math.max(d.maxSleep, sleepDuration)
//            d.maxSteps = Math.max(d.maxSteps, blockSteps)
        }
        d.averageSleep /= count;

        // fetch typical sleep as weekday average
        var averageStartDate = new Date(startDate)
        averageStartDate.setDate(averageStartDate.getDate() - 30);
        var averageEndDate = new Date(startDate)
        averageEndDate.setDate(averageEndDate.getDate() + 6)
        var lastWeekDay = new Date(startDate);
        lastWeekDay.setDate(lastWeekDay.getDate() + 4)
        var averageSleepTimes = root.pebble.averageSleepTimes(lastWeekDay);
        d.averageSleepTime = averageSleepTimes.fallasleep || -1;
        d.averageWakeupTime = averageSleepTimes.wakeup || -1;

        d.typicalSleep = root.pebble.sleepAverage(averageStartDate, averageEndDate)
        var averageDeepSleepDuration = root.pebble.deepSleepAverage(averageStartDate, averageEndDate)


//        var weekEndDate = new Date(startDate);
//        weekEndDate.setDate(startDate.getDate() + 7);
//        d.typicalSteps = root.pebble.averageSteps(startDate, weekEndDate) / 7;
//        d.maxSteps = Math.max(d.maxSteps, d.averageSteps)

//        d.averageSteps = root.pebble.steps(startDate, weekEndDate) / 7;

        var avgFallAsleep = new Date(d.averageSleepTime * 1000);
        var avgWakeup = new Date(d.averageWakeupTime * 1000);

        root.infoRowModel.clear();
        root.infoRowModel.append({type: "duration", title: "Avg Deep Sleep", duration: averageDeepSleepDuration})
        root.infoRowModel.append({type: "time", title: "Avg Fall Asleep", time: pad(avgFallAsleep.getHours(), 2) + ":" + pad(avgFallAsleep.getMinutes(), 2)})
        root.infoRowModel.append({type: "time", title: "Avg Wakeup", time: pad(avgWakeup.getHours(), 2) + ":" + pad(avgWakeup.getMinutes(), 2)})

        requestPaint();
    }

    QtObject {
        id: d
        property var sleepList: []
        property int typicalSleep: 0
        property int averageSleep: 0
        property int maxSleep: 0
        property int averageSleepTime : 0
        property int averageWakeupTime: 0
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
        print("bla", d.typicalSleep, d.maxSleep)
        var avgY = root.height - d.typicalSleep * root.height / 2 / d.maxSleep + units.gu(.25)
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

            if (d.sleepList[i].sleepDuration >= 0) {
                ctx.beginPath();
                ctx.fillStyle = Qt.lighter(root.baseColor, 1.4)
                // s : ms = h : r.h
                height = d.sleepList[i].sleepDuration * root.height / 2 / d.maxSleep
                roundRect(ctx, slotsWidth * i + units.gu(1), root.height - height, slotsWidth - units.gu(2), height, units.dp(3), true, false)
                ctx.fill();
                ctx.closePath();
            }

            if (d.sleepList[i].deepSleepDuration >= 0) {
                ctx.beginPath();
                ctx.fillStyle = Qt.lighter(root.baseColor, 1.2)
                // s : ms = h : r.h
                height = d.sleepList[i].deepSleepDuration * root.height / 2 / d.maxSleep
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

        paintDurationSummary(ctx, root.width / 2, units.gu(2), d.averageSleep, i18n.tr("AVERAGE SLEEP"), i18n.tr("TYPICAL %1H %2M").arg(Math.floor(d.typicalSleep / 60 / 60)).arg(Math.round(d.typicalSleep / 60 % 60)), root.baseColor)
        ctx.restore();
    }

}

