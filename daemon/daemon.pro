TEMPLATE = app
TARGET = harbour-aas-installer-patcher-helper

QT += core dbus
CONFIG += console c++20
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=gnu++20

SOURCES += \
    main.cpp

# Install location (typical for root helpers)
target.path = /usr/libexec/harbour-aas-installer-patcher
INSTALLS += target

