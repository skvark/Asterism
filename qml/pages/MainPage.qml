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

            MenuItem {
                text: "About"
                onClicked: pageStack.push("About.qml");
            }
            MenuItem {
                text: "Settings"
                onClicked: pageStack.push("Settings.qml");
            }
        }

        SlideshowView {
            id: viewContainer
            itemWidth: width
            itemHeight: height
            height: page.height - navigation.height
            clip: true
            currentIndex: navigation.selected

            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right
            }

            model: VisualItemModel {
                id: views
                InfoView { id: infoView }
                FilesView { id: filesView }
                PeersView { id: peersView }
            }

            onCurrentIndexChanged: {
                navigation.selected = currentIndex
                if (currentIndex === 1) {
                    ipfsapi.setCurrentPath("/");
                    ipfsapi.files_ls()
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
            pageStack.push("WarningDialog.qml");
        }
    }
}
