import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import upebble 1.0

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    applicationName: "upebble.mzanetti"

    width: units.gu(100)
    height: units.gu(75)

    ListModel {
        id: mainMenuModel
        ListElement { icon: "clock-app-symbolic"; text: "Manage Pebbles"; page: "PebblesPage.qml" }
        ListElement { icon: "messaging-app-symbolic"; text: "Manage Notifications"; page: "NotificationsPage.qml" }
    }

    ServiceController {
        id: serviceController
        serviceName: "upebbled"
        Component.onCompleted: {
            if (!serviceController.serviceFileInstalled) {
                serviceController.installServiceFile();
            }
            if (!serviceController.serviceRunning) {
                serviceController.startService();
            }
        }
    }

    PageStack {
        id: pageStack
        Component.onCompleted: push(mainPage)

        Page {
            id: mainPage
            title: "uPebble"

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
                                source: model.icon
                            }
                            Label {
                                text: model.text
                                Layout.fillWidth: true
                            }
                        }
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl(model.page))
                        }
                    }
                }
            }
        }
    }
}

