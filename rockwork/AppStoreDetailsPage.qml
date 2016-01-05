import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3

Page {
    id: root
    title: i18n.tr("App details")

    property var pebble: null
    property var app: null

    ColumnLayout {
        anchors.fill: parent
        spacing: units.gu(1)

        Item {
            Layout.fillWidth: true
            height: headerColumn.height + units.gu(1)

            RowLayout {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                spacing: units.gu(1)
                height: headerColumn.height

                UbuntuShape {
                    id: iconShape
                    Layout.fillHeight: true
                    Layout.preferredWidth: height

                    source: Image {
                        height: iconShape.height
                        width: iconShape.width
                        source: root.app.icon
                    }
                }

                ColumnLayout {
                    id: headerColumn
                    Layout.fillWidth: true
                    Label {
                        text: root.app.name
                        fontSize: "large"
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Label {
                        text: root.app.vendor
                        Layout.fillWidth: true
                    }
                }

                Button {
                    text: enabled ? i18n.tr("Install") : i18n.tr("Installed")
                    color: UbuntuColors.green
                    enabled: !root.pebble.installedApps.contains(root.app.storeId) && !root.pebble.installedWatchfaces.contains(root.app.storeId)
                    onClicked: {
                        root.pebble.installApp(root.app.storeId)
                    }
                }
            }
        }

        Flickable {
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentHeight: contentColumn.height
            bottomMargin: units.gu(1)
            clip: true

            Column {
                id: contentColumn
                width: parent.width
                height: childrenRect.height

                Image {
                    width: parent.width
                    // ss.w : ss.h = w : h
                    height: sourceSize.height * width / sourceSize.width
                    fillMode: Image.PreserveAspectFit
                    source: root.app.headerImage
                }

                RowLayout {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: units.gu(6)

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Row {
                            anchors.centerIn: parent
                            spacing: units.gu(1)
                            Icon {
                                name: "like"
                                height: parent.height
                                width: height
                            }
                            Label {
                                text: root.app.hearts
                            }
                        }
                    }

                    Rectangle {
                        Layout.preferredHeight: parent.height - units.gu(2)
                        Layout.preferredWidth: units.dp(1)
                        color: UbuntuColors.lightGrey
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Row {
                            anchors.centerIn: parent
                            spacing: units.gu(1)
                            Icon {
                                name: root.app.isWatchFace ? "clock-app-symbolic" : "stock_application"
                                height: parent.height
                                width: height
                            }
                            Label {
                                text: root.app.isWatchFace ? "Watchface" : "Watchapp"
                            }
                        }
                    }
                }

                ColumnLayout {
                    anchors { left: parent.left; right: parent.right; margins: units.gu(1) }
                    spacing: units.gu(1)

                    ListView {
                        Layout.preferredHeight: units.gu(10)
                        Layout.fillWidth: true
                        orientation: ListView.Horizontal
                        spacing: units.gu(1)

                        model: root.app.screenshotImages
                        delegate: Image {
                            height: units.gu(10)
                            width: height
                            source: modelData
                        }
                    }

                    Label {
                        Layout.fillWidth: true
                        font.bold: true
                        text: i18n.tr("Description")
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: units.dp(1)
                        color: UbuntuColors.lightGrey
                    }

                    Label {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        wrapMode: Text.WordWrap
                        text: root.app.description
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columns: 2
                        columnSpacing: units.gu(1)
                        rowSpacing: units.gu(1)
                        Label {
                            text: i18n.tr("Developer")
                            font.bold: true
                        }
                        Label {
                            text: root.app.vendor
                            Layout.fillWidth: true
                        }
                        Label {
                            text: i18n.tr("Version")
                            font.bold: true
                        }
                        Label {
                            text: root.app.version
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
