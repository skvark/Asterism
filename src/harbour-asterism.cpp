#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QGuiApplication>
#include <QCoreApplication>
#include <sailfishapp.h>
#include <signal.h>
#include <ipfsapi.h>

#include <QtDebug>

void handle_pipe(int sig)
{
}

int main(int argc, char *argv[])
{
    // I think that go-ipfs tries to (maybe) sometimes write to a closed socket
    // and causes SIGPIPE -> program crashes.
    // This handler ignores the error.
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = handle_pipe;
    action.sa_flags = 0;

    QCoreApplication::setApplicationName("harbour-asterism");
    QCoreApplication::setOrganizationName("harbour-asterism");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();

    IPFSApi ipfsapi;
    sigaction(SIGPIPE, &action, NULL);
    view->rootContext()->setContextProperty("ipfsapi", &ipfsapi);
    view->setSource(SailfishApp::pathTo("qml/harbour-asterism.qml"));
    view->showFullScreen();

    app->exec();
}
