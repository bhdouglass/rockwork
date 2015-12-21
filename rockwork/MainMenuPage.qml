import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

Page {
    id: root
    title: pebble.name

    property var pebble: null

    ListModel {
        id: mainMenuModel
        ListElement { icon: "stock_notification"; text: "Manage notifications"; page: "NotificationsPage.qml" }
        ListElement { icon: "stock_application"; text: "Manage apps"; page: "InstalledAppsPage.qml"; showWatchApps: true }
        ListElement { icon: "stock_alarm-clock"; text: "Manage watchfaces"; page: "InstalledAppsPage.qml"; showWatchFaces: true }
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
                        name:  model.icon
                    }
                    Label {
                        text: model.text
                        Layout.fillWidth: true
                    }
                }
                onClicked: {
                    var options = {};
                    options["pebble"] = root.pebble
                    options["showWatchApps"] = model.showWatchApps
                    options["showWatchFaces"] = model.showWatchFaces
                    pageStack.push(Qt.resolvedUrl(model.page), options)
                }
            }
        }
    }
}

