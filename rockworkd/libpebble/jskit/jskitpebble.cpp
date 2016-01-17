#include <QUrl>
#include <QCryptographicHash>
#include <QSettings>

#include "jskitpebble.h"
#include "jskitxmlhttprequest.h"

static const char *token_salt = "0feeb7416d3c4546a19b04bccd8419b1";

JSKitPebble::JSKitPebble(const AppInfo &info, JSKitManager *mgr, QObject *parent) :
    QObject(parent),
    l(metaObject()->className()),
    m_appInfo(info),
    m_mgr(mgr)
{
}

void JSKitPebble::addEventListener(const QString &type, QJSValue function)
{
    m_listeners[type].append(function);
}

void JSKitPebble::removeEventListener(const QString &type, QJSValue function)
{
    if (!m_listeners.contains(type)) return;

    QList<QJSValue> &callbacks = m_listeners[type];
    for (QList<QJSValue>::iterator it = callbacks.begin(); it != callbacks.end(); ) {
        if (it->strictlyEquals(function)) {
            it = callbacks.erase(it);
        } else {
            ++it;
        }
    }

    if (callbacks.empty()) {
        m_listeners.remove(type);
    }
}

void JSKitPebble::showSimpleNotificationOnPebble(const QString &title, const QString &body)
{
    qCDebug(l) << "showSimpleNotificationOnPebble" << title << body;
    emit m_mgr->appNotification(m_appInfo.uuid(), title, body);
}

uint JSKitPebble::sendAppMessage(QJSValue message, QJSValue callbackForAck, QJSValue callbackForNack)
{
    QVariantMap data = message.toVariant().toMap();
    QPointer<JSKitPebble> pebbObj = this;
    uint transactionId = m_mgr->m_appmsg->nextTransactionId();

    qCDebug(l) << "sendAppMessage" << data;

    m_mgr->m_appmsg->send(
        m_appInfo.uuid(),
        data,
        [this, pebbObj, transactionId, callbackForAck]() mutable {
            if (pebbObj.isNull()) return;

            if (callbackForAck.isCallable()) {
                QJSValue event = pebbObj->buildAckEventObject(transactionId);
                QJSValue result = callbackForAck.call(QJSValueList({event}));

                if (result.isError()) {
                    qCWarning(l) << "error while invoking ACK callback"
                        << callbackForAck.toString() << ":"
                        << JSKitManager::describeError(result);
                }
            }
        },
        [this, pebbObj, transactionId, callbackForNack]() mutable {
            if (pebbObj.isNull()) return;

            if (callbackForNack.isCallable()) {
                QJSValue event = pebbObj->buildAckEventObject(transactionId, "NACK from watch");
                QJSValue result = callbackForNack.call(QJSValueList({event}));

                if (result.isError()) {
                    qCWarning(l) << "error while invoking NACK callback"
                        << callbackForNack.toString() << ":"
                        << JSKitManager::describeError(result);
                }
            }
        }
    );

    return transactionId;
}

void JSKitPebble::getTimelineToken(QJSValue successCallback, QJSValue failureCallback)
{
    //TODO actually implement this
    qCDebug(l) << "call to unsupported method Pebble.getTimelineToken";
    Q_UNUSED(successCallback);

    if (failureCallback.isCallable()) {
        failureCallback.call();
    }
}

void JSKitPebble::timelineSubscribe(const QString &topic, QJSValue successCallback, QJSValue failureCallback)
{
    //TODO actually implement this
    qCDebug(l) << "call to unsupported method Pebble.timelineSubscribe";
    Q_UNUSED(topic);
    Q_UNUSED(successCallback);

    if (failureCallback.isCallable()) {
        failureCallback.call();
    }
}

void JSKitPebble::timelineUnsubscribe(const QString &topic, QJSValue successCallback, QJSValue failureCallback)
{
    //TODO actually implement this
    qCDebug(l) << "call to unsupported method Pebble.timelineUnsubscribe";
    Q_UNUSED(topic);
    Q_UNUSED(successCallback);

    if (failureCallback.isCallable()) {
        failureCallback.call();
    }
}

void JSKitPebble::timelineSubscriptions(QJSValue successCallback, QJSValue failureCallback)
{
    //TODO actually implement this
    qCDebug(l) << "call to unsupported method Pebble.timelineSubscriptions";
    Q_UNUSED(successCallback);

    if (failureCallback.isCallable()) {
        failureCallback.call();
    }
}


QString JSKitPebble::getAccountToken() const
{
    // We do not have any account system, so we just fake something up.
    QCryptographicHash hasher(QCryptographicHash::Md5);

    hasher.addData(token_salt, strlen(token_salt));
    hasher.addData(m_appInfo.uuid().toByteArray());

    QSettings settings;
    QString token = settings.value("accountToken").toString();

    if (token.isEmpty()) {
        token = QUuid::createUuid().toString();
        qCDebug(l) << "created new account token" << token;
        settings.setValue("accountToken", token);
    }

    hasher.addData(token.toLatin1());

    QString hash = hasher.result().toHex();
    qCDebug(l) << "returning account token" << hash;

    return hash;
}

QString JSKitPebble::getWatchToken() const
{
    QCryptographicHash hasher(QCryptographicHash::Md5);

    hasher.addData(token_salt, strlen(token_salt));
    hasher.addData(m_appInfo.uuid().toByteArray());
    hasher.addData(m_mgr->m_pebble->serialNumber().toLatin1());

    QString hash = hasher.result().toHex();
    qCDebug(l) << "returning watch token" << hash;

    return hash;
}

void JSKitPebble::openURL(const QUrl &url)
{
    emit m_mgr->openURL(m_appInfo.uuid().toString(), url.toString());
}

QJSValue JSKitPebble::createXMLHttpRequest()
{
    JSKitXMLHttpRequest *xhr = new JSKitXMLHttpRequest(m_mgr->engine());
    // Should be deleted by JS engine.
    return m_mgr->engine()->newQObject(xhr);
}

QJSValue JSKitPebble::buildAckEventObject(uint transaction, const QString &message) const
{
    QJSEngine *engine = m_mgr->engine();
    QJSValue eventObj = engine->newObject();
    QJSValue dataObj = engine->newObject();

    dataObj.setProperty("transactionId", engine->toScriptValue(transaction));
    eventObj.setProperty("data", dataObj);

    if (!message.isEmpty()) {
        QJSValue errorObj = engine->newObject();

        errorObj.setProperty("message", engine->toScriptValue(message));
        eventObj.setProperty("error", errorObj);
    }

    return eventObj;
}

void JSKitPebble::invokeCallbacks(const QString &type, const QJSValueList &args)
{
    if (!m_listeners.contains(type)) return;
    QList<QJSValue> &callbacks = m_listeners[type];

    for (QList<QJSValue>::iterator it = callbacks.begin(); it != callbacks.end(); ++it) {
        qCDebug(l) << "invoking callback" << type << it->toString();
        QJSValue result = it->call(args);
        if (result.isError()) {
            qCWarning(l) << "error while invoking callback"
                << type << it->toString() << ":"
                << JSKitManager::describeError(result);
        }
    }
}
