#include "blobdb.h"
#include "watchconnection.h"
#include "pebble.h"

#include <QDebug>

BlobDB::BlobDB(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_connection(connection)
{

}

void BlobDB::insertNotification(const QString &sender, const QString &subject, const QString &data)
{
    QUuid itemUuid = QUuid::createUuid();
    TimelineItem timelineItem(itemUuid, TimelineItem::TypeNotification);
    timelineItem.setFlags(TimelineItem::FlagSingleEvent);

    TimelineAttribute titleAttribute(TimelineAttribute::TypeTitle, sender.left(64).toUtf8());
    timelineItem.appendAttribute(titleAttribute);

    TimelineAttribute subjectAttribute(TimelineAttribute::TypeSubtitle, subject.left(64).toUtf8());
    timelineItem.appendAttribute(subjectAttribute);

    TimelineAttribute bodyAttribute(TimelineAttribute::TypeBody, data.left(64).toUtf8());
    timelineItem.appendAttribute(bodyAttribute);

    // Haven't decrypted this yet. Copied from sailfish...
    QByteArray blubb;
    static quint16 bla = 1;
    blubb.append(bla & 0xFF); blubb.append((bla >> 8) & 0xFF); blubb.append('\0'); blubb.append('\0'); // content
    bla++;
    TimelineAttribute sourceAttribute(TimelineAttribute::TypeTinyIcon, blubb);
    timelineItem.appendAttribute(sourceAttribute);

    TimelineAction dismissAction(0, TimelineAction::TypeDismiss);
    TimelineAttribute dismissAttribute(TimelineAttribute::TypeTitle, "Dismiss");
    dismissAction.appendAttribute(dismissAttribute);
    timelineItem.appendAction(dismissAction);

    TimelineAction dismiss1Action(1, TimelineAction::TypeSnooze);
    TimelineAttribute dismiss1Attribute(TimelineAttribute::TypeTitle, "Blubb");
    dismiss1Action.appendAttribute(dismiss1Attribute);
    timelineItem.appendAction(dismiss1Action);

    insert(BlobDB::BlobDBIdNotification, timelineItem);
}

void BlobDB::insertTimelinePin(TimelineItem::Layout layout)
{
    TimelineItem item(TimelineItem::TypePin, TimelineItem::FlagSingleEvent, QDateTime::currentDateTime().addMSecs(1000 * 60 * 2), 60);
    item.setLayout(layout);

    TimelineAttribute titleAttribute(TimelineAttribute::TypeTitle, "PinTitlex");
    item.appendAttribute(titleAttribute);

    TimelineAttribute subjectAttribute(TimelineAttribute::TypeSubtitle, "PinSubtitle");
    item.appendAttribute(subjectAttribute);

    TimelineAttribute bodyAttribute(TimelineAttribute::TypeBody, "PinBody");
    item.appendAttribute(bodyAttribute);

//    TimelineAttribute t1Attribute(TimelineAttribute::TypeTinyIcon, "T1");
//    item.appendAttribute(t1Attribute);

    QByteArray color;
    color.append('\0'); color.append('\0'); color.append('\0'); color.append(0xff);
    TimelineAttribute t2Attribute(TimelineAttribute::TypeGuess2, color);
    item.appendAttribute(t2Attribute);

//    TimelineAttribute t3Attribute(TimelineAttribute::TypeGuess2, "T3");
//    item.appendAttribute(t3Attribute);

    TimelineAction dismissAction(0, TimelineAction::TypeDismiss);
    TimelineAttribute dismissAttribute(TimelineAttribute::TypeTitle, "Dismiss");
    dismissAction.appendAttribute(dismissAttribute);
    item.appendAction(dismissAction);

    insert(BlobDB::BlobDBIdPin, item);
}

void BlobDB::insertReminder()
{

    TimelineItem item(TimelineItem::TypeReminder, TimelineItem::FlagSingleEvent, QDateTime::currentDateTime().addMSecs(1000 * 60 * 2), 0);

    TimelineAttribute titleAttribute(TimelineAttribute::TypeTitle, "ReminderTitle");
    item.appendAttribute(titleAttribute);

    TimelineAttribute subjectAttribute(TimelineAttribute::TypeSubtitle, "ReminderSubtitle");
    item.appendAttribute(subjectAttribute);

    TimelineAttribute bodyAttribute(TimelineAttribute::TypeBody, "ReminderBody");
    item.appendAttribute(bodyAttribute);

    //    TimelineAttribute guessAttribute(TimelineAttribute::TypeGuess, "ReminderGuess");
    //    item.appendAttribute(guessAttribute);

    TimelineAction dismissAction(0, TimelineAction::TypeDismiss);
    TimelineAttribute dismissAttribute(TimelineAttribute::TypeTitle, "Dismiss");
    dismissAction.appendAttribute(dismissAttribute);
    item.appendAction(dismissAction);

    insert(BlobDB::BlobDBIdReminder, item);
    //    qDebug() << "adding timeline item" << ddd.toHex();

}

void BlobDB::insert(BlobDBId database, TimelineItem item)
{
    BlobCommand cmd;
    cmd.m_command = BlobDB::OperationInsert;
    cmd.m_token = generateToken();
    cmd.m_database = database;

    cmd.m_key = item.itemId().toRfc4122();
    cmd.m_value = item.serialize();

    m_connection->writeToPebble(WatchConnection::EndpointBlobDB, cmd.serialize());
}

void BlobDB::clear(BlobDB::BlobDBId database)
{
    BlobCommand cmd;
    cmd.m_command = BlobDB::OperationClear;
    cmd.m_token = generateToken();
    cmd.m_database = database;

    m_connection->writeToPebble(WatchConnection::EndpointBlobDB, cmd.serialize());
}

quint16 BlobDB::generateToken()
{
    return (qrand() % ((int)pow(2, 16) - 2)) + 1;
}


QByteArray BlobCommand::serialize() const
{
    QByteArray ret;
    ret.append((quint8)m_command);
    ret.append(m_token & 0xFF); ret.append(((m_token >> 8) & 0xFF));
    ret.append((quint8)m_database);

    if (m_command == BlobDB::OperationInsert || m_command == BlobDB::OperationDelete) {
        ret.append(m_key.length() & 0xFF);
        ret.append(m_key);
    }
    if (m_command == BlobDB::OperationInsert) {
        ret.append(m_value.length() & 0xFF); ret.append((m_value.length() >> 8) & 0xFF); // value length
        ret.append(m_value);
    }

    return ret;
}

TimelineItem::TimelineItem(TimelineItem::Type type, Flags flags, const QDateTime &timestamp, quint16 duration):
    TimelineItem(QUuid::createUuid(), type, flags, timestamp, duration)
{

}

TimelineItem::TimelineItem(const QUuid &uuid, TimelineItem::Type type, Flags flags, const QDateTime &timestamp, quint16 duration):
    PebblePacket(),
    m_itemId(uuid),
    m_timestamp(timestamp),
    m_duration(duration),
    m_type(type),
    m_flags(flags)
{

}

QUuid TimelineItem::itemId() const
{
    return m_itemId;
}

void TimelineItem::setLayout(quint8 layout)
{
    m_layout = layout;
}

void TimelineItem::setFlags(Flags flags)
{
    m_flags = flags;
}

void TimelineItem::appendAttribute(const TimelineAttribute &attribute)
{
    m_attributes.append(attribute);
}

void TimelineItem::appendAction(const TimelineAction &action)
{
    m_actions.append(action);
}

QByteArray TimelineItem::serialize() const
{
    QByteArray ret;
    ret.append(m_itemId.toRfc4122());
    ret.append(m_parentId.toRfc4122());
    int ts = m_timestamp.toMSecsSinceEpoch() / 1000;
    ret.append(ts & 0xFF); ret.append((ts >> 8) & 0xFF); ret.append((ts >> 16) & 0xFF); ret.append((ts >> 24) & 0xFF);
    ret.append(m_duration & 0xFF); ret.append(((m_duration >> 8) & 0xFF));
    ret.append((quint8)m_type);
    ret.append(m_flags & 0xFF); ret.append(((m_flags >> 8) & 0xFF));
    ret.append(m_layout);

    QByteArray serializedAttributes;
    foreach (const TimelineAttribute &attribute, m_attributes) {
        serializedAttributes.append(attribute.serialize());
    }

    QByteArray serializedActions;
    foreach (const TimelineAction &action, m_actions) {
        serializedActions.append(action.serialize());
    }
    quint16 dataLength = serializedAttributes.length() + serializedActions.length();
    ret.append(dataLength & 0xFF); ret.append(((dataLength >> 8) & 0xFF));
    ret.append(m_attributes.count());
    ret.append(m_actions.count());
    ret.append(serializedAttributes);
    ret.append(serializedActions);
    return ret;
}

TimelineAction::TimelineAction(quint8 actionId, TimelineAction::Type type, const QList<TimelineAttribute> &attributes):
    PebblePacket(),
    m_actionId(actionId),
    m_type(type),
    m_attributes(attributes)
{

}

void TimelineAction::appendAttribute(const TimelineAttribute &attribute)
{
    m_attributes.append(attribute);
}

QByteArray TimelineAttribute::serialize() const
{
    QByteArray ret;
    ret.append((quint8)m_type);
    ret.append(m_content.length() & 0xFF); ret.append(((m_content.length() >> 8) & 0xFF)); // length
    ret.append(m_content);
    return ret;
}
