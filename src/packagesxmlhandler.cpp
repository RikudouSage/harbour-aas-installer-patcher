#include "packagesxmlhandler.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <cstring>
#include <sstream>

#include "../third_party/abx/src/cpp/abx2xml/abx2xml.hpp"
#include "../third_party/abx/src/cpp/xml2abx/xml2abx.hpp"

PackagesXmlHandler::PackagesXmlHandler(QObject *parent) : QObject(parent)
{

}

const PackageInstallerMap PackagesXmlHandler::packageInstallerMap(const QByteArray &fileContent) const
{
    auto xmlBytes = fileContent;
    if (isAbx(fileContent)) {
        xmlBytes = convertFromAbx(xmlBytes);
    }

#ifdef QT_DEBUG
    QFile tmpOut("/tmp/packages.orig.xml");
    if (tmpOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        tmpOut.write(xmlBytes);
        tmpOut.close();
    }
#endif

    QXmlStreamReader xml(xmlBytes);

    PackageInstallerMap result;
    while (!xml.atEnd()) {
        xml.readNext();

        if (!xml.isStartElement()) {
            continue;
        }

        if (xml.name() != "package") {
            continue;
        }

        const auto attributes = xml.attributes();

        const auto package = attributes.value("name").toString();
        const auto installer = attributes.value("installer").toString();

        if (package.isEmpty() || installer.isEmpty()) {
            continue;
        }

        result.insert(package, installer);
    }

    return result;
}

const QByteArray PackagesXmlHandler::updateInstallerMap(const QByteArray &fileContent,PackageInstallerMap updatedMap)
{
    auto wasAbx = false;

    QByteArray xmlBytes = fileContent;
    if (isAbx(fileContent)) {
        wasAbx = true;
        xmlBytes = convertFromAbx(xmlBytes);
    }

    QXmlStreamReader reader(xmlBytes);

    QByteArray outputBytes;
    QXmlStreamWriter writer(&outputBytes);
    writer.setAutoFormatting(true);

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartDocument()) {
            writer.writeStartDocument();
            continue;
        }

        if (reader.isEndDocument()) {
            writer.writeEndDocument();
            continue;
        }

        if (reader.isStartElement()) {
            const QString elementName = reader.name().toString();

            if (elementName == "package") {
                auto attributes = reader.attributes();

                QString packageName = attributes.value("name").toString();

                if (updatedMap.contains(packageName)) {
                    QString newInstaller = updatedMap.value(packageName);

                    QXmlStreamAttributes newAttrs;
                    for (const auto &attr : attributes) {
                        if (attr.name() == "installer") {
                            continue;
                        }
                        newAttrs.append(attr);
                    }

                    newAttrs.append("installer", newInstaller);

                    writer.writeStartElement("package");
                    writer.writeAttributes(newAttrs);
                }
                else {
                    writer.writeStartElement("package");
                    writer.writeAttributes(attributes);
                }

                continue;
            }

            writer.writeStartElement(elementName);
            writer.writeAttributes(reader.attributes());
            continue;
        }

        if (reader.isEndElement()) {
            writer.writeEndElement();
            continue;
        }

        if (reader.isCharacters()) {
            writer.writeCharacters(reader.text().toString());
            continue;
        }

        if (reader.isComment()) {
            writer.writeComment(reader.text().toString());
            continue;
        }
    }

    if (reader.hasError()) {
        throw QString("XML parse error: %1").arg(reader.errorString());
    }

#ifdef QT_DEBUG
    QFile tmpOut("/tmp/packages.out.xml");
    if (tmpOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        tmpOut.write(outputBytes);
        tmpOut.close();
    }
#endif

    if (wasAbx) {
        return convertToAbx(outputBytes);
    }

    return outputBytes;
}

bool PackagesXmlHandler::isAbx(const QByteArray &fileContent) const
{
    if (fileContent.size() < 4) {
        return false;
    }

    return std::memcmp(fileContent.constData(), "ABX\0", 4) == 0;
}

const QByteArray PackagesXmlHandler::convertFromAbx(const QByteArray &data) const
{
    std::istringstream in(std::string(data.constData(), data.size()), std::ios::binary);
    std::ostringstream out;

    abx2xml::AbxToXmlConverter::convert(in, out);

    return QByteArray::fromStdString(out.str());
}

const QByteArray PackagesXmlHandler::convertToAbx(const QByteArray &data) const
{
    std::ostringstream out(std::ios::binary);
    xml2abx::XmlToAbxConverter::convert_from_string(
        std::string(data.constData(), data.size()),
        out,
        true
    );

    return QByteArray::fromStdString(out.str());
}
