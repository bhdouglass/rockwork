import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

Page {
    id: root
    title: pebble.name

    property var pebble: null

    //Creating the menu list this way to allow the text field to be translatable (http://askubuntu.com/a/476331)
    Item {
        ListModel {
            id: mainMenuModel
            dynamicRoles: true
        }

        Component.onCompleted: {
            mainMenuModel.append({
                icon: "stock_notification",
                text: i18n.tr("Manage notifications"),
                page: "NotificationsPage.qml",
            });

            mainMenuModel.append({
                icon: "stock_application",
                text: i18n.tr("Manage Apps"),
                page: "InstalledAppsPage.qml",
                showWatchApps: true,
            });

            mainMenuModel.append({
                icon: "stock_alarm-clock",
                text: i18n.tr("Manage Watchfaces"),
                page: "InstalledAppsPage.qml",
                showWatchFaces: true,
            });
        }
    }

    Column {
        anchors.fill: parent

        Repeater {
            model: mainMenuModel
            delegate: ListItem {
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: units.gu(1)

                    Icon {
                        Layout.fillHeight: true
                        Layout.preferredWidth: height
                        implicitHeight: parent.height
                        implicitWidth: height
                        name: model.icon
                    }

                    Label {
                        text: model.text
                        Layout.fillWidth: true
                    }
                }

                onClicked: {
                    var options = {};
                    options["pebble"] = root.pebble
                    var modelItem = mainMenuModel.get(index)
                    options["showWatchApps"] = modelItem.showWatchApps
                    options["showWatchFaces"] = modelItem.showWatchFaces
                    pageStack.push(Qt.resolvedUrl(model.page), options)
                }
            }
        }
    }

    Connections {
        target: pebble
        onOpenURL: {
            if (url) {
                pageStack.push(Qt.resolvedUrl("AppSettingsPage.qml"), {uuid: uuid, url: url, pebble: pebble})
            }
        }
    }
}
