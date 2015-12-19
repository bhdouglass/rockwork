#include "calendarevent.h"

#include <QSettings>
#include <QStandardPaths>
#include <QFile>

CalendarEvent::CalendarEvent()
{
}

bool CalendarEvent::isValid() const
{
    return !m_id.isNull();
}

QUuid CalendarEvent::id() const
{
    return m_id;
}

void CalendarEvent::setUuid(const QUuid &id)
{
    m_id = id;
}

QString CalendarEvent::title() const
{
    return m_title;
}

void CalendarEvent::setTitle(const QString &title)
{
    m_title = title;
}

QString CalendarEvent::description() const
{
    return m_description;
}

void CalendarEvent::setDescription(const QString &description)
{
    m_description = description;
}

QDateTime CalendarEvent::startTime() const
{
    return m_startTime;
}

void CalendarEvent::setStartTime(const QDateTime &startTime)
{
    m_startTime = startTime;
}

QDateTime CalendarEvent::endTime() const
{
    return m_endTime;
}

void CalendarEvent::setEndTime(const QDateTime &endTime)
{
    m_endTime = endTime;
}

QString CalendarEvent::location() const
{
    return m_location;
}

void CalendarEvent::setLocation(const QString &location)
{
    m_location = location;
}

QString CalendarEvent::calendar() const
{
    return m_calendar;
}

void CalendarEvent::setCalendar(const QString &calendar)
{
    m_calendar = calendar;
}

QString CalendarEvent::comment() const
{
    return m_comment;
}

void CalendarEvent::setComment(const QString &comment)
{
    m_comment = comment;
}

QStringList CalendarEvent::guests() const
{
    return m_guests;
}

void CalendarEvent::setGuests(const QStringList &guests)
{
    m_guests = guests;
}

bool CalendarEvent::recurring() const
{
    return m_recurring;
}

void CalendarEvent::setRecurring(bool recurring)
{
    m_recurring = recurring;
}

bool CalendarEvent::operator==(const CalendarEvent &other) const
{
    return m_id == other.id()
            && m_title == other.title()
            && m_description == other.description()
            && m_startTime == other.startTime()
            && m_endTime == other.endTime()
            && m_location == other.location()
            && m_calendar == other.calendar()
            && m_comment == other.comment()
            && m_guests == other.guests()
            && m_recurring == other.recurring();

}

void CalendarEvent::saveToCache() const
{
    QSettings s(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/calendarevent-" + m_id.toString(), QSettings::IniFormat);
    s.setValue("title", m_title);
    s.setValue("description", m_description);
    s.setValue("startTime", m_startTime);
    s.setValue("endTime", m_endTime);
    s.setValue("location", m_location);
    s.setValue("calendar", m_calendar);
    s.setValue("comment", m_comment);
    s.setValue("guests", m_guests);
    s.setValue("recurring", m_recurring);
}

void CalendarEvent::loadFromCache(const QUuid &id)
{
    m_id = id;
    QSettings s(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/calendarevent-" + m_id.toString(), QSettings::IniFormat);
    m_title = s.value("title").toString();
    m_description = s.value("description").toString();
    m_startTime = s.value("startTime").toDateTime();
    m_endTime = s.value("endTime").toDateTime();
    m_location = s.value("location").toString();
    m_calendar = s.value("calendar").toString();
    m_comment = s.value("comment").toString();
    m_guests = s.value("guests").toStringList();
    m_recurring = s.value("recurring").toBool();
}

void CalendarEvent::removeFromCache() const
{
    QSettings s(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/calendarevent-" + m_id.toString(), QSettings::IniFormat);
    s.remove("");
    QFile::remove(s.fileName());
}

