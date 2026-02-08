TARGET = harbour-aas-installer-patcher

QT += dbus
CONFIG += sailfishapp c++20
QMAKE_CXXFLAGS += -std=gnu++20

SOURCES += src/main.cpp \
    src/appmanager.cpp \
    src/appsettings.cpp \
    src/filehelper.cpp \
    src/packagesxmlhandler.cpp \
    third_party/abx/src/cpp/abx2xml/abx2xml.cc \
    third_party/abx/src/cpp/xml2abx/xml2abx.cc \
    third_party/pugixml/src/pugixml.cpp

HEADERS += \
    src/appmanager.h \
    src/appsettings.h \
    src/filehelper.h \
    src/packagesxmlhandler.h \
    third_party/abx/src/cpp/abx2xml/abx2xml.hpp \
    third_party/abx/src/cpp/xml2abx/xml2abx.hpp \
    third_party/pugixml/src/pugixml.hpp

DISTFILES += qml/harbour-aas-installer-patcher.qml \
    qml/components/DefaultPage.qml \
    qml/components/SafePage.qml \
    qml/components/StandardLabel.qml \
    qml/cover/CoverPage.qml \
    qml/pages/AppList.qml \
    qml/pages/CheckerPage.qml \
    qml/pages/ErrorPage.qml \
    rpm/harbour-aas-installer-patcher.changes.in \
    rpm/harbour-aas-installer-patcher.changes.run.in \
    rpm/harbour-aas-installer-patcher.spec \
    translations/*.ts \
    daemon/dev.chrastecky.aas_patcher.daemon.debug.policy \
    harbour-aas-installer-patcher.desktop

INCLUDEPATH += third_party/pugixml/src

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n sailfishapp_i18n_idbased

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-aas-installer-patcher-en.ts \
                translations/harbour-aas-installer-patcher-cs.ts
