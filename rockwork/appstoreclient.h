#ifndef APPSTORECLIENT_H
#define APPSTORECLIENT_H

#include <QObject>

class QNetworkAccessManager;
class ApplicationsModel;
class AppItem;

class AppStoreClient : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(ApplicationsModel* model READ model CONSTANT)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    enum Type {
        TypeWatchapp,
        TypeWatchface
    };

    explicit AppStoreClient(QObject *parent = 0);

    ApplicationsModel *model() const;

    int limit() const;
    void setLimit(int limit);

    bool busy() const;

signals:
    void busyChanged();
    void limitChanged();

public slots:
    void fetchHome(Type type);
    void fetchLink(const QString &link);
    void fetch(Type type, const QString &hardwarePlatform, int limit = 15, int offset = 0);

    void fetchAppDetails(const QString &appId);

private slots:
    void fetched();

private:
    AppItem *parseAppItem(const QVariantMap &map);
    void setBusy(bool busy);

private:
    QNetworkAccessManager *m_nam;
    ApplicationsModel *m_model;
    int m_limit = 20;
    bool m_busy = false;
};

#endif // APPSTORECLIENT_H
