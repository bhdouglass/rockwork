import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Content 1.3
import RockWork 1.0

Page {
    id: root

    title: i18n.tr("Screenshots")
    head {
        locked: true
        visible: !contentPeerPicker.visible
    }

    property var pebble: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.gu(1)
        spacing: units.gu(1)

        GridView {
            id: grid
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true

            property int columns: 2

            cellWidth: width / columns
            cellHeight: cellWidth

            model: ScreenshotModel {
                id: screenshotModel
            }

            displaced: Transition {
                UbuntuNumberAnimation { properties: "x,y" }
            }

            delegate: Item {
                width: grid.cellWidth
                height: grid.cellHeight
                Image {
                    anchors.fill: parent
                    anchors.margins: units.gu(.5)
                    fillMode: Image.PreserveAspectFit
                    source: "file://" + model.absoluteFilename
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        PopupUtils.open(dialogComponent, root, {filename: model.filename, absoluteFilename: model.absoluteFilename})
                    }
                }
            }
        }

        AbstractButton {
            Layout.preferredHeight: units.gu(5)
            Layout.preferredWidth: height
            Layout.alignment: Qt.AlignHCenter
            Icon {
                anchors.fill: parent
                name: "camera-app-symbolic"
            }
            onClicked: {
                root.pebble.requestScreenshot()
            }
        }

    }

    Component {
        id: dialogComponent
        Dialog {
            id: dialog
            title: i18n.tr("Screenshot options")

            property string filename
            property string absoluteFilename

            Button {
                text: i18n.tr("Share")
                color: UbuntuColors.blue
                onClicked: {
                    pageStack.push(pickerPageComponent, {handler: ContentHandler.Share, absoluteFilename: absoluteFilename })
                    PopupUtils.close(dialog)
                }
            }
            Button {
                text: i18n.tr("Save")
                color: UbuntuColors.green
                onClicked: {
                    pageStack.push(pickerPageComponent, {handler: ContentHandler.Destination, absoluteFilename: absoluteFilename })
                    PopupUtils.close(dialog)
                }
            }

            Button {
                text: i18n.tr("Delete")
                color: UbuntuColors.red
                onClicked: {
                    screenshotModel.deleteFile(filename)
                    PopupUtils.close(dialog)
                }
            }
            Button {
                text: i18n.tr("Cancel")
                onClicked: {
                    PopupUtils.close(dialog)
                }
            }
        }
    }

    Component {
        id: pickerPageComponent

        Page {
            id: pickerPage
            head {
                locked: true
                visible: false
            }

            property alias handler: contentPeerPicker.handler
            property string absoluteFilename

            Component {
                id: exportItemComponent
                ContentItem {
                    name: i18n.tr("Pebble screenshot")
                }
            }
            ContentPeerPicker {
                id: contentPeerPicker
                anchors.fill: parent
                contentType: ContentType.Pictures

                onCancelPressed: pageStack.pop()

                onPeerSelected: {
                    var transfer = peer.request();
                    var items = [];
                    var item = exportItemComponent.createObject();
                    item.url = "file://" + pickerPage.absoluteFilename;
                    items.push(item)
                    transfer.items = items;
                    transfer.state = ContentTransfer.Charged;
                    pageStack.pop();
                }
            }
        }
    }

}

