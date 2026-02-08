import QtQuick 2.0
import Sailfish.Silica 1.0

import "../components"

import "../js/arrays.js" as Arrays
import "../js/objects.js" as Objects

DefaultPage {
    property var installers: {
        "com.android.vending": "Google Play",
        "com.aurora.store": "Aurora Store",
        //: The name of the default Android package installer
        //% "Package Installer"
        "com.android.packageinstaller": qsTrId("app.package_installer"),
    }
    property var appsRaw: []
    property var apps: ({})

    id: root
    //% "App list"
    title: qsTrId("app_list.title")
    loading: true

    onAppsRawChanged: {
        apps = Arrays.objectify(appsRaw, "package");
    }

    VerticalScrollDecorator {}

    Connections {
        target: appManager

        onErrorOccurred: {
            safeCall(function() {
                pageStack.replace("ErrorPage.qml", {error: error});
            });
        }
    }

    Repeater {
        model: appsRaw

        delegate: ListItem {
            property var packageData: modelData
            property real offset: Theme.paddingSmall

            width: root.width
            contentHeight: Theme.itemSizeMedium

            menu: ContextMenu {
                Repeater {
                    model: Objects.entries(root.installers);

                    delegate: MenuItem {
                        text: modelData[1]

                        onClicked: {
                            if (!appManager.setInstaller(packageData.package, modelData[0])) {
                                //% "Error: Failed setting the installer, looks like the app wasn't found"
                                notificationStack.push(qsTrId("app_list.failed_settings_installer"), true);
                                return;
                            }

                            if (!appManager.syncPackages()) {
                                //% "Error: Failed saving the updated installer map"
                                notificationStack.push(qsTrId("app_list.failed_sync"), true);
                                return;
                            }

                            //% "Success! %1 was successfully updated"
                            notificationStack.push(qsTrId("app_list.package_success").arg(packageData.name));
                        }
                    }
                }
            }

            onClicked: {

            }

            Image {
                id: icon
                source: modelData.icon
                height: Theme.iconSizeLarge
                width: height
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
            }

            Label {
                id: appName
                text: modelData.name
                anchors.top: parent.top
                anchors.topMargin: parent.offset
                anchors.left: icon.right
                anchors.leftMargin: Theme.paddingLarge
                font.bold: true
                font.pixelSize: Theme.fontSizeLarge
            }

            Label {
                property string installerName: typeof apps[modelData.installer] !== 'undefined'
                    ? apps[modelData.installer].name
                    : typeof installers[modelData.installer] !== 'undefined'
                        ? installers[modelData.installer]
                        : modelData.installer

                //% "Installer: %1"
                text: qsTrId("app_item.installer").arg(installerName)
                anchors.bottom: parent.bottom
                anchors.bottomMargin: parent.offset
                anchors.left: appName.left
                font.pixelSize: Theme.fontSizeSmall
            }
        }
    }

    Component.onCompleted: {
        appsRaw = appManager.apps;
        loading = false;
    }
}
