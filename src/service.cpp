#include "service.h"

#include <sailfishapp.h>
#include <QQuickView>
#include <QDBusInterface>
#include <QDBusConnectionInterface>

#include <QDebug>

static const QString c_dbusServiceName = QStringLiteral("harbour.asterism");
static const QString c_dbusObjectPath = QStringLiteral("/service");
static const QString c_dbusInterface = QStringLiteral("harbour.asterism");
static const QString c_dbusShowMethod = QStringLiteral("showUi");
static const QString c_dbusExitMethod = QStringLiteral("exit");

Service::Service(QObject *parent)
    : QObject(parent)
{
}

void Service::initialize()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();

    dbus.registerObject(c_dbusObjectPath, this, QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);
    dbus.registerService(c_dbusServiceName);
}

bool Service::isRegistered()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(c_dbusServiceName);
}

bool Service::raise()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    QDBusMessage showUi = QDBusMessage::createMethodCall(
                c_dbusServiceName,
                c_dbusObjectPath,
                c_dbusInterface,
                c_dbusShowMethod);
    return dbus.send(showUi);
}

void Service::showUi()
{
    if (m_view && m_ipfs_api) {
        return;
    }

    if (!m_ipfs_api) {
        QString repoRoot = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/ipfs";

        QDir dir(repoRoot);
        if (!dir.exists()) {
            dir.mkpath(".");
            QTimer::singleShot(200, this, SIGNAL(firstUse()));
        }

        m_ipfs_api = new IPFSApi(repoRoot);
    }

    if (!m_view) {

        m_view = SailfishApp::createView();

        m_view->rootContext()->setContextProperty("service", this);

        if (m_ipfs_api) {
            m_view->rootContext()->setContextProperty("ipfsapi", m_ipfs_api);
            qDebug() << "Context property set.";
        }

        m_view->setSource(SailfishApp::pathTo("qml/harbour-asterism.qml"));
        m_view->showFullScreen();

        connect(m_view, &QQuickView::destroyed, this, &Service::onUiDestroyed);
        connect(m_view, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onUiClosing()));
    }
}

void Service::onUiDestroyed()
{
    disconnect(m_view, 0, 0, 0);
    m_view = nullptr;
}

void Service::onUiClosing()
{
    m_view->destroy();
    m_view->deleteLater();
}

void Service::closeApp() {
    if (m_ipfs_api->isRunning()) {
        m_ipfs_api->stop();
    }

    delete m_ipfs_api;
    m_ipfs_api = nullptr;
    QTimer::singleShot(100, qGuiApp, SLOT(quit()));
}


