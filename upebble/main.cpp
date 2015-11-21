#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>
#include <QFile>

#include "notificationsourcemodel.h"
#include "servicecontrol.h"

void installServiceFile() {
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<NotificationSourceModel>("upebble", 1, 0, "NotificationSourceModel");
    qmlRegisterType<ServiceControl>("upebble", 1, 0, "ServiceController");

    QQuickView view;
//    view.rootContext()->setContextProperty("pebbleConnection", &wc);
    view.setSource(QUrl(QStringLiteral("qrc:///Main.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
    return app.exec();
}

