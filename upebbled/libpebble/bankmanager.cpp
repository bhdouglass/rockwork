#include <QFile>
#include <QDir>
#include "watchdatareader.h"
#include "watchdatawriter.h"
#include "bankmanager.h"
#include "watchconnection.h"
#include "uploadmanager.h"
#include "appmanager.h"

#if 0
// TODO -- This is how language files seems to be installed.
if (slot == -4) {
    qCDebug(l) << "starting lang install";
    QFile *pbl = new QFile(QDir::home().absoluteFilePath("es.pbl"));
    if (!pbl->open(QIODevice::ReadOnly)) {
        qCWarning(l) << "Failed to open pbl";
        return false;
    }

    upload->uploadFile("lang", pbl, [this]() {
        qCDebug(l) << "success";
    }, [this](int code) {
        qCWarning(l) << "Some error" << code;
    });

    return true;
}
#endif

BankManager::BankManager(WatchConnection *connection, AppManager *apps, QObject *parent) :
    QObject(parent),
    m_connection(connection), apps(apps), _refresh(new QTimer(this))
{
    connect(m_connection, &WatchConnection::watchConnected,
            this, &BankManager::handleWatchConnected);

    m_connection->registerEndpointHandler(WatchConnection::EndpointAppManager, this, "handleAppManagerMessage");
    m_connection->registerEndpointHandler(WatchConnection::EndpointZero, this, "handleAppManagerMessage");

    _refresh->setInterval(5000);
    _refresh->setSingleShot(true);
    connect(_refresh, &QTimer::timeout,
            this, &BankManager::refresh);
}

int BankManager::numSlots() const
{
    return _slots.size();
}

bool BankManager::isUsed(int slot) const
{
    return _slots.at(slot).used;
}

QUuid BankManager::appAt(int slot) const
{
    return _slots.at(slot).uuid;
}

bool BankManager::uploadApp(const QUuid &uuid, int slot)
{
    AppInfo info = apps->info(uuid);
    if (info.uuid() != uuid) {
        qWarning() << "uuid" << uuid << "is not installed";
        return false;
    }
    if (slot == -1) {
        slot = findUnusedSlot();
        if (slot == -1) {
            qWarning() << "no free slots!";
            return false;
        }
    }
    if (slot < 0 || slot > _slots.size()) {
        qWarning() << "invalid slot index";
        return false;
    }
    if (_slots[slot].used) {
        qWarning() << "slot in use";
        return false;
    }

    qDebug() << "about to install app" << info.shortName() << "into slot" << slot;

    QSharedPointer<QIODevice> binaryFile(info.openFile(AppInfo::BINARY));
    if (!binaryFile) {
        qWarning() << "failed to open" << info.shortName() << "AppInfo::BINARY";
        return false;
    }

    qDebug() << "binary file size is" << binaryFile->size();

    // Mark the slot as used, but without any app, just in case.
    _slots[slot].used = true;
    _slots[slot].name.clear();
    _slots[slot].uuid = QUuid();

    m_connection->uploadManager()->uploadAppBinary(slot, binaryFile.data(), info.crcFile(AppInfo::BINARY),
    [this, info, binaryFile, slot]() {
        qDebug() << "app binary upload succesful";
        binaryFile->close();

        // Proceed to upload the resource file
        QSharedPointer<QIODevice> resourceFile(info.openFile(AppInfo::RESOURCES));
        if (resourceFile) {
            m_connection->uploadManager()->uploadAppResources(slot, resourceFile.data(), info.crcFile(AppInfo::RESOURCES),
            [this, resourceFile, slot]() {
                qDebug() << "app resources upload succesful";
                resourceFile->close();
                // Upload succesful
                // Tell the watch to reload the slot
                refreshWatchApp(slot, [this]() {
                    qDebug() << "app refresh succesful";
                    _refresh->start();
                }, [this](int code) {
                    qWarning() << "app refresh failed" << code;
                    _refresh->start();
                });
            }, [this, resourceFile](int code) {
                resourceFile->close();
                qWarning() << "app resources upload failed" << code;
                _refresh->start();
            });
        } else {
            // No resource file
            // Tell the watch to reload the slot
            refreshWatchApp(slot, [this]() {
                qDebug() << "app refresh succesful";
                _refresh->start();
            }, [this](int code) {
                qWarning() << "app refresh failed" << code;
                _refresh->start();
            });
        }
    }, [this, binaryFile](int code) {
        binaryFile->close();
        qWarning() << "app binary upload failed" << code;
        _refresh->start();
    });

    return true;
}

bool BankManager::unloadApp(int slot)
{
    if (slot < 0 || slot > _slots.size()) {
        qWarning() << "invalid slot index";
        return false;
    }
    if (!_slots[slot].used) {
        qWarning() << "slot is empty";
        return false;
    }

    qDebug() << "going to unload app" << _slots[slot].name << "in slot" << slot;

    int installId = _slots[slot].id;

    QByteArray msg;
    msg.reserve(1 + 2 * sizeof(quint32));
    WatchDataWriter writer(&msg);
    writer.write<quint8>(AppManager::ActionRemoveApp);
    writer.write<quint32>(installId);
    writer.write<quint32>(slot);

    m_connection->writeToPebble(WatchConnection::EndpointAppManager, msg);
    return true; // Operation in progress
}

