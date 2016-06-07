import QtQuick 2.4
import Ubuntu.Components 1.3

BaseDialog {
    title: i18n.tr("App Settings Prompt")

    TextField {
        id: input
        text: model.defaultValue
        onAccepted: model.accept(input.text)
    }

    Button {
        text: i18n.tr("OK")
        color: "green"
        onClicked: model.accept(input.text)
    }

    Button {
        text: i18n.tr("Cancel")
        color: UbuntuColors.coolGrey
        onClicked: model.reject()
    }

    Binding {
        target: model
        property: "currentValue"
        value: input.text
    }
}
