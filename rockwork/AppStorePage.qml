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

    property string link: ""

    AppStoreClient {
        id: client
        hardwarePlatform: pebble.hardwarePlatform
        Component.onCompleted: {
            if (root.link) {
                fetchLink(link)
            } else {
                if (showWatchApps) {
                    fetchHome(AppStoreClient.TypeWatchapp)
                } else {
                    fetchHome(AppStoreClient.TypeWatchface)
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        ListView {
            anchors.fill: parent
            model: ApplicationsFilterModel {
                id: appsFilterModel
                model: client.model
            }
            clip: true
            section.property: "groupId"
            section.labelPositioning: ViewSection.CurrentLabelAtStart |
                                      ViewSection.InlineLabels
            section.delegate: ListItem {
                height: section ? label.implicitHeight + units.gu(3) : 0

                Rectangle {
                    anchors.fill: parent
                    color: "white"
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: units.gu(1)
                    Label {
                        id: label
                        text: client.model.groupName(section)
                        fontSize: "large"
//                        font.weight: Font.DemiBold
                        Layout.fillWidth: true
                    }
                    AbstractButton {
                        Layout.fillHeight: true
                        implicitWidth: seeAllLabel.implicitWidth + height
                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            Label {
                                id: seeAllLabel
                                text: i18n.tr("See all")
                            }
                            Icon {
                                implicitHeight: parent.height
                                implicitWidth: height
                                name: "go-next"
                            }
                        }
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("AppStorePage.qml"), {pebble: root.pebble, link: client.model.groupLink(section), title: client.model.groupName(section)});
                        }
                    }
                }
            }

            footer: Item {
                height: client.model.links.length > 0 ? units.gu(6) : 0
                width: parent.width

                RowLayout {
                    anchors {
                        fill: parent
                        margins: units.gu(1)
                    }
                    spacing: units.gu(1)

                    Repeater {
                        model: client.model.links
                        Button {
                            text: client.model.linkName(client.model.links[index])
                            onClicked: client.fetchLink(client.model.links[index]);
                            color: UbuntuColors.orange
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            delegate: ListItem {
                height: delegateColumn.height + units.gu(2)

                RowLayout {
                    id: delegateRow
                    anchors.fill: parent
                    anchors.margins: units.gu(1)
                    spacing: units.gu(1)

                    Image {
                        Layout.fillHeight: true
                        Layout.preferredWidth: height
                        source: model.icon
                    }

                    ColumnLayout {
                        id: delegateColumn
                        Layout.fillWidth: true;
                        Layout.fillHeight: true;
                        Label {
                            Layout.fillWidth: true
                            text: model.name
                            font.weight: Font.DemiBold
                            elide: Text.ElideRight
                        }
                        Label {
                            Layout.fillWidth: true
                            text: model.category
                        }
                        RowLayout {
                            Icon {
                                name: "like"
                                Layout.preferredHeight: parent.height
                                Layout.preferredWidth: height
                                implicitHeight: parent.height
                            }
                            Label {
                                Layout.fillWidth: true
                                text: model.hearts
                            }
                            Icon {
                                id: tickIcon
                                name: "tick"
                                implicitHeight: parent.height
                                Layout.preferredWidth: height
                                visible: root.pebble.installedApps.contains(model.storeId) || root.pebble.installedWatchfaces.contains(model.storeId)
                                Connections {
                                    target: root.pebble.installedApps
                                    onChanged: {
                                        tickIcon.visible = root.pebble.installedApps.contains(model.storeId) || root.pebble.installedWatchfaces.contains(model.storeId)
                                    }
                                }

                                Connections {
                                    target: root.pebble.installedWatchfaces
                                    onChanged: {
                                        tickIcon.visible = root.pebble.installedApps.contains(model.storeId) || root.pebble.installedWatchfaces.contains(model.storeId)
                                    }
                                }

                            }
                        }
                    }

                }

                onClicked: {
                    client.fetchAppDetails(model.storeId);
                    pageStack.push(Qt.resolvedUrl("AppStoreDetailsPage.qml"), {app: appsFilterModel.get(index), pebble: root.pebble})
                }
            }
        }

//        RowLayout {
//            id: buttonRow
//            anchors { left: parent.left; bottom: parent.bottom; right: parent.right; margins: units.gu(1) }
//            spacing: units.gu(1)
//            Button {
//                text: i18n.tr("Previous")
//                Layout.fillWidth: true
//                enabled: client.offset > 0
//                onClicked: {
//                    client.previous()
//                }
//            }
//            Button {
//                text: i18n.tr("Next")
//                Layout.fillWidth: true
//                onClicked: {
//                    client.next()
//                }
//            }
//        }
    }

    ActivityIndicator {
        anchors.centerIn: parent
        running: client.busy
    }
}

