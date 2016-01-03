import QtQuick 2.4
import Ubuntu.Components 1.3
import QtQuick.Layouts 1.1
import RockWork 1.0

Page {
    id: root
    title: showWatchApps ? i18n.tr("Add new watchapp") : i18n.tr("Add new watchface")

    property var pebble: null
    property bool showWatchApps: false
    property bool showWatchFaces: false

    AppStoreClient {
        id: client
        Component.onCompleted: {
            refresh()
        }

        property int offset: 0
        property int limit: 25

        function refresh() {
            if (showWatchApps) {
                client.fetch(AppStoreClient.TypeWatchapp, root.pebble.hardwarePlatform, limit, offset)
            } else {
                client.fetch(AppStoreClient.TypeWatchface, root.pebble.hardwarePlatform, limit, offset)
            }
        }

        function next() {
            offset += limit;
            refresh();
        }

        function previous() {
            offset = Math.max(0, offset - limit);
            refresh();
        }
    }

    ListView {
        anchors.fill: parent
        anchors.bottomMargin: buttonRow.height + units.gu(1)
        model: client.model
        clip: true
        delegate: ListItem {
            RowLayout {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                spacing: units.gu(1)

                Image {
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    source: model.icon
                }

                Label {
                    Layout.fillWidth: true
                    text: model.name
                }
            }

            onClicked: {
                pebble.installApp(model.id)
            }
        }
    }

    RowLayout {
        id: buttonRow
        anchors { left: parent.left; bottom: parent.bottom; right: parent.right; margins: units.gu(1) }
        spacing: units.gu(1)
        Button {
            text: i18n.tr("Previous")
            Layout.fillWidth: true
            enabled: client.offset > 0
            onClicked: {
                client.previous()
            }
        }
        Button {
            text: i18n.tr("Next")
            Layout.fillWidth: true
            onClicked: {
                client.next()
            }
        }
    }
}

