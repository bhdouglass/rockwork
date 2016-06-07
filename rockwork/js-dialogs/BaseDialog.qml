import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3 as Popups

Popups.Dialog {
    text: model.message

    // Set the parent at construction time, instead of letting show()
    // set it later on, which for some reason results in the size of
    // the dialog not being updated.
    parent: QuickUtils.rootItem(this)

    Component.onCompleted: show()
}
