import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

CoverBackground {
    property string currentGroupName: ''

    Column {
        id: branding
        anchors.fill: parent
        anchors.topMargin: Theme.paddingLarge

        /*HighlightImage {
            id: icon
            source: "file:///usr/share/harbour-spliit/icons/bare.png"
            sourceSize.width: Theme.iconSizeMedium
            sourceSize.height: sourceSize.width
        }*/

        StandardLabel {
            //% "AAS Installer Patcher"
            text: qsTrId("app.name")
            font.pixelSize: Theme.fontSizeLarge
            horizontalAlignment: Qt.AlignHCenter
        }
    }
}
