#ifndef PACKAGESXMLHANDLER_H
#define PACKAGESXMLHANDLER_H

#include <QObject>
#include <QMap>
#include <QFile>

class PackagesXmlHandler : public QObject
{
    Q_OBJECT
public:
    explicit PackagesXmlHandler(QObject *parent = nullptr);
    const QMap<QString, QString> packageInstallerMap(const QByteArray &fileContent) const;

signals:


private:
    bool isAbx(const QByteArray &fileContent) const;
    const QByteArray convertFromAbx(const QByteArray &data) const;
    const QByteArray convertToAbx(const QByteArray &data) const;
};

#endif // PACKAGESXMLHANDLER_H
