import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

DefaultPage {
    //% "Loading..."
    title: qsTrId("global.loading")
    //% "Detecting Android subsystem..."
    loadText: qsTrId("checker.detecting")
    loading: true

    readonly property var potentialPaths: [
        "/home/.appsupport/instance/defaultuser/data/system/packages.xml",
        "/home/.android/data/system/packages.xml"
    ];

    Connections {
        target: appManager

        onInitialized: {
            safeCall(function() {
                pageStack.replace("AppList.qml");
            });
        }

        onErrorOccurred: {
            safeCall(function() {
                pageStack.replace("ErrorPage.qml", {error: error});
            });
        }
    }


    StandardLabel {
        //% "Could not detect the Android packages file (packages.xml) in any of the known paths. If you know where it is, you can provide the file manually below."
        text: qsTrId("checker.no_aas")
    }

    TextField {
        property bool pathExists
        id: pathTextField
        //% "/path/to/packages.xml"
        placeholderText: qsTrId("checker.path_placeholder")

        onTextChanged: {
            pathExists = fileHelper.fileExists(text)
        }
    }

    Button {
        enabled: pathTextField.text !== "" && pathTextField.pathExists
        //% "Save"
        text: qsTrId("global.save")
        anchors.horizontalCenter: parent.horizontalCenter
        visible: !loading

        onClicked: {
            loading = true;
            settings.packagesXmlPath = text;
            pageStack.replace("AppList.qml");
        }
    }

    Component.onCompleted: {
        if (!settings.packagesXmlPath) {
            for (var index in potentialPaths) {
                if (!potentialPaths.hasOwnProperty(index)) {
                    continue;
                }
                const file = potentialPaths[index];
                if (!fileHelper.fileExists(file)) {
                    continue;
                }
                settings.packagesXmlPath = file;
                appManager.initialize();
                return;
            }

            loading = false;
        } else {
            appManager.initialize();
        }
    }
}
