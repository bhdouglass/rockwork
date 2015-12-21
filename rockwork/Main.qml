import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import RockWork 1.0

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    applicationName: "rockwork.mzanetti"

    width: units.gu(100)
    height: units.gu(75)

    ServiceController {
        id: serviceController
        serviceName: "rockworkd"
        Component.onCompleted: {
            if (!serviceController.serviceFileInstalled) {
                serviceController.installServiceFile();
            }
            if (!serviceController.serviceRunning) {
                serviceController.startService();
            }
            if (pebbles.version !== version) {
                serviceController.restartService();
            }
        }
    }

    Pebbles {
        id: pebbles
    }

    PageStack {
        id: pageStack
        Component.onCompleted: push(Qt.resolvedUrl("PebblesPage.qml"));

    }
}

