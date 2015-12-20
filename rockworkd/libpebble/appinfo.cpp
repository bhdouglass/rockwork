#include <QSharedData>
#include <QBuffer>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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

struct AppInfoData : public QSharedData {
    QString appid; // The store end works with this. Let's keep track of it to identify packages without having to unpack them
    QUuid uuid;
    QString shortName;
    QString longName;
    QString companyName;
    int versionCode;
    QString versionLabel;
    bool watchface;
    bool jskit;
    AppInfo::Capabilities capabilities;
    QHash<QString, int> keyInts;
    QHash<int, QString> keyNames;
    bool menuIcon;
    int menuIconResource;

    // Entries from the binary header
    quint32 flags;
    quint32 icon;
    quint8 appVersionMajor;
    quint8 appVersionMinor;
    quint8 sdkVersionMajor;
    quint8 sdkVersionMinor;
};

QLoggingCategory AppInfo::l("AppInfo");

AppInfo::AppInfo() : Bundle(), d(new AppInfoData)
{
    d->versionCode = 0;
    d->watchface = false;
    d->jskit = false;
    d->capabilities = 0;
    d->menuIcon = false;
    d->menuIconResource = -1;
}

AppInfo::AppInfo(const AppInfo &rhs) : Bundle(rhs), d(rhs.d)
{}

AppInfo::AppInfo(const Bundle &rhs) : Bundle(rhs), d(new AppInfoData)
{}

