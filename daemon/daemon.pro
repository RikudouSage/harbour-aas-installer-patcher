TEMPLATE = app
TARGET = harbour-aas-installer-patcher-daemon

QT += core dbus
CONFIG += console c++20
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=gnu++20

SOURCES += \
    daemon.cpp \
    main.cpp

HEADERS += \
    daemon.h

# Install location
target.path = /usr/libexec
INSTALLS += target

# D-Bus system service activation
dbus_service.files = dev.chrastecky.aas_patcher.daemon.service
dbus_service.path = /usr/share/dbus-1/system-services
INSTALLS += dbus_service

# D-Bus daemon policy
dbus_policy.files = dev.chrastecky.aas_patcher.daemon.conf
dbus_policy.path = /usr/share/dbus-1/system.d
INSTALLS += dbus_policy

# systemd unit (used by D-Bus activation)
systemd_unit.files = harbour-aas-installer-patcher-daemon.service
systemd_unit.path = /usr/lib/systemd/system
INSTALLS += systemd_unit

# polkit policy
polkit_policy.files = dev.chrastecky.aas_patcher.daemon.policy
polkit_policy.path = /usr/share/polkit-1/actions
INSTALLS += polkit_policy
