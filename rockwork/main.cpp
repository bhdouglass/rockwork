#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>
#include <QFile>

#include "notificationsourcemodel.h"
#include "servicecontrol.h"
#include "pebbles.h"
#include "pebble.h"

void installServiceFile() {
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<Pebble>("RockWork", 1, 0, "Pebble", "Get them from the model");
    qmlRegisterType<Pebbles>("RockWork", 1, 0, "Pebbles");
    qmlRegisterType<NotificationSourceModel>("RockWork", 1, 0, "NotificationSourceModel");
    qmlRegisterType<ServiceControl>("RockWork", 1, 0, "ServiceController");

    QQuickView view;
    view.engine()->rootContext()->setContextProperty("version", VERSION);
    view.setSource(QUrl(QStringLiteral("qrc:///Main.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
    return app.exec();
}

