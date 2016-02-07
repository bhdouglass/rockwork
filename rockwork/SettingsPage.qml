import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3

Page {
    id: root
    title: i18n.tr("Settings")

    property var pebble: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: units.gu(1)
        spacing: units.gu(1)

        Label {
            Layout.fillWidth: true
            text: i18n.tr("Distance Units")
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            CheckBox {
                id: metricalUnitsCheckbox
                checked: !root.pebble.imperialUnits
                onClicked: {
                    checked = true
                    root.pebble.imperialUnits = false;
                    imperialUnitsCheckBox.checked = false;
                }
            }
            Label {
                text: i18n.tr("Metrical")
                Layout.fillWidth: true
            }
            CheckBox {
                id: imperialUnitsCheckBox
                checked: root.pebble.imperialUnits
                onClicked: {
                    checked = true
                    root.pebble.imperialUnits = true;
                    metricalUnitsCheckbox.checked = false;
                }
            }
            Label {
                text: i18n.tr("Imperial")
                Layout.fillWidth: true
            }
        }
        ThinDivider {}

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}

