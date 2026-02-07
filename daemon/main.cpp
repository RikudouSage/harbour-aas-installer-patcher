#include <QCoreApplication>
#include <QDBusConnection>

#include "daemon.h"

constexpr int errorNotConnected = 2;
constexpr int errorServiceNotRegistered = 3;

namespace {
constexpr char kServiceName[] = "dev.chrastecky.aas_patcher.daemon";
constexpr char kObjectPath[] = "/dev/chrastecky/aas_patcher/Daemon";
} // namespace

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    auto bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        return errorNotConnected;
    }

    if (!bus.registerService(kServiceName)) {
        return errorServiceNotRegistered;
    }

    Daemon daemon;
    if (!bus.registerObject(kObjectPath, &daemon, QDBusConnection::ExportAllSlots)) {
        return errorServiceNotRegistered;
    }

    return app.exec();
}
