#include "blobdb.h"
#include "watchconnection.h"
#include "watchdatareader.h"

#include <QDebug>
#include <QOrganizerRecurrenceRule>

BlobDB::BlobDB(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_connection(connection)
{
    m_connection->registerEndpointHandler(WatchConnection::EndpointActionHandler, this, "actionInvoked");
}

void BlobDB::insertNotification(const Notification &notification)
{
    TimelineAttribute::IconID iconId = TimelineAttribute::IconIDDefaultBell;
    TimelineAttribute::Color color = TimelineAttribute::ColorRed;
    QString muteName;
    switch (notification.type()) {
    case Notification::NotificationTypeAlarm:
        iconId = TimelineAttribute::IconIDAlarm;
        muteName = "Alarms";
        break;
    case Notification::NotificationTypeFacebook:
        iconId = TimelineAttribute::IconIDFacebook;
        color = TimelineAttribute::ColorBlue;
        muteName = "facebook";
        break;
    case Notification::NotificationTypeGMail:
        iconId = TimelineAttribute::IconIDGMail;
        muteName = "GMail";
        break;
    case Notification::NotificationTypeHangout:
        iconId = TimelineAttribute::IconIDHangout;
        color = TimelineAttribute::ColorGreen;
        muteName = "Hangout";
        break;
    case Notification::NotificationTypeMissedCall:
        iconId = TimelineAttribute::IconIDDefaultMissedCall;
        muteName = "call notifications";
        break;
    case Notification::NotificationTypeMusic:
        iconId = TimelineAttribute::IconIDMusic;
        muteName = "music";
        break;
    case Notification::NotificationTypeReminder:
        iconId = TimelineAttribute::IconIDReminder;
        muteName = "reminders";
        break;
    case Notification::NotificationTypeTelegram:
        iconId = TimelineAttribute::IconIDTelegram;
        color = TimelineAttribute::ColorLightBlue;
        muteName = "Telegram";
        break;
    case Notification::NotificationTypeTwitter:
        iconId = TimelineAttribute::IconIDTwitter;
        color = TimelineAttribute::ColorBlue2;
        muteName = "Twitter";
        break;
    case Notification::NotificationTypeWeather:
        iconId = TimelineAttribute::IconIDWeather;
        muteName = "Weather";
        break;
    case Notification::NotificationTypeWhatsApp:
        iconId = TimelineAttribute::IconIDWhatsApp;
        color = TimelineAttribute::ColorGreen;
        muteName = "WhatsApp";
        break;
    case Notification::NotificationTypeSMS:
        muteName = "SMS";
        iconId = TimelineAttribute::IconIDDefaultBell;
        break;
    case Notification::NotificationTypeEmail:
    default:
        muteName = "e mails";
        iconId = TimelineAttribute::IconIDDefaultBell;
        break;
    }

    QUuid itemUuid = QUuid::createUuid();
    TimelineItem timelineItem(itemUuid, TimelineItem::TypeNotification);
    timelineItem.setFlags(TimelineItem::FlagSingleEvent);

    TimelineAttribute titleAttribute(TimelineAttribute::TypeTitle, notification.sender().left(64).toUtf8());
    timelineItem.appendAttribute(titleAttribute);

    TimelineAttribute subjectAttribute(TimelineAttribute::TypeSubtitle, notification.subject().left(64).toUtf8());
    timelineItem.appendAttribute(subjectAttribute);

    TimelineAttribute bodyAttribute(TimelineAttribute::TypeBody, notification.body().toUtf8());
    timelineItem.appendAttribute(bodyAttribute);

    TimelineAttribute iconAttribute(TimelineAttribute::TypeTinyIcon, iconId);
    timelineItem.appendAttribute(iconAttribute);

    TimelineAttribute colorAttribute(TimelineAttribute::TypeColor, color);
    timelineItem.appendAttribute(colorAttribute);

    TimelineAction dismissAction(0, TimelineAction::TypeDismiss);
    TimelineAttribute dismissAttribute(TimelineAttribute::TypeTitle, "Dismiss");
    dismissAction.appendAttribute(dismissAttribute);
    timelineItem.appendAction(dismissAction);

    TimelineAction muteAction(1, TimelineAction::TypeGeneric);
    TimelineAttribute muteActionAttribute(TimelineAttribute::TypeTitle, "Mute " + muteName.toUtf8());
    muteAction.appendAttribute(muteActionAttribute);
    timelineItem.appendAction(muteAction);

    if (!notification.actToken().isEmpty()) {
        TimelineAction actAction(2, TimelineAction::TypeGeneric);
        TimelineAttribute actActionAttribute(TimelineAttribute::TypeTitle, "Open on phone");
        actAction.appendAttribute(actActionAttribute);
        timelineItem.appendAction(actAction);
    }

    insert(BlobDB::BlobDBIdNotification, timelineItem);
    m_notificationSources.insert(itemUuid, notification);
}

void BlobDB::insertTimelinePin(TimelineItem::Layout layout, const QDateTime &startTime, const QDateTime &endTime, const QString &title, const QString &desctiption, const QMap<QString, QString> fields, bool recurring)
{
//    TimelineItem item(TimelineItem::TypePin, TimelineItem::FlagSingleEvent, QDateTime::currentDateTime().addMSecs(1000 * 60 * 2), 60);

    qDebug() << "inserting timeline pin:" << title << startTime << endTime;
    int duration = (endTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()) / 1000 / 60;
    TimelineItem item(TimelineItem::TypePin, TimelineItem::FlagSingleEvent, startTime, duration);
    item.setLayout(layout);

    TimelineAttribute titleAttribute(TimelineAttribute::TypeTitle, title.toUtf8());
    item.appendAttribute(titleAttribute);

    TimelineAttribute bodyAttribute(TimelineAttribute::TypeBody, desctiption.left(128).toUtf8());
    item.appendAttribute(bodyAttribute);

//    TimelineAttribute iconAttribute(TimelineAttribute::TypeTinyIcon, TimelineAttribute::IconIDTelegram);
//    item.appendAttribute(iconAttribute);

    if (!fields.isEmpty()) {
        TimelineAttribute fieldNames(TimelineAttribute::TypeFieldNames, fields.keys());
        item.appendAttribute(fieldNames);

        TimelineAttribute fieldValues(TimelineAttribute::TypeFieldValues, fields.values());
        item.appendAttribute(fieldValues);
    }

    if (recurring) {
        TimelineAttribute guess(TimelineAttribute::TypeRecurring, 1);
        item.appendAttribute(guess);
    }

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

    QByteArray data;
    data.append(0x07); data.append('\0'); data.append('\0'); data.append(0x80);
    TimelineAttribute guessAttribute(TimelineAttribute::TypeTinyIcon, data);
    item.appendAttribute(guessAttribute);
    qDebug() << "attrib" << guessAttribute.serialize();

    TimelineAction dismissAction(0, TimelineAction::TypeDismiss);
    TimelineAttribute dismissAttribute(TimelineAttribute::TypeTitle, "Dismiss");
    dismissAction.appendAttribute(dismissAttribute);
    item.appendAction(dismissAction);

    insert(BlobDB::BlobDBIdReminder, item);
    //    qDebug() << "adding timeline item" << ddd.toHex();

}

void BlobDB::syncCalendar(const QList<CalendarEvent> &events)
{
    clear(BlobDB::BlobDBIdPin);

    foreach (const CalendarEvent &event, events) {
        if (!event.startTime().isValid() || !event.endTime().isValid()
                || event.startTime().addDays(2) < QDateTime::currentDateTime()
                || QDateTime::currentDateTime().addDays(5) < event.startTime()) {
//            qWarning() << "Not adding timeline pin. Out of date";
            continue;
        }

        QMap<QString, QString> fields;
        if (!event.location().isEmpty()) fields.insert("Location", event.location());
        if (!event.calendar().isEmpty()) fields.insert("Calendar", event.calendar());
        if (!event.comment().isEmpty()) fields.insert("Comments", event.comment());
        if (!event.guests().isEmpty()) fields.insert("Guests", event.guests().join(", "));
        insertTimelinePin(TimelineItem::LayoutCalendar, event.startTime(), event.endTime(), event.title(), event.description(), fields, event.recurring());
    }
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

void BlobDB::actionInvoked(const QByteArray &actionReply)
{
    WatchDataReader reader(actionReply);
    TimelineAction::Type actionType = (TimelineAction::Type)reader.read<quint8>();
    QUuid notificationId = reader.readUuid();
    quint8 actionId = reader.read<quint8>();
    quint8 param = reader.read<quint8>(); // Is this correct? So far I've only seen 0x00 in here

    // Not sure what to do with those yet
    Q_UNUSED(actionType)
    Q_UNUSED(param)

    qDebug() << "have action reply" << actionId << actionReply.toHex();

    Status status = StatusError;
    QList<TimelineAttribute> attributes;

    Notification notification = m_notificationSources.value(notificationId);
    QString sourceId = notification.sourceId();
    if (sourceId.isEmpty()) {
        status = StatusError;
    } else {
        switch (actionId) {
        case 1: { // Mute source
            TimelineAttribute textAttribute(TimelineAttribute::TypeSubtitle, "Muted!");
            attributes.append(textAttribute);
//            TimelineAttribute iconAttribute(TimelineAttribute::TypeLargeIcon, TimelineAttribute::IconIDTelegram);
//            attributes.append(iconAttribute);
            emit muteSource(sourceId);
            status = StatusSuccess;
            break;
        }
        case 2: { // Open on phone
            TimelineAttribute textAttribute(TimelineAttribute::TypeSubtitle, "Opened!");
            attributes.append(textAttribute);
            qDebug() << "opening" << notification.actToken();
            emit actionTriggered(notification.actToken());
            status = StatusSuccess;
        }
        }
    }

    QByteArray reply;
    reply.append(0x11); // Length of id & status code
    reply.append(notificationId.toRfc4122());
    reply.append(status);
    reply.append(attributes.count());
    foreach (const TimelineAttribute &attrib, attributes) {
        reply.append(attrib.serialize());
    }
    m_connection->writeToPebble(WatchConnection::EndpointActionHandler, reply);
}

void BlobDB::sendActionReply()
{

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
