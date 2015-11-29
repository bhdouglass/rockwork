QT += core bluetooth dbus network contacts qml location
QT -= gui

load(ubuntu-click)

TARGET = upebbled
CONFIG += c++11
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include/telepathy-qt5/ /usr/include/qmenumodel/
LIBS += -lquazip-qt5 -ltelepathy-qt5 -lqmenumodel

SOURCES += main.cpp \
    libpebble/watchconnection.cpp \
    libpebble/pebble.cpp \
    libpebble/appinstaller.cpp \
    libpebble/watchdatareader.cpp \
    libpebble/watchdatawriter.cpp \
    libpebble/notificationendpoint.cpp \
    libpebble/musicendpoint.cpp \
    libpebble/phonecallendpoint.cpp \
    libpebble/musicmetadata.cpp \
    libpebble/jskitmanager.cpp \
    libpebble/jskitobjects.cpp \
    libpebble/appinfo.cpp \
    libpebble/appmanager.cpp \
    libpebble/appmsgmanager.cpp \
    libpebble/uploadmanager.cpp \
    libpebble/bundle.cpp \
    libpebble/bankmanager.cpp \
    libpebble/bluez/bluezclient.cpp \
    libpebble/bluez/bluez_agentmanager1.cpp \
    libpebble/bluez/bluez_adapter1.cpp \
    libpebble/bluez/bluez_device1.cpp \
    libpebble/bluez/freedesktop_objectmanager.cpp \
    libpebble/bluez/freedesktop_properties.cpp \
    libpebble/bluez/device.cpp \
    core.cpp \
    notificationmanager.cpp \
# Platform integration part
    platformintegration/ubuntu/ubuntuplatform.cpp \
    platformintegration/ubuntu/callchannelobserver.cpp \
    pebblemanager.cpp \
    dbusinterface.cpp

HEADERS += \
    libpebble/watchconnection.h \
    libpebble/pebble.h \
    libpebble/appinstaller.h \
    libpebble/watchdatareader.h \
    libpebble/watchdatawriter.h \
    libpebble/notificationendpoint.h \
    libpebble/musicendpoint.h \
    libpebble/musicmetadata.h \
    libpebble/phonecallendpoint.h \
    libpebble/platforminterface.h \
    libpebble/jskitmanager.h \
    libpebble/jskitobjects.h \
    libpebble/appinfo.h \
    libpebble/appmanager.h \
    libpebble/appmsgmanager.h \
    libpebble/uploadmanager.h \
    libpebble/bundle.h \
    libpebble/bankmanager.h \
    libpebble/bluez/bluezclient.h \
    libpebble/bluez/bluez_agentmanager1.h \
    libpebble/bluez/bluez_adapter1.h \
    libpebble/bluez/bluez_device1.h \
    libpebble/bluez/freedesktop_objectmanager.h \
    libpebble/bluez/freedesktop_properties.h \
    libpebble/bluez/device.h \
    core.h \
    notificationmanager.h \
# Platform integration part
    platformintegration/ubuntu/ubuntuplatform.h \
    platformintegration/ubuntu/callchannelobserver.h \
    pebblemanager.h \
    dbusinterface.h

testing: {
    SOURCES += platformintegration/testing/testingplatform.cpp
    HEADERS += platformintegration/testing/testingplatform.h
    RESOURCES += platformintegration/testing/testui.qrc
    DEFINES += ENABLE_TESTING
    QT += qml quick
}

libs.files = /usr/lib/arm-linux-gnueabihf/libQt5Bluetooth.so.5.4.1 \
             /usr/lib/arm-linux-gnueabihf/libQt5Bluetooth.so.5 \
             /usr/lib/arm-linux-gnueabihf/libquazip-qt5.so.1.0.0 \
             /usr/lib/arm-linux-gnueabihf/libquazip-qt5.so.1
libs.path = $${UBUNTU_CLICK_BINARY_PATH}/..
INSTALLS += libs


# Default rules for deployment.
target.path = $${UBUNTU_CLICK_BINARY_PATH}
INSTALLS+=target

