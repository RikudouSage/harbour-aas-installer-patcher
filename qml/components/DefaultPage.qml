import QtQuick 2.0
import Sailfish.Silica 1.0

SafePage {
    property alias title: pageHeader.title
    property alias loading: loader.running
    property alias flickable: flickable
    //% "Loading..."
    property string loadText: qsTrId("global.loading")
    default property alias main: column.data


    signal contentYChanged(int contentY)

    id: page

    BusyLabel {
        id: loader
        text: loadText
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: column.height

        onContentYChanged: {
            page.contentYChanged(contentY)
        }

        Column {
            id: column
            spacing: Theme.paddingLarge
            width: page.width
            visible: !loading

            PageHeader {
                id: pageHeader
            }
        }
    }
}
