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

    setMaxRepoSize(0);
    repoPath_ = repoRoot;
    running_ = false;
    starting_ = false;
    listingFiles_ = false;
    currentPath_ = "/";
    addingFiles_ = false;
    listingConns_ = false;
    gcRunning_ = false;
    mode_ = "";
    fileMap_.insert("/", QVariantList());

    register_callback_class_instance(this);

    QDir dir(repoRoot);
    if (!dir.exists()) {
        setMaxRepoSize(512);
        dir.mkpath(repoPath_);
        QTimer::singleShot(50, this, SIGNAL(firstUse()));
    } else {
        start();
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

    QByteArray ba1;
    QByteArray ba2;
    char* size = NULL;
    char* mode = NULL;

    if (maxRepoSize_ != 0) {
        ba1 = maxRepoSizeString_.toLocal8Bit();
        size = ba1.data();
    }

    if (mode_ != "") {
        ba2 = mode_.toLocal8Bit();
        mode = ba2.data();
    }

    qDebug() << size << mode;

    ipfs_start(
        QStringToChar(repoPath_),
        size,
        mode,
        (void*)&IPFSApi::callback
    );
}

void IPFSApi::restart()
{
    stop();
    QTimer::singleShot(200, this, SLOT(startSlot()));
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

bool IPFSApi::add(QString path)
{
    // multiple concurrent adds not working
    if (addingFiles_ || !running_) {
        return false;
    }

    addingFiles_ = true;
    emit isAddingFilesChanged();

    qDebug() << QString("IPFS add: %1").arg(path);
    char* cpath = QStringToChar(path);
    ipfs_add_path_or_file(cpath, 0, (void*)&IPFSApi::callback);

    return true;
}

void IPFSApi::add_bytes(QByteArray data)
{
    if (!running_) {
        return;
    }

    qDebug() << QString("IPFS add bytes");
    char* cdata = data.data();
    ipfs_add_bytes(cdata, 0, (void*)&IPFSApi::callback);
}

void IPFSApi::cat(QString path)
{
    if (!running_) {
        return;
    }
    qDebug() << QString("IPFS cat: %1").arg(path);
    char* cpath = QStringToChar(path);
    ipfs_cat(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::ls(QString path)
{
    if (!running_) {
        return;
    }
    qDebug() << QString("IPFS ls: %1").arg(path);
    char* cpath = QStringToChar(path);
    ipfs_ls(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::unpin(QString cid)
{
    if (!running_) {
        return;
    }
    qDebug() << QString("IPFS unpin: %1").arg(cid);
    char* ccid = QStringToChar(cid);
    ipfs_unpin(ccid, (void*)&IPFSApi::callback);
}

void IPFSApi::conns()
{
    if (!running_ || listingConns_) {
        return;
    }
    listingConns_ = true;
    qDebug() << QString("IPFS conns");
    ipfs_peers((void*)&IPFSApi::callback);
}

void IPFSApi::id(QString cid)
{
    if (!running_) {
        return;
    }
    qDebug() << QString("IPFS id: %1").arg(cid);
    char* ccid = QStringToChar(cid);
    ipfs_id(ccid, (void*)&IPFSApi::callback);
}

void IPFSApi::gc()
{
    if (!running_) {
        return;
    }

    gcRunning_ = true;
    emit gcChanged();

    qDebug() << QString("IPFS garbage collect");
    ipfs_gc((void*)&IPFSApi::callback);
}

void IPFSApi::repostats()
{
    if (!running_) {
        return;
    }
    qDebug() << QString("IPFS repo stats");
    ipfs_repo_stats((void*)&IPFSApi::callback);
}

void IPFSApi::repoconfig()
{
    if (!running_) {
        return;
    }
    qDebug() << QString("IPFS config");
    ipfs_config((void*)&IPFSApi::callback);
}

void IPFSApi::files_ls()
{
    if (!running_ || listingFiles_) {
        return;
    }

    listingFiles_ = true;
    qDebug() << QString("IPFS files ls");
    char* cpath = QStringToChar(currentPath_);
    ipfs_files_ls(cpath, (void*)&IPFSApi::callback);
}

void IPFSApi::files_mkdir(QString dir)
{
    if (!running_) {
        return;
    }
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

bool IPFSApi::isAddingFiles()
{
    return addingFiles_;
}

bool IPFSApi::isGcRunning()
{
    return gcRunning_;
}

void IPFSApi::setMaxRepoSize(int size)
{
    maxRepoSize_ = size;
    if (size == 0) {
        maxRepoSizeString_ = "";
    } else {
        maxRepoSizeString_ = QString(QString::number(size) + QString("MB"));
    }
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
        qDebug() << parts;
        parts.removeLast();
        parts.removeLast();
        currentPath_ = parts.join("/");
        qDebug() << currentPath_;
        if (currentPath_ == "") {
            currentPath_ = "/";
        } else {
            currentPath_.append("/");
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

void IPFSApi::setMode(QString mode)
{
    mode_ = mode;
    emit modeChanged();
}

QString IPFSApi::mode()
{
    return mode_;
}

QVariantList IPFSApi::files()
{
    return fileMap_[currentPath_];
}

QString IPFSApi::currentPath()
{
    return currentPath_;
}

QVariantList IPFSApi::nodeConns()
{
    return connList_;
}

void IPFSApi::startSlot()
{
    start();
}

char *IPFSApi::QStringToChar(QString str)
{
    if (str.isEmpty()) {
        return NULL;
    }
    QByteArray ba = str.toLocal8Bit();
    return ba.data();
}

void IPFSApi::handleStats(char *data, size_t size)
{
    if (data == NULL) {
        qDebug() << "null data received";
        return;
    }

    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument stats = QJsonDocument::fromJson(ba, &e);
    QJsonObject s = stats.object();
    stats_ = s.toVariantMap();

    QString repoMax = stats_.value("StorageMax").toString().replace("MB", "");

    bool ok;
    int intSize = repoMax.toInt(&ok);
    if(ok) {
        setMaxRepoSize(intSize / (1000 * 1000));
        emit repoSizeChanged();
    }

    emit statsChanged();
}

void IPFSApi::handleConfig(char *data, size_t size)
{
    if (data == NULL) {
        qDebug() << "null data received";
        return;
    }

    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument config = QJsonDocument::fromJson(ba, &e);

    QJsonObject s = config.object();
    QJsonObject routing = s.value("Routing").toObject();

    config_ = s.toVariantMap();
    mode_ = routing.value("Type").toString();

    emit configChanged();
    emit modeChanged();
}

void IPFSApi::handleConns(char *data, size_t size)
{
    if (data == NULL) {
        qDebug() << "null data received";
        return;
    }

    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument conns = QJsonDocument::fromJson(ba, &e);
    QJsonArray arr = conns.array();

    connList_ = arr.toVariantList();
    emit connsChanged();

    listingConns_ = false;
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
    if (data == NULL) {
        qDebug() << "null data received";
        return;
    }

    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument files = QJsonDocument::fromJson(ba, &e);
    QJsonArray farr = files.array();

    fileMap_[currentPath_] = farr.toVariantList();
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
    if (data == NULL) {
        qDebug() << "null data received";
        return;
    }

    QByteArray ba = QByteArray::fromRawData(data, size);
    QJsonParseError e;
    QJsonDocument doc = QJsonDocument::fromJson(ba, &e);
    QJsonObject obj = doc.object();

    QFileInfo path = QFileInfo(obj.value("Path").toString());
    QString dest;

    if (path.isDir()) {
        dest = currentPath_ + path.absoluteDir().dirName();
    } else {
        dest = currentPath_ + path.fileName();
    }

    addingFiles_ = false;
    emit isAddingFilesChanged();

    char* ipfsPath = QStringToChar("/ipfs/" + obj.value("Cid").toString());
    char* destPath = QStringToChar(dest);

    ipfs_files_cp(ipfsPath, destPath, (void*)&IPFSApi::callback);
}

void IPFSApi::handleFilesCp(char *data, size_t size)
{
    files_ls();
}

void IPFSApi::handleGc(char *data, size_t size)
{
    gcRunning_ = false;
    emit gcChanged();
}
