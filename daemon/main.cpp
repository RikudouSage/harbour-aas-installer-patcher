#include <QCoreApplication>
#include <QDBusConnection>

constexpr int errorNotConnected = 2;
constexpr int errorServiceNotRegistered = 3;

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    auto bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        return errorNotConnected;
    }

    if (!bus.registerService("dev.chrastecky.aas_patcher.daemon")) {
        return errorServiceNotRegistered;
    }

    return app.exec();
}
