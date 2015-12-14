TEMPLATE = app
TARGET = upebble

include(../version.pri)
load(ubuntu-click)

QT += qml quick dbus

CONFIG += c++11

HEADERS += \
    notificationsourcemodel.h \
    servicecontrol.h \
    pebble.h \
    pebbles.h

SOURCES += main.cpp \
    notificationsourcemodel.cpp \
    servicecontrol.cpp \
    pebble.cpp \
    pebbles.cpp

RESOURCES += upebble.qrc

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES +=  upebble.apparmor \
               upebble.svg \
               upebble.desktop

AP_TEST_FILES += tests/autopilot/run \
                 $$files(tests/*.py,true)

#show all the files in QtCreator
OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               $${AP_TEST_FILES} \


#specify where the config files are installed to
config_files.path = /upebble
config_files.files += $${CONF_FILES}
INSTALLS+=config_files

#install the desktop file, a translated version is 
#automatically created in the build directory
desktop_file.path = /upebble
desktop_file.files = $$OUT_PWD/upebble.desktop
desktop_file.CONFIG += no_check_exist
INSTALLS+=desktop_file

# Default rules for deployment.
target.path = $${UBUNTU_CLICK_BINARY_PATH}
INSTALLS+=target

DISTFILES += \
    NotificationsPage.qml \
    PebblesPage.qml
