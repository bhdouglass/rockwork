import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import upebble 1.0

Page {
    title: "Notifications"

    ListView {
        anchors.fill: parent
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

