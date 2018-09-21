import QtQuick 2.2
import Sailfish.Silica 1.0

Dialog {
    property string dir

    Column {
        width: parent.width

        DialogHeader { }

        TextField {
            id: dirField
            width: parent.width
            placeholderText: "Insert directory name."
            label: "Directory"
        }
    }

    onDone: {
        if (result == DialogResult.Accepted) {
            dir = dirField.text
        }
    }
}
