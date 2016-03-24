import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Page {
    id: root
    title: i18n.tr("Developer Tools")

    property var pebble: null

    //Creating the menu list this way to allow the text field to be translatable (http://askubuntu.com/a/476331)
    ListModel {
        id: devMenuModel
        dynamicRoles: true
    }

    Component.onCompleted: {
        populateDevMenu();
    }

    function populateDevMenu() {
        devMenuModel.clear();

        devMenuModel.append({
            icon: "camera-app-symbolic",
            text: i18n.tr("Screenshots"),
            page: "ScreenshotsPage.qml",
            dialog: "",
            color: "gold"
        });
        devMenuModel.append({
            icon: "stock_application",
            text: i18n.tr("Install app or watchface from file"),
            page: "ImportPackagePage.qml",
            dialog: null,
            color: UbuntuColors.blue
        });

    }

    ColumnLayout {
        anchors.fill: parent

        Repeater {
            id: menuRepeater
            model: devMenuModel
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
                    if (model.page) {
                        pageStack.push(Qt.resolvedUrl(model.page), {pebble: root.pebble})
                    }
                    if (model.dialog) {
                        PopupUtils.open(model.dialog)
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}

