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
            text: "This is experimental InterPlanetary File System application. " +
                  "Please read the notes below carefully." +
                  "<br /><br />" +
                  "This application will spin up a full IPFS peer-to-peer node on your Sailfish OS device. " +
                  "The application does not fully exit unless you select 'Exit App' from the pulley menu. " +
                  "If you closed the app by some other means (like by swiping from top to down) the node will stay running in the background " +
                  "even if you don't see the app in the multitasking view." +
                  "<br /><br />" +
                  "IPFS itself is in alpha state. There is no bandwidth limiting and thus the app may consume huge amounts of bandwidth. " +
                  "Additionally, all files you add to the IPFS are public: please don't add any personal data or secrets to the IPFS." +
                  "<br /><br />" +
                  "To avoid consuming all disk space from your device, please select below the amount of storage you allow IPFS to use."
        }
    }
}
