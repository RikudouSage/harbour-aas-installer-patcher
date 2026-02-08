import QtQuick 2.0
import Sailfish.Silica 1.0

import "pages"
import "components"

ApplicationWindow {
    property bool appSupportRunning: aasChecker.appSupportRunning
    property bool appSupportStateReady: aasChecker.stateReady

    id: app

    initialPage: Component { CheckerPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    NotificationStack {
        id: notificationStack
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: Theme.paddingLarge * 3
        }
    }

    AasChecker {
        id: aasChecker
    }
}
