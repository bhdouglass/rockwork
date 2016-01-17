#ifndef JSKITLOCALSTORAGE_P_H
#define JSKITLOCALSTORAGE_P_H

#include <QSettings>
#include <QJSValue>
#include <QUuid>

class JSKitLocalStorage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int length READ length)

public:
    explicit JSKitLocalStorage(const QString &storagePath, const QUuid &uuid, QObject *parent=0);

    int length() const;

    Q_INVOKABLE QJSValue getItem(const QString &key) const;
    Q_INVOKABLE void setItem(const QString &key, const QString &value);
    Q_INVOKABLE void removeItem(const QString &key);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QJSValue key(int index);

private:
    static QString getStorageFileFor(const QString &storageDir, const QUuid &uuid);

private:
    QSettings *m_storage;
};

#endif // JSKITLOCALSTORAGE_P_H
