import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: "Exit App"
                onClicked: service.closeApp();
            }
        }

        Column {
            id: column
            width: page.width - (2 * Theme.paddingLarge)
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge
            anchors.topMargin: Theme.paddingLarge
            spacing: Theme.paddingMedium

            Image {
                id: logo
                anchors.topMargin: Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                horizontalAlignment: Image.AlignHCenter
                width: 300
                height: 300
                source: "../images/ipfs-logo-512-ice-text.png"
            }

            Column {
                id: innercolumn
                anchors.topMargin: Theme.paddingLarge
                width: parent.width
                spacing: Theme.paddingMedium

                Label {
                    text: "Node ID"
                    width: parent.width
                    color: Theme.highlightColor
                    font.family: Theme.fontFamilyHeading
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    text: ipfsapi.config.Identity ? ipfsapi.config.Identity.PeerID : "-"
                    width: parent.width
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeTiny
                }

                Label {
                    text: "Size"
                    width: parent.width
                    color: Theme.highlightColor
                    font.family: Theme.fontFamilyHeading
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    text: ipfsapi.stats.RepoSize ?
                               Math.round(ipfsapi.stats.RepoSize / (1024 * 1024)) +
                               " / " +
                               Math.round(ipfsapi.stats.StorageMax / (1024 * 1024)) +
                               " MB"
                             : "- / -"
                    width: parent.width
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeSmall
                }

                Label {
                    text: "Objects"
                    width: parent.width
                    color: Theme.highlightColor
                    font.family: Theme.fontFamilyHeading
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    text: ipfsapi.stats.NumObjects ? ipfsapi.stats.NumObjects : "-"
                    width: parent.width
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeSmall
                }

                Button {
                   id: button
                   text: ipfsapi.isRunning ? "Stop Node" : "Start Node"
                   onClicked: ipfsapi.isRunning ? ipfsapi.stop() : ipfsapi.start()
                   enabled: !ipfsapi.isStarting
                   anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        BusyIndicator {
            anchors.top: column.bottom
            anchors.topMargin: Theme.paddingLarge
            size: BusyIndicatorSize.Medium
            anchors.horizontalCenter: parent.horizontalCenter
            running: ipfsapi.isStarting
        }
    }

    Connections {
        target: service
        onFirstUse: {
            pageStack.push("WarningDialog.qml");
        }
    }
}
