#include "daemon.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMessage>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

namespace {
constexpr char kActionExists[] = "dev.chrastecky.aas_patcher.daemon.file_exists";
constexpr char kActionRead[] = "dev.chrastecky.aas_patcher.daemon.read_file";
constexpr char kActionWrite[] = "dev.chrastecky.aas_patcher.daemon.write_file";
constexpr int kIdleShutdownTimeoutMs = 60'000;

QTimer *idleShutdownTimer()
{
    static QTimer *timer = nullptr;
    if (timer == nullptr) {
        timer = new QTimer(qApp);
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, []() {
            qInfo() << "No DBus requests received, shutting down daemon";
            QCoreApplication::quit();
        });
    }
    return timer;
}

void armIdleShutdownTimer()
{
    idleShutdownTimer()->start(kIdleShutdownTimeoutMs);
}
} // namespace

bool Daemon::fileExists(const QString &path) {
    armIdleShutdownTimer();
    if (!authorize(kActionExists)) {
        return false;
    }
    return QFileInfo::exists(path);
}

QByteArray Daemon::readFile(const QString &path) {
    armIdleShutdownTimer();
    if (!authorize(kActionRead)) {
        return {};
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        sendErrorReply(QDBusError::Failed, QString("Failed to open file for reading: %1").arg(path));
        return {};
    }
    return file.readAll();
}

bool Daemon::writeFile(const QString &path, const QByteArray &data) {
    armIdleShutdownTimer();
    if (!authorize(kActionWrite)) {
        return false;
    }
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        sendErrorReply(QDBusError::Failed, QString("Failed to open file for writing: %1").arg(path));
        return false;
    }
    if (file.write(data) != data.size()) {
        sendErrorReply(QDBusError::Failed, QString("Failed to write all data to: %1").arg(path));
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        sendErrorReply(QDBusError::Failed, QString("Failed to commit file: %1").arg(path));
        return false;
    }
    return true;
}

bool Daemon::authorize(const QString &actionId) {
    if (!calledFromDBus()) {
        return false;
    }

    const auto sender = message().service();
    if (!polkitCheckAuthorization(sender, actionId)) {
        sendErrorReply(QDBusError::AccessDenied, "Not authorized");
        return false;
    }
    return true;
}

bool Daemon::polkitCheckAuthorization(const QString &sender, const QString &actionId) {
    QDBusMessage msg = QDBusMessage::createMethodCall(
        "org.freedesktop.PolicyKit1",
        "/org/freedesktop/PolicyKit1/Authority",
        "org.freedesktop.PolicyKit1.Authority",
        "CheckAuthorization");

    QVariantMap subjectDetails;
    subjectDetails.insert("name", sender);

    QDBusArgument subject;
    subject.beginStructure();
    subject << QString("system-bus-name") << subjectDetails;
    subject.endStructure();

    const QMap<QString, QString> details;
    const quint32 flags = 0; // No user interaction in daemon context
    const QString cancellationId;

    msg << QVariant::fromValue(subject)
        << actionId
        << QVariant::fromValue(details)
        << flags
        << cancellationId;

    const auto reply = QDBusConnection::systemBus().call(msg);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Polkit call failed for action" << actionId << "sender" << sender << ":"
                   << reply.errorName() << reply.errorMessage();
        return false;
    }

    if (reply.arguments().isEmpty()) {
        return false;
    }

    bool isAuthorized = false;
    bool isChallenge = false;
    QVariantMap resultDetails;

    const auto arg = reply.arguments().at(0).value<QDBusArgument>();
    arg.beginStructure();
    arg >> isAuthorized >> isChallenge >> resultDetails;
    arg.endStructure();

    if (!isAuthorized) {
        qWarning() << "Polkit denied action" << actionId
                   << "sender" << sender
                   << "challenge" << isChallenge
                   << "details" << resultDetails;
    }

    return isAuthorized;
}
