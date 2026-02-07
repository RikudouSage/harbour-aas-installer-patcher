#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QScopedPointer>
#include <QCoreApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QGuiApplication>
#include <QLocale>
#include <QDebug>
#include <QTranslator>

#include <sailfishapp.h>

#include "appsettings.h"
#include "filehelper.h"
#include "appmanager.h"

// todo don't hardcode
constexpr auto TRANSLATION_INSTALL_DIR = "/usr/share/harbour-aas-installer-patcher/translations";

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));

    const auto settings = new AppSettings(app.data());
    const auto fileHelper = new FileHelper(app.data());
    const auto appManager = new AppManager(settings, app.data());

    auto language = settings->rawLanguage();
    if (language.isEmpty()) {
        language = QLocale::system().name();
    }
    qInfo() << "Using language: " << language;

    QTranslator *defaultLang = new QTranslator(app.data());
    if (!defaultLang->load("harbour-aas-installer-patcher-en", TRANSLATION_INSTALL_DIR)) {
        qWarning() << "Could not load English translation file!";
    }
    QCoreApplication::installTranslator(defaultLang);

    QTranslator *translator = new QTranslator(app.data());
    if (!translator->load(QLocale(language), "harbour-aas-installer-patcher", "-", TRANSLATION_INSTALL_DIR)) {
        qWarning() << "Could not load translations for" << language;
    }
    QCoreApplication::installTranslator(translator);

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->rootContext()->setContextProperty("settings", settings);
    view->rootContext()->setContextProperty("fileHelper", fileHelper);
    view->rootContext()->setContextProperty("appManager", appManager);

#ifdef QT_DEBUG
    view->rootContext()->setContextProperty("isDebug", true);
#else
    view->rootContext()->setContextProperty("isDebug", false);
#endif

    view->setSource(SailfishApp::pathToMainQml());
    view->show();

    return app->exec();
}
