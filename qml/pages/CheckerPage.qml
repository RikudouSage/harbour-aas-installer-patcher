import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

DefaultPage {
    readonly property var potentialPaths: [
        "/home/.appsupport/instance/defaultuser/data/system/packages.xml",
        "/home/.android/data/system/packages.xml"
    ];
    property bool appSupportRunning: app.appSupportRunning
    property bool appSupportStateReady: app.appSupportStateReady
    property bool initializationRequested: false
    property bool initialized: false

    function navigateToMainIfReady() {
        if (initialized && appSupportStateReady && !appSupportRunning) {
            safeCall(function() {
                pageStack.replace("AppList.qml");
            });
        }
    }

    function initializeIfReady() {
        if (!settings.packagesXmlPath) {
            loading = false;
            return;
        }

        if (!appSupportStateReady) {
            loading = true;
            return;
        }

        if (appSupportRunning) {
            loading = false;
            return;
        }

        if (initialized || initializationRequested) {
            navigateToMainIfReady();
            return;
        }

        initializationRequested = true;
        loading = true;
        appManager.initialize();
    }

    //% "Loading..."
    title: qsTrId("global.loading")
    //% "Detecting Android subsystem..."
    loadText: qsTrId("checker.detecting")
    loading: true

    Connections {
        target: appManager

        onInitialized: {
            initializationRequested = false;
            initialized = true;
            navigateToMainIfReady();
        }

        onErrorOccurred: {
            initializationRequested = false;
            safeCall(function() {
                pageStack.replace("ErrorPage.qml", {error: error});
            });
        }
    }


    StandardLabel {
        //% "Could not detect the Android packages file (packages.xml) in any of the known paths. If you know where it is, you can provide the file manually below."
        text: qsTrId("checker.no_aas")
        visible: !settings.packagesXmlPath
    }

    TextField {
        property bool pathExists
        id: pathTextField
        visible: !settings.packagesXmlPath
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
        visible: !loading && !settings.packagesXmlPath

        onClicked: {
            settings.packagesXmlPath = text;
            initializeIfReady();
        }
    }

    StandardLabel {
        //% "Android AppSupport is currently running. Stop it first before continuing."
        text: qsTrId("checker.aas_running")
        visible: !loading && appSupportRunning
    }

    onAppSupportStateReadyChanged: {
        initializeIfReady();
    }

    onAppSupportRunningChanged: {
        if (appSupportRunning) {
            loading = false;
            return;
        }

        initializeIfReady();
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
                initializeIfReady();

                return;
            }

            loading = false;
        } else {
            initializeIfReady();
        }
    }
}
