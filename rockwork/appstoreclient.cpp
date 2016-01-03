#include "appstoreclient.h"
#include "applicationsmodel.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonParseError>

AppStoreClient::AppStoreClient(QObject *parent):
    QObject(parent),
    m_nam(new QNetworkAccessManager(this)),
    m_model(new ApplicationsModel(this))
{
}

ApplicationsModel *AppStoreClient::model() const
{
    return m_model;
}

bool AppStoreClient::busy() const
{
    return m_busy;
}

void AppStoreClient::fetch(Type type, const QString &hardwarePlatform, int limit, int offset)
{
    m_model->clear();

    if (limit > 100) {
        qWarning() << "Store API return items is limited to 100, can't fetch" << limit;
        limit = 100;
    }
    QUrlQuery query;
    query.addQueryItem("offset", QString::number(offset));
    query.addQueryItem("limit", QString::number(limit));
    query.addQueryItem("image_ratio", "1"); // Not sure yet what this does
    query.addQueryItem("filter_hardware", "true");
    query.addQueryItem("firmware_version", "3");
    query.addQueryItem("hardware", hardwarePlatform);
    query.addQueryItem("platform", "all");

    QString url;
    if (type == TypeWatchapp) {
        url = "https://api2.getpebble.com/v2/apps/collection/all/watchapps-and-companions";
    } else {
        url = "https://api2.getpebble.com/v2/apps/collection/all/watchfaces";
    }
    QUrl storeUrl(url);
    storeUrl.setQuery(query);

    QNetworkRequest request(storeUrl);

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, &AppStoreClient::fetched);

    m_busy = true;
    emit busyChanged();
}

void AppStoreClient::fetched()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    m_busy = false;
    emit busyChanged();

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Store reply parse error!" << error.errorString();
        return;
    }

    qDebug() << jsonDoc.toJson();

    foreach (const QVariant &entry, jsonDoc.toVariant().toMap().value("data").toList()) {
        AppItem *item = new AppItem();
        item->setId(entry.toMap().value("id").toString());
        item->setName(entry.toMap().value("title").toString());
        item->setIcon(entry.toMap().value("list_image").toMap().value("144x144").toString());
        m_model->insert(item);
    }

}

