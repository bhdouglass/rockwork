import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Content 1.3

Dialog {
    id: sendLogsDialog
    title: i18n.tr("Report problem")

    property var pebble: null

    ActivityIndicator {
        id: busyIndicator
        visible: false
        running: visible
    }
    Label {
        text: i18n.tr("Preparing logs package...")
        visible: busyIndicator.visible
        horizontalAlignment: Text.AlignHCenter
        fontSize: "large"
    }

    Connections {
        target: sendLogsDialog.pebble
        onLogsDumped: {
            if (success) {
                var filename = "/tmp/pebble.log"
                pageStack.addPageToCurrentColumn(root, Qt.resolvedUrl("ContentPeerPickerPage.qml"), {itemName: i18n.tr("pebble.log"),handler: ContentHandler.Share, contentType: ContentType.All, filename: filename })
            }
            PopupUtils.close(sendLogsDialog)
        }
    }

    Button {
        text: i18n.tr("Send rockworkd.log")
        color: UbuntuColors.blue
        visible: !busyIndicator.visible
        onClicked: {
            var filename = homePath + "/.cache/upstart/rockworkd.log"
            pageStack.addPageToCurrentColumn(root, Qt.resolvedUrl("ContentPeerPickerPage.qml"), {itemName: i18n.tr("rockworkd.log"),handler: ContentHandler.Share, contentType: ContentType.All, filename: filename })
            PopupUtils.close(sendLogsDialog)
        }
    }
    Button {
        text: i18n.tr("Send watch logs")
        color: UbuntuColors.blue
        visible: !busyIndicator.visible && sendLogsDialog.pebble.connected
        onClicked: {
            busyIndicator.visible = true
            sendLogsDialog.pebble.dumpLogs("/tmp/pebble.log")
        }
    }
    Button {
        text: i18n.tr("Cancel")
        color: UbuntuColors.red
        visible: !busyIndicator.visible
        onClicked: {
            PopupUtils.close(sendLogsDialog)
        }
    }
}
