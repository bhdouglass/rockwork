#ifndef SCREENSHOTMODEL_H
#define SCREENSHOTMODEL_H

#include <QAbstractListModel>

class ScreenshotModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        RoleFileName
    };

    ScreenshotModel(QObject *parent = nullptr);
    QString path() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void clear();
    void insert(const QString &filename);
    void remove(const QString &filename);

private:
    QStringList m_files;
};

#endif // SCREENSHOTMODEL_H
