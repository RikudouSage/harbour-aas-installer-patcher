import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.DBus 2.0

import "../components"

DefaultPage {
    readonly property var potentialPaths: [
        "/home/.appsupport/instance/defaultuser/data/system/packages.xml",
        "/home/.android/data/system/packages.xml"
    ];
    readonly property var appSupportSystemServiceCandidates: [
        "appsupport@defaultuser.service",
        "aliendalvik.service"
    ];
    property string appSupportState: ""
    property bool appSupportRunning: appSupportState === "active" || appSupportState === "activating" || appSupportState === "deactivating"

    //% "Loading..."
    title: qsTrId("global.loading")
    //% "Detecting Android subsystem..."
    loadText: qsTrId("checker.detecting")
    loading: true

    Connections {
        target: appManager

        onInitialized: {
            safeCall(function() {
                if (appSupportRunning) {
                    loading = false;
                    return;
                }
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
            loading = true;
            settings.packagesXmlPath = text;
            checkAppSupportAndContinue(function() {
                pageStack.replace("AppList.qml");
            });
        }
    }

    StandardLabel {
        //% "Android AppSupport is currently running. Stop it first, then retry."
        text: qsTrId("checker.aas_running")
        visible: !loading && appSupportRunning
    }

    Button {
        //% "Retry"
        text: qsTrId("checker.retry")
        visible: !loading && appSupportRunning
        anchors.horizontalCenter: parent.horizontalCenter

        onClicked: {
            loading = true;
            checkAppSupportAndContinue(function() {
                if (settings.packagesXmlPath) {
                    appManager.initialize();
                } else {
                    loading = false;
                }
            });
        }
    }

    DBusInterface {
        id: appSupportSystemService

        bus: DBus.SystemBus
        service: "org.freedesktop.systemd1"
        iface: "org.freedesktop.systemd1.Unit"
    }

    DBusInterface {
        id: systemdManager

        bus: DBus.SystemBus
        service: "org.freedesktop.systemd1"
        path: "/org/freedesktop/systemd1"
        iface: "org.freedesktop.systemd1.Manager"
    }

    function resolveUnitState(managerInterface, unitInterface, serviceNames, onResolved) {
        var remainingCandidates = serviceNames.slice(0);

        function tryResolveUnitPath() {
            if (remainingCandidates.length === 0) {
                unitInterface.path = "";
                onResolved("");
                return;
            }

            var serviceName = remainingCandidates.shift();
            managerInterface.typedCall(
                "GetUnit",
                [{ "type": "s", "value": serviceName }],
                function(unitPath) {
                    unitInterface.path = unitPath;
                    onResolved(unitInterface.getProperty("ActiveState") || "");
                },
                function() {
                    tryResolveUnitPath();
                }
            );
        }

        tryResolveUnitPath();
    }

    function isPrimaryAppSupportUnit(name) {
        return /^appsupport@.+\.service$/.test(name)
            || /^aliendalvik(\@.+)?\.service$/.test(name);
    }

    function findRunningAppSupportState(units) {
        if (!units || units.length === 0) {
            return "";
        }

        for (var index = 0; index < units.length; index++) {
            var unit = units[index];
            if (!unit || unit.length < 4) {
                continue;
            }

            var name = String(unit[0]);
            var activeState = String(unit[3] || "");
            if (!isRunningState(activeState)) {
                continue;
            }

            if (isPrimaryAppSupportUnit(name)) {
                return activeState;
            }
        }

        return "";
    }

    function resolveAppSupportState(managerInterface, unitInterface, fallbackCandidates, onResolved) {
        managerInterface.typedCall(
            "ListUnitsByPatterns",
            [
                { "type": "as", "value": [] },
                { "type": "as", "value": ["appsupport@*.service", "aliendalvik*.service"] }
            ],
            function(units) {
                var runningState = findRunningAppSupportState(units);
                if (runningState !== "") {
                    onResolved(runningState);
                    return;
                }

                resolveUnitState(managerInterface, unitInterface, fallbackCandidates, onResolved);
            },
            function() {
                resolveUnitState(managerInterface, unitInterface, fallbackCandidates, onResolved);
            }
        );
    }

    function isRunningState(state) {
        return state === "active" || state === "activating" || state === "deactivating";
    }

    function checkAppSupportAndContinue(callback) {
        appSupportState = "";
        appSupportRunning = false;

        resolveAppSupportState(systemdManager, appSupportSystemService, appSupportSystemServiceCandidates, function(systemState) {
            appSupportState = systemState;
            appSupportRunning = isRunningState(systemState);
            if (appSupportRunning) {
                loading = false;
                return;
            }

            callback();
        });
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
                checkAppSupportAndContinue(function() {
                    appManager.initialize();
                });
                return;
            }

            loading = false;
        } else {
            checkAppSupportAndContinue(function() {
                appManager.initialize();
            });
        }
    }
}
