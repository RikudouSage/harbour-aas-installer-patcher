#include "filehelper.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

namespace {
constexpr char kServiceName[] = "dev.chrastecky.aas_patcher.daemon";
constexpr char kObjectPath[] = "/dev/chrastecky/aas_patcher/Daemon";
constexpr char kInterfaceName[] = "dev.chrastecky.aas_patcher.Daemon";
}

FileHelper::FileHelper(QObject *parent) : QObject(parent)
{

}

bool FileHelper::fileExists(const QString &path) const
{
    if (QFileInfo::exists(path)) {
        return true;
    }

    return daemonFileExists(path);
}

const QByteArray FileHelper::readFile(const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }

    const auto localError = file.errorString();
    QString daemonError;
    const auto daemonContent = daemonReadFile(path, &daemonError);
    if (daemonError.isEmpty()) {
        return daemonContent;
    }

    throw QString("The file '%1' could not be read. Local error: %2. Daemon error: %3")
            .arg(path, localError, daemonError);
}

bool FileHelper::writeToFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return file.write(content) == content.size();
    }

    const auto localError = file.errorString();
    QString daemonError;

    auto result = daemonWriteFile(path, content, &daemonError);
    if (daemonError.isEmpty()) {
        return result;
    }

    throw QString("The file '%1' could not be written to. Local error: %2. Daemon error: %3")
            .arg(path, localError, daemonError);
}

bool FileHelper::daemonFileExists(const QString &path) const
{
    const auto bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        qWarning() << "System DBus is not connected";
        return false;
    }

    QDBusInterface daemon(kServiceName, kObjectPath, kInterfaceName, bus);
    const QDBusReply<bool> reply = daemon.call("fileExists", path);

    if (!reply.isValid()) {
        qWarning() << "Daemon fileExists call failed for" << path << ":" << reply.error().message();
        return false;
    }

    return reply.value();
}

QByteArray FileHelper::daemonReadFile(const QString &path, QString *errorMessage) const
{
    const auto bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("system DBus is not connected");
        }
        return {};
    }

    QDBusInterface daemon(kServiceName, kObjectPath, kInterfaceName, bus);
    const QDBusReply<QByteArray> reply = daemon.call("readFile", path);

    if (!reply.isValid()) {
        if (errorMessage) {
            *errorMessage = QString("%1 (%2)").arg(reply.error().message(), reply.error().name());
        }
        qWarning() << "Daemon readFile call failed for" << path << ":" << reply.error().message();
        return {};
    }

    if (errorMessage) {
        errorMessage->clear();
    }
    return reply.value();
}

bool FileHelper::daemonWriteFile(const QString &path, const QByteArray &content, QString *errorMessage)
{
    const auto bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("system DBus is not connected");
        }
        return false;
    }

    QDBusInterface daemon(kServiceName, kObjectPath, kInterfaceName, bus);
    const QDBusReply<bool> reply = daemon.call("writeFile", path, content);

    if (!reply.isValid()) {
        if (errorMessage) {
            *errorMessage = QString("%1 (%2)").arg(reply.error().message(), reply.error().name());
        }
        qWarning() << "Daemon writeFile call failed for" << path << ":" << reply.error().message();
        return false;
    }

    if (errorMessage) {
        errorMessage->clear();
    }

    return reply.value();
}
