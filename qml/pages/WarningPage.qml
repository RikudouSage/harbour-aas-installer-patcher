import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

DefaultPage {
    property string version

    //% "Warning"
    title: qsTrId("warning.title")

    StandardLabel {
        color: Theme.highlightColor
        //% "<p>Warning: This app modifies the Android runtime which might lead to unexpected results. In the <i>worst case</i> scenarion you may <strong>destroy your Android runtime</strong> and you may have to <strong>install every app from scratch</strong></p><br><p>That said, I myself use the app and consider it safe enough, but you should be aware of the risks.</p>"
        text: qsTrId("warning.warning")
    }

    Button {
        //% "Akcnowledge"
        text: qsTrId("warning.acknowledge")
        onClicked: {
            settings.warningAcknowledged = version;
            safeCall(function() {
                pageStack.replace("CheckerPage.qml");
            });
        }
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
