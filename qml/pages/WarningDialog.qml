import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    DialogHeader {
       id: header
       acceptText: "Proceed"
    }

    backNavigation: false

    Column {
        id: infolabel
        width: parent.width - (2 * Theme.pageStackIndicatorWidth)
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.leftMargin: Theme.pageStackIndicatorWidth
        anchors.rightMargin: Theme.pageStackIndicatorWidth
        height: childrenRect.height
        spacing: Theme.paddingLarge

        Label {
            width: parent.width
            wrapMode: Text.Wrap
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            textFormat: Text.RichText;
            text: "<style>a:link { color: " + Theme.highlightColor + "; }</style>" +
                  "This experimental <a href='https://ipfs.io/'>InterPlanetary File System</a> application " +
                  "brings decentralized web to your Sailfish OS device. Please read the notes below carefully." +
                  "<br /><br />" +
                  "This application will make your device a fully working IPFS peer-to-peer node. " +
                  "IPFS is currently in alpha state which means there might be unexpected bugs in this application." +
                  "<br /><br />" +
                  "The app may consume huge amounts of bandwidth if left running in the background. This has also negative effect on battery life. " +
                  "To decrease the network usage, turn 'DHT Client Only' mode on from settings. " +
                  "<br /><br />" +
                  "All files you add to the network are public. Don't add any personal data or secrets to the IPFS." +
                  "<br /><br />" +
                  "Please select below the amount of storage you allow IPFS to use. You can change this later in the settings."
        }

        Slider {
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
    }
}
