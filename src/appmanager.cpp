#include "appmanager.h"

#include <QStandardPaths>
#include <QDirIterator>
#include <QFile>
#include <QXmlStreamReader>
#include <QMap>
#include <QDebug>

AppManager::AppManager(AppSettings *settings, QObject *parent) : QObject(parent), m_settings(settings)
{
}

void AppManager::initialize()
{
    try {
        parseInstallers();

        m_apps.clear();
        const auto desktopPaths = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
        for (const auto &path : desktopPaths) {
            QDirIterator files(path, {"*.desktop"});
            while (files.hasNext()) {
                const auto path = files.next();
                if (!isAndroidDesktopFile(path)) {
                    continue;
                }
                const auto parsed = parseDesktopFile(path);
                if (!parsed.isValid()) {
                    continue;
                }

                m_apps.append(parsed);
            }
        }

        emit appsChanged();
        emit initialized();
    } catch (const QString &error) {
        emit errorOccurred(error);
        qDebug() << error;
    }
}

QVariantList AppManager::apps() const
{
    QVariantList result;
    result.reserve(m_apps.size());

    for (const auto &app : m_apps) {
        QVariantMap appMap;
        appMap.insert("name", app.name());
        appMap.insert("package", app.package());
        appMap.insert("icon", app.icon());
        appMap.insert("installer", app.installer());
        result.append(appMap);
    }

    return result;
}

bool AppManager::setInstaller(const QString &package, const QString &installer)
{
    auto changed = false;
    for (auto &app : m_apps) {
        if (app.package() != package) {
            continue;
        }

        changed = true;
        app.setInstaller(installer);
        m_packageInstallerMap[package] = installer;
        break;
    }

    return changed;
}

bool AppManager::syncPackages()
{
    try {
        if (!fileHelper->fileExists(m_settings->packagesXmlPath())) {
            throw QString("The packages.xml config file at '" + m_settings->packagesXmlPath() + "' does not exist");
        }

        const auto xmlPath = m_settings->packagesXmlPath();
        const auto fileContent = fileHelper->readFile(xmlPath);
        const auto updated = packageXml->updateInstallerMap(fileContent, m_packageInstallerMap);

        if (fileHelper->writeToFile(xmlPath, updated)) {
            emit appsChanged();
            return true;
        }

        return false;
    }  catch (const QString &error) {
        qDebug() << error;
        emit errorOccurred(error);
        return false;
    }
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
        const auto value = parts.mid(1).join("=").trimmed();

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