void BankManager::refresh()
{
    qDebug() << "refreshing bank status";

    m_connection->writeToPebble(WatchConnection::EndpointAppManager, QByteArray(1, AppManager::ActionGetAppBankStatus));

    // Test call for uploading "Tiny Bird"
//    uploadApp("52f7a04b777f7eee6d000258");
}

int BankManager::findUnusedSlot() const
{
    for (int i = 0; i < _slots.size(); ++i) {
        if (!_slots[i].used) {
            return i;
        }
    }

    return -1;
}

void BankManager::refreshWatchApp(int slot, std::function<void ()> successCallback, std::function<void (int)> errorCallback)
{
    QByteArray msg;
    WatchDataWriter writer(&msg);
    writer.write<quint8>(AppManager::ActionRefreshApp);
    writer.write<quint32>(slot);

    m_connection->writeToPebble(WatchConnection::EndpointAppManager, msg);
}

void BankManager::handleWatchConnected()
{
    if (m_connection->isConnected()) {
        _refresh->start();
    }
}

void BankManager::handleAppManagerMessage(const QByteArray &data)
{

    WatchDataReader reader(data);
    AppManager::Action action = (AppManager::Action)reader.read<quint8>();
    qDebug() << "***********************************" << data.toHex() << action;
    switch(action) {
    case AppManager::ActionRemoveApp: {
        uint result = reader.read<quint32>();
        switch (result) {
        case Success: /* Success */
            qDebug() << "sucessfully unloaded app";
            break;
        default:
            qWarning() << "could not unload app. result code:" << result;
            break;
        }
        _refresh->start();
        break;
    }
    case AppManager::ActionGetAppBankStatus: {
        if (data.size() < 9) {
            qWarning() << "invalid getAppbankStatus response";
            return;
        }
        reader.skip(sizeof(quint8));

        unsigned int num_banks = reader.read<quint32>();
        unsigned int apps_installed = reader.read<quint32>();

        qDebug() << "Bank status:" << apps_installed << "/" << num_banks;

        _slots.resize(num_banks);
        for (unsigned int i = 0; i < num_banks; i++) {
            _slots[i].used = false;
            _slots[i].id = 0;
            _slots[i].name.clear();
            _slots[i].company.clear();
            _slots[i].flags = 0;
            _slots[i].version = 0;
            _slots[i].uuid = QUuid();
        }

        for (unsigned int i = 0; i < apps_installed; i++) {
           unsigned int id = reader.read<quint32>();
           int index = reader.read<quint32>();
           QString name = reader.readFixedString(32);
           QString company = reader.readFixedString(32);
           unsigned int flags = reader.read<quint32>();
           unsigned short version = reader.read<quint16>();

           if (index < 0 || index >= _slots.size()) {
               qWarning() << "Invalid slot index" << index;
               continue;
           }

           if (reader.bad()) {
               qWarning() << "short read";
               return;
           }

           _slots[index].used = true;
           _slots[index].id = id;
           _slots[index].name = name;
           _slots[index].company = company;
           _slots[index].flags = flags;
           _slots[index].version = version;

           AppInfo info = apps->info(name);
           if (info.shortName() != name) {
               info = AppInfo::fromSlot(_slots[index]);
               apps->insertAppInfo(info);
           }
           QUuid uuid = info.uuid();
           _slots[index].uuid = uuid;

           qDebug() << index << id << name << company << flags << version << uuid;
        }

        emit this->slotsChanged();
        break;
    }
    case AppManager::ActionRefreshApp: {
        int code = reader.read<quint32>();
        if (code == Success) {
            qDebug() << "Successfully removed app";
        } else {
            qDebug() << "Error removing app";
        }
        break;
    }

    }
}

#if 0
void BankManager::getAppbankUuids(const function<void(const QList<QUuid> &)>& callback)
{
    watch->sendMessage(WatchConnector::watchAPP_MANAGER,
                       QByteArray(1, WatchConnector::appmgrGET_APPBANK_UUIDS),
                [this, callback](const QByteArray &data) {
        if (data.at(0) != WatchConnector::appmgrGET_APPBANK_UUIDS) {
            return false;
        }
        qCDebug(l) << "getAppbankUuids response" << data.toHex();

        if (data.size() < 5) {
            qCWarning(l) << "invalid getAppbankUuids response";
            return true;
        }

        Unpacker u(data);

        u.skip(sizeof(quint8));

        unsigned int apps_installed = u.read<quint32>();

        qCDebug(l) << apps_installed;

        QList<QUuid> uuids;

        for (unsigned int i = 0; i < apps_installed; i++) {
           QUuid uuid = u.readUuid();

           qCDebug(l) << uuid.toString();

           if (u.bad()) {
               qCWarning(l) << "short read";
               return true;
           }

           uuids.push_back(uuid);
        }

        qCDebug(l) << "finished";

        callback(uuids);

        return true;
    });
}
#endif
