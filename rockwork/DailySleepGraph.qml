import QtQuick 2.4
import Ubuntu.Components 1.3

HealthGraph {
    id: root

    onDateChanged: {
        if (!root.date) {
            return;
        }

        d.entryList = root.pebble.sleepDataForDay(root.date);
        print("have sleep data", d.entryList.length)
        d.sleepDuration = 0;
        d.deepSleepDuration = 0;
        for (var i = 0; i < d.entryList.length; i++) {
            var entry = d.entryList[i];
            if (i == 0 || (entry.type == 0 && entry.starttime < d.sleepTime)) {
                d.sleepTime = entry.starttime;
            }
            if (i == 0 || (entry.type == 0 && entry.starttime + entry.duration > d.wakeupTime)) {
                d.wakeupTime = entry.starttime + entry.duration;
            }
            if (entry.type == 0) {
                d.sleepDuration += entry.duration;
            }
            if (entry.type == 1) {
                print("adding deep sleep", entry.duration)
                d.deepSleepDuration += entry.duration;
            }
        }
        print("total deep sleep", d.deepSleepDuration)

        var sleepDateTime = new Date(d.sleepTime * 1000);
        sleepDateTime.setMinutes(0);
        d.graphStartTime = sleepDateTime.getTime() / 1000;
        var wakeupDateTime = new Date(d.wakeupTime * 1000);
        wakeupDateTime.setMinutes(0);
        wakeupDateTime.setHours(wakeupDateTime.getHours() + 1);
        d.graphEndTime = wakeupDateTime.getTime() / 1000;

        print("gone to bed:", sleepDateTime, d.sleepTime, "woke up", wakeupDateTime)

        var averageSleepTimes = root.pebble.averageSleepTimes(date);
        d.averageSleepTime = averageSleepTimes.fallasleep || -1;
        d.averageWakeupTime = averageSleepTimes.wakeup || -1;

        var tmp = root.date;
        var tmp2 = new Date(d.averageSleepTime * 1000);
        tmp.setHours(tmp2.getHours());
        tmp.setMinutes(0);
        print("have starttime", new Date(d.averageSleepTime * 1000), d.averageSleepTime)
        if (d.averageSleepTime >= 0 && d.graphStartTime > tmp.getTime() / 1000) {
            d.graphStartTime = tmp.getTime() / 1000;
        }

        tmp = date;
        tmp2 = new Date(d.averageWakeupTime * 1000);
        tmp.setHours(tmp2.getHours() + 1);
        tmp.setMinutes(0);
//                    if (tmp.getHours() < date.getHours()) {
//                        tmp.setDate(tmp.getDate() + 1)
//                    }
        print("have endtime", new Date(d.averageWakeupTime * 1000))
        if (d.averageWakeupTime >= 0 && d.graphEndTime < tmp.getTime() / 1000) {
            print("correcting graph end time to", tmp)
            d.graphEndTime = tmp.getTime() / 1000;
        }

        print("avg sleep:", d.averageSleepTime, d.averageWakeupTime)

        infoRowModel.clear();
        infoRowModel.append({ type: "duration", title: "Avg sleep time", duration: averageSleepTimes.sleepTime })
        infoRowModel.append({ type: "duration", title: "Avg deep sleep", duration: averageSleepTimes.deepSleep })

        root.requestPaint()
    }

    onPaint: {
        var ctx = getContext('2d');
        ctx.save();
        ctx.reset()

        ctx.beginPath();
        ctx.fillStyle = Qt.darker(root.baseColor);
        ctx.rect(0, 0, root.width, root.height)
        ctx.fill();
        ctx.closePath();

        ctx.font = "" + units.gu(1.5) + "px Ubuntu";

        d.paintDailySleep(ctx);

        ctx.restore();
    }

    QtObject {
        id: d
        property var entryList: null

        // Set them to init
        property int sleepTime: 0
        property int wakeupTime: 0
        property int sleepDuration: 0
        property int deepSleepDuration: 0
        property int averageSleepTime: 0
        property int averageWakeupTime: 0

        property int graphStartTime: 0
        property int graphEndTime: 0

        // hardcoded settings
        property int barHeight: units.gu(4)

        // auto-calculated
        property int minutes: (graphEndTime - graphStartTime) / 60
        property int slots: (graphEndTime - graphStartTime) / 60
        property real slotsWidth: 1.0 * root.width / slots

        function paintDailySleep(ctx) {
            ctx.strokeStyle = "black";
            ctx.fillStyle = "yellow"
            ctx.lineWidth = units.dp(1);

            if (d.entryList.length == 0) {
                ctx.beginPath();
                var text = i18n.tr("Oops! No sleep data.");
                var textWidth = ctx.measureText(text).width;
                ctx.text(text, (root.width - textWidth) / 2, root.height / 2)
                ctx.fill();
                ctx.closePath();
                return;
            }

            ctx.fillStyle = root.baseColor

            // sleep Time
            paintDurationText(ctx, root.width / 4, root.height * .9, d.sleepDuration, "SLEEP TIME")

            // deep sleep Time
            paintDurationText(ctx, root.width / 4 * 3, root.height * .9, d.deepSleepDuration, "DEEP SLEEP")

            // fall asleep Time
            paintTimeText(ctx, root.width / 4, root.height * .25, new Date(d.sleepTime * 1000), "FALL ASLEEP")
            paintTimeText(ctx, root.width / 4 * 3, root.height * .25, new Date(d.wakeupTime * 1000), "WAKEUP")

            // Background bar
            ctx.strokeStyle = "transparent"
            ctx.beginPath();
            ctx.rect(0, (root.height - d.barHeight) / 2, root.width, d.barHeight);
            ctx.fill();
            ctx.closePath();

            // numbering
            ctx.font = "" + units.gu(1.5) + "px Ubuntu";
            for (var i = 0; i < d.slots; i++) {
                var entryDate = new Date((d.graphStartTime + (i * 60)) * 1000);
                if (entryDate.getMinutes() != 0 || entryDate.getHours() % 2 != 0) continue;
                ctx.beginPath();
                ctx.text(pad(entryDate.getHours(), 2) + ":00", i * d.slotsWidth, root.height / 2 + d.barHeight)
                ctx.fill();
                ctx.closePath();
            }

            // sleep markers
            for (var i = 0; i < d.entryList.length; i++) {
                var entry = d.entryList[i];
                if (entry.type == 0) {
                    ctx.fillStyle = Qt.lighter(root.baseColor)
                } else {
                    var tmpColor = root.baseColor
                    ctx.fillStyle = Qt.rgba(tmpColor.r, tmpColor.g, tmpColor.b, .5);
                }

                ctx.beginPath();
    //                            print("have starttime", entry.starttime, "type", entry.type)
                var startSlot = (entry.starttime - d.graphStartTime) / 60;
                var endSlot = startSlot + entry.duration / 60
                print("startSlot", startSlot, "endslot", endSlot, "total", d.slots)
                var startX = startSlot * d.slotsWidth;
                var startY = (root.height - d.barHeight) / 2;
                var width = (endSlot - startSlot) * d.slotsWidth;
                var height = d.barHeight;
                if (entry.type == 0) {
                    roundRect(ctx, startX, startY, width, height, units.dp(5), ctx.fillStyle, ctx.strokeStyle)
                } else {
                    ctx.rect(startSlot * d.slotsWidth, (root.height - d.barHeight) / 2, (endSlot - startSlot) * d.slotsWidth, d.barHeight)
                }
                ctx.stroke();
                ctx.fill();
                ctx.closePath();
            }

            ctx.beginPath();
            ctx.fillStyle = "yellow"
            var avgSleep = new Date(d.averageSleepTime * 1000);
            var tmp = new Date(d.graphStartTime * 1000)
            tmp.setHours(avgSleep.getHours());
            tmp.setMinutes(avgSleep.getMinutes());
            print("avg:", avgSleep)
            startSlot = (tmp.getTime() / 1000 - d.graphStartTime) / 60
            print("startslot", startSlot)
            ctx.rect(startSlot * d.slotsWidth, (root.height - d.barHeight) / 2, units.gu(.3), d.barHeight);
            ctx.text(pad(avgSleep.getHours(), 2) + ":" + pad(avgSleep.getMinutes(), 2), startSlot * d.slotsWidth, (root.height - d.barHeight) / 2 - units.gu(.5));

            var avgWakeup = new Date(d.averageWakeupTime * 1000);
            tmp = new Date(d.graphStartTime * 1000)
            tmp.setHours(avgWakeup.getHours());
            tmp.setMinutes(avgWakeup.getMinutes());
            print("avg:", avgWakeup)
            if (tmp < new Date(d.graphStartTime * 1000)) {
                tmp.setDate(tmp.getDate() + 1)
            }

            startSlot = (tmp.getTime() / 1000 - d.graphStartTime) / 60
            print("startslot", startSlot)
            ctx.rect(startSlot * d.slotsWidth, (root.height - d.barHeight) / 2, units.gu(.3), d.barHeight);
            var textWidth = ctx.measureText(pad(avgWakeup.getHours(), 2) + ":" + pad(avgWakeup.getMinutes(), 2)).width
            ctx.text(pad(avgWakeup.getHours(), 2) + ":" + pad(avgWakeup.getMinutes(), 2), startSlot * d.slotsWidth - textWidth, (root.height - d.barHeight) / 2 - units.gu(.5));


            ctx.fill();
            ctx.closePath();

        }

    }
}
