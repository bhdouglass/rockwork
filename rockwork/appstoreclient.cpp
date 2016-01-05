#include "appstoreclient.h"
#include "applicationsmodel.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonParseError>

#include <libintl.h>

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

int AppStoreClient::limit() const
{
    return m_limit;
}

void AppStoreClient::setLimit(int limit)
{
    m_limit = limit;
    emit limitChanged();
}

bool AppStoreClient::busy() const
{
    return m_busy;
}

void AppStoreClient::fetchHome(Type type)
{
    m_model->clear();
    setBusy(true);

    QUrlQuery query;
    query.addQueryItem("firmware_version", "3");

    QString url;
    if (type == TypeWatchapp) {
        url = "https://api2.getpebble.com/v2/home/apps";
    } else {
        url = "https://api2.getpebble.com/v2/home/watchfaces";
    }
    QUrl storeUrl(url);
    storeUrl.setQuery(query);
    QNetworkRequest request(storeUrl);

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QVariantMap resultMap = jsonDoc.toVariant().toMap();

        QHash<QString, QStringList> collections;
        foreach (const QVariant &entry, resultMap.value("collections").toList()) {
            QStringList appIds;
            foreach (const QVariant &appId, entry.toMap().value("application_ids").toList()) {
                appIds << appId.toString();
            }
            QString slug = entry.toMap().value("slug").toString();
            collections[slug] = appIds;
            m_model->insertGroup(slug, entry.toMap().value("name").toString(), entry.toMap().value("links").toMap().value("apps").toString());
        }

        QHash<QString, QString> categoryNames;
        foreach (const QVariant &entry, resultMap.value("categories").toList()) {
            categoryNames[entry.toMap().value("id").toString()] = entry.toMap().value("name").toString();
        }

        foreach (const QVariant &entry, jsonDoc.toVariant().toMap().value("applications").toList()) {
            AppItem* item = parseAppItem(entry.toMap());
            foreach (const QString &collection, collections.keys()) {
                if (collections.value(collection).contains(item->storeId())) {
                    item->setGroupId(collection);
                    break;
                }
            }
            item->setCategory(categoryNames.value(entry.toMap().value("category_id").toString()));

            qDebug() << "have entry" << item->name() << item->groupId();

            if (item->groupId().isEmpty()) {
                // Skip items that we couldn't match to a collection
                delete item;
                continue;
            }
            m_model->insert(item);
            setBusy(false);
        }
    });


}

void AppStoreClient::fetchLink(const QString &link)
{
    m_model->clear();
    setBusy(true);

    QUrl storeUrl(link);
    QUrlQuery query(storeUrl);
    query.removeQueryItem("limit");
    // We fetch one more than we actually want so we can see if we need to display
    // a next button
    query.addQueryItem("limit", QString::number(m_limit + 1));
    int currentOffset = query.queryItemValue("offset").toInt();
    query.removeQueryItem("offset");
    query.addQueryItem("offset", QString::number(qMax(0, currentOffset - 1)));
    storeUrl.setQuery(query);
    QNetworkRequest request(storeUrl);
    qDebug() << "fetching link" << request.url();

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QVariantMap resultMap = jsonDoc.toVariant().toMap();

        bool haveMore = false;
        foreach (const QVariant &entry, resultMap.value("data").toList()) {
            if (model()->rowCount() >= m_limit) {
                haveMore = true;
                break;
            }
            AppItem *item = parseAppItem(entry.toMap());
            m_model->insert(item);
        }

        if (resultMap.contains("links") && resultMap.value("links").toMap().contains("nextPage") &&
                !resultMap.value("links").toMap().value("nextPage").isNull()) {
            int currentOffset = resultMap.value("offset").toInt();
            QString nextLink = resultMap.value("links").toMap().value("nextPage").toString();

            if (currentOffset > 0) {
                QUrl previousLink(nextLink);
                QUrlQuery query(previousLink);
                query.removeQueryItem("limit");
                query.addQueryItem("limit", QString::number(m_limit + 1));
                query.removeQueryItem("offset");
                query.addQueryItem("offset", QString::number(qMax(0, currentOffset - m_limit + 1)));
                previousLink.setQuery(query);
                m_model->addLink(previousLink.toString(), gettext("Previous"));
            }
            if (haveMore) {
                m_model->addLink(nextLink, gettext("Next"));
            }
        }
        setBusy(false);
    });

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

void AppStoreClient::fetchAppDetails(const QString &appId)
{
    QNetworkRequest request(QUrl("https://api2.getpebble.com/v2/apps/id/" + appId));
    QNetworkReply * reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply, appId]() {
        reply->deleteLater();
        AppItem *item = m_model->findByStoreId(appId);
        if (!item) {
            qWarning() << "Can't find item with id" << appId;
            return;
        }
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QVariantMap replyMap = jsonDoc.toVariant().toMap().value("data").toList().first().toMap();
        if (replyMap.contains("header_images") && replyMap.value("header_images").toList().count() > 0) {
            item->setHeaderImage(replyMap.value("header_images").toList().first().toMap().value("orig").toString());
        }
        item->setVendor(replyMap.value("author").toString());
        item->setVersion(replyMap.value("latest_release").toMap().value("version").toString());
        item->setIsWatchFace(replyMap.value("type").toString() == "watchface");
    });
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
        item->setStoreId(entry.toMap().value("id").toString());
        item->setName(entry.toMap().value("title").toString());
        item->setIcon(entry.toMap().value("list_image").toMap().value("144x144").toString());
        m_model->insert(item);
    }
}

AppItem* AppStoreClient::parseAppItem(const QVariantMap &map)
{
    AppItem *item = new AppItem();
    item->setStoreId(map.value("id").toString());
    item->setName(map.value("title").toString());
    item->setIcon(map.value("list_image").toMap().value("144x144").toString());
    item->setDescription(map.value("description").toString());
    item->setHearts(map.value("hearts").toInt());
    item->setCategory(map.value("category_name").toString());
    QStringList screenshotImages;
    foreach (const QVariant &screenshotItem, map.value("screenshot_images").toList()) {
        screenshotImages << screenshotItem.toMap().first().toString();
    }
    item->setScreenshotImages(screenshotImages);

    return item;
}

void AppStoreClient::setBusy(bool busy)
{
    m_busy = busy;
    emit busyChanged();
}

