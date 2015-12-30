#include <QSharedData>
#include <QBuffer>
#include <QDir>
#include <QJsonDocument>
#include <QUuid>
#include "appinfo.h"
#include "watchdatareader.h"
#include "pebble.h"

namespace {
struct ResourceEntry {
    int index;
    quint32 offset;
    quint32 length;
    quint32 crc;
};
}

AppInfo::AppInfo(const QString &path):
    m_path(path)
{
    if (path.isEmpty()) {
        return;
    }

    QFile f(path + "/appinfo.json");
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Error opening appinfo.json";
        return;
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(f.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing appinfo.json";
        return;
    }

    QVariantMap map = jsonDoc.toVariant().toMap();

    m_uuid = map.value("uuid").toUuid();
    m_shortName = map.value("shortName").toString();
    m_longName = map.value("longName").toString();
    m_companyName = map.value("companyName").toString();
    m_versionCode = map.value("versionCode").toInt();
    m_versionLabel = map.value("versionLabel").toString();

    m_isWatchface = map.value("watchapp").toMap().value("watchface").toBool();

    if (map.contains("appKeys")) {
        QVariantMap appKeyMap = map.value("appKeys").toMap();
        foreach (const QString &key, appKeyMap.keys()) {
            m_appKeys.insert(key, appKeyMap.value(key).toInt());
        }
    }

    QFile jsApp(path + "/pebble-js-app.js");
    m_isJsKit = jsApp.exists();
}


AppInfo::~AppInfo()
{}

QString AppInfo::path() const
{
    return m_path;
}

bool AppInfo::isValid() const
{
    return !m_uuid.isNull();
}

QUuid AppInfo::uuid() const
{
    return m_uuid;
}

QString AppInfo::shortName() const
{
    return m_shortName;
}

QString AppInfo::longName() const
{
    return m_longName;
}

QString AppInfo::companyName() const
{
    return m_companyName;
}

int AppInfo::versionCode() const
{
    return m_versionCode;
}

QString AppInfo::versionLabel() const
{
    return m_versionLabel;
}

bool AppInfo::isWatchface() const
{
    return m_isWatchface;
}

bool AppInfo::isJSKit() const
{
    return m_isJsKit;
}

QHash<QString, int> AppInfo::appKeys() const
{
    return m_appKeys;
}

AppInfo::Capabilities AppInfo::capabilities() const
{
    return m_capabilities;
}

QString AppInfo::file(AppInfo::FileType type, HardwarePlatform hardwarePlatform) const
{
    // Those two will always be in the top level dir. HardwarePlatform is irrelevant.
    switch (type) {
    case FileTypeAppInfo:
        return m_path + "/appInfo.js";
    case FileTypeJsApp:
        return m_path + "/pebble-js-app.js";
    default:
        ;
    }

    // For all the others we have to find the manifest file
    QList<QString> possibleDirs;

    switch (hardwarePlatform) {
    case HardwarePlatformAplite:
        if (QFileInfo::exists(path() + "/aplite/")) {
            possibleDirs.append("aplite");
        }
        possibleDirs.append("");
        break;
    case HardwarePlatformBasalt:
        if (QFileInfo::exists(path() + "/basalt/")) {
            possibleDirs.append("basalt");
        }
        possibleDirs.append("");
        break;
    case HardwarePlatformChalk:
        if (QFileInfo::exists(path() + "/chalk/")) {
            possibleDirs.append("chalk");
        }
        break;
    default:
        ;
    }

    QString manifestFilename;
    QString subDir;
    foreach (const QString &dir, possibleDirs) {
        if (QFileInfo::exists(m_path + "/" + dir + "/manifest.json")) {
            subDir = "/" + dir;
            manifestFilename = m_path + subDir + "/manifest.json";
            break;
        }
    }
    if (manifestFilename.isEmpty()) {
        qWarning() << "Error finding manifest.json";
        return QString();
    }

    // We want the manifiest file. just return it without parsing it
    if (type == FileTypeManifest) {
        return manifestFilename;
    }

    QFile manifest(manifestFilename);
    if (!manifest.open(QFile::ReadOnly)) {
        qWarning() << "Error opening" << manifestFilename;
        return QString();
    }
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(manifest.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing" << manifestFilename;
        return QString();
    }

    QVariantMap manifestMap = jsonDoc.toVariant().toMap();
    switch (type) {
    case FileTypeApplication:
        return m_path + subDir + "/" + manifestMap.value("application").toMap().value("name").toString();
    case FileTypeResources:
        if (manifestMap.contains("resources")) {
            return m_path + subDir + "/" + manifestMap.value("resources").toMap().value("name").toString();
        } else {
            return QString();
        }
    case FileTypeWorker:
        if (manifestMap.contains("worker")) {
            return m_path + subDir + "/" + manifestMap.value("worker").toMap().value("name").toString();
        } else {
            return QString();
        }
    default:
        ;
    }
    return QString();
}

quint32 AppInfo::crc(AppInfo::FileType type, HardwarePlatform hardwarePlatform) const
{
    switch (type) {
    case FileTypeAppInfo:
    case FileTypeJsApp:
    case FileTypeManifest:
        qWarning() << "Cannot get crc for file type" << type;
        return 0;
    default: ;
    }

    QFile manifest(file(FileTypeManifest, hardwarePlatform));
    if (!manifest.open(QFile::ReadOnly)) {
        qWarning() << "Error opening manifest file";
        return 0;
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(manifest.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing manifest file";
        return 0;
    }

    QVariantMap manifestMap = jsonDoc.toVariant().toMap();
    switch (type) {
    case FileTypeApplication:
        return manifestMap.value("application").toMap().value("crc").toUInt();
    case FileTypeResources:
        return manifestMap.value("resources").toMap().value("crc").toUInt();
    case FileTypeWorker:
        return manifestMap.value("worker").toMap().value("crc").toUInt();
    default:
        ;
    }
    return 0;
}

