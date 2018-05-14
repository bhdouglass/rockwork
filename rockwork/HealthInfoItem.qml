import QtQuick 2.4
import Ubuntu.Components 1.3
import QtQuick.Layouts 1.1

UbuntuShape {
    id: root
    property string title: ""

    default property alias data: graphContent.data
    property alias infoModel: infoRow.model

    property string mode: "day" // "week", "month"
    property int offset: 0

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: units.gu(6)

            Item {
                Layout.preferredWidth: units.gu(1)
            }

            Label {
                text: root.title
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "white"
                fontSize: "large"
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: units.gu(1)
                    border.width: units.dp(2)
                    border.color: "white"
                    color: "transparent"
                    radius: units.gu(.5)

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: units.dp(2)
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: root.mode == "day" ? "white" : "transparent"
                            Label {
                                text: "Day"
                                horizontalAlignment: Text.AlignHCenter
                                color: root.mode == "day" ? root.color : "white"
                                anchors.centerIn: parent
                            }
                            AbstractButton {
                                anchors.fill: parent
                                onClicked: root.mode = "day"
                            }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: root.mode == "week" ? "white" : "transparent"
                            Label {
                                text: "Week"
                                anchors.centerIn: parent
                                color: root.mode == "week" ? root.color : "white"
                            }
                            AbstractButton {
                                anchors.fill: parent
                                onClicked: root.mode = "week"
                            }
                        }
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: root.mode == "month" ? "white" : "transparent"
                            Label {
                                text: "Month"
                                Layout.fillWidth: true
                                anchors.centerIn: parent
                                color: root.mode == "month" ? root.color : "white"
                            }
                            AbstractButton {
                                anchors.fill: parent
                                onClicked: root.mode = "month"
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: units.dp(1)
            color: Qt.darker(root.color, 1.2)
        }

        RowLayout {
            Layout.fillWidth: true
//            Layout.margins: units.gu(1)
            Icon {
                Layout.preferredWidth: units.gu(3)
                Layout.preferredHeight: width
                name: "previous"
                color: "white"
                AbstractButton {
                    anchors.fill: parent
                    onClicked: root.offset--
                }
            }
            Label {
                id: dateLabel
                property var date: {
                    switch (root.mode) {
                    case "day":
                        return new Date().setDate(new Date().getDate() + root.offset)
                    case "week":
                        return new Date().setDate(new Date().getDate() + root.offset * 7)
                    case "month":
                        return new Date().setMonth(new Date().getMonth() + root.offset)
                    }
                    return new Date();
                }

                text: {
                    switch (root.mode) {
                    case "day":
                        return root.offset == 0 ? "Today" : root.offset == -1 ? "Yesterday" : new Date(dateLabel.date).toDateString()
                    case "week":
                        return root.offset == 0 ? "This week" : root.offset == -1 ? "Last week" : new Date(dateLabel.date).toDateString();
                    case "month":
                        return root.offset == 0 ? "This month" : root.offset == -1 ? "Last month" : new Date(dateLabel.date).toDateString();
                    }
                    return "";
                }
                color: "white"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            Icon {
                Layout.preferredWidth: units.gu(3)
                Layout.preferredHeight: width
                name: "next"
                color: "white"
                opacity: enabled ? 1 : .5
                enabled: root.offset < 0
                AbstractButton {
                    anchors.fill: parent
                    onClicked: root.offset++
                }
            }
        }

        Rectangle {
            id: graphContent
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: Qt.darker(root.color, 1.2)

        }
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: units.gu(8)

            RowLayout {
                anchors.fill: parent
                Repeater {
                    id: infoRow
                    model: 1
                    delegate: ColumnLayout {
                        Layout.fillWidth: true
                        Label {
                            Layout.fillWidth: true
                            text: model.title
                            horizontalAlignment: Text.AlignHCenter
                            color: Qt.lighter(root.color)
                        }
                        Row {
                            Layout.alignment: Qt.AlignHCenter
                            visible: model.type === "duration"
                            Label {
                                text: Math.floor(model.duration / 60 / 60)
                                color: "white"
                                fontSize: "x-large"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                            }
                            Label {
                                text: "H"
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                            }
                            Label {
                                text: Math.floor(model.duration / 60 % 60)
                                color: "white"
                                fontSize: "x-large"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                            }
                            Label {
                                text: "M"
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                            }
                        }
                        Row {
                            Layout.alignment: Qt.AlignHCenter
                            visible: model.type === "distance"
                            Label {
                                text: model.distance
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                                fontSize: "x-large"
                            }
                            Label {
                                text: i18n.tr("KM")
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                            }
                        }
                        Row {
                            Layout.alignment: Qt.AlignHCenter
                            visible: model.type === "calories"
                            Label {
                                text: model.calories
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                                fontSize: "x-large"
                            }
                        }
                        Row {
                            Layout.alignment: Qt.AlignHCenter
                            visible: model.type === "time"
                            Label {
                                text: model.time
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                                fontSize: "x-large"
                            }
                        }
                        Row {
                            Layout.alignment: Qt.AlignHCenter
                            visible: model.type === "activetime"
                            Label {
                                text: model.activeTime
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                                fontSize: "x-large"
                            }
                            Label {
                                text: i18n.tr("M")
                                color: "white"
                                anchors.baseline: parent.bottom
                                anchors.baselineOffset: units.gu(-1)
                            }
                        }
                    }
                }
            }
        }
    }
}
