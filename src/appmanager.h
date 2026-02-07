#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include "appsettings.h"
#include "packagesxmlhandler.h"
#include "filehelper.h"

class AppManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error CONSTANT)
public:
    explicit AppManager(AppSettings *settings, QObject *parent = nullptr);
    const QString error() const;

private:
    const AppSettings *m_settings;
    PackagesXmlHandler *packageXml = new PackagesXmlHandler(this);
    QMap<QString, QString> m_packageInstallerMap;
    QString m_error;
    FileHelper *fileHelper = new FileHelper(this);

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
    bool isAndroidDesktopFile(const QString &path) const;
    const AndroidApp parseDesktopFile(const QString &path) const;
    void parseInstallers();
};

#endif // APPMANAGER_H
