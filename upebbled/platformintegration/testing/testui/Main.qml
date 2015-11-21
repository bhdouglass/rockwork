import QtQuick 2.4
import QtQuick.Controls 1.3
import PebbleTest 1.0

Column {
    spacing: 10
    Button {
        text: "Fake Email Notification"
        onClicked: {
           handler.sendNotification(0, "TestFrom", "TestSubject", "TestText")
        }
    }
    Button {
        text: "Fake Sms Notification"
        onClicked: {
           handler.sendNotification(1, "TestFrom", "TestSubject", "TestText")
        }
    }
    Button {
        text: "Fake incoming phone call"
        onClicked: {
            handler.fakeIncomingCall(1, "123456789", "TestCaller")
        }
    }
    Button {
        text: "pick up incoming phone call"
        onClicked: {
            handler.callStarted(1)
        }
    }
    Button {
        text: "hang up incoming phone call"
        onClicked: {
            handler.endCall(1, false)
        }
    }
    Button {
        text: "miss incoming phone call"
        onClicked: {
            handler.endCall(1, true)
        }
    }
}

