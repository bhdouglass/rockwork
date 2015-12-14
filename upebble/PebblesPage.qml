import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

Page {
    title: "Manage Pebbles"

    head {
        actions: [
            Action {
                iconName: "settings"
                onTriggered: {
                    onClicked: Qt.openUrlExternally("settings://system/bluetooth")
                }
            }
        ]
    }

    ListView {
        anchors.fill: parent
        model: pebbles
        delegate: ListItem {
            RowLayout {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Label {
                        text: model.name
                    }
                    Label {
                        text: model.connected ? "Connected" : "Disconnected"
                        fontSize: "small"
                    }
                }

            }
        }
    }

    Column {
        anchors.centerIn: parent
        width: parent.width - units.gu(4)
        spacing: units.gu(4)
        visible: pebbles.count === 0
        Label {
            text: "No Pebble set up yet. Please connect your Pebble watch using the system settings app."
            fontSize: "large"
            width: parent.width
            wrapMode: Text.WordWrap
        }
        Button {
            text: "Open system settings"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: Qt.openUrlExternally("settings://system/bluetooth")
        }
    }

}

