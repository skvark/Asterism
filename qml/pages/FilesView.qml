import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0

Item {
    id: filesPage
    height: parent.height
    width: parent.width

    Component.onCompleted: {
        ipfsapi.files_ls()
    }

    Component {
        id: contentPickerPage
        ContentPickerPage {
            title: "Select file"
            onSelectedContentPropertiesChanged: {
                if (selectedContentProperties.filePath !== "") {
                    ipfsapi.add(selectedContentProperties.filePath)
                }
            }
        }
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: childrenRect.height
        width: parent.width

        PullDownMenu {
            id: menu
            MenuItem {
                text: "Add File Here"
                onClicked: {
                    pageStack.push(contentPickerPage)
                }
            }
            MenuItem {
                text: "Create Directory Here"
                onClicked: {
                    var dialog = pageStack.push(Qt.resolvedUrl("CreateDir.qml"))
                    dialog.accepted.connect(function() {
                        ipfsapi.files_mkdir(dialog.dir)
                    })
                }
            }
        }

        PageHeader {
            id: header
            title: "IPFS Mutable File System"
        }

        BackgroundItem {
            id: parentFolder
            width: parent.width
            anchors.top: header.bottom
            height: Theme.itemSizeSmall
            enabled: ipfsapi.currentPath != "/"

            Image {
                fillMode: Image.Pad
                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter
                anchors.leftMargin: Theme.paddingLarge
                id: folderup
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                source: "image://theme/icon-m-back"
                visible: ipfsapi.currentPath != "/"
            }

            Text {
                width: parent.width - folderup.width - Theme.paddingLarge * 3
                anchors.top: parent.top
                anchors.left: folderup.right
                anchors.right: parent.right
                height: parent.height
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.primaryColor
                verticalAlignment: Text.AlignVCenter
                anchors.leftMargin: Theme.paddingSmall
                anchors.rightMargin: Theme.paddingSmall
                text: ipfsapi.currentPath
            }

            onClicked: {
                ipfsapi.setCurrentPath("..")
            }
        }

        SilicaListView {
            id: fileList
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parentFolder.bottom
            anchors.topMargin: Theme.paddingSmall
            height: filesPage.height - header.height - parentFolder.height
            width: parent.width
            clip: true
            model: ipfsapi.files

            delegate: BackgroundItem {

                 id: fileDelegate
                 width: parent.width
                 height: Theme.itemSizeSmall
                 anchors.left: parent.left
                 anchors.right: parent.right

                 Image {
                     id: icon
                     anchors.left: parent.left
                     anchors.leftMargin: Theme.paddingLarge
                     source: model.modelData.Type === 1
                             ? "image://theme/icon-m-folder"
                             : "image://theme/icon-m-note"
                 }



                 Label {
                     id: namelabel
                     anchors {
                         left: icon.right
                         right: parent.right
                         leftMargin: Theme.paddingLarge
                         rightMargin: Theme.paddingLarge
                         topMargin: 15
                     }
                     textFormat: Text.RichText
                     text: model.modelData.Name
                     truncationMode: TruncationMode.Fade
                 }

                 Label {
                     id: sizelabel
                     anchors {
                         left: icon.right
                         right: parent.right
                         top: namelabel.bottom
                         leftMargin: Theme.paddingLarge
                         rightMargin: Theme.paddingLarge
                     }

                     font.pixelSize: Screen.Small ? 19 : 22
                     textFormat: Text.RichText
                     text: model.modelData.Type === 0
                           ? model.modelData.Hash + ", " + model.modelData.Size / 1000 + " kB"
                           : model.modelData.Hash
                     color: Theme.rgba(Theme.secondaryColor, 0.5)
                 }

                 onClicked: {
                     if (model.modelData.Type === 1) {
                        ipfsapi.setCurrentPath(model.modelData.Name)
                     }
                 }
             }

             VerticalScrollDecorator {
                 flickable: fileList
             }
        }
    }
}
