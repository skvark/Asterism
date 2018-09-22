import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: settingsPage
    height: parent.height
    width: parent.width

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: childrenRect.height
        width: parent.width

        PullDownMenu {
            busy: ipfsapi.isStarting
            MenuItem {
                text: "Restart Node"
                onClicked: {
                    if (ipfsapi.isRunning) {
                        ipfsapi.restart();
                    } else {
                        ipfsapi.start();
                    }
                }
            }
        }

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10

            PageHeader {
                id: header
                title: "Settings"
            }

            SectionHeader {
                id: restart
                text: "Settings which require Node restart"
            }

            Label {
                text: "You can restart the node via the pulley menu."
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.right: parent.right
                anchors.rightMargin: Theme.paddingLarge
            }

            TextSwitch {
                id: mode
                text: "Full DHT mode"
                automaticCheck: false
                checked: ipfsapi.mode === "dht"
                description: "Switches between full DHT and DHT client only mode. Client only mode will reduce network traffic and battery consumption."
                onClicked: {
                    if (!checked) {
                        checked = true;
                        ipfsapi.setMode("dht");
                    } else {
                        checked = false;
                        ipfsapi.setMode("dhtclient")
                    }
                }
            }

            Slider {
                id: sizeslider
                width: parent.width
                label: "Storage max. size in MB"
                minimumValue: 24
                maximumValue: 1024
                value: ipfsapi.repoSize
                valueText: value + " MB"
                stepSize: 8
                onValueChanged: {
                    ipfsapi.repoSize = value
                }
            }

            SectionHeader {
                id: other
                text: "Other Settings"
            }

            Button {
               text: "Run Garbage Collection"
               enabled: !ipfsapi.isGcRunning
               anchors.horizontalCenter: parent.horizontalCenter
               onClicked: {
                   ipfsapi.gc()
               }
            }

            Label {
                anchors.right: parent.right
                anchors.left: parent.left
                text: "Garbage collection removes cached files which have not been pinned."
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.rightMargin: Theme.paddingLarge
                anchors.leftMargin: Theme.paddingLarge
                wrapMode: Text.Wrap
            }

        }

    }

    Connections {
        target: ipfsapi
        onModeChanged: {
            mode.checked = ipfsapi.mode === "dht"
        }
        onRepoSizeChanged: {
            sizeslider.value = ipfsapi.repoSize
        }
    }
}
