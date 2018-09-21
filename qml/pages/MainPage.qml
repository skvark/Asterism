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
