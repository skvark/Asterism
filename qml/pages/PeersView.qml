import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: peersPage
    height: parent.height
    width: parent.width
    property bool autoUpdate: false;

    Timer {
        interval: 2000
        running: autoUpdate
        repeat: true
        onTriggered: ipfsapi.conns()
    }


    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: childrenRect.height
        width: parent.width

        PullDownMenu {
            MenuItem {
                text: "Set Auto Refresh " + (autoUpdate ? "Off" : "On")
                onClicked: {
                    autoUpdate = !autoUpdate
                }
            }
        }

        PageHeader {
            id: header
            title: ipfsapi.nodeConns.length + " Peers"
        }

        SilicaListView {
            id: peerList
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: header.bottom
            anchors.topMargin: Theme.paddingSmall
            height: peersPage.height - header.height
            width: parent.width
            clip: true
            model: ipfsapi.nodeConns

            delegate: ListItem {
                 id: peerDelegate
                 width: parent.width
                 height: Theme.itemSizeSmall

                 Item {
                     anchors.left: parent.left
                     anchors.right: parent.right
                     anchors.top: parent.top
                     anchors.bottom: parent.bottom
                     anchors.topMargin: Theme.paddingSmall
                     anchors.leftMargin: Theme.paddingLarge
                     anchors.rightMargin: Theme.paddingLarge
                     height: parent.height

                     Label {
                         id: idlabel
                         width: parent.width
                         textFormat: Text.RichText
                         font.pixelSize: Theme.fontSizeExtraSmall
                         text: model.modelData.Pid
                         truncationMode: TruncationMode.Fade
                     }

                     Label {
                         id: sizelabel
                         anchors.top: idlabel.bottom
                         width: parent.width
                         font.pixelSize: Theme.fontSizeTiny
                         textFormat: Text.RichText
                         text: model.modelData.Addr
                         color: Theme.rgba(Theme.secondaryColor, 0.5)
                     }
                 }

             }

             VerticalScrollDecorator {
                 flickable: peerList
             }
        }

    }
}
