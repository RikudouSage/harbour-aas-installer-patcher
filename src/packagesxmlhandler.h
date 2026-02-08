#ifndef PACKAGESXMLHANDLER_H
#define PACKAGESXMLHANDLER_H

#include <QObject>
#include <QMap>
#include <QFile>

typedef QMap<QString, QString> PackageInstallerMap;

class PackagesXmlHandler : public QObject
{
    Q_OBJECT
public:
    explicit PackagesXmlHandler(QObject *parent = nullptr);
    const PackageInstallerMap packageInstallerMap(const QByteArray &fileContent) const;
    const QByteArray updateInstallerMap(const QByteArray &fileContent, PackageInstallerMap updatedMap);

signals:


private:
    bool isAbx(const QByteArray &fileContent) const;
    const QByteArray convertFromAbx(const QByteArray &data) const;
    const QByteArray convertToAbx(const QByteArray &data) const;
};

#endif // PACKAGESXMLHANDLER_H
