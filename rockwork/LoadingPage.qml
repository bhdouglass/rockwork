import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    id: loadingPage
    header: PageHeader {
        title: "RockWork"
    }

    Column {
        width: parent.width - units.gu(4)
        anchors.centerIn: parent
        spacing: units.gu(4)

        Rectangle {
            id: upgradeIcon
            height: units.gu(10)
            width: height
            radius: width / 2
            color: UbuntuColors.blue
            anchors.horizontalCenter: parent.horizontalCenter
            Icon {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                name: "preferences-system-updates-symbolic"
                color: "white"
            }

            RotationAnimation on rotation {
                duration: 2000
                loops: Animation.Infinite
                from: 0
                to: 360
                running: upgradeIcon.visible
            }
        }

        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            fontSize: "large"
            text: i18n.tr("Loading...")
        }
    }

}
