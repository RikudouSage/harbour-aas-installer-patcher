import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property var _doAfterLoad: []

    function safeCall(callable) {
        if (page.status === PageStatus.Active && !pageStack.busy) {
            callable();
        } else {
            _doAfterLoad.push(callable);
        }
    }

    function _flushQueue() {
        while (_doAfterLoad.length) {
            const callable = _doAfterLoad.shift();
            callable();
        }
    }

    id: page
    allowedOrientations: Orientation.All

    onStatusChanged: {
        if (page.status === PageStatus.Active && !pageStack.busy) {
            _flushQueue();
        }
    }

    Connections {
        target: pageStack

        onBusyChanged: {
            if (page.status === PageStatus.Active && !pageStack.busy) {
                _flushQueue();
            }
        }
    }
}
