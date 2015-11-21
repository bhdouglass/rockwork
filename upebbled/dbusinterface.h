#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include <QObject>

class DBusInterface : public QObject
{
    Q_OBJECT
public:
    explicit DBusInterface(QObject *parent = 0);

signals:

public slots:
};

#endif // DBUSINTERFACE_H
