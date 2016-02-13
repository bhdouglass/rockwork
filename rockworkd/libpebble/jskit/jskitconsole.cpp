#include <QDebug>

#include "jskitconsole.h"

JSKitConsole::JSKitConsole(QObject *parent) :
    QObject(parent),
    l(metaObject()->className())
{
}

void JSKitConsole::log(const QJSValue &msg0, const QJSValue &msg1, const QJSValue &msg2, const QJSValue &msg3, const QJSValue &msg4, const QJSValue &msg5, const QJSValue &msg6, const QJSValue &msg7, const QJSValue &msg8, const QJSValue &msg9)
{
    qCDebug(l) << msg0.toString();

    if (!msg1.isUndefined()) {
        qCDebug(l) << msg1.toString();
    }

    if (!msg2.isUndefined()) {
        qCDebug(l) << msg2.toString();
    }

    if (!msg3.isUndefined()) {
        qCDebug(l) << msg3.toString();
    }

    if (!msg4.isUndefined()) {
        qCDebug(l) << msg4.toString();
    }

    if (!msg5.isUndefined()) {
        qCDebug(l) << msg5.toString();
    }

    if (!msg6.isUndefined()) {
        qCDebug(l) << msg6.toString();
    }

    if (!msg7.isUndefined()) {
        qCDebug(l) << msg7.toString();
    }

    if (!msg8.isUndefined()) {
        qCDebug(l) << msg8.toString();
    }

    if (!msg9.isUndefined()) {
        qCDebug(l) << msg9.toString();
    }
}

void JSKitConsole::warn(const QJSValue &msg0, const QJSValue &msg1, const QJSValue &msg2, const QJSValue &msg3, const QJSValue &msg4, const QJSValue &msg5, const QJSValue &msg6, const QJSValue &msg7, const QJSValue &msg8, const QJSValue &msg9)
{
    qCWarning(l) << msg0.toString();

    if (!msg1.isUndefined()) {
        qCWarning(l) << msg1.toString();
    }

    if (!msg2.isUndefined()) {
        qCWarning(l) << msg2.toString();
    }

    if (!msg3.isUndefined()) {
        qCWarning(l) << msg3.toString();
    }

    if (!msg4.isUndefined()) {
        qCWarning(l) << msg4.toString();
    }

    if (!msg5.isUndefined()) {
        qCWarning(l) << msg5.toString();
    }

    if (!msg6.isUndefined()) {
        qCWarning(l) << msg6.toString();
    }

    if (!msg7.isUndefined()) {
        qCWarning(l) << msg7.toString();
    }

    if (!msg8.isUndefined()) {
        qCWarning(l) << msg8.toString();
    }

    if (!msg9.isUndefined()) {
        qCWarning(l) << msg9.toString();
    }
}

void JSKitConsole::error(const QJSValue &msg0, const QJSValue &msg1, const QJSValue &msg2, const QJSValue &msg3, const QJSValue &msg4, const QJSValue &msg5, const QJSValue &msg6, const QJSValue &msg7, const QJSValue &msg8, const QJSValue &msg9)
{
    qCCritical(l) << msg0.toString();

    if (!msg1.isUndefined()) {
        qCCritical(l) << msg1.toString();
    }

    if (!msg2.isUndefined()) {
        qCCritical(l) << msg2.toString();
    }

    if (!msg3.isUndefined()) {
        qCCritical(l) << msg3.toString();
    }

    if (!msg4.isUndefined()) {
        qCCritical(l) << msg4.toString();
    }

    if (!msg5.isUndefined()) {
        qCCritical(l) << msg5.toString();
    }

    if (!msg6.isUndefined()) {
        qCCritical(l) << msg6.toString();
    }

    if (!msg7.isUndefined()) {
        qCCritical(l) << msg7.toString();
    }

    if (!msg8.isUndefined()) {
        qCCritical(l) << msg8.toString();
    }

    if (!msg9.isUndefined()) {
        qCCritical(l) << msg9.toString();
    }
}

void JSKitConsole::info(const QJSValue &msg0, const QJSValue &msg1, const QJSValue &msg2, const QJSValue &msg3, const QJSValue &msg4, const QJSValue &msg5, const QJSValue &msg6, const QJSValue &msg7, const QJSValue &msg8, const QJSValue &msg9)
{
    qCDebug(l) << msg0.toString();

    if (!msg1.isUndefined()) {
        qCDebug(l) << msg1.toString();
    }

    if (!msg2.isUndefined()) {
        qCDebug(l) << msg2.toString();
    }

    if (!msg3.isUndefined()) {
        qCDebug(l) << msg3.toString();
    }

    if (!msg4.isUndefined()) {
        qCDebug(l) << msg4.toString();
    }

    if (!msg5.isUndefined()) {
        qCDebug(l) << msg5.toString();
    }

    if (!msg6.isUndefined()) {
        qCDebug(l) << msg6.toString();
    }

    if (!msg7.isUndefined()) {
        qCDebug(l) << msg7.toString();
    }

    if (!msg8.isUndefined()) {
        qCDebug(l) << msg8.toString();
    }

    if (!msg9.isUndefined()) {
        qCDebug(l) << msg9.toString();
    }
}
