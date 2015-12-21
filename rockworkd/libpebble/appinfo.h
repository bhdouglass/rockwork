#ifndef APPINFO_H
#define APPINFO_H

#include <QSharedDataPointer>
#include <QUuid>
#include <QHash>
#include <QImage>
#include <QLoggingCategory>
#include "bundle.h"
#include "appmetadata.h"

class AppInfoData;

class AppInfo : public Bundle
{
    Q_GADGET

    static QLoggingCategory l;

public:
    enum Capability {
        Location = 1 << 0,
        Configurable = 1 << 2
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

    static AppInfo fromPath(const QString &path);

    AppMetadata toAppMetadata();
public:
    AppInfo();
    AppInfo(const AppInfo &);
    AppInfo(const Bundle &);
    AppInfo &operator=(const AppInfo &);
    ~AppInfo();

    QString id() const;
    bool isLocal() const;
    bool isValid() const;
    QUuid uuid() const;
    QString shortName() const;
    QString longName() const;
    QString companyName() const;
    int versionCode() const;
    QString versionLabel() const;
    bool isWatchface() const;
    bool isJSKit() const;
    Capabilities capabilities() const;
    bool hasMenuIcon() const;

    void addAppKey(const QString &key, int value);
    bool hasAppKeyValue(int value) const;
    QString appKeyForValue(int value) const;

    bool hasAppKey(const QString &key) const;
    int valueForAppKey(const QString &key) const;

    QImage getMenuIconImage(HardwarePlatform hardwarePlatform) const;
    QByteArray getMenuIconPng(HardwarePlatform hardwarePlatform) const;
    QString getJSApp() const;

    void setInvalid();

protected:
    QByteArray extractFromResourcePack(QIODevice *dev, int id) const;
    QImage decodeResourceImage(const QByteArray &data) const;

private:
    QSharedDataPointer<AppInfoData> d;
};

#endif // APPINFO_H
