#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>

class AppSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString rawLanguage READ rawLanguage WRITE setRawLanguage NOTIFY rawLanguageChanged)
    Q_PROPERTY(QString packagesXmlPath READ packagesXmlPath WRITE setPackagesXmlPath NOTIFY packagesXmlPathChanged)
public:
    explicit AppSettings(QObject *parent = nullptr);
    ~AppSettings();

    const QString language() const;
    void setLanguage(const QString &value);
    const QString rawLanguage() const;
    void setRawLanguage(const QString &value);
    const QString packagesXmlPath() const;
    void setPackagesXmlPath(const QString &value);

signals:
    void languageChanged();
    void rawLanguageChanged();
    void packagesXmlPathChanged();

private:
    void saveConfig(const QString &name, const QVariant &value);

    QSettings* settings = new QSettings(
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.ini",
        QSettings::IniFormat,
        this
    );

    QString prop_language;
    QString prop_packagesXmlPath;
};

#endif // APPSETTINGS_H
