#include <QDebug>

#include "jskitconsole.h"

JSKitConsole::JSKitConsole(QObject *parent) :
    QObject(parent),
    l(metaObject()->className())
{
}

void JSKitConsole::log(const QString &msg)
{
    qCDebug(l) << msg;
}

void JSKitConsole::warn(const QString &msg)
{
    qCWarning(l) << msg;
}

void JSKitConsole::error(const QString &msg)
{
    qCCritical(l) << msg;
}

void JSKitConsole::info(const QString &msg)
{
    qCDebug(l) << msg;
}
