import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: infoPage
    height: parent.height
    width: parent.width

    Column {
        id: column
        width: parent.width - (2 * Theme.paddingLarge)
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: Theme.paddingLarge
        anchors.rightMargin: Theme.paddingLarge
        anchors.topMargin: Theme.paddingLarge
        spacing: Theme.paddingSmall

        Label {
            text: "Asterism"
            width: parent.width
            color: Theme.highlightColor
            font.family: Theme.fontFamilyHeading
            font.pixelSize: Theme.fontSizeExtraLarge
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

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
                text: "Node Status"
                width: parent.width
                color: Theme.highlightColor
                font.family: Theme.fontFamilyHeading
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: ipfsapi.isStarting ? "Starting" : ipfsapi.isRunning ? "Running"  : "Stopped"
                width: parent.width
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeSmall
                font.bold: true
                color: ipfsapi.isStarting ? Theme.rgba("#FEFEFE", 0.6) : ipfsapi.isRunning
                       ? Theme.rgba(Theme.secondaryHighlightColor, 0.9)
                       : Theme.rgba("#CD5C5C", 0.6)
            }

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
                           Math.round(ipfsapi.stats.RepoSize / (1000 * 1000)) +
                           " / " +
                           Math.round(ipfsapi.stats.StorageMax / (1000 * 1000)) +
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
        }
    }
}
