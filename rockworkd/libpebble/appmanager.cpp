#include <QStandardPaths>
#include <QDir>
#include "appmanager.h"
#include "pebble.h"

#include "watchconnection.h"
#include "watchdatareader.h"
#include "watchdatawriter.h"
#include "uploadmanager.h"

AppManager::AppManager(Pebble *pebble, WatchConnection *connection)
    : QObject(pebble),
      m_pebble(pebble),
      m_connection(connection)
{
    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dataDir.mkpath("apps")) {
        qWarning() << "could not create apps dir" << dataDir.absoluteFilePath("apps");
    }
    qDebug() << "install apps in" << dataDir.absoluteFilePath("apps");

    m_connection->registerEndpointHandler(WatchConnection::EndpointAppFetch, this, "handleAppFetchMessage");
}

QStringList AppManager::appPaths() const
{
    return QStandardPaths::locateAll(QStandardPaths::DataLocation,
                                     QLatin1String("apps"),
                                     QStandardPaths::LocateDirectory);
}

QList<QUuid> AppManager::appUuids() const
{
    return m_appsUuids.keys();
}

//QList<QString> AppManager::appIds() const
//{
//    return m_appsIds.keys();
//}

AppInfo AppManager::info(const QUuid &uuid) const
{
    return m_appsUuids.value(uuid);
}

//AppInfo AppManager::info(const QString &id) const
//{
//    return m_appsUuids.value(m_appsIds.value(id));
//}

void AppManager::rescan()
{
    Q_FOREACH(const AppInfo &appInfo, m_appsUuids) {
        m_appsUuids.remove(appInfo.uuid());
//        m_appsIds.remove(appInfo.id());
    }

    Q_FOREACH(const QString &path, appPaths()) {
        QDir dir(path);
        qDebug() << "scanning dir" << dir.absolutePath();
        QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable | QDir::Executable);
        entries << dir.entryList(QStringList("*.pbw"), QDir::Files | QDir::Readable);
        qDebug() << "scanning dir results" << entries;
        Q_FOREACH(const QString &path, entries) {
            QString appPath = dir.absoluteFilePath(path);
            if (dir.exists(path + "/appinfo.json")) {
                scanApp(appPath);
            } else if (QFileInfo(appPath).isFile()) {
                scanApp(appPath);
            }
        }
    }
}

void AppManager::handleAppFetchMessage(const QByteArray &data)
{
    WatchDataReader reader(data);
    reader.read<quint8>();
    QUuid uuid = reader.readUuid();
    quint32 appFetchId = reader.read<quint32>();

    bool haveApp = m_appsUuids.contains(uuid);

    AppFetchResponse response;
    if (haveApp) {
        response.setStatus(AppFetchResponse::StatusStart);
        m_connection->writeToPebble(WatchConnection::EndpointAppFetch, response.serialize());
    } else {
        qWarning() << "App with uuid" << uuid.toString() << "which is not installed.";
        response.setStatus(AppFetchResponse::StatusInvalidUUID);
        m_connection->writeToPebble(WatchConnection::EndpointAppFetch, response.serialize());
        emit idMismatchDetected();
        return;
    }

    AppInfo appInfo = m_appsUuids.value(uuid);

    QString binaryFile = appInfo.file(AppInfo::FileTypeApplication, m_pebble->hardwarePlatform());
    quint32 crc = appInfo.crc(AppInfo::FileTypeApplication, m_pebble->hardwarePlatform());
    qDebug() << "opened binary" << binaryFile << "for hardware" << m_pebble->hardwarePlatform() << "crc" << crc;
    m_connection->uploadManager()->uploadAppBinary(appFetchId, binaryFile, crc, [this, appInfo, appFetchId](){
        qDebug() << "binary file uploaded successfully";

        QString resourcesFile = appInfo.file(AppInfo::FileTypeResources, m_pebble->hardwarePlatform());
        quint32 crc = appInfo.crc(AppInfo::FileTypeResources, m_pebble->hardwarePlatform());
        qDebug() << "uploadign resource file" << resourcesFile;
        m_connection->uploadManager()->uploadAppResources(appFetchId, resourcesFile, crc, [this, appInfo, appFetchId]() {
            qDebug() << "resource file uploaded successfully";

            QString workerFile = appInfo.file(AppInfo::FileTypeWorker, m_pebble->hardwarePlatform());
            if (!workerFile.isEmpty()) {
                quint32 crc = appInfo.crc(AppInfo::FileTypeWorker, m_pebble->hardwarePlatform());
                m_connection->uploadManager()->uploadAppWorker(appFetchId, workerFile, crc, [this]() {
                    qDebug() << "worker file uploaded successfully";
                });
            }
        });
    });
}

void AppManager::insertAppInfo(const AppInfo &info)
{
    m_appsUuids.insert(info.uuid(), info);
//    m_appsIds.insert(info.id(), info.uuid());
    emit appsChanged();
}

QUuid AppManager::scanApp(const QString &path)
{
    qDebug() << "scanning app" << path;
    AppInfo info(path);
    if (info.isValid()) {
        insertAppInfo(info);
    }
    return info.uuid();
}

void AppManager::removeApp(const QUuid &uuid)
{
    AppInfo info = m_appsUuids.take(uuid);
    QDir dir(info.path());
    dir.removeRecursively();
    emit appsChanged();
}

AppFetchResponse::AppFetchResponse(Status status):
    m_status(status)
{

}

void AppFetchResponse::setStatus(AppFetchResponse::Status status)
{
    m_status = status;
}

QByteArray AppFetchResponse::serialize() const
{
    QByteArray ret;
    WatchDataWriter writer(&ret);
    writer.write<quint8>(m_command);
    writer.write<quint8>(m_status);
    return ret;
}
