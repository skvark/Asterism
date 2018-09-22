import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        width: parent.width
        anchors.fill: parent

        PullDownMenu {
            width: parent.width
            visible: viewContainer.currentIndex === 0
            busy: ipfsapi.isStarting

            MenuItem {
                text: "About"
                onClicked: pageStack.push("About.qml");
            }

            MenuItem {
                text: ipfsapi.isRunning ? "Stop Node" : "Start Node"
                onClicked: ipfsapi.isRunning ? ipfsapi.stop() : ipfsapi.start()
                enabled: !ipfsapi.isStarting
            }
        }

        SlideshowView {
            id: viewContainer
            width: parent.width
            itemWidth: width
            itemHeight: height
            height: page.height - navigation.height
            clip: true
            currentIndex: navigation.selected

            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }

            model: VisualItemModel {
                id: views
                InfoView { id: infoView }
                FilesView { id: filesView }
                PeersView { id: peersView }
                SettingsView { id: settingsView }
            }

            onCurrentIndexChanged: {
                navigation.selected = currentIndex
                if (currentIndex === 0 || currentIndex === 3) {
                    ipfsapi.repoconfig();
                    ipfsapi.repostats();
                } else if (currentIndex === 1) {
                    ipfsapi.setCurrentPath("/");
                    ipfsapi.files_ls()
                } else {
                    ipfsapi.conns();
                }
            }
        }
    }

    TabPanel {
        id: navigation
        width: parent.width
        anchors.bottom: parent.bottom
    }

    Connections {
        target: ipfsapi
        onFirstUse: {
            var dialog = pageStack.push("WarningDialog.qml");
            dialog.accepted.connect(function() {
                ipfsapi.start();
            })
        }
    }
}
