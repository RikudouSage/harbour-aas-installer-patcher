#include "daemon.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMessage>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QDebug>

namespace {
constexpr char kActionExists[] = "dev.chrastecky.aas_patcher.daemon.file_exists";
constexpr char kActionRead[] = "dev.chrastecky.aas_patcher.daemon.read_file";
constexpr char kActionWrite[] = "dev.chrastecky.aas_patcher.daemon.write_file";
} // namespace

bool Daemon::fileExists(const QString &path) {
    if (!authorize(kActionExists)) {
        return false;
    }
    return QFileInfo::exists(path);
}

QByteArray Daemon::readFile(const QString &path) {
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
    QDBusMessage pidMsg = QDBusMessage::createMethodCall(
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus",
        "GetConnectionUnixProcessID");
    pidMsg << sender;
    const auto pidReply = QDBusConnection::systemBus().call(pidMsg);
    if (pidReply.type() == QDBusMessage::ErrorMessage || pidReply.arguments().isEmpty()) {
        qWarning() << "Failed to resolve sender PID for" << sender << ":" << pidReply.errorName() << pidReply.errorMessage();
        return false;
    }

    bool ok = false;
    const quint32 pid = pidReply.arguments().at(0).toUInt(&ok);
    if (!ok || pid == 0) {
        qWarning() << "Invalid sender PID for" << sender;
        return false;
    }
    qDebug() << "Polkit subject resolved for action" << actionId << "sender" << sender << "pid" << pid;

    QDBusMessage msg = QDBusMessage::createMethodCall(
        "org.freedesktop.PolicyKit1",
        "/org/freedesktop/PolicyKit1/Authority",
        "org.freedesktop.PolicyKit1.Authority",
        "CheckAuthorization");

    QVariantMap subjectDetails;
    subjectDetails.insert("pid", pid);
    subjectDetails.insert("start-time", static_cast<qulonglong>(0));

    QDBusArgument subject;
    subject.beginStructure();
    subject << QString("unix-process") << subjectDetails;
    subject.endStructure();

    const QMap<QString, QString> details{
        {QStringLiteral("AllowUserInteraction"), QStringLiteral("true")}
    };
    const quint32 flags = 1; // Allow user interaction via polkit agent
    const QString cancellationId;

    msg << QVariant::fromValue(subject)
        << actionId
        << QVariant::fromValue(details)
        << flags
        << cancellationId;

    QDBusMessage reply;
    for (int attempt = 0; attempt < 2; ++attempt) {
        reply = QDBusConnection::systemBus().call(msg, QDBus::Block, 600000);
        if (reply.type() == QDBusMessage::ErrorMessage &&
            reply.errorName() == QLatin1String("org.freedesktop.DBus.Error.ServiceUnknown")) {
            qWarning() << "Polkit service unavailable, retrying authorization call";
            continue;
        }
        break;
    }

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
    QMap<QString, QString> resultDetails;

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
