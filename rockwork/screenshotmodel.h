#ifndef SCREENSHOTMODEL_H
#define SCREENSHOTMODEL_H

#include <QAbstractListModel>

class ScreenshotModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path CONSTANT)

public:
    enum Role {
        RoleFileName,
        RoleAbsoluteFileName
    };

    ScreenshotModel(QObject *parent = nullptr);
    QString path() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void deleteFile(const QString &filename);
private:
    QString m_path;
    QStringList m_files;
};

#endif // SCREENSHOTMODEL_H
