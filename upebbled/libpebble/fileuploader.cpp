#include "fileuploader.h"

#include "pebble.h"
#include "watchdatareader.h"
#include "watchdatawriter.h"

FileUploader::FileUploader(Pebble *pebble, WatchConnection *connection):
    QObject(pebble),
    m_pebble(pebble),
    m_watchConnection(connection)
{

}

void FileUploader::uploadFile(int slot, UploadType type, const QString &filename)
{
    Upload *upload = new Upload();
//    upload.id = ++_lastUploadId;
    upload->type = type;
    upload->slot = slot;
    upload->filename = filename;
//    upload.device = device;
//    if (size < 0) {
//        upload.size = device->size();
//    } else {
//        upload.size = size;
//    }
//    upload.remaining = upload.size;
//    upload.crc = crc;
//    upload.successCallback = successCallback;
//    upload.errorCallback = errorCallback;
//    upload.progressCallback = progressCallback;

//    if (upload.remaining <= 0) {
//        qCWarning(l) << "upload is empty";
//        if (errorCallback) {
//            errorCallback(-1);
//            return -1;
//        }
//    }

//    _pending.enqueue(upload);

//    if (_pending.size() == 1) {
//        startNextUpload();
//    }

//    return upload.id;

    m_uploadQueue.append(upload);
    processUploadQueue();

}

void FileUploader::processUploadQueue()
{
    if (m_uploadQueue.isEmpty() || m_currentUpload) {
        return;
    }
    m_currentUpload = m_uploadQueue.takeFirst();
    m_currentUpload->file.setFileName(m_currentUpload->filename);
    if (!m_currentUpload->file.open(QFile::ReadOnly)) {
        qWarning() << "could not open file for reading" << m_currentUpload->filename;
        delete m_currentUpload;
        m_currentUpload = nullptr;
        processUploadQueue();
        return;
    }

    QByteArray msg;
    WatchDataWriter wd(&msg);
    wd.write<quint8>(PutBytesCommandInit);
    wd.write<quint32>(m_currentUpload->file.size());
    wd.write<quint8>(m_currentUpload->type);
    wd.write<quint8>(m_currentUpload->slot);
    if (!m_currentUpload->filename.isEmpty()) {
        wd.writeCString(m_currentUpload->filename);
    }

//    qCDebug(l).nospace() << "starting new upload " << upload.id
//                         << ", size:" << upload.remaining
//                         << ", type:" << upload.type
//                         << ", slot:" << upload.index
//                         << ", crc:" << qPrintable(QString("0x%1").arg(upload.crc, 0, 16));

//    _state = StateWaitForToken;
    m_watchConnection->writeData(WatchConnection::EndpointPutBytes, msg);

}

void FileUploader::processUploadChunk(const QByteArray &reply)
{
    WatchDataReader reader(reply);

    int status = reader.read<quint8>();
    //    qDebug() << ""
}
