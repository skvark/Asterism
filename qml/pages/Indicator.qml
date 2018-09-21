import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: indicator
    height: Theme.itemSizeSmall / 12
    width: parent.width

    states: [
        State { name: "visible";
            PropertyChanges {
                target: indicator;
                color: Theme.rgba(Theme.highlightColor, 0.7)
            }
        },
        State { name: "hidden";
            PropertyChanges {
                target: indicator;
                color: Theme.rgba(Theme.highlightColor, 0.0)
            }
        }
    ]

    transitions: Transition {
        ColorAnimation { property: "color"; duration: 250}
    }
}
