import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Content 1.3
import RockWork 1.0

Page {
    id: root

    title: i18n.tr("Screenshots")

    property var pebble: null

    head {
        actions: [
            Action {
                iconName: "camera-app-symbolic"
                onTriggered: root.pebble.requestScreenshot()
            }
        ]
    }

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

            model: root.pebble.screenshots

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
                    source: "file://" + model.filename
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        PopupUtils.open(dialogComponent, root, {filename: model.filename})
                    }
                }
            }
        }
    }

    Component {
        id: dialogComponent
        Dialog {
            id: dialog
            title: i18n.tr("Screenshot options")

            property string filename

            Button {
                text: i18n.tr("Share")
                color: UbuntuColors.blue
                onClicked: {
                    pageStack.push(pickerPageComponent, {handler: ContentHandler.Share, filename: filename })
                    PopupUtils.close(dialog)
                }
            }
            Button {
                text: i18n.tr("Save")
                color: UbuntuColors.green
                onClicked: {
                    pageStack.push(pickerPageComponent, {handler: ContentHandler.Destination, filename: filename })
                    PopupUtils.close(dialog)
                }
            }

            Button {
                text: i18n.tr("Delete")
                color: UbuntuColors.red
                onClicked: {
                    root.pebble.removeScreenshot(filename)
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
            property string filename

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
                    item.url = "file://" + pickerPage.filename;
                    items.push(item)
                    transfer.items = items;
                    transfer.state = ContentTransfer.Charged;
                    pageStack.pop();
                }
            }
        }
    }

}

