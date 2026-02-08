import QtQuick 2.0
import Nemo.DBus 2.0

Item {
    readonly property var appSupportSystemServiceCandidates: [
        "appsupport@defaultuser.service",
        "aliendalvik.service"
    ];
    property string appSupportState: ""
    property bool appSupportRunning: appSupportState === "active" || appSupportState === "activating" || appSupportState === "deactivating"
    property bool stateReady: false
    property bool monitoringEnabled: true

    DBusInterface {
        id: appSupportSystemService

        bus: DBus.SystemBus
        service: "org.freedesktop.systemd1"
        iface: "org.freedesktop.systemd1.Unit"
        signalsEnabled: true

        onPropertiesChanged: runningUpdateTimer.restart()
        onPathChanged: refreshStateFromTrackedUnit()
    }

    DBusInterface {
        id: systemdManager

        bus: DBus.SystemBus
        service: "org.freedesktop.systemd1"
        path: "/org/freedesktop/systemd1"
        iface: "org.freedesktop.systemd1.Manager"
        signalsEnabled: true

        signal unitNew(string name)
        onUnitNew: {
            if (isPrimaryAppSupportUnit(name)) {
                stateUpdateTimer.restart();
            }
        }

        signal unitRemoved(string name)
        onUnitRemoved: {
            if (isPrimaryAppSupportUnit(name)) {
                stateUpdateTimer.restart();
            }
        }
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

    function refreshStateFromTrackedUnit() {
        if (appSupportSystemService.path !== "") {
            appSupportState = appSupportSystemService.getProperty("ActiveState") || "";
        } else {
            appSupportState = "";
        }
    }

    function updateState() {
        resolveAppSupportState(systemdManager, appSupportSystemService, appSupportSystemServiceCandidates, function(systemState) {
            appSupportState = systemState;
            if (appSupportState === "") {
                refreshStateFromTrackedUnit();
            }
            stateReady = true;
        });
    }

    Timer {
        // Coalesce bursts of systemd changes to one state refresh.
        id: stateUpdateTimer
        interval: 100
        repeat: false
        onTriggered: updateState()
    }

    Timer {
        id: runningUpdateTimer
        interval: 100
        repeat: false
        onTriggered: refreshStateFromTrackedUnit()
    }

    Timer {
        // Fallback polling to stay in sync even if signal delivery is inconsistent.
        interval: 3000
        repeat: true
        running: monitoringEnabled
        triggeredOnStart: false
        onTriggered: updateState()
    }

    Component.onCompleted: {
        updateState();
    }

}
