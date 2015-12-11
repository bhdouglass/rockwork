#ifndef BLOBDB_H
#define BLOBDB_H

#include "watchconnection.h"
#include "pebble.h"
#include "timelineitem.h"

#include <QObject>
#include <QDateTime>
#include <QOrganizerItem>

QTORGANIZER_USE_NAMESPACE


class BlobDB : public QObject
{
    Q_OBJECT
public:
    enum BlobDBId {
        BlobDBIdTest = 0,
        BlobDBIdPin = 1,
        BlobDBIdApp = 2,
        BlobDBIdReminder = 3,
        BlobDBIdNotification = 4

    };
    enum Operation {
        OperationInsert = 0x01,
        OperationDelete = 0x04,
        OperationClear = 0x05
    };

    enum Status {
        StatusSuccess = 0x00,
        StatusError = 0x01
    };


    explicit BlobDB(Pebble *pebble, WatchConnection *connection);

    void insertNotification(const Notification &notification);
    void insertTimelinePin(TimelineItem::Layout layout);
    void insertReminder();
    void syncCalendar(const QList<QOrganizerItem> &items);

    void insert(BlobDBId database, TimelineItem item);
    void clear(BlobDBId database);

private slots:
    void actionInvoked(const QByteArray &data);
    void sendActionReply();

signals:
    void muteSource(const QString &sourceId);

public:
    // FIXME: make private after finishing testing
    quint16 generateToken();

private:
    Pebble *m_pebble;
    WatchConnection *m_connection;

    QHash<QUuid, Notification> m_notificationSources;

};

class BlobCommand: public PebblePacket
{
public:
    BlobDB::Operation m_command; // quint8
    quint16 m_token;
    BlobDB::BlobDBId m_database;

    QByteArray m_key;
    QByteArray m_value;

    QByteArray serialize() const override;
};

#endif // BLOBDB_H
