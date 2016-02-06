#ifndef JSKITCONSOLE_H
#define JSKITCONSOLE_H

#include <QLoggingCategory>

class JSKitConsole : public QObject
{
    Q_OBJECT
    QLoggingCategory l;

public:
    explicit JSKitConsole(QObject *parent=0);

    Q_INVOKABLE void log(const QString &msg);
    Q_INVOKABLE void warn(const QString &msg);
    Q_INVOKABLE void error(const QString &msg);
    Q_INVOKABLE void info(const QString &msg);
};

#endif // JSKITCONSOLE_H
