#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QByteArray>
#include <QObject>

class FileHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = nullptr);
    Q_INVOKABLE bool fileExists(const QString &path) const;
    const QByteArray readFile(const QString &path);

private:
    bool daemonFileExists(const QString &path) const;
    QByteArray daemonReadFile(const QString &path, QString *errorMessage = nullptr) const;
};

#endif // FILEHELPER_H
