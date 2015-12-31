#include <QFile>
#include <QDir>

#include "jskitmanager.h"
#include "jskitobjects.h"

JSKitManager::JSKitManager(Pebble *pebble, WatchConnection *connection, AppManager *apps, AppMsgManager *appmsg, QObject *parent) :
    QObject(parent),
    m_pebble(pebble),
    m_connection(connection), _apps(apps), _appmsg(appmsg), _engine(0)
{
    m_configurationId = QUuid();
    connect(_appmsg, &AppMsgManager::appStarted, this, &JSKitManager::handleAppStarted);
    connect(_appmsg, &AppMsgManager::appStopped, this, &JSKitManager::handleAppStopped);
}

JSKitManager::~JSKitManager()
{
    if (_engine) {
        stopJsApp();
    }
}

QJSEngine * JSKitManager::engine()
{
    return _engine;
}

bool JSKitManager::isJSKitAppRunning() const
{
    return _engine != 0;
}

QString JSKitManager::describeError(QJSValue error)
{
    return QString("%1:%2: %3")
            .arg(error.property("fileName").toString())
            .arg(error.property("lineNumber").toInt())
            .arg(error.toString());
}

void JSKitManager::showConfiguration()
{
    if (_engine) {
        qDebug() << "requesting configuration";
        _jspebble->invokeCallbacks("showConfiguration");
    } else {
        qWarning() << "requested to show configuration, but JS engine is not running";
    }
}

void JSKitManager::handleWebviewClosed(const QString &result)
{
    if (_engine) {
        QJSValue eventObj = _engine->newObject();
        eventObj.setProperty("response", _engine->toScriptValue(result));

        qDebug() << "webview closed with the following result: " << result;

        _jspebble->invokeCallbacks("webviewclosed", QJSValueList({eventObj}));
    } else {
        qWarning() << "webview closed event, but JS engine is not running";
    }
}

void JSKitManager::setConfigurationId(const QUuid &id)
{
    m_configurationId = id;
}

void JSKitManager::handleAppStarted(const QUuid &uuid)
{
    qDebug() << "handleAppStarted!!!" << uuid;
    AppInfo info = _apps->info(uuid);
    if (!info.uuid().isNull() && info.isJSKit()) {
        qDebug() << "Preparing to start JSKit app" << info.uuid() << info.shortName();
        _curApp = info;
        startJsApp();
    }
}

void JSKitManager::handleAppStopped(const QUuid &uuid)
{
    if (!_curApp.uuid().isNull()) {
        if (_curApp.uuid() != uuid) {
            qWarning() << "Closed app with invalid UUID";
        }

        stopJsApp();
        _curApp.setInvalid();
        qDebug() << "App stopped" << uuid;
    }
}

void JSKitManager::handleAppMessage(const QUuid &uuid, const QVariantMap &msg)
{
    qDebug() << "handleAppMessage" << uuid << msg;
    if (_curApp.uuid() == uuid) {
        qDebug() << "received a message for the current JSKit app";

        if (!_engine) {
            qDebug() << "but engine is stopped";
            return;
        }

        QJSValue eventObj = _engine->newObject();
        eventObj.setProperty("payload", _engine->toScriptValue(msg));

        _jspebble->invokeCallbacks("appmessage", QJSValueList({eventObj}));
    }
}

bool JSKitManager::loadJsFile(const QString &filename)
{
    Q_ASSERT(_engine);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to load JS file:" << file.fileName();
        return false;
    }

    qDebug() << "now parsing" << file.fileName();

    QJSValue result = _engine->evaluate(QString::fromUtf8(file.readAll()), file.fileName());
    if (result.isError()) {
        qWarning() << "error while evaluating JS script:" << describeError(result);
        return false;
    }

    qDebug() << "JS script evaluated";

    return true;
}

void JSKitManager::startJsApp()
{
    qDebug() << "startJsApp called";

    if (_engine) stopJsApp();

    if (_curApp.uuid().isNull()) {
        qWarning() << "Attempting to start JS app with invalid UUID";
        return;
    }

    _engine = new QJSEngine(this);
    _jspebble = new JSKitPebble(_curApp, this, _engine);
    _jsconsole = new JSKitConsole(_engine);
    _jsstorage = new JSKitLocalStorage(_curApp.uuid(), _engine);
    _jsgeo = new JSKitGeolocation(this, _engine);

    qDebug() << "starting JS app" << _curApp.shortName();

    QJSValue globalObj = _engine->globalObject();

    globalObj.setProperty("Pebble", _engine->newQObject(_jspebble));
    globalObj.setProperty("console", _engine->newQObject(_jsconsole));
    globalObj.setProperty("localStorage", _engine->newQObject(_jsstorage));

    QJSValue navigatorObj = _engine->newObject();
    navigatorObj.setProperty("geolocation", _engine->newQObject(_jsgeo));
    navigatorObj.setProperty("language", _engine->toScriptValue(QLocale().name()));
    globalObj.setProperty("navigator", navigatorObj);

    // Set this.window = this
    globalObj.setProperty("window", globalObj);

    // Shims for compatibility...
    QJSValue result = _engine->evaluate(
                "function XMLHttpRequest() { return Pebble.createXMLHttpRequest(); }\n\
                function setInterval(func, time) { return Pebble.setInterval(func, time); }\n\
                function clearInterval(id) { Pebble.clearInterval(id); }\n\
                function setTimeout(func, time) { return Pebble.setTimeout(func, time); }\n\
                function clearTimeout(id) { Pebble.clearTimeout(id); }\n\
                ");
    Q_ASSERT(!result.isError());

    // Polyfills...
    loadJsFile("/usr/share/pebble/js/typedarray.js");

    // Now the actual script
    _engine->evaluate(_curApp.getJSApp());

    // Setup the message callback
    QUuid uuid = _curApp.uuid();
    _appmsg->setMessageHandler(uuid, [this, uuid](const QVariantMap &msg) {
        QMetaObject::invokeMethod(this, "handleAppMessage", Qt::QueuedConnection,
                                  Q_ARG(QUuid, uuid),
                                  Q_ARG(QVariantMap, msg));

        // Invoke the slot as a queued connection to give time for the ACK message
        // to go through first.

        return true;
    });

    // We try to invoke the callbacks even if script parsing resulted in error...
    _jspebble->invokeCallbacks("ready");

    if (m_configurationId == _curApp.uuid()) {
        qDebug() << "going to launch config for" << m_configurationId;
        showConfiguration();
    }

    m_configurationId = QUuid();
}

void JSKitManager::stopJsApp()
{
    qDebug() << "stop js app";
    if (!_engine) return; // Nothing to do!

    qDebug() << "stopping JS app";

    if (!_curApp.uuid().isNull()) {
        _appmsg->clearMessageHandler(_curApp.uuid());
    }

    _engine->collectGarbage();

    _engine->deleteLater();
    _engine = 0;
}
