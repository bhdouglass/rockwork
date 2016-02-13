#ifndef JSKITCONSOLE_H
#define JSKITCONSOLE_H

#include <QLoggingCategory>
#include <QJSValue>

class JSKitConsole : public QObject
{
    Q_OBJECT
    QLoggingCategory l;

public:
    explicit JSKitConsole(QObject *parent=0);

    Q_INVOKABLE void log(const QJSValue &msg0=QJSValue(), const QJSValue &msg1=QJSValue(), const QJSValue &msg2=QJSValue(), const QJSValue &msg3=QJSValue(), const QJSValue &msg4=QJSValue(), const QJSValue &msg5=QJSValue(), const QJSValue &msg6=QJSValue(), const QJSValue &msg7=QJSValue(), const QJSValue &msg8=QJSValue(), const QJSValue &msg9=QJSValue());
    Q_INVOKABLE void warn(const QJSValue &msg0=QJSValue(), const QJSValue &msg1=QJSValue(), const QJSValue &msg2=QJSValue(), const QJSValue &msg3=QJSValue(), const QJSValue &msg4=QJSValue(), const QJSValue &msg5=QJSValue(), const QJSValue &msg6=QJSValue(), const QJSValue &msg7=QJSValue(), const QJSValue &msg8=QJSValue(), const QJSValue &msg9=QJSValue());
    Q_INVOKABLE void error(const QJSValue &msg0=QJSValue(), const QJSValue &msg1=QJSValue(), const QJSValue &msg2=QJSValue(), const QJSValue &msg3=QJSValue(), const QJSValue &msg4=QJSValue(), const QJSValue &msg5=QJSValue(), const QJSValue &msg6=QJSValue(), const QJSValue &msg7=QJSValue(), const QJSValue &msg8=QJSValue(), const QJSValue &msg9=QJSValue());
    Q_INVOKABLE void info(const QJSValue &msg0=QJSValue(), const QJSValue &msg1=QJSValue(), const QJSValue &msg2=QJSValue(), const QJSValue &msg3=QJSValue(), const QJSValue &msg4=QJSValue(), const QJSValue &msg5=QJSValue(), const QJSValue &msg6=QJSValue(), const QJSValue &msg7=QJSValue(), const QJSValue &msg8=QJSValue(), const QJSValue &msg9=QJSValue());
};

#endif // JSKITCONSOLE_H
