#ifndef IPFSAPI_H
#define IPFSAPI_H

#include <QString>
#include <QJsonObject>
#include <QVariantMap>
#include <QDebug>
#include <libipfs.h>
#include <stdlib.h>

class IPFSApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap stats READ stats NOTIFY statsChanged)
    Q_PROPERTY(QVariantMap config READ config NOTIFY configChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool isStarting READ isStarting NOTIFY startingChanged)
    Q_PROPERTY(int repoSize READ maxRepoSize WRITE setMaxRepoSize NOTIFY repoSizeChanged)
public:
    explicit IPFSApi(QObject *parent = nullptr);
    ~IPFSApi();

    // basic commands
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    void add(QString path);
    void add_bytes(QByteArray data);

    void cat(QString path);
    void ls(QString path);

    // advanced commands
    void unpin(QString cid);

    // swarm
    void conns();

    // network
    void id(QString cid = "");

    // repo management
    void gc();

    Q_INVOKABLE void repostats();
    Q_INVOKABLE void repoconfig();

    // exposed to QML
    /*
     * RepoSize
     * StorageMax
     * NumObjects
     * RepoPath
     * Version
    */
    QVariantMap stats();

    QVariantMap config();

    bool isRunning();
    bool isStarting();
    void setMaxRepoSize(int size);
    int maxRepoSize();

    static void callback(char* error, char* data, size_t size, int method, void* instance) {

        IPFSApi* api = static_cast<IPFSApi*>(instance);
        bool ok = api->checkCallback(error, size, api);

        if (!ok) {
            if (method == f_ipfs_start) {
                api->handleStartFail();
            }
            return;
        }

        switch(method) {
            case f_ipfs_start:
                qDebug() << "start done";
                api->handleStart();
                break;
            case f_ipfs_add_bytes:
                qDebug() << "add bytes done";
                break;
            case f_ipfs_add_path_or_file:
                qDebug() << "add folder / file done";
                break;
            case f_ipfs_cat:
                qDebug() << "cat done";
                break;
            case f_ipfs_ls:
                qDebug() << "ls done";
                break;
            case f_ipfs_gc:
                qDebug() << "gc done";
                break;
            case f_ipfs_peers:
                qDebug() << "conns done";
                break;
            case f_ipfs_id:
                qDebug() << "id done";
                break;
            case f_ipfs_unpin:
                qDebug() << "unpin done";
                break;
            case f_ipfs_repo_stats:
                qDebug() << "stats done";
                api->handleStats(data, size);
                break;
            case f_ipfs_config:
                qDebug() << "config done";
                api->handleConfig(data, size);
                break;
            default:
                break;
        }
    }

signals:
    void firstUse();
    void ipfsError(QString error);
    void statsChanged();
    void configChanged();
    void runningChanged();
    void startingChanged();
    void repoSizeChanged();

private:
    char *QStringToChar(QString str);

    void handleStats(char* data, size_t size);
    void handleConfig(char* data, size_t size);
    void handleStart();
    void handleStartFail();

    bool running_;
    bool starting_;
    int maxRepoSize_;
    QString repoPath_;
    QVariantMap stats_;
    QVariantMap config_;

    static bool checkCallback(char* error, size_t size, IPFSApi* api) {
        QString e = checkError(error, size);
        if (!e.isEmpty()) {
            qDebug() << e;
            emit api->ipfsError(e);
            return false;
        }
        return true;
    }

    static QString checkError(char* error, size_t size) {
        QString err;
        if (err == NULL) {
            err = QString("");
        } else {
            err = QString::fromLatin1(error, size);
        }
        return err;
    }
};

#endif // IPFSAPI_H
