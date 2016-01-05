#ifndef APPINFO_H
#define APPINFO_H

#include <QUuid>
#include <QHash>
#include <QImage>
#include <QLoggingCategory>

#include "enums.h"

class AppInfo
{
public:
    enum FileType {
        FileTypeAppInfo,
        FileTypeJsApp,
        FileTypeManifest,
        FileTypeApplication,
        FileTypeResources,
        FileTypeWorker
    };

    enum Capability {
        Location = 1 << 0,
        Configurable = 1 << 2
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

    AppInfo(const QString &path = QString());
    AppInfo(const QUuid &uuid, bool isWatchFace, const QString &name, const QString &vendor);
    ~AppInfo();

    QString path() const;
    bool isValid() const;
    QUuid uuid() const;
    QString storeId() const;
    QString shortName() const;
    QString longName() const;
    QString companyName() const;
    int versionCode() const;
    QString versionLabel() const;
    bool isWatchface() const;
    bool isJSKit() const;
    bool isSystemApp() const;
    QHash<QString, int> appKeys() const;
    Capabilities capabilities() const;
    bool hasSettings() const;

    bool hasMenuIcon() const;
    QImage getMenuIconImage(HardwarePlatform hardwarePlatform) const;

    QString file(FileType type, HardwarePlatform hardwarePlatform) const;
    quint32 crc(FileType type, HardwarePlatform hardwarePlatform) const;

private:
    QString m_path;

    QUuid m_uuid;
    QString m_storeId;
    QString m_shortName;
    QString m_longName;
    QString m_companyName;
    int m_versionCode = 0;
    QString m_versionLabel;
    QHash<QString, int> m_appKeys;
    Capabilities m_capabilities;

    bool m_isJsKit = false;
    bool m_isWatchface = false;
    bool m_isSystemApp = false;
};

#endif // APPINFO_H
