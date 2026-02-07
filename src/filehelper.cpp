#include "filehelper.h"

#include <QFile>
#include <QProcess>
#include <QDebug>

FileHelper::FileHelper(QObject *parent) : QObject(parent)
{

}

bool FileHelper::fileExists(const QString &path) const
{
    const QFile file(path);
    if (file.exists()) {
        return true;
    }

    // todo daemon
    return false;
}

const QByteArray FileHelper::readFile(const QString &path)
{
    if (!fileExists(path)) {
        throw QString("The file '" + path + "' does not exist");
    }

    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }

    // todo daemon
    return QByteArray();
}
