import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    property alias acceptText: dialogHeader.acceptText
    property alias cancelText: dialogHeader.cancelText
    property alias loading: loader.running
    //% "Loading..."
    property string loadText: qsTrId("global.loading")

    default property alias main: column.data

    BusyLabel {
        id: loader
        text: loadText
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height
        visible: !loading

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            DialogHeader {
                id: dialogHeader
            }
        }
    }
}
