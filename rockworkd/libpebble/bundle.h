#ifndef BUNDLE_H
#define BUNDLE_H

#include <QLoggingCategory>
#include <QSharedDataPointer>
#include <QString>
#include <QIODevice>

#include "enums.h"

class BundleData;

class Bundle
{
    Q_GADGET

    static QLoggingCategory l;

public:
    enum File {
        MANIFEST,
        INFO,
        BINARY,
        RESOURCES,
        APPJS
    };

    static Bundle fromPath(const QString &path);

    Bundle();
    Bundle(const Bundle &);
    Bundle &operator=(const Bundle &);
    ~Bundle();

    QString type() const;
    QString path() const;
    bool isValid() const;

    QIODevice *openFile(enum File, HardwarePlatform hardwarePlatform, QIODevice::OpenMode mode = 0) const;
    bool fileExists(enum File, HardwarePlatform hardwarePlatform) const;
    quint32 crcFile(enum File, HardwarePlatform hardwarePlatform) const;

private:
    QSharedDataPointer<BundleData> b;
};

#endif // BUNDLE_H
