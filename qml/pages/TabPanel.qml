import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: tabpanel
    height: Theme.itemSizeSmall + Theme.itemSizeSmall / 12
    width: parent.width
    color: Theme.rgba(Theme.highlightDimmerColor, Theme.highlightBackgroundOpacity)
    property int tabCount: 3;
    property int selected: 0;

    Row {
        id: row
        height: parent.height
        width: parent.width

        BackgroundItem {
            id: info
            width: parent.width / tabCount
            height: parent.height
            onClicked: { selected = 0 }

            Indicator {
                state: selected === 0 ? "visible" : "hidden"
            }

            Image {
                anchors.horizontalCenter:  parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: "image://theme/icon-m-about?" + (pressed
                        ? Theme.highlightColor
                        : Theme.primaryColor)
            }
        }

        BackgroundItem {
            id: files
            width: parent.width / tabCount
            height: parent.height
            onClicked: { selected = 1 }

            Indicator {
                state: selected === 1 ? "visible" : "hidden"
            }

            Image {
                anchors.horizontalCenter:  parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: "image://theme/icon-m-folder?" + (pressed
                        ? Theme.highlightColor
                        : Theme.primaryColor)
            }
        }

        BackgroundItem {
            id: peers
            width: parent.width / tabCount
            height: parent.height
            onClicked: { selected = 2 }

            Indicator {
                state: selected === 2 ? "visible" : "hidden"
            }

            Image {
                anchors.horizontalCenter:  parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: "image://theme/icon-m-share?" + (pressed
                        ? Theme.highlightColor
                        : Theme.primaryColor)
                rotation: 300
            }
        }
    }
}
