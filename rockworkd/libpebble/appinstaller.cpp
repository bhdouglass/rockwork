#include "appinstaller.h"
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

AppInstaller::AppInstaller(QObject *parent) : QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
    m_storagePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/apps/";

}

void AppInstaller::installPackage()
{
    downloadApp("https://www.filepicker.io/api/file/lm9hUnL9Txi6SsoFIEQw");
}

void AppInstaller::removePackage(int slot)
{
    if (slot < 0 || slot >= m_totalSlots) {
        qWarning().nospace() << "Invalid slot " << slot<< ". Have only " << m_totalSlots << " slots.";
        return;
    }
    if (!m_slots[slot].valid) {
        qWarning() << "No app appears to be installed in slot" << slot;
        return;
    }

    int id = m_slots[slot].id;

    QByteArray msg;
    msg.reserve(1 + 2 * sizeof(quint32));
    WatchDataWriter wd(&msg);
    wd.write<quint8>(AppManagerOperationRemoveApp);
    wd.write<quint32>(id);
    wd.write<quint32>(slot);

//    WatchConnection::instance()->writeData(WatchConnection::EndpointAppManager, msg, this, "removeAppFinished");
}

void AppInstaller::downloadApp(const QString &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, &AppInstaller::downloadFinished);
}

void AppInstaller::unpackArchive(const QString &hash)
{

    QString zipName = m_storagePath + hash + "/" + hash + ".zip";

    qDebug() << "should unpack archive" << zipName;

//    QuaZipFile *manifestFile = new QuaZipFile(zipName, "manifest.json");
//    qDebug() << "created manifest";
//    if (!manifestFile->open(QFile::ReadOnly)) {
//        qDebug() << "error opening manifest file";
//        return;
//    }
//    qDebug() << "opened zipfile";
//    QByteArray manifestData = manifestFile->readAll();
//    qDebug() << "have manifest" << manifestData;

    QuaZip zipFile(zipName);
    zipFile.setZipName(m_storagePath + hash + "/" + hash + ".zip");
    qDebug() << "created zip file";
    if (!zipFile.open(QuaZip::mdUnzip)) {
        qWarning() << "Failed to open zip file" << zipFile.getZipName();
        return;
    }
    qDebug() << "here!";
    foreach (const QuaZipFileInfo &fi, zipFile.getFileInfoList()) {
        qDebug() << "archive has file:" << fi.name;

        QuaZipFile f(zipName, fi.name);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "could not extract file" << fi.name;
            continue;
        }
        qDebug() << "Inflating:" << fi.name;
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

    uploadApp(hash);
}

void AppInstaller::uploadApp(const QString &hash)
{
    int slot = -1;
    for (int i = 0; i < m_totalSlots; i++) {
        if (!m_slots[i].valid) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        qDebug() << "No empty slot found. Cannot upload app to watch.";
        return;
    }

    QFile manifestFile(m_storagePath + hash + "/manifest.json");
    if (!manifestFile.open(QFile::ReadOnly)) {
        qDebug() << "Cannot read manifest file";
        return;
    }
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(manifestFile.readAll(), &parseError);
    manifestFile.close();
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Cannot parse manifest file";
        return;
    }
    qDebug() << "have json" << jsonDoc.toJson(QJsonDocument::Indented);

    QVariantMap manifest = jsonDoc.toVariant().toMap();
    QString binaryPath = m_storagePath + hash + "/" + manifest.value("application").toMap().value("name").toString();

//    WatchConnection::instance()->uploadFile(slot, WatchConnection::UploadTypeBinary, binaryPath);
//    QFile binaryFile(binaryPath);
//    if (!binaryFile.open(QFile::ReadOnly)) {
//        qDebug() << "Cannot open binary file for reading.";
//        return;
//    }

}

void AppInstaller::downloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    qDebug() << "downloaded" << reply->request().url().fileName();

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

void AppInstaller::refreshReply(const QByteArray &replyData)
{
    WatchDataReader wd(replyData);

    qDebug() << "refreshReply" << replyData.toHex();
    if (wd.read<quint8>() != AppManagerOperationGetAppBankStatus) {
        return;
    }

    m_totalSlots = wd.read<quint32>();
    int usedSlots = wd.read<quint32>();

    qDebug() << "have slots" << m_totalSlots << "used" << usedSlots;

    for (int i = 0; i < m_totalSlots; i++) {
        m_slots[i] = AppInfo();
    }

    for (int i = 0; i < usedSlots; i++) {
        int id = wd.read<quint32>();
        int index = wd.read<quint32>();
        m_slots[index].id = id;
        m_slots[index].name = wd.readFixedString(32);
        m_slots[index].company = wd.readFixedString(32);
        m_slots[index].flags = wd.read<quint32>();
        m_slots[index].version = wd.read<quint16>();
        m_slots[index].valid = true;
        qDebug() << "Have installed app: index:" << index << "id:" << m_slots[index].id << m_slots[index].name << m_slots[index].company << m_slots[index].version;
    }

    installPackage();
}

void AppInstaller::removeAppFinished(const QByteArray &replyData)
{
    qDebug() << "App removed.";
    refreshWatchStatus();
}

void AppInstaller::refreshWatchStatus()
{
    QByteArray data(1, AppManagerOperationGetAppBankStatus);
//    WatchConnection::instance()->writeData(WatchConnection::EndpointAppManager, data, this, "refreshReply");
}
