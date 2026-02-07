#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QtDBus>

#include "daemon.h"

constexpr int errorNotConnected = 2;
constexpr int errorServiceNotRegistered = 3;
constexpr int errorObjectNotRegistered = 4;

namespace {
constexpr char kServiceName[] = "dev.chrastecky.aas_patcher.daemon";
constexpr char kObjectPath[] = "/dev/chrastecky/aas_patcher/Daemon";
} // namespace

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    qDBusRegisterMetaType<QMap<QString, QString>>();

    auto bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        qCritical() << "System bus is not connected:" << bus.lastError().name() << bus.lastError().message();
        return errorNotConnected;
    }

    if (!bus.registerService(kServiceName)) {
        qCritical() << "Failed to register service" << kServiceName << ":" << bus.lastError().name() << bus.lastError().message();
        return errorServiceNotRegistered;
    }

    Daemon daemon;
    if (!bus.registerObject(kObjectPath, &daemon, QDBusConnection::ExportAllSlots)) {
        qCritical() << "Failed to register object" << kObjectPath << ":" << bus.lastError().name() << bus.lastError().message();
        return errorObjectNotRegistered;
    }

    return app.exec();
}
