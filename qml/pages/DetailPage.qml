import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

import "../js/objects.js" as Objects

DefaultDialog {
    property var defaultInstallers: ({})
    property var appList: []
    property var app: ({})

    property string value: comboBox.currentValue === comboBox.customInstallerValue
        ? customInstallerField.text.trim()
        : comboBox.currentValue

    //% "Save"
    acceptText: qsTrId("global.save")
    //% "Cancel"
    cancelText: qsTrId("global.cancel")

    canAccept: comboBox.currentValue !== comboBox.customInstallerValue || customInstallerField.text.trim().length > 0

    StandardLabel {
        horizontalAlignment: Text.AlignRight
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeLarge
        text: app.name
    }

    Row {
        id: packageIdRow
        spacing: Theme.paddingMedium
        x: Theme.horizontalPageMargin

        Label {
            //% "Package ID:"
            text: qsTrId("detail.package_id")
            color: Theme.highlightColor
            x: Theme.horizontalPageMargin
        }

        Label {
            text: app.package
        }
    }

    Row {
        id: installerIdRow
        spacing: packageIdRow.spacing
        x: packageIdRow.x

        Label {
            //% "Installer ID:"
            text: qsTrId("detail.installer_id")
            color: Theme.highlightColor
            x: Theme.horizontalPageMargin
        }

        Label {
            text: app.installer
        }
    }

    Row {
        spacing: packageIdRow.spacing
        x: packageIdRow.x

        Label {
            //% "Installer:"
            text: qsTrId("detail.installer_name")
            color: Theme.highlightColor
        }

        Label {
            text: typeof appList[app.installer] !== 'undefined'
                ? appList[app.installer].name
                : typeof defaultInstallers[app.installer] !== 'undefined'
                    ? defaultInstallers[app.installer]
                    //: Unknown installer, shown when the app is not installed
                    //% "Unknown"
                    : qsTrId("detail.unknown_installer")
        }
    }

    StandardLabel {
        horizontalAlignment: Text.AlignRight
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeLarge
        //% "Change installer"
        text: qsTrId("detail.change_installer")
    }

    ComboBox {
        readonly property string customInstallerValue: 'custom'
        property string currentValue: currentItem.value

        id: comboBox
        width: parent.width
        //% "Installer"
        label: qsTrId("detail.installer")

        menu: ContextMenu {

            Repeater {
                model: Object.keys(defaultInstallers)
                delegate: MenuItem {
                    property string value: modelData
                    text: defaultInstallers[modelData]
                }
            }

            MenuItem {
                property string value: comboBox.customInstallerValue
                //: Custom installer
                //% "Custom"
                text: qsTrId("detail.custom_installer")
            }
        }

        Component.onCompleted: {
            var installerIds = Object.keys(defaultInstallers);
            var index = installerIds.indexOf(app.installer);
            if (index > -1) {
                currentIndex = index;
            } else {
                // custom = installerIds + 1
                currentIndex = installerIds.length;
            }
        }
    }

    TextField {
        id: customInstallerField
        //% "Custom installer"
        description: qsTrId("detail.custom_installer_long")
        visible: comboBox.currentValue === comboBox.customInstallerValue
        text: app.installer
    }
}
