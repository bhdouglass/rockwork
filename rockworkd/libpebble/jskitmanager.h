#ifndef JSKITMANAGER_H
#define JSKITMANAGER_H

#include <QJSEngine>
#include <QPointer>
#include "appmanager.h"
#include "watchconnection.h"
#include "pebble.h"
#include "appmsgmanager.h"
//#include "settings.h"

class JSKitPebble;
class JSKitConsole;
class JSKitLocalStorage;
class JSKitGeolocation;

class JSKitManager : public QObject
{
    Q_OBJECT

public:
    explicit JSKitManager(Pebble *pebble, WatchConnection *connection, AppManager *apps, AppMsgManager *appmsg, QObject *parent = 0);
    ~JSKitManager();

    QJSEngine * engine();
    bool isJSKitAppRunning() const;

    static QString describeError(QJSValue error);

    void showConfiguration();
    void handleWebviewClosed(const QString &result);
    void setConfigurationId(const QUuid &id);

signals:
    void appNotification(const QUuid &uuid, const QString &title, const QString &body);
    void appOpenUrl(const QUrl &url);

private slots:
    void handleAppStarted(const QUuid &uuid);
    void handleAppStopped(const QUuid &uuid);
    void handleAppMessage(const QUuid &uuid, const QVariantMap &msg);

private:
    bool loadJsFile(const QString &filename);
    void startJsApp();
    void stopJsApp();

private:
    friend class JSKitPebble;

    Pebble *m_pebble;
    WatchConnection *m_connection;
    AppManager *_apps;
    AppMsgManager *_appmsg;
    AppInfo _curApp;
    QJSEngine *_engine;
    QPointer<JSKitPebble> _jspebble;
    QPointer<JSKitConsole> _jsconsole;
    QPointer<JSKitLocalStorage> _jsstorage;
    QPointer<JSKitGeolocation> _jsgeo;
    QUuid m_configurationId;
};

#endif // JSKITMANAGER_H
