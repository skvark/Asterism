#ifndef IPFSAPI_H
#define IPFSAPI_H

#include <QString>
#include <QJsonObject>
#include <QVariantMap>
#include <QVariantList>
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
    Q_PROPERTY(bool isListingFiles READ isListingFiles NOTIFY isListingFilesChanged)
    Q_PROPERTY(int repoSize READ maxRepoSize WRITE setMaxRepoSize NOTIFY repoSizeChanged)
    Q_PROPERTY(QVariantList files READ files NOTIFY filesChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
public:
    explicit IPFSApi(QObject *parent = nullptr);
    ~IPFSApi();

    // basic commands
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void add(QString path);
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

    // files
    Q_INVOKABLE void files_ls();
    Q_INVOKABLE void files_mkdir(QString dir);

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
    bool isListingFiles();
    void setMaxRepoSize(int size);
    int maxRepoSize();
    Q_INVOKABLE void setCurrentPath(QString path);
    QVariantList files();
    QString currentPath();

    static void callback(char* error, char* data, size_t size, int method, void* instance) {

        IPFSApi* api = static_cast<IPFSApi*>(instance);

        if (method == -1) {
            QString err = api->checkError(error, size);
            qDebug() << err;
            return;
        }

        if (method == f_ipfs_start && error != NULL) {
            api->handleStartFail();
            return;
        }

        qDebug() << method << data;

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
                api->handleAdd(data, size);
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
            case f_ipfs_files_cp:
                qDebug() << "files ls done";
                api->handleFilesCp(data, size);
                break;
            case f_ipfs_files_ls:
                qDebug() << "files ls done";
                api->handleFilesLs(data, size);
                break;
            case f_ipfs_files_mkdir:
                qDebug() << "files mkdir done";
                api->handleFilesMkdir(data, size);
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
    void filesChanged();
    void isListingFilesChanged();
    void currentPathChanged();

private:
    char *QStringToChar(QString str);

    void handleStats(char* data, size_t size);
    void handleConfig(char* data, size_t size);
    void handleFilesLs(char* data, size_t size);
    void handleFilesMkdir(char* data, size_t size);
    void handleAdd(char* data, size_t size);
    void handleFilesCp(char* data, size_t size);
    void handleStart();
    void handleStartFail();

    bool running_;
    bool starting_;
    bool listingFiles_;
    int maxRepoSize_;
    QString repoPath_;
    QVariantMap stats_;
    QVariantMap config_;
    QString currentPath_;
    QMap<QString, QVariantList> fileMap_;

    static QString checkError(char* error, size_t size) {
        QString err;
        if (error == NULL) {
            err = QString("");
        } else {
            err = QString::fromUtf8(error, size);
        }
        return err;
    }
};

#endif // IPFSAPI_H
