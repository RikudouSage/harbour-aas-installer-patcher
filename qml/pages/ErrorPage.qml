import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

DefaultPage {
    property string error

    //% "Error"
    title: qsTrId("error.title")

    StandardLabel {
        id: errorText
        color: Theme.errorColor
        //% "There was an error while initializing the app: %1"
        text: qsTrId("error.error").arg(error)
    }
}
