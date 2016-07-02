import QtQuick 2.4
import QtQuick.Controls 1.3
import PebbleTest 1.0

Column {
    spacing: 10
    Label {
        text: pebble.name
        width: parent.width
    }

    Button {
        text: "Insert Timeline Pin"
        onClicked: {
           pebble.insertTimelinePin();
        }
    }
    Button {
        text: "Create Reminder"
        onClicked: {
           pebble.insertReminder();
        }
    }
    Button {
        text: "Clear Timeline"
        onClicked: {
           pebble.clearTimeline();
        }
    }
    Button {
        text: "take screenshot"
        onClicked: {
            pebble.requestScreenshot();
        }
    }

    Button {
        text: "dump logs"
        onClicked: {
            pebble.dumpLogs();
        }
    }
    Button {
        text: "print today's steps"
        onClicked: {
            var starttime = new Date()
            starttime.setHours(0, 0, 0, 0)
            var endtime = new Date();
            endtime.setHours(23, 59, 59, 0)
//            starttime.setDate(17)
            print("steps:", pebble.steps(starttime, endtime));
        }
    }
    Button {
        text: "print yesterdays's sleep"
        onClicked: {
            var day = new Date()
            day.setDate(day.getDate() - 1)
            print("sleep data:", pebble.sleepDataForDay(day));
        }
    }

    Button {
        text: "print last weeks's sleep average"
        onClicked: {
            var day1 = new Date()
            day1.setDate(day1.getDate() - 7)
            var day2 = new Date()
            print("sleep data:", pebble.sleepAverage(day1, day2) / 60 / 60);
        }
    }
    Button {
        text: "print last weeks's deep sleep average"
        onClicked: {
            var day1 = new Date()
            day1.setDate(day1.getDate() - 7)
            var day2 = new Date()
            print("deep sleep average:", pebble.deepSleepAverage(day1, day2) / 60 / 60);
        }
    }

}

