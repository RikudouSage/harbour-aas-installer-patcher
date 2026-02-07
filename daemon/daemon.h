#pragma once

#include <QDBusContext>
#include <QObject>

class Daemon final : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "dev.chrastecky.aas_patcher.Daemon")

public:
    using QObject::QObject;

public slots:
    bool fileExists(const QString &path);
    QByteArray readFile(const QString &path);
    bool writeFile(const QString &path, const QByteArray &data);

private:
    bool authorize(const QString &actionId);
    bool polkitCheckAuthorization(const QString &sender, const QString &actionId);
};
