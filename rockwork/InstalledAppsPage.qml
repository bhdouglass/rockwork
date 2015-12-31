import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import RockWork 1.0

Page {
    id: root
    title: showWatchApps ? (showWatchFaces ? i18n.tr("Apps & Watchfaces") : i18n.tr("Apps")) : i18n.tr("Watchfaces")

    property var pebble: null
    property bool showWatchApps: false
    property bool showWatchFaces: false

    ApplicationsFilterModel {
        id: filterModel
        model: root.pebble.installedApps
        showWatchApps: root.showWatchApps
        showWatchFaces: root.showWatchFaces
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: filterModel

        delegate: ListItem {
            leadingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "delete"
                        onTriggered: {
                            pebble.removeApp(model.id)
                        }
                    }
                ]
            }

            trailingActions: ListItemActions {
                actions: [
                    Action {
                        visible: model.hasSettings
                        iconName: "settings"
                        onTriggered: {
                            pebble.requestConfigurationURL(model.id);
                        }
                    }
                ]
            }

            RowLayout {
                anchors {
                    fill: parent
                    margins: units.gu(1)
                }
                spacing: units.gu(1)

                Image {
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    source: "file://" + model.icon
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Label {
                        text: model.name
                        Layout.fillWidth: true
                    }

                    Label {
                        text: model.vendor
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
