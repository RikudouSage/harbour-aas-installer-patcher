#include "appsettings.h"

#include <QLocale>

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    prop_language = settings->value("language", "").toString();
    prop_packagesXmlPath = settings->value("packages_xml_path", "").toString();
    prop_warningAcknowledged = settings->value("warning_acknowledged", "").toString();
}

AppSettings::~AppSettings() {
    settings->sync();
}

const QString AppSettings::language() const
{
    if (prop_language != "") {
        return prop_language;
    }

    const auto lang = QLocale::system().language();
    if (lang == QLocale::C) {
        return QStringLiteral("en");
    }

    return QLocale::languageToString(QLocale::system().language()).toLower();
}

void AppSettings::setLanguage(const QString &value)
{
    setRawLanguage(value);
}

const QString AppSettings::rawLanguage() const
{
    return prop_language;
}

void AppSettings::setRawLanguage(const QString &value)
{
    if (value == prop_language) {
        return;
    }

    settings->setValue("language", value);
    prop_language = value;

    emit languageChanged();
    emit rawLanguageChanged();
}

const QString AppSettings::packagesXmlPath() const
{
    return prop_packagesXmlPath;
}

void AppSettings::setPackagesXmlPath(const QString &value)
{
    if (value == prop_packagesXmlPath) {
        return;
    }

    settings->setValue("packages_xml_path", value);
    prop_packagesXmlPath = value;

    emit packagesXmlPathChanged();
}

const QString AppSettings::warningAcknowledged() const
{
    return prop_warningAcknowledged;
}

void AppSettings::setWarningAcknowledged(const QString &value)
{
    if (value == prop_warningAcknowledged) {
        return;
    }

    settings->setValue("warning_acknowledged", value);
    prop_warningAcknowledged = value;

    emit warningAcknowledgedChanged();
}
