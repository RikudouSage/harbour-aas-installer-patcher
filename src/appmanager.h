#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QList>

#include "appsettings.h"
#include "packagesxmlhandler.h"
#include "filehelper.h"

class AppManager : public QObject
{
    Q_OBJECT
public:
    explicit AppManager(AppSettings *settings, QObject *parent = nullptr);
    Q_INVOKABLE void initialize();

signals:
    void errorOccurred(const QString &error);
    void initialized();

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
    QList<const AndroidApp> m_apps;

private:
    bool isAndroidDesktopFile(const QString &path) const;
    const AndroidApp parseDesktopFile(const QString &path) const;
    void parseInstallers();
};

#endif // APPMANAGER_H
