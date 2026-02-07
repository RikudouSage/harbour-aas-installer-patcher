#include "appmanager.h"

#include <QStandardPaths>
#include <QDirIterator>
#include <QFile>
#include <QXmlStreamReader>
#include <QMap>
#include <QDebug>

AppManager::AppManager(AppSettings *settings, QObject *parent) : QObject(parent), m_settings(settings)
{
    try {
        parseInstallers();

        const auto desktopPaths = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
        for (const auto &path : desktopPaths) {
            QDirIterator files(path, {"*.desktop"});
            while (files.hasNext()) {
                const auto path = files.next();
                if (!isAndroidDesktopFile(path)) {
                    continue;
                }
                const auto parsed = parseDesktopFile(path);
            }
        }
    } catch (const QString &error) {
        m_error = error;
        qDebug() << error;
    }
}

const QString AppManager::error() const
{
    return m_error;
}

bool AppManager::isAndroidDesktopFile(const QString &path) const
{
    QFile file(path);
    if (!file.exists()) {
        throw QString("The desktop file '" + path + "' does not exist");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        throw QString("The desktop file '" + path + "' could not be opened for reading");
    }

    const auto content = QString::fromUtf8(file.readAll());

    return content.contains("apkd-launcher");
}

const AppManager::AndroidApp AppManager::parseDesktopFile(const QString &path) const
{
    QFile file(path);
    if (!file.exists()) {
        throw QString("The desktop file '" + path + "' does not exist");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        throw QString("The desktop file '" + path + "' could not be opened for reading");
    }

    AndroidApp app;

    while (!file.atEnd()) {
        const QString line = file.readLine();
        const auto parts = line.split("=");

        const auto name = parts.first();
        const auto value = parts.mid(1).join("=");

        if (!app.package().isEmpty() && app.installer().isEmpty()) {
            app.setInstaller(m_packageInstallerMap[app.package()]);
        }

        if (name == "Name") {
            app.setName(value);
        } else if (name == "X-apkd-packageName") {
            app.setPackage(value);
        } else if (name == "Icon") {
            app.setIcon(value);
        }

        if (app.isValid()) {
            break;
        }
    }

    return app;
}

void AppManager::parseInstallers()
{
    if (!fileHelper->fileExists(m_settings->packagesXmlPath())) {
        throw QString("The packages.xml config file at '" + m_settings->packagesXmlPath() + "' does not exist");
    }

    const auto fileContent = fileHelper->readFile(m_settings->packagesXmlPath());
    qDebug() << fileContent;
    m_packageInstallerMap = packageXml->packageInstallerMap(fileContent);
    qDebug() << m_packageInstallerMap;
}

const QString AppManager::AndroidApp::name() const
{
    return m_name;
}

void AppManager::AndroidApp::setName(const QString &name)
{
    m_name = name;
}

const QString AppManager::AndroidApp::package() const
{
    return m_package;
}

void AppManager::AndroidApp::setPackage(const QString &package)
{
    m_package = package;
}

const QString AppManager::AndroidApp::icon() const
{
    return m_icon;
}

void AppManager::AndroidApp::setIcon(const QString &icon)
{
    m_icon = icon;
}

const QString AppManager::AndroidApp::installer() const
{
    return m_installer;
}

void AppManager::AndroidApp::setInstaller(const QString &installer)
{
    m_installer = installer;
}

bool AppManager::AndroidApp::isValid() const
{
    return !m_name.isEmpty() && !m_package.isEmpty() && !m_icon.isEmpty() && !m_installer.isEmpty();
}
