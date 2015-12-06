#ifndef BLOBDB_H
#define BLOBDB_H

#include "watchconnection.h"

#include <QObject>
#include <QDateTime>

class Pebble;

class TimelineAttribute
{
public:
    enum Type {
        TypeTitle = 1,
        TypeSubtitle = 2,
        TypeBody = 3,
        TypeTinyIcon = 4,
        TypeGuess = 5,
        TypeGuess2 = 6
    };

    TimelineAttribute(Type type, const QByteArray &content):
        m_type(type),
        m_content(content)
    {}

    QByteArray serialize() const;
private:
    Type m_type;
    QByteArray m_content;
};

class TimelineAction: public PebblePacket
{
public:
    enum Type {
        TypeAncsDismiss = 1,
        TypeGeneric = 2,
        TypeResponse = 3,
        TypeDismiss = 4,
        TypeHTTP = 5,
        TypeSnooze = 6,
        TypeOpenWatchApp = 7,
        TypeEmpty = 8,
        TypeRemove = 9,
        TypeOpenPin = 10
    };
    TimelineAction(quint8 actionId, Type type, const QList<TimelineAttribute> &attributes = QList<TimelineAttribute>());
    void appendAttribute(const TimelineAttribute &attribute);

    QByteArray serialize() const override {
        QByteArray ret;
        ret.append(m_actionId);
        ret.append((quint8)m_type);
        ret.append(m_attributes.count());
        foreach (const TimelineAttribute &attr, m_attributes) {
            ret.append(attr.serialize());
        }
        return ret;
    }

private:
    quint8 m_actionId;
    Type m_type;
    QList<TimelineAttribute> m_attributes;
};

class TimelineItem: public PebblePacket
{
public:
    enum Type {
        TypeNotification = 1,
        TypePin = 2,
        TypeReminder = 3
    };

    // TODO: this is probably not complete and maybe even wrong.
    enum Flag {
        FlagNone = 0x00,
        FlagSingleEvent = 0x01,
        FlagTimeInUTC = 0x02,
        FlagAllDay = 0x04
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    // TODO: This is not complete
    enum Layout {
        LayoutGenericPin = 0x01,
        LayoutCalendar = 0x02
    };

    TimelineItem(Type type, TimelineItem::Flags flags = FlagNone, const QDateTime &timestamp = QDateTime::currentDateTime(), quint16 duration = 0);
    TimelineItem(const QUuid &uuid, Type type, Flags flags = FlagNone, const QDateTime &timestamp = QDateTime::currentDateTime(), quint16 duration = 0);

    QUuid itemId() const;

    void setLayout(quint8 layout);
    void setFlags(Flags flags);

    void appendAttribute(const TimelineAttribute &attribute);
    void appendAction(const TimelineAction &action);

    QByteArray serialize() const override;

private:
    QUuid m_itemId;
    QUuid m_parentId;
    QDateTime m_timestamp;
    quint16 m_duration = 0;
    Type m_type;
    Flags m_flags; // quint16
    quint8 m_layout = 0x01; // TODO: find out what this is about
    QList<TimelineAttribute> m_attributes;
    QList<TimelineAction> m_actions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TimelineItem::Flags)

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


    explicit BlobDB(Pebble *pebble, WatchConnection *connection);

    void insertNotification(const QString &sender, const QString &subject, const QString &data);
    void insertTimelinePin(TimelineItem::Layout layout);
    void insertReminder();

    void insert(BlobDBId database, TimelineItem item);
    void clear(BlobDBId database);

public:
    // FIXME: make private after finishing testing
    quint16 generateToken();

private:
    Pebble *m_pebble;
    WatchConnection *m_connection;
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
