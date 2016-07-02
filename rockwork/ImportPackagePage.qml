import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Content 1.3

Page {
    id: root
    header: PageHeader {
        title: i18n.tr("Import watchapp or watchface")
    }

    property var pebble: null

    ContentPeerPicker {
        anchors.fill: parent
        anchors.topMargin: root.header.height
        handler: ContentHandler.Source
        contentType: ContentType.All
        showTitle: false

        onPeerSelected: {
            var transfer = peer.request();

            transfer.stateChanged.connect(function() {
                if (transfer.state == ContentTransfer.Charged) {
                    for (var i = 0; i < transfer.items.length; i++) {
                        print("sideloading package", transfer.items[i].url)
                        root.pebble.sideloadApp(transfer.items[i].url)
                    }
                    pageStack.pop();
                }
            })
        }
    }
}

