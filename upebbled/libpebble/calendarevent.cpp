#include "calendarevent.h"

CalendarEvent::CalendarEvent()
{

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
    return m_title == other.title()
            && m_description == other.description()
            && m_startTime == other.startTime()
            && m_endTime == other.endTime()
            && m_location == other.location()
            && m_calendar == other.calendar()
            && m_comment == other.comment()
            && m_guests == other.guests()
            && m_recurring == other.recurring();

}

