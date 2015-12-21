#include "applicationsfiltermodel.h"
#include "applicationsmodel.h"

ApplicationsFilterModel::ApplicationsFilterModel(QObject *parent):
    QSortFilterProxyModel(parent)
{

}

ApplicationsModel *ApplicationsFilterModel::appsModel() const
{
    return m_appsModel;
}

void ApplicationsFilterModel::setAppsModel(ApplicationsModel *model)
{
    if (m_appsModel != model) {
        m_appsModel = model;
        setSourceModel(m_appsModel);
        emit appsModelChanged();
    }
}

bool ApplicationsFilterModel::showWatchApps() const
{
    return m_showWatchApps;
}

void ApplicationsFilterModel::setShowWatchApps(bool showWatchApps)
{
    if (m_showWatchApps != showWatchApps) {
        m_showWatchApps = showWatchApps;
        emit showWatchAppsChanged();
        invalidateFilter();
    }
}

bool ApplicationsFilterModel::showWatchFaces() const
{
    return m_showWatchFaces;
}

void ApplicationsFilterModel::setShowWatchFaces(bool showWatchFaces)
{
    if (m_showWatchFaces != showWatchFaces) {
        m_showWatchFaces = showWatchFaces;
        emit showWatchFacesChanged();
        invalidateFilter();
    }
}

bool ApplicationsFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    AppItem *item = m_appsModel->get(source_row);
    if (m_showWatchApps && !item->isWatchFace()) {
        return true;
    }
    if (m_showWatchFaces && item->isWatchFace()) {
        return true;
    }
    return false;
}
