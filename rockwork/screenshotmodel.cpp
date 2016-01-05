#include "screenshotmodel.h"

#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFileSystemWatcher>

ScreenshotModel::ScreenshotModel(QObject *parent):
    QAbstractListModel(parent)
{
    m_path = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "/.local/share/rockworkd/screenshots/";
    QDir dir(m_path);
    qDebug() << "searching dir" << dir.absolutePath() << dir.entryList(QStringList() << "*");
    foreach (const QFileInfo &fi, dir.entryInfoList({"*.jpg"})) {
        m_files.append(fi.fileName());
    }

    QFileSystemWatcher *w = new QFileSystemWatcher(this);
    qDebug() << "watching" << w->addPath(dir.absolutePath());
    connect(w, &QFileSystemWatcher::directoryChanged, [this](const QString &path) {
        qDebug() << "path changed" << path;
        QDir dir(path);
        foreach (const QFileInfo &fi, dir.entryInfoList({"*.jpg"})) {
            if (!m_files.contains(fi.fileName())) {
                beginInsertRows(QModelIndex(), 0, 0);
                m_files.prepend(fi.fileName());
                endInsertRows();
            }
        }
        QStringList filesToRemove;
        foreach (const QString &file, m_files) {
            if (!dir.entryList({"*.jpg"}).contains(file)) {
                filesToRemove << file;
            }
        }

        while (!filesToRemove.isEmpty()) {
            beginRemoveRows(QModelIndex(), m_files.indexOf(filesToRemove.first()), m_files.indexOf(filesToRemove.first()));
            m_files.removeOne(filesToRemove.first());
            endRemoveRows();
            filesToRemove.takeFirst();
        }
    });
}

QString ScreenshotModel::path() const
{
    return m_path;
}


int ScreenshotModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_files.count();
}

QVariant ScreenshotModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case RoleFileName:
        return m_files.at(index.row());
    case RoleAbsoluteFileName:
        return m_path + m_files.at(index.row());
    }
    return QVariant();
}

QHash<int, QByteArray> ScreenshotModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(RoleFileName, "filename");
    roles.insert(RoleAbsoluteFileName, "absoluteFilename");
    return roles;
}

void ScreenshotModel::deleteFile(const QString &filename)
{
    QFile f(m_path + filename);
    qDebug() << "deleting file" << m_path + filename << f.remove();
}
