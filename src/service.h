#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <ipfsapi.h>

class QQuickView;

class Service : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "harbour.asterism")
public:
    explicit Service(QObject *parent = 0);
    void initialize();

    static bool isRegistered();
    static bool raise();
    Q_INVOKABLE void closeApp();

signals:
    void firstUse();

public slots:
    Q_SCRIPTABLE void showUi();

private slots:
    void onUiDestroyed();
    void onUiClosing();

private:
    QQuickView *m_view = nullptr;
    quint32 m_notificationId = 0;
    IPFSApi *m_ipfs_api = nullptr;
};

#endif // SERVICE_H
