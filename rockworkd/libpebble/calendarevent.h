#ifndef CALENDAREVENT_H
#define CALENDAREVENT_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QUuid>

class CalendarEvent
{
public:
    CalendarEvent();

    bool isValid() const;

    QString id() const;
    void setId(const QString &id);

    QUuid uuid() const;

    QString title() const;
    void setTitle(const QString &title);

    QString description() const;
    void setDescription(const QString &description);

    QDateTime startTime() const;
    void setStartTime(const QDateTime &startTime);

    QDateTime endTime() const;
    void setEndTime(const QDateTime &endTime);

    QString location() const;
    void setLocation(const QString &location);

    QString calendar() const;
    void setCalendar(const QString &calendar);

    QString comment() const;
    void setComment(const QString &comment);

    QStringList guests() const;
    void setGuests(const QStringList &guests);

    bool recurring() const;
    void setRecurring(bool recurring);

    bool operator==(const CalendarEvent &other) const;

    void saveToCache() const;
    void loadFromCache(const QString &uuid);
    void removeFromCache() const;

private:
    QString m_id;
    QUuid m_uuid;
    QString m_title;
    QString m_description;
    QDateTime m_startTime;
    QDateTime m_endTime;
    QString m_location;
    QString m_calendar;
    QString m_comment;
    QStringList m_guests;
    bool m_recurring = false;
};

#endif // CALENDAREVENT_H