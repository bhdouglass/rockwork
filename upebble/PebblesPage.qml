import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    title: "Manage Pebbles"

    Column {
        anchors.centerIn: parent
        width: parent.width - units.gu(4)
        spacing: units.gu(4)
        Label {
            text: "Nothing to see here yet. Please connect your Pebble watch using the system settings app."
            fontSize: "large"
            width: parent.width
            wrapMode: Text.WordWrap
        }
        Button {
            text: "Open system settings"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: Qt.openUrlExternally("settings://bluetooth")
        }
    }

}

