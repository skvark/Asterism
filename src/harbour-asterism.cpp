#include "service.h"

#include <QGuiApplication>
#include <sailfishapp.h>

#include <QtDebug>
#include <QFile>
#include <QTextStream>

void messageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;

    switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(msg);
        break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(msg);
        break;
        case QtFatalMsg:
            txt = QString("Fatal: %1").arg(msg);
        break;
    }

    QFile outFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("harbour-asterism");
    QCoreApplication::setOrganizationName("harbour-asterism");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    qInstallMessageHandler(messageHandler);

    if (app->arguments().last() == QStringLiteral("activate")) {
        if (!Service::isRegistered()) {
            app->setQuitOnLastWindowClosed(false);

            Service service;
            service.initialize();
            service.showUi();

            return app->exec();
        } else {
            return 1;
        }
    } else {
        if (Service::raise()) {
            return 0;
        } else {
            return 1;
        }
    }
}
