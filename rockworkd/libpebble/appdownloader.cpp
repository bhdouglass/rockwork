#include "appdownloader.h"
#include "watchconnection.h"
#include "watchdatareader.h"
#include "watchdatawriter.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QJsonDocument>
#include <quazip/quazipfile.h>
#include <quazip/quazip.h>

AppDownloader::AppDownloader(const QString &storagePath, QObject *parent) :
    QObject(parent),
    m_storagePath(storagePath + "/apps/")
{
    m_nam = new QNetworkAccessManager(this);
}

void AppDownloader::downloadApp(const QString &id)
{
    QNetworkRequest request(QUrl("https://api2.getpebble.com/v2/apps/id/" + id));
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, &AppDownloader::appJsonFetched);
}

void AppDownloader::appJsonFetched()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error fetching App Json" << reply->errorString();
        return;
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing App Json" << error.errorString();
        return;
    }

    QVariantMap map = jsonDoc.toVariant().toMap();
    if (!map.contains("data") || map.value("data").toList().length() == 0) {
        qWarning() << "Unexpected json content:" << jsonDoc.toJson();
        return;
    }
    QVariantMap appMap = map.value("data").toList().first().toMap();
    QString pbwFileUrl = appMap.value("latest_release").toMap().value("pbw_file").toString();
    if (pbwFileUrl.isEmpty()) {
        qWarning() << "pbw file url empty." << jsonDoc.toJson();
        return;
    }

    fetchPackage(pbwFileUrl);
}

void AppDownloader::fetchPackage(const QString &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, &AppDownloader::packageFetched);
}

void AppDownloader::packageFetched()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    QDir dir;
    dir.mkpath(m_storagePath + reply->request().url().fileName());
    QFile f(m_storagePath + reply->request().url().fileName() + "/" + reply->request().url().fileName() + ".zip");
    if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Error opening file for writing";
        return;
    }
    f.write(reply->readAll());
    f.flush();
    f.close();

    unpackArchive(reply->request().url().fileName());
}

void AppDownloader::unpackArchive(const QString &hash)
{
    QString zipName = m_storagePath + hash + "/" + hash + ".zip";

    QuaZip zipFile(zipName);
    zipFile.setZipName(m_storagePath + hash + "/" + hash + ".zip");
    qDebug() << "created zip file";
    if (!zipFile.open(QuaZip::mdUnzip)) {
        qWarning() << "Failed to open zip file" << zipFile.getZipName();
        return;
    }

    foreach (const QuaZipFileInfo &fi, zipFile.getFileInfoList()) {
        QuaZipFile f(zipName, fi.name);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "could not extract file" << fi.name;
            continue;
        }
        qDebug() << "Inflating:" << fi.name;
        QFileInfo dirInfo(m_storagePath + hash + "/" + fi.name);
        if (!dirInfo.absoluteDir().exists()) {
            dirInfo.absoluteDir().mkpath(dirInfo.absolutePath());
        }
        QFile of(m_storagePath + hash + "/" + fi.name);
        if (!of.open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "Could not open output file for writing" << fi.name;
            f.close();
            continue;
        }
        of.write(f.readAll());
        f.close();
        of.close();
    }

    emit downloadFinished(hash);
}