#ifndef APPINSTALLER_H
#define APPINSTALLER_H

#include <QObject>
#include <QMap>

class QNetworkAccessManager;

class AppInfo
{
public:
    bool valid = false;
    uint id = 0;
    QString name;
    QString company;
    unsigned int flags = 0;
    unsigned short version = 0;
};

class AppInstaller : public QObject
{
    Q_OBJECT
public:
    enum AppManagerOperation {
        AppManagerOperationGetAppBankStatus = 1,
        AppManagerOperationRemoveApp = 2,
        AppManagerOperationRefreshApp = 3,
        AppManagerOperationGetAppBankUuids = 5
    };

    explicit AppInstaller(QObject *parent = 0);

public slots:
    void installPackage();
    void removePackage(int slot);

signals:

private slots:
    void downloadFinished();

    void refreshReply(const QByteArray &replyData);
    void removeAppFinished(const QByteArray &replyData);

private:
    void refreshWatchStatus();
    void downloadApp(const QString &url);
    void unpackArchive(const QString &hash);
    void uploadApp(const QString &hash);

    QNetworkAccessManager *m_nam;
    QString m_storagePath;

    QMap<int, AppInfo> m_slots;
    int m_totalSlots;
};

#endif // APPINSTALLER_H
