import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property var _doAfterLoad: []

    function safeCall(callable) {
        if (page.status === PageStatus.Active) {
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
        if (status === PageStatus.Active) {
            _flushQueue();
        }
    }
}
