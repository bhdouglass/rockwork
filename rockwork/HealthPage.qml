import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    id: root
    header: PageHeader {
        title: i18n.tr("Health info")
    }

    property var pebble: null

    Flickable {
        anchors.fill: parent
        anchors.topMargin: root.header.height
        contentHeight: mainColumn.childrenRect.height + units.gu(4)

        Column {
            id: mainColumn
            anchors { left: parent.left; top: parent.top; right: parent.right; margins: units.gu(2) }
            spacing: units.gu(2)

            HealthInfoItem {
                id: stepsItem
                width: parent.width
                height: units.gu(50)
                title: "Activity"
                color: UbuntuColors.green
                infoModel: stepsGraphLoader.item.infoRowModel

                Loader {
                    id: stepsGraphLoader
                    anchors.fill: parent
                    sourceComponent: {
                        switch (stepsItem.mode) {
                        case "day":
                            return dailyStepsGraph;
                        case "week":
                            return weeklyStepsGraph;
                        case "month":
                            return monthlyStepsGraph;
                        }
                        return "";
                    }
                }

                Binding {
                    target: stepsGraphLoader.item
                    property: "date"
                    value: {
                        var date = new Date();
                        switch(stepsItem.mode) {
                        case "day":
                            date.setDate(date.getDate() + stepsItem.offset);
                            break;
                        case "week":
                            date.setDate(date.getDate() + stepsItem.offset * 7)
                            break;
                        case "month":
                            date.setDate(1)
                            print("month is", date.getMonth());
                            date.setMonth(date.getMonth() + stepsItem.offset)
                            print("month is now", date.getMonth());
                        }
                        print("binding date", date.toString(), stepsItem.offset)
                        return date;
                    }
                }

                Component {
                    id: dailyStepsGraph
                    DailyStepsGraph {
                        pebble: root.pebble
                        baseColor: stepsItem.color
                    }
                }
                Component {
                    id: weeklyStepsGraph
                    WeeklyStepsGraph {
                        pebble: root.pebble
                        baseColor: stepsItem.color
                    }
                }
                Component {
                    id: monthlyStepsGraph
                    MonthlyStepsGraph {
                        pebble: root.pebble
                        baseColor: stepsItem.color
                    }
                }
            }
            HealthInfoItem {
                id: sleepItem
                width: parent.width
                height: units.gu(50)
                color: UbuntuColors.blue
                title: "Sleep"
                infoModel: sleepGraphLoader.item.infoRowModel

                Loader {
                    id: sleepGraphLoader
                    anchors.fill: parent
                    sourceComponent: {
                        switch (sleepItem.mode) {
                        case "day":
                            return dailySleepGraph;
                        case "week":
                            return weeklySleepGraph;
                        case "month":
                            return monthlySleepGraph;
                        }
                    }
                }
                Binding {
                    target: sleepGraphLoader.item
                    property: "date"
                    value: {
                        var date = new Date();
                        switch (sleepItem.mode) {
                        case "day":
                            date.setDate(date.getDate() + sleepItem.offset);
                            break;
                        case "week":
                            date.setDate(date.getDate() + sleepItem.offset * 7)
                            break;

                        }

                        return date;
                    }
                }

                Component {
                    id: dailySleepGraph
                    DailySleepGraph {
                        pebble: root.pebble
                        baseColor: sleepItem.color
                    }
                }
                Component {
                    id: weeklySleepGraph
                    WeeklySleepGraph {
                        pebble: root.pebble
                        baseColor: sleepItem.color
                    }
                }
            }
        }
    }
}

