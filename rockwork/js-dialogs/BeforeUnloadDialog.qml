import QtQuick 2.4
import Ubuntu.Components 1.3

BaseDialog {
    title: i18n.tr("App Settings Confirm Navigation")

    Button {
        text: i18n.tr("Leave")
        onClicked: model.accept()
    }

    Button {
        text: i18n.tr("Stay")
        onClicked: model.reject()
    }
}
