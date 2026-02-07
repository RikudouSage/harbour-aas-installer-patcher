#include "packagesxmlhandler.h"

#include <QXmlStreamReader>

#include <cstring>
#include <sstream>

#include "../third_party/abx/src/cpp/abx2xml/abx2xml.hpp"
#include "../third_party/abx/src/cpp/xml2abx/xml2abx.hpp"

PackagesXmlHandler::PackagesXmlHandler(QObject *parent) : QObject(parent)
{

}

const QMap<QString, QString> PackagesXmlHandler::packageInstallerMap(const QByteArray &fileContent) const
{
    auto xmlBytes = fileContent;
    if (isAbx(fileContent)) {
        xmlBytes = convertFromAbx(xmlBytes);
    }

    QXmlStreamReader xml(fileContent);

    QMap<QString, QString> result;
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
