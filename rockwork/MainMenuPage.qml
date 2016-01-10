import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

Page {
    id: root
    title: pebble.name

    property var pebble: null

    //Creating the menu list this way to allow the text field to be translatable (http://askubuntu.com/a/476331)
    ListModel {
        id: mainMenuModel
        dynamicRoles: true
    }

    Component.onCompleted: {
        mainMenuModel.append({
            icon: "stock_notification",
            text: i18n.tr("Manage notifications"),
            page: "NotificationsPage.qml",
            color: "blue"
        });

        mainMenuModel.append({
            icon: "stock_application",
            text: i18n.tr("Manage Apps"),
            page: "InstalledAppsPage.qml",
            showWatchApps: true,
            color: UbuntuColors.green
        });

        mainMenuModel.append({
            icon: "clock-app-symbolic",
            text: i18n.tr("Manage Watchfaces"),
            page: "InstalledAppsPage.qml",
            showWatchFaces: true,
            color: "black"
        });

        mainMenuModel.append({
            icon: "camera-app-symbolic",
            text: i18n.tr("Watch screenshots"),
            page: "ScreenshotsPage.qml",
            showWatchFaces: true,
            color: "gold"
        });
    }

    GridLayout {
        anchors.fill: parent
        columns: parent.width > parent.head ? 2 : 1

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Image {
                Layout.preferredWidth: implicitWidth
                Layout.fillHeight: true
                source: "snowywhite.png"
                fillMode: Image.PreserveAspectFit

            }
            ColumnLayout {
                Layout.fillWidth: true
                Label {
                    text: root.pebble.name
                }
                Label {
                    text: root.pebble.connected ? i18n.tr("Connected") : i18n.tr("Disconnected")
                }
            }
        }


        Column {
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height
            Repeater {
                model: mainMenuModel
                delegate: ListItem {

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: units.gu(1)

                        UbuntuShape {
                            Layout.fillHeight: true
                            Layout.preferredWidth: height
                            backgroundColor: model.color
                            Icon {
                                anchors.fill: parent
                                anchors.margins: units.gu(.5)
                                name: model.icon
                                color: "white"
                            }
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
