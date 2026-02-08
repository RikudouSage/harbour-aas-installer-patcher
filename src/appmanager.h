#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QList>
#include <QVariantList>

#include "appsettings.h"
#include "packagesxmlhandler.h"
#include "filehelper.h"

class AppManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList apps READ apps NOTIFY appsChanged)
public:
    explicit AppManager(AppSettings *settings, QObject *parent = nullptr);
    Q_INVOKABLE void initialize();
    QVariantList apps() const;
    Q_INVOKABLE bool setInstaller(const QString &package, const QString &installer);
    Q_INVOKABLE bool syncPackages();

signals:
    void errorOccurred(const QString &error);
    void initialized();
    void appsChanged();
    void packagesSynced();
    void packagesNotSynced();

private:
    class AndroidApp {
    public:
        const QString name() const;
        void setName(const QString &name);
        const QString package() const;
        void setPackage(const QString &package);
        const QString icon() const;
        void setIcon(const QString &icon);
        const QString installer() const;
        void setInstaller(const QString &installer);

        bool isValid() const;

    private:
        QString m_name;
        QString m_package;
        QString m_icon;
        QString m_installer;
    };

private:
    const AppSettings *m_settings;
    PackagesXmlHandler *packageXml = new PackagesXmlHandler(this);
    QMap<QString, QString> m_packageInstallerMap;
    FileHelper *fileHelper = new FileHelper(this);
    QList<AndroidApp> m_apps;

private:
    bool isAndroidDesktopFile(const QString &path) const;
    const AndroidApp parseDesktopFile(const QString &path) const;
    void parseInstallers();
};

#endif // APPMANAGER_H