AppInfo &AppInfo::operator=(const AppInfo &rhs)
{
    Bundle::operator=(rhs);
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

AppInfo::~AppInfo()
{}

QString AppInfo::id() const
{
    return d->appid;
}

bool AppInfo::isLocal() const
{
    return ! path().isEmpty();
}

bool AppInfo::isValid() const
{
    return ! d->uuid.isNull();
}

void AppInfo::setInvalid()
{
    d->uuid = QUuid(); // Clear the uuid to force invalid app
}

QUuid AppInfo::uuid() const
{
    return d->uuid;
}

QString AppInfo::shortName() const
{
    return d->shortName;
}

QString AppInfo::longName() const
{
    return d->longName;
}

QString AppInfo::companyName() const
{
    return d->companyName;
}

int AppInfo::versionCode() const
{
    return d->versionCode;
}

QString AppInfo::versionLabel() const
{
    return d->versionLabel;
}

bool AppInfo::isWatchface() const
{
    return d->watchface;
}

bool AppInfo::isJSKit() const
{
    return d->jskit;
}

AppInfo::Capabilities AppInfo::capabilities() const
{
    return d->capabilities;
}

void AppInfo::addAppKey(const QString &key, int value)
{
    d->keyInts.insert(key, value);
    d->keyNames.insert(value, key);
}

bool AppInfo::hasAppKeyValue(int value) const
{
    return d->keyNames.contains(value);
}

QString AppInfo::appKeyForValue(int value) const
{
    return d->keyNames.value(value);
}

bool AppInfo::hasAppKey(const QString &key) const
{
    return d->keyInts.contains(key);
}

int AppInfo::valueForAppKey(const QString &key) const
{
    return d->keyInts.value(key, -1);
}

bool AppInfo::hasMenuIcon() const
{
    return d->menuIcon && d->menuIconResource >= 0;
}

quint32 AppInfo::flags() const
{
    return d->flags;
}

quint32 AppInfo::icon() const
{
    return d->icon;
}

quint8 AppInfo::appVersionMajor() const
{
    return d->appVersionMajor;
}

quint8 AppInfo::appVersionMinor() const
{
    return d->appVersionMinor;
}

quint8 AppInfo::sdkVersionMajor() const
{
    return d->sdkVersionMajor;
}

quint8 AppInfo::sdkVersionMinor() const
{
    return d->sdkVersionMinor;
}

QImage AppInfo::getMenuIconImage() const
{
    if (hasMenuIcon()) {
        QScopedPointer<QIODevice> imageRes(openFile(AppInfo::RESOURCES, m_hardwarePlatform));
        QByteArray data = extractFromResourcePack(imageRes.data(), d->menuIconResource);
        if (!data.isEmpty()) {
            return decodeResourceImage(data);
        }
    }

    return QImage();
}

QByteArray AppInfo::getMenuIconPng() const
{
    QByteArray data;
    QBuffer buf(&data);
    buf.open(QIODevice::WriteOnly);
    getMenuIconImage().save(&buf, "PNG");
    buf.close();
    return data;
}

QString AppInfo::getJSApp() const
{
    if (!isValid() || !isLocal()) return QString();

    QScopedPointer<QIODevice> appJS(openFile(AppInfo::APPJS, m_hardwarePlatform, QIODevice::Text));
    if (!appJS) {
        qCWarning(l) << "cannot find app" << d->shortName << "app.js";
        return QString();
    }

    return QString::fromUtf8(appJS->readAll());
}

AppInfo AppInfo::fromPath(const QString &path, Pebble::HardwarePlatform hardwarePlatform)
{
    AppInfo info(Bundle::fromPath(path, hardwarePlatform));
    info.m_hardwarePlatform = hardwarePlatform;

    if (!static_cast<Bundle>(info).isValid()) {
        qCWarning(l) << "bundle" << path << "is not valid";
        return AppInfo();
    }

    QScopedPointer<QIODevice> appInfoJSON(info.openFile(AppInfo::INFO, hardwarePlatform, QIODevice::Text));
    if (!appInfoJSON) {
        qCWarning(l) << "cannot find app" << path << "info json";
        return AppInfo();
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(appInfoJSON->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qCWarning(l) << "cannot parse app" << path << "info json" << parseError.errorString();
        return AppInfo();
    }
    appInfoJSON->close();

    info.d->appid = path.split("/").last();

    const QJsonObject root = doc.object();
    info.d->uuid = QUuid(root["uuid"].toString());
    info.d->shortName = root["shortName"].toString();
    info.d->longName = root["longName"].toString();
    info.d->companyName = root["companyName"].toString();
    info.d->versionCode = root["versionCode"].toInt();
    info.d->versionLabel = root["versionLabel"].toString();

    const QJsonObject watchapp = root["watchapp"].toObject();
    info.d->watchface = watchapp["watchface"].toBool();

    info.d->jskit = info.fileExists(AppInfo::APPJS, hardwarePlatform);

    if (root.contains("capabilities")) {
        const QJsonArray capabilities = root["capabilities"].toArray();
        AppInfo::Capabilities caps = 0;
        for (auto it = capabilities.constBegin(); it != capabilities.constEnd(); ++it) {
            QString cap = (*it).toString();
            if (cap == "location") caps |= AppInfo::Location;
            if (cap == "configurable") caps |= AppInfo::Configurable;
        }
        info.d->capabilities = caps;
    }

    if (root.contains("appKeys")) {
        const QJsonObject appkeys = root["appKeys"].toObject();
        for (auto it = appkeys.constBegin(); it != appkeys.constEnd(); ++it) {
            info.addAppKey(it.key(), it.value().toInt());
        }
    }

    if (root.contains("resources")) {
        const QJsonObject resources = root["resources"].toObject();
        const QJsonArray media = resources["media"].toArray();
        int index = 0;

        for (auto it = media.constBegin(); it != media.constEnd(); ++it) {
            const QJsonObject res = (*it).toObject();
            const QJsonValue menuIcon = res["menuIcon"];

            switch (menuIcon.type()) {
            case QJsonValue::Bool:
                info.d->menuIcon = menuIcon.toBool();
                info.d->menuIconResource = index;
                break;
            case QJsonValue::String:
                info.d->menuIcon = !menuIcon.toString().isEmpty();
                info.d->menuIconResource = index;
                break;
            default:
                break;
            }

            index++;
        }
    }

    if (info.uuid().isNull() || info.shortName().isEmpty()) {
        qCWarning(l) << "invalid or empty uuid/name in json of" << path;
        return AppInfo();
    }

    QIODevice* appBinary = info.openFile(AppInfo::BINARY, hardwarePlatform, QIODevice::ReadOnly);
    QByteArray data = appBinary->read(512);
    WatchDataReader reader(data);
    qDebug() << "Header:" << reader.readFixedString(8);
    qDebug() << "struct Major version:" << reader.read<quint8>();
    qDebug() << "struct Minor version:" << reader.read<quint8>();
    info.d->sdkVersionMajor = reader.read<quint8>();
    qDebug() << "sdk Major version:" << info.d->sdkVersionMajor;
    info.d->sdkVersionMinor = reader.read<quint8>();
    qDebug() << "sdk Minor version:" << info.d->sdkVersionMinor;
    info.d->appVersionMajor = reader.read<quint8>();
    qDebug() << "app Major version:" << info.d->appVersionMajor;
    info.d->appVersionMinor = reader.read<quint8>();
    qDebug() << "app Minor version:" << info.d->appVersionMinor;
    qDebug() << "size:" << reader.readLE<quint16>();
    qDebug() << "offset:" << reader.readLE<quint32>();
    qDebug() << "crc:" << reader.readLE<quint32>();
    qDebug() << "App name:" << reader.readFixedString(32);
    qDebug() << "Vendor name:" << reader.readFixedString(32);
    info.d->icon = reader.readLE<quint32>();
    qDebug() << "Icon:" << info.d->icon;
    qDebug() << "Symbol table address:" << reader.readLE<quint32>();
    info.d->flags = reader.readLE<quint32>();
    qDebug() << "Flags:" << info.d->flags;
    qDebug() << "Num relocatable entries:" << reader.readLE<quint32>();

    appBinary->close();
    qDebug() << "app data" << data.toHex();

    return info;
}

AppMetadata AppInfo::toAppMetadata()
{
    AppMetadata metadata;
    metadata.setUuid(uuid());
    metadata.setFlags(flags());
    metadata.setAppVersion(appVersionMajor(), appVersionMinor());
    metadata.setSDKVersion(sdkVersionMajor(), sdkVersionMinor());
    metadata.setAppFaceBgColor(0);
    metadata.setAppFaceTemplateId(0);
    metadata.setAppName(shortName());
    metadata.setIcon(icon());
    return metadata;
}

QByteArray AppInfo::extractFromResourcePack(QIODevice *dev, int wanted_id) const
{
    if (!dev) {
        qCWarning(l) << "requested resource" << wanted_id
                     << "from NULL resource file";
        return QByteArray();
    }

    QByteArray data = dev->readAll();
    WatchDataReader wd(data);

    int num_files = wd.readLE<quint32>();
    wd.readLE<quint32>(); // crc for entire file
    wd.readLE<quint32>(); // timestamp

    qDebug() << "reading" << num_files << "resources";

    QList<ResourceEntry> table;

    for (int i = 0; i < num_files; i++) {
        ResourceEntry e;
        e.index = wd.readLE<quint32>();
        e.offset = wd.readLE<quint32>();
        e.length = wd.readLE<quint32>();
        e.crc = wd.readLE<quint32>();

        if (wd.bad()) {
            qCWarning(l) << "short read on resource file";
            return QByteArray();
        }

        table.append(e);
    }

    if (wanted_id >= table.size()) {
        qCWarning(l) << "specified resource does not exist";
        return QByteArray();
    }

    const ResourceEntry &e = table[wanted_id];

    int offset = 12 + 256 * 16 + e.offset;

    return data.mid(offset, e.length);
}

QImage AppInfo::decodeResourceImage(const QByteArray &data) const
{
    WatchDataReader wd(data);
    int scanline = wd.readLE<quint16>();
    wd.skip(sizeof(quint16) + sizeof(quint32));
    int width = wd.readLE<quint16>();
    int height = wd.readLE<quint16>();

    QImage img(width, height, QImage::Format_MonoLSB);
    const uchar *src = reinterpret_cast<const uchar *>(&data.constData()[12]);
    for (int line = 0; line < height; ++line) {
        memcpy(img.scanLine(line), src, qMin(scanline, img.bytesPerLine()));
        src += scanline;
    }

    return img;
}
