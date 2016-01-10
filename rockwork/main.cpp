#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>
#include <QFile>

#include "notificationsourcemodel.h"
#include "servicecontrol.h"
#include "pebbles.h"
#include "pebble.h"
#include "applicationsmodel.h"
#include "applicationsfiltermodel.h"
#include "appstoreclient.h"
#include "screenshotmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<Pebble>("RockWork", 1, 0, "Pebble", "Get them from the model");
    qmlRegisterUncreatableType<ApplicationsModel>("RockWork", 1, 0, "ApplicationsModel", "Get them from a Pebble object");
    qmlRegisterUncreatableType<AppItem>("RockWork", 1, 0, "AppItem", "Get them from an ApplicationsModel");
    qmlRegisterType<ApplicationsFilterModel>("RockWork", 1, 0, "ApplicationsFilterModel");
    qmlRegisterType<Pebbles>("RockWork", 1, 0, "Pebbles");
    qmlRegisterUncreatableType<NotificationSourceModel>("RockWork", 1, 0, "NotificationSourceModel", "Get it from a Pebble object");
    qmlRegisterType<ServiceControl>("RockWork", 1, 0, "ServiceController");
    qmlRegisterType<AppStoreClient>("RockWork", 1, 0, "AppStoreClient");
    qmlRegisterType<ScreenshotModel>("RockWork", 1, 0, "ScreenshotModel");

    QQuickView view;
    view.engine()->rootContext()->setContextProperty("version", QStringLiteral(VERSION));
    view.setSource(QUrl(QStringLiteral("qrc:///Main.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
    return app.exec();
}
