#include "XMLFile.h"
#include "FileSystem.h"

namespace CgEngine {
    XMLFile* CgEngine::XMLFile::createResource(const std::string &name) {
        return new XMLFile(name);
    }

    XMLFile::XMLFile(const std::string &name) {
        xmlDocument.load_string(FileSystem::readFileToString(name).c_str());
    }

    const pugi::xml_document& XMLFile::getXMLDocument() {
        return xmlDocument;
    }

}
