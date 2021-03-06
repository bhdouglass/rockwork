import QtQuick 2.4
import Ubuntu.Web 0.2
import Ubuntu.Components 1.3
import com.canonical.Oxide 1.0 as Oxide
import "js-dialogs" as Dialogs

Page {
    id: settings

    property string uuid;
    property string url;
    property var pebble;

    title: i18n.tr("App Settings")

    WebContext {
        id: webcontext
        userAgent: "Mozilla/5.0 (Linux; Android 5.0; Nexus 5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.102 Mobile Safari/537.36 Ubuntu Touch (RockWork)"
        allowedExtraUrlSchemes: ["pebblejs"]
    }

    WebView {
        id: webview
        anchors {
            fill: parent
            bottom: parent.bottom
        }
        width: parent.width
        height: parent.height

        context: webcontext
        url: settings.url
        preferences.localStorageEnabled: true
        preferences.appCacheEnabled: true
        preferences.javascriptCanAccessClipboard: true

        alertDialog: Dialogs.AlertDialog {}
        confirmDialog: Dialogs.ConfirmDialog {}
        promptDialog: Dialogs.PromptDialog {}
        beforeUnloadDialog: Dialogs.BeforeUnloadDialog {}

        function navigationRequestedDelegate(request) {
            var url = request.url.toString();
            if (url.substring(0, 16) == 'pebblejs://close') {
                pebble.configurationClosed(settings.uuid, url);
                request.action = Oxide.NavigationRequest.ActionReject;
                pageStack.pop();
            }
        }

        contextualActions: ActionList {
            Action {
                text: i18n.tr("Cut")
                enabled: webview.contextModel && webview.contextModel.isEditable &&
                     (webview.contextModel.editFlags & Oxide.WebView.CutCapability)
                onTriggered: webview.executeEditingCommand(Oxide.WebView.EditingCommandCut)
            }
            Action {
                text: i18n.tr("Copy")
                enabled: webview.contextModel && webview.contextModel.isEditable &&
                     (webview.contextModel.editFlags & Oxide.WebView.CopyCapability)
                onTriggered: webview.executeEditingCommand(Oxide.WebView.EditingCommandCopy)
            }
            Action {
                text: i18n.tr("Paste")
                enabled: webview.contextModel && webview.contextModel.isEditable &&
                     (webview.contextModel.editFlags & Oxide.WebView.PasteCapability)
                onTriggered: webview.executeEditingCommand(Oxide.WebView.EditingCommandPaste)
            }
        }

        Component.onCompleted: {
            preferences.localStorageEnabled = true;
        }
    }

    ProgressBar {
        height: units.dp(3)
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        showProgressPercentage: false
        value: (webview.loadProgress / 100)
        visible: (webview.loading && !webview.lastLoadStopped)
    }

    /*Connections {
        target: UriHandler
        onOpened: {
            if (uris && uris[0] && uris[0].length) {
                var url = uris[0];

                if (url.substring(0, 16) == 'pebblejs://close') {
                    pebble.configurationClosed(settings.uuid, url);
                    pageStack.pop();
                }
            }
        }
    }*/
}
