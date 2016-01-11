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
            text: i18n.tr("Screenshots"),
            page: "ScreenshotsPage.qml",
            showWatchFaces: true,
            color: "gold"
        });
    }

    GridLayout {
        anchors.fill: parent
        columns: parent.width > parent.hight ? 2 : 1

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: units.gu(30)

            RowLayout {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                spacing: units.gu(1)

                Image {
                    Layout.preferredWidth: implicitWidth * height / implicitHeight
                    Layout.fillHeight: true

                    ListModel {
                        id: modelModel
                        ListElement { image: 'artwork/tintin-black.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-white.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-red.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-orange.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-grey.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/bianca-silver.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/bianca-black.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-blue.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-green.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/tintin-pink.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/snowy-white.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/snowy-black.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/snowy-red.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/bobby-silver.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/bobby-black.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/bobby-gold.png'; shape: "rectangle" }
                        ListElement { image: 'artwork/spalding-14mm-silver.png'; shape: "round" }
                        ListElement { image: 'artwork/spalding-14mm-black.png'; shape: "round" }
                        ListElement { image: 'artwork/spalding-20mm-silver.png'; shape: "round" }
                        ListElement { image: 'artwork/spalding-20mm-black.png'; shape: "round" }
                        ListElement { image: 'artwork/spalding-14mm-rose-gold.png'; shape: "round" }


                    }

                    source:  modelModel.get(root.pebble.model - 1).image
                    fillMode: Image.PreserveAspectFit

                    Item {
                        id: watchFace
                        height: parent.height * (modelModel.get(root.pebble.model - 1).shape === "rectangle" ? .5 : .515)
                        width: height * (modelModel.get(root.pebble.model - 1).shape === "rectangle" ? .85 : 1)
                        anchors.centerIn: parent
                        anchors.horizontalCenterOffset: units.dp(1)
                        anchors.verticalCenterOffset: units.dp(modelModel.get(root.pebble.model - 1).shape === "rectangle" ? 0 : 1)

                        Image {
                            id: image
                            anchors.fill: parent
                            source: "file://" + root.pebble.screenshots.latestScreenshot
                            visible: false
                        }

                        Component.onCompleted: {
                            if (!root.pebble.screenshots.latestScreenshot) {
                                root.pebble.requestScreenshot();
                            }
                        }

                        Rectangle {
                            id: textItem
                            anchors.fill: parent
                            layer.enabled: true
                            radius: modelModel.get(root.pebble.model - 1).shape === "rectangle" ? units.gu(.5) : height / 2
                            // This item should be used as the 'mask'
                            layer.samplerName: "maskSource"
                            layer.effect: ShaderEffect {
                                property var colorSource: image;
                                fragmentShader: "
                                    uniform lowp sampler2D colorSource;
                                    uniform lowp sampler2D maskSource;
                                    uniform lowp float qt_Opacity;
                                    varying highp vec2 qt_TexCoord0;
                                    void main() {
                                        gl_FragColor =
                                            texture2D(colorSource, qt_TexCoord0)
                                            * texture2D(maskSource, qt_TexCoord0).a
                                            * qt_Opacity;
                                    }
                                "
                            }
                        }
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: units.gu(2)
                    Rectangle {
                        height: units.gu(10)
                        width: height
                        radius: height / 2
                        color: root.pebble.connected ? UbuntuColors.green : UbuntuColors.red
                        Layout.alignment: Qt.AlignHCenter

                        Icon {
                            anchors.fill: parent
                            anchors.margins: units.gu(2)
                            color: "white"
                            name: root.pebble.connected ? "tick" : "dialog-error-symbolic"
                        }
                    }

                    Label {
                        text: root.pebble.connected ? i18n.tr("Connected") : i18n.tr("Disconnected")
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                    }
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
