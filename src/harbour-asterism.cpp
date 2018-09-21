#include <QGuiApplication>
#include <sailfishapp.h>
#include <ipfsapi.h>

#include <QtDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("harbour-asterism");
    QCoreApplication::setOrganizationName("harbour-asterism");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();

    IPFSApi ipfsapi;

    view->rootContext()->setContextProperty("ipfsapi", &ipfsapi);
    view->setSource(SailfishApp::pathTo("qml/harbour-asterism.qml"));
    view->showFullScreen();

    app->exec();
}
