#include "ipfsapi.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>

IPFSApi::IPFSApi(QObject *parent) :
    QObject(parent)
{
    QString repoRoot = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/ipfs";

    repoPath_ = repoRoot;
    running_ = false;
    starting_ = false;
    listingFiles_ = false;
    maxRepoSize_ = 256;
    currentPath_ = "/";
    fileMap_.insert("/", QVariantList());

    register_callback_class_instance(this);

    QDir dir(repoRoot);
    if (!dir.exists()) {
        dir.mkpath(".");
        QTimer::singleShot(50, this, SIGNAL(firstUse()));
    }
}

IPFSApi::~IPFSApi() {
    if (running_) {
        stop();
    }
}

void IPFSApi::start()
{
    starting_ = true;
    emit startingChanged();
    char* size = QStringToChar(QString::number(maxRepoSize_) + QString("MB"));
    ipfs_start(QStringToChar(repoPath_), size, (void*)&IPFSApi::callback);
}

void IPFSApi::stop()
{
    starting_ = true;
    emit startingChanged();
    qDebug() << "Stopping IPFS...";
    ipfs_stop();
    running_ = false;
    starting_ = false;
    qDebug() << "IPFS stopped.";
    emit startingChanged();
    emit runningChanged();
}

void IPFSApi::add(QString path)
{
    qDebug() << QString("IPFS add: %1").arg(path);
    char* cpath = QStringToChar(path);
    ipfs_add_path_or_file(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::add_bytes(QByteArray data)
{
    qDebug() << QString("IPFS add bytes");
    char* cdata = data.data();
    ipfs_add_bytes(cdata, 0, (void*)&IPFSApi::callback);
}

void IPFSApi::cat(QString path)
{
    qDebug() << QString("IPFS cat: %1").arg(path);
    char* cpath = QStringToChar(path);
    ipfs_cat(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::ls(QString path)
{
    qDebug() << QString("IPFS ls: %1").arg(path);
    char* cpath = QStringToChar(path);
    ipfs_ls(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::unpin(QString cid)
{
    qDebug() << QString("IPFS unpin: %1").arg(cid);
    char* ccid = QStringToChar(cid);
    ipfs_unpin(ccid, (void*)&IPFSApi::callback);
}

void IPFSApi::conns()
{
    qDebug() << QString("IPFS conns");
    ipfs_peers((void*)&IPFSApi::callback);
}

void IPFSApi::id(QString cid)
{
    qDebug() << QString("IPFS id: %1").arg(cid);
    char* ccid = QStringToChar(cid);
    ipfs_id(ccid, (void*)&IPFSApi::callback);
}

void IPFSApi::gc()
{
    qDebug() << QString("IPFS garbage collect");
    ipfs_gc((void*)&IPFSApi::callback);
}

void IPFSApi::repostats()
{
    qDebug() << QString("IPFS repo stats");
    ipfs_repo_stats((void*)&IPFSApi::callback);
}

void IPFSApi::repoconfig()
{
    qDebug() << QString("IPFS config");
    ipfs_config((void*)&IPFSApi::callback);
}

void IPFSApi::files_ls()
{
    if (!running_) {
        return;
    }

    listingFiles_ = true;
    qDebug() << QString("IPFS files ls");
    char* cpath = QStringToChar(currentPath_);
    ipfs_files_ls(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::files_mkdir(QString dir)
{
    qDebug() << QString("IPFS files mkdir");
    char* cdir = QStringToChar(currentPath_ + dir);
    ipfs_files_mkdir(cdir, 1, (void*)&IPFSApi::callback);
}

QVariantMap IPFSApi::stats()
{
    return stats_;
}

QVariantMap IPFSApi::config()
{
    return config_;
}

bool IPFSApi::isRunning()
{
    return running_;
}

bool IPFSApi::isStarting()
{
    return starting_;
}

bool IPFSApi::isListingFiles()
{
    return listingFiles_;
}

void IPFSApi::setMaxRepoSize(int size)
{
    maxRepoSize_ = size;
}

int IPFSApi::maxRepoSize()
{
    return maxRepoSize_;
}

void IPFSApi::setCurrentPath(QString path)
{
    if (path == "/") {
        currentPath_ = "/";
    } else if (path == "..") {
        QList<QString> parts = currentPath_.split("/");
        parts.removeLast();
        parts.removeLast();
        currentPath_ = parts.join("/");
        if(currentPath_ != "/") {
            currentPath_.prepend('/');
        }
    } else {
        currentPath_ = currentPath_ + path + "/";
    }

    if (!fileMap_.contains(path)) {
        fileMap_.insert(path, QVariantList());
    }
    files_ls();
    emit currentPathChanged();
}

QVariantList IPFSApi::files()
{
    return fileMap_[currentPath_];
}

QString IPFSApi::currentPath()
{
    return currentPath_;
}

char *IPFSApi::QStringToChar(QString str)
{
    QByteArray ba = str.toUtf8();
    return ba.data();
}

void IPFSApi::handleStats(char *data, size_t size)
{
    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument stats = QJsonDocument::fromJson(ba, &e);
    QJsonObject s = stats.object();
    stats_ = s.toVariantMap();

    QString repoMax = stats_.value("StorageMax").toString().replace("MB", "");

    bool ok;
    int intSize = repoMax.toInt(&ok);
    if(ok) {
        maxRepoSize_ = intSize;
    }

    emit statsChanged();
}

void IPFSApi::handleConfig(char *data, size_t size)
{
    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument config = QJsonDocument::fromJson(ba, &e);
    QJsonObject s = config.object();
    config_ = s.toVariantMap();
    emit configChanged();
}

void IPFSApi::handleStart()
{
    running_ = true;
    starting_ = false;
    emit startingChanged();
    emit runningChanged();
    repoconfig();
    repostats();
}

void IPFSApi::handleStartFail()
{
    running_ = false;
    starting_ = false;
    emit startingChanged();
    emit runningChanged();
}

void IPFSApi::handleFilesLs(char* data, size_t size)
{
    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument files = QJsonDocument::fromJson(ba, &e);
    QJsonArray farr = files.array();
    fileMap_[currentPath_] = farr.toVariantList();
    qDebug() << fileMap_[currentPath_];
    listingFiles_ = false;
    emit isListingFilesChanged();
    emit filesChanged();
}

void IPFSApi::handleFilesMkdir(char *data, size_t size)
{
    files_ls();
}

void IPFSApi::handleAdd(char *data, size_t size)
{
    char* curDir = QStringToChar(currentPath_);
    QString hash = QString::fromUtf8(data, size);
    char* ipfsPath = QStringToChar("/ipfs/" + hash);
    ipfs_files_cp(ipfsPath, curDir, (void*)&IPFSApi::callback);
}

void IPFSApi::handleFilesCp(char *data, size_t size)
{
    files_ls();
}
