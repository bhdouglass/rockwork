#ifndef APPSTORECLIENT_H
#define APPSTORECLIENT_H

#include <QObject>

class QNetworkAccessManager;
class ApplicationsModel;

class AppStoreClient : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(ApplicationsModel* model READ model CONSTANT)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
public:
    enum Type {
        TypeWatchapp,
        TypeWatchface
    };

    explicit AppStoreClient(QObject *parent = 0);

    ApplicationsModel *model() const;

    bool busy() const;

signals:
    void busyChanged();

public slots:
    void fetch(Type type, const QString &hardwarePlatform, int limit = 15, int offset = 0);

private slots:
    void fetched();

private:
    QNetworkAccessManager *m_nam;
    ApplicationsModel *m_model;
    bool m_busy = false;
};

#endif // APPSTORECLIENT_H
