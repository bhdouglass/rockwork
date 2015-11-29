import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import upebble 1.0

Page {
    title: "Notifications"

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: units.gu(1)
        Item {
            Layout.fillWidth: true
            implicitHeight: infoLabel.height

            Label {
                id: infoLabel
                anchors { left: parent.left; right: parent.right; margins: units.gu(1) }
                wrapMode: Text.WordWrap
                text: "Entries here will appear as notifications keep coming. Selected notifications will be shown on your Pebble."
            }
        }


        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: NotificationSourceModel {
                id: notificationSourceModel
            }
            delegate: ListItem {
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: units.gu(1)
                    Label {
                        text: model.name
                        Layout.fillWidth: true
                    }
                    Switch {
                        checked: model.enabled
                        onCheckedChanged: {
                            print("new checked", checked)
                            notificationSourceModel.setEnabled(index, checked)
                        }
                    }
                }
            }
        }
    }

}

