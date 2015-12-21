#ifndef APPLICATIONSFILTERMODEL_H
#define APPLICATIONSFILTERMODEL_H

#include <QSortFilterProxyModel>

class ApplicationsModel;

class ApplicationsFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(ApplicationsModel* model READ appsModel WRITE setAppsModel NOTIFY appsModelChanged)
    Q_PROPERTY(bool showWatchApps READ showWatchApps WRITE setShowWatchApps NOTIFY showWatchAppsChanged)
    Q_PROPERTY(bool showWatchFaces READ showWatchFaces WRITE setShowWatchFaces NOTIFY showWatchFacesChanged)

public:
    ApplicationsFilterModel(QObject *parent = nullptr);

    ApplicationsModel *appsModel() const;
    void setAppsModel(ApplicationsModel *model);

    bool showWatchApps() const;
    void setShowWatchApps(bool showWatchApps);

    bool showWatchFaces() const;
    void setShowWatchFaces(bool showWatchFaces);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

signals:
    void appsModelChanged();
    void showWatchAppsChanged();
    void showWatchFacesChanged();

public slots:

private:
    ApplicationsModel *m_appsModel;

    bool m_showWatchApps = true;
    bool m_showWatchFaces = true;
};

#endif // APPLICATIONSFILTERMODEL_H
