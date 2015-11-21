#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H

#include "watchconnection.h"

#include <QObject>

class Pebble;
class WatchConnection;
class Upload;

class FileUploader : public QObject
{
    Q_OBJECT
public:
    enum UploadType {
        UploadTypeFirmware = 1,
        UploadTypeRecovery = 2,
        UploadTypeSystemResources = 3,
        UploadTypeResources = 4,
        UploadTypeBinary = 5,
        UploadTypeFile = 6,
        UploadTypeWorker = 7
    };
    enum PutBytesCommand {
        PutBytesCommandInit = 1,
        PutBytesCommandSend = 2,
        PutBytesCommandCommit = 3,
        PutBytesCommandAbort = 4,
        PutBytesCommandComplete = 5
    };

    explicit FileUploader(Pebble *pebble, WatchConnection *connection);

signals:

public slots:
    void uploadFile(int slot, UploadType type, const QString &filename);

private:
    void processUploadQueue();
    void processUploadChunk(const QByteArray &reply);

    Pebble *m_pebble;
    WatchConnection *m_watchConnection;

    QList<Upload*> m_uploadQueue;
    Upload *m_currentUpload;

};

class Upload
{
public:
//    int id;
    FileUploader::UploadType type;
    int slot;
    QString filename;

    QFile file;
    int transferred = 0;
};

#endif // FILEUPLOADER_H
