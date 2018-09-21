#include "ipfsapi.h"
#include <QJsonDocument>
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
    maxRepoSize_ = 256;

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

void IPFSApi::setMaxRepoSize(int size)
{
    maxRepoSize_ = size;
}

int IPFSApi::maxRepoSize()
{
    return maxRepoSize_;
}

char *IPFSApi::QStringToChar(QString str)
{
    QByteArray ba = str.toLatin1();
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
