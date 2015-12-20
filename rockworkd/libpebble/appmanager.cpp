#include <QStandardPaths>
#include <QDir>
#include "appmanager.h"

#include "watchconnection.h"
#include "watchdatareader.h"
#include "watchdatawriter.h"
#include "uploadmanager.h"

AppManager::AppManager(Pebble *pebble, WatchConnection *connection)
    : QObject(pebble),
      m_pebble(pebble),
      m_connection(connection),
      _watcher(new QFileSystemWatcher(this))
{
//    connect(_watcher, &QFileSystemWatcher::directoryChanged,
//            this, &AppManager::rescan);

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

AppInfo AppManager::info(const QUuid &uuid) const
{
    return m_appsUuids.value(uuid);
}

AppInfo AppManager::info(const QString &id) const
{
    return m_appsUuids.value(m_appsIds.value(id));
}

//AppInfo AppManager::info(const QString &name) const
//{
//    QUuid uuid = _names.value(name);
//    if (!uuid.isNull()) {
//        return info(uuid);
//    } else {
//        return AppInfo();
//    }
//}

void AppManager::rescan()
{
    QStringList watchedDirs = _watcher->directories();
    if (!watchedDirs.isEmpty()) _watcher->removePaths(watchedDirs);
    QStringList watchedFiles = _watcher->files();
    if (!watchedFiles.isEmpty()) _watcher->removePaths(watchedFiles);
    Q_FOREACH(const AppInfo &appInfo, m_appsUuids) {
        if (appInfo.isLocal()) {
            m_appsUuids.remove(appInfo.uuid());
            m_appsIds.remove(appInfo.id());
        }
    }

    Q_FOREACH(const QString &path, appPaths()) {
        QDir dir(path);
        _watcher->addPath(dir.absolutePath());
        qDebug() << "scanning dir" << dir.absolutePath();
        QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable | QDir::Executable);
        entries << dir.entryList(QStringList("*.pbw"), QDir::Files | QDir::Readable);
        qDebug() << "scanning dir results" << entries;
        Q_FOREACH(const QString &path, entries) {
            QString appPath = dir.absoluteFilePath(path);
            _watcher->addPath(appPath);
            if (dir.exists(path + "/appinfo.json")) {
                _watcher->addPath(appPath + "/appinfo.json");
                scanApp(appPath);
            } else if (QFileInfo(appPath).isFile()) {
                scanApp(appPath);
            }
        }
    }

    qDebug() << "now watching" << _watcher->directories() << _watcher->files();
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
        return;
    }

    AppInfo appInfo = m_appsUuids.value(uuid);

    QIODevice *binaryFile = appInfo.openFile(AppInfo::BINARY, m_pebble->hardwarePlatform(), QFile::ReadOnly);
    quint32 crc = appInfo.crcFile(AppInfo::BINARY);
    m_connection->uploadManager()->uploadAppBinary(binaryFile, crc, appFetchId, [this, binaryFile, appInfo, appFetchId](){
        qDebug() << "binary file uploaded successfully";
        binaryFile->close();

        QIODevice *resourcesFile = appInfo.openFile(AppInfo::RESOURCES, m_pebble->hardwarePlatform(), QFile::ReadOnly);
        quint32 crc = appInfo.crcFile(AppInfo::RESOURCES);
        m_connection->uploadManager()->uploadAppResources(appFetchId, resourcesFile, crc, [this, resourcesFile]() {
            qDebug() << "resource file uploaded successfully";
            resourcesFile->close();
        });
    });
}

void AppManager::insertAppInfo(const AppInfo &info)
{
    m_appsUuids.insert(info.uuid(), info);
    m_appsIds.insert(info.id(), info.uuid());

    const char *type = info.isWatchface() ? "watchface" : "app";
    const char *local = info.isLocal() ? "local" : "watch";
    qDebug() << "found" << local << type << info.shortName() << info.versionCode() << "/" << info.versionLabel() << "with uuid" << info.uuid().toString();
    emit appsChanged();
}

void AppManager::scanApp(const QString &path)
{
    qDebug() << "scanning app" << path;
    const AppInfo &info = AppInfo::fromPath(path, m_pebble->hardwarePlatform());
    if (info.isValid() && info.isLocal()) insertAppInfo(info);
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
