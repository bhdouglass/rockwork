QT += core bluetooth dbus network contacts qml location organizer websockets sql
QT -= gui

include(../version.pri)
load(ubuntu-click)

TARGET = rockworkd
CONFIG += c++11
#CONFIG -= app_bundle

TEMPLATE = app

#TODO: figure why pkgconfig doesn't work in the click chroot
#CONFIG += link_pkgconfig
#PKGCONFIG += url-dispatcher-1
INCLUDEPATH += /usr/lib/arm-linux-gnueabihf/glib-2.0/include /usr/lib/x86_64-linux-gnu/glib-2.0/include/ /usr/include/glib-2.0/
LIBS += -lurl-dispatcher

INCLUDEPATH += /usr/include/telepathy-qt5/ /usr/include/qmenumodel/
#LIBS += -lquazip5 -ltelepathy-qt5 -lqmenumodel
LIBS += -lquazip-qt5 -ltelepathy-qt5 -lqmenumodel

SOURCES += main.cpp \
    libpebble/watchconnection.cpp \
    libpebble/pebble.cpp \
    libpebble/watchdatareader.cpp \
    libpebble/watchdatawriter.cpp \
    libpebble/notificationendpoint.cpp \
    libpebble/musicendpoint.cpp \
    libpebble/phonecallendpoint.cpp \
    libpebble/musicmetadata.cpp \
    libpebble/jskit/jskitmanager.cpp \
    libpebble/jskit/jskitconsole.cpp \
    libpebble/jskit/jskitgeolocation.cpp \
    libpebble/jskit/jskitlocalstorage.cpp \
    libpebble/jskit/jskitpebble.cpp \
    libpebble/jskit/jskitxmlhttprequest.cpp \
    libpebble/jskit/jskittimer.cpp \
    libpebble/jskit/jskitperformance.cpp \
    libpebble/jskit/jskitwebsocket.cpp \
    libpebble/appinfo.cpp \
    libpebble/appmanager.cpp \
    libpebble/appmsgmanager.cpp \
    libpebble/uploadmanager.cpp \
    libpebble/bluez/bluezclient.cpp \
    libpebble/bluez/bluez_agentmanager1.cpp \
    libpebble/bluez/bluez_adapter1.cpp \
    libpebble/bluez/bluez_device1.cpp \
    libpebble/bluez/freedesktop_objectmanager.cpp \
    libpebble/bluez/freedesktop_properties.cpp \
    core.cpp \
    pebblemanager.cpp \
    dbusinterface.cpp \
# Platform integration part
    platformintegration/ubuntu/ubuntuplatform.cpp \
    platformintegration/ubuntu/callchannelobserver.cpp \
    libpebble/blobdb.cpp \
    libpebble/timelineitem.cpp \
    libpebble/notification.cpp \
    platformintegration/ubuntu/organizeradapter.cpp \
    libpebble/calendarevent.cpp \
    platformintegration/ubuntu/syncmonitorclient.cpp \
    libpebble/appmetadata.cpp \
    libpebble/appdownloader.cpp \
    libpebble/screenshotendpoint.cpp \
    libpebble/firmwaredownloader.cpp \
    libpebble/bundle.cpp \
    libpebble/watchlogendpoint.cpp \
    libpebble/ziphelper.cpp \
    libpebble/healthparams.cpp \
    libpebble/dataloggingendpoint.cpp \
    libpebble/healthdata.cpp

HEADERS += \
    libpebble/watchconnection.h \
    libpebble/pebble.h \
    libpebble/watchdatareader.h \
    libpebble/watchdatawriter.h \
    libpebble/notificationendpoint.h \
    libpebble/musicendpoint.h \
    libpebble/musicmetadata.h \
    libpebble/phonecallendpoint.h \
    libpebble/platforminterface.h \
    libpebble/jskit/jskitmanager.h \
    libpebble/jskit/jskitconsole.h \
    libpebble/jskit/jskitgeolocation.h \
    libpebble/jskit/jskitlocalstorage.h \
    libpebble/jskit/jskitpebble.h \
    libpebble/jskit/jskitxmlhttprequest.h \
    libpebble/jskit/jskittimer.h \
    libpebble/jskit/jskitperformance.h \
    libpebble/jskit/jskitwebsocket.h \
    libpebble/appinfo.h \
    libpebble/appmanager.h \
    libpebble/appmsgmanager.h \
    libpebble/uploadmanager.h \
    libpebble/bluez/bluezclient.h \
    libpebble/bluez/bluez_agentmanager1.h \
    libpebble/bluez/bluez_adapter1.h \
    libpebble/bluez/bluez_device1.h \
    libpebble/bluez/freedesktop_objectmanager.h \
    libpebble/bluez/freedesktop_properties.h \
    core.h \
    pebblemanager.h \
    dbusinterface.h \
# Platform integration part
    platformintegration/ubuntu/ubuntuplatform.h \
    platformintegration/ubuntu/callchannelobserver.h \
    libpebble/blobdb.h \
    libpebble/timelineitem.h \
    libpebble/notification.h \
    platformintegration/ubuntu/organizeradapter.h \
    libpebble/calendarevent.h \
    platformintegration/ubuntu/syncmonitorclient.h \
    libpebble/appmetadata.h \
    libpebble/appdownloader.h \
    libpebble/enums.h \
    libpebble/screenshotendpoint.h \
    libpebble/firmwaredownloader.h \
    libpebble/bundle.h \
    libpebble/watchlogendpoint.h \
    libpebble/ziphelper.h \
    libpebble/healthparams.h \
    libpebble/dataloggingendpoint.h \
    libpebble/healthdata.h

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

QMAKE_POST_LINK = sed -i s/@VERSION@/$$VERSION/g $$OUT_PWD/../manifest.json || exit 0
#QMAKE_POST_LINK = echo $$OUT_PWD/../manifest.json > /tmp/huhu;

RESOURCES += \
    libpebble/jskit/jsfiles.qrc
