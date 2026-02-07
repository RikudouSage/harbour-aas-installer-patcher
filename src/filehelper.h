#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>

class FileHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = nullptr);
    Q_INVOKABLE bool fileExists(const QString &path) const;
    const QByteArray readFile(const QString &path);

signals:

};

#endif // FILEHELPER_H
