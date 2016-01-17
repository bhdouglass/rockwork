#include <QDesktopServices>
#include <QDir>
#include <QDebug>

#include "jskitlocalstorage.h"

JSKitLocalStorage::JSKitLocalStorage(const QString &storagePath, const QUuid &uuid, QObject *parent):
    QObject(parent),
    m_storage(new QSettings(getStorageFileFor(storagePath, uuid), QSettings::IniFormat, this))
{
}

int JSKitLocalStorage::length() const
{
    return m_storage->allKeys().size();
}

QJSValue JSKitLocalStorage::getItem(const QString &key) const
{
    QVariant value = m_storage->value(key);

    if (value.isValid()) {
        return QJSValue(value.toString());
    } else {
        return QJSValue(QJSValue::NullValue);
    }
}

void JSKitLocalStorage::setItem(const QString &key, const QString &value)
{
    m_storage->setValue(key, QVariant::fromValue(value));
}

void JSKitLocalStorage::removeItem(const QString &key)
{
    m_storage->remove(key);
}

void JSKitLocalStorage::clear()
{
    m_storage->clear();
}

QJSValue JSKitLocalStorage::key(int index)
{
    QStringList keys = m_storage->allKeys();
    QJSValue key(QJSValue::NullValue);

    if (keys.size() > index) {
        key = QJSValue(keys[index]);
    }

    return key;
}

QString JSKitLocalStorage::getStorageFileFor(const QString &storageDir, const QUuid &uuid)
{
    QDir dataDir(storageDir + "/js-storage");
    if (!dataDir.exists() && !dataDir.mkpath(dataDir.absolutePath())) {
        qWarning() << "Error creating jskit storage dir";
        return QString();
    }

    QString fileName = uuid.toString();
    fileName.remove('{');
    fileName.remove('}');
    return dataDir.absoluteFilePath(fileName + ".ini");
}
