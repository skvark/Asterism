#include "ipfsapi.h"
#include <QJsonDocument>

IPFSApi::IPFSApi(QObject *parent) :
    QObject(parent)
{
}

IPFSApi::IPFSApi(QString repoPath)
{
    repoPath_ = repoPath;
    running_ = false;
    starting_ = false;
    register_callback_class_instance(this);
}

void IPFSApi::start()
{
    starting_ = true;
    emit startingChanged();
    char* size = QStringToChar(QString("500MB"));
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
    qDebug() << e.errorString();
    QJsonObject s = stats.object();
    stats_ = s.toVariantMap();
    emit statsChanged();
}

void IPFSApi::handleConfig(char *data, size_t size)
{
    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument config = QJsonDocument::fromJson(ba, &e);
    qDebug() << e.errorString();
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
