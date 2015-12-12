#include "organizeradapter.h"

#include <QOrganizerItemFetchRequest>
#include <QDebug>
#include <QOrganizerEventOccurrence>
#include <QOrganizerItemDetail>

QTORGANIZER_USE_NAMESPACE

#define MANAGER           "eds"
#define MANAGER_FALLBACK  "memory"

OrganizerAdapter::OrganizerAdapter(QObject *parent) : QObject(parent)
{
    QString envManager(qgetenv("ALARM_BACKEND"));
    if (envManager.isEmpty())
        envManager = MANAGER;
    if (!QOrganizerManager::availableManagers().contains(envManager)) {
        envManager = MANAGER_FALLBACK;
    }
    m_manager = new QOrganizerManager(envManager);
    m_manager->setParent(this);
}

void OrganizerAdapter::refresh()
{
    QOrganizerItemFetchRequest *operation = new QOrganizerItemFetchRequest(this);
    operation->setManager(m_manager);
    connect(operation, &QOrganizerItemFetchRequest::stateChanged, this, &OrganizerAdapter::readStateChanged);
    operation->start();
}

QList<CalendarEvent> OrganizerAdapter::items() const
{
    return m_items;
}

void OrganizerAdapter::readStateChanged(QOrganizerAbstractRequest::State state)
{
    QOrganizerItemFetchRequest *operation = static_cast<QOrganizerItemFetchRequest*>(sender());
    QList<CalendarEvent> items;
    foreach (const QOrganizerItem &item, operation->items()) {
        QOrganizerEvent organizerEvent(item);
        if (organizerEvent.displayLabel().isEmpty()) {
            continue;
        }
        CalendarEvent event;
        event.setTitle(organizerEvent.displayLabel());
        event.setDescription(organizerEvent.description());
        event.setStartTime(organizerEvent.startDateTime());
        event.setEndTime(organizerEvent.endDateTime());
        event.setLocation(organizerEvent.location());
        event.setComment(organizerEvent.comments().join(";"));
        QStringList attendees;
        foreach (const QOrganizerItemDetail &attendeeDetail, organizerEvent.details(QOrganizerItemDetail::TypeEventAttendee)) {
            attendees.append(attendeeDetail.value(QOrganizerItemDetail::TypeEventAttendee + 1).toString());
        }
        event.setGuests(attendees);
        event.setRecurring(organizerEvent.recurrenceRules().count() > 0);

        items.append(event);


        long startTimestamp = QDateTime::currentMSecsSinceEpoch();
        startTimestamp -= 1000 * 60 * 60 * 24 * 7;

        foreach (const QOrganizerItem &occurranceItem, m_manager->itemOccurrences(item, QDateTime::fromMSecsSinceEpoch(startTimestamp), QDateTime::currentDateTime().addDays(7))) {
            QOrganizerEventOccurrence organizerOccurrance(occurranceItem);
            event.setStartTime(organizerOccurrance.startDateTime());
            event.setEndTime(organizerOccurrance.endDateTime());
            items.append(event);
        }
    }
    if (m_items != items) {
        m_items = items;
        emit itemsChanged(m_items);
    }
}

