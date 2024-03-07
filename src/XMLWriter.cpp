#include "XMLWriter.h"
#include "XMLEntity.h"
#include "DataSink.h"
#include "DataSource.h"
#include "expat.h"
#include <queue>

// Define the implementation struct
struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    std::shared_ptr<CDataSource> DDataSource;
    XML_Parser DXMLParser;
    using EntityQueue = std::queue<SXMLEntity>;
    EntityQueue DEntityQueue;

    // Start element handler function
    void StartElementHandler(const std::string &name, const std::vector<std::string> &attrs) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::StartElement;
        for (size_t Index = 0; Index < attrs.size(); Index += 2) {
            TempEntity.SetAttribute(attrs[Index], attrs[Index + 1]);
        }
        DEntityQueue.push(TempEntity);
    }

    // End element handler function
    void EndElementHandler(const std::string &name) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::EndElement;
        DEntityQueue.push(TempEntity);
    }

    // Character data handler callback function
    static void XMLCALL CharacterDataHandlerCallback(void *context, const XML_Char *s, int len) {
        SImplementation *WriterObject = static_cast<SImplementation *>(context);
        std::string data(s, len);
        SXMLEntity TempEntity;
        TempEntity.DNameData = data;
        TempEntity.DType = SXMLEntity::EType::CharData;
        WriterObject->DEntityQueue.push(TempEntity);
    }

    // Constructor
    SImplementation(std::shared_ptr<CDataSink> sink)
        : DDataSink(sink), DXMLParser(XML_ParserCreate(NULL)) {
        XML_SetStartElementHandler(DXMLParser, StartElementHandlerCallback);
        XML_SetEndElementHandler(DXMLParser, EndElementHandlerCallback);
        XML_SetCharacterDataHandler(DXMLParser, CharacterDataHandlerCallback);
        XML_SetUserData(DXMLParser, this);
    }

    // Destructor
    ~SImplementation() {
        XML_ParserFree(DXMLParser);
    }

    // Start element handler callback function
    static void XMLCALL StartElementHandlerCallback(void *context, const XML_Char *name, const XML_Char **atts) {
        SImplementation *WriterObject = static_cast<SImplementation *>(context);
        std::vector<std::string> Attributes;
        auto AttrPtr = atts;
        while (*AttrPtr) {
            Attributes.push_back(*AttrPtr);
            AttrPtr++;
        }
        WriterObject->StartElementHandler(name, Attributes);
    }

    // End element handler callback function
    static void XMLCALL EndElementHandlerCallback(void *context, const XML_Char *name) {
        SImplementation *WriterObject = static_cast<SImplementation *>(context);
        WriterObject->EndElementHandler(name);
    }
};

// Constructor
CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink)) {
    // Initialize your XML writer
}

// Destructor
CXMLWriter::~CXMLWriter() = default;

// Flush function
bool CXMLWriter::Flush() {

    if (!DImplementation->DDataSink) {
        return false;
    }

    std::vector<char> remainingDataBuffer(256); 

    while (DImplementation->DDataSource && !DImplementation->DDataSource->End()) {
        if (!DImplementation->DDataSource->Read(remainingDataBuffer, remainingDataBuffer.size())) {
            return false;
        }
        if (!DImplementation->DDataSink->Write(remainingDataBuffer)) {
            return false;
        }
    }
    return true;
}

// Write entity function
bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {

    if (!DImplementation->DDataSink) {
        return false; 
    }

    std::string xmlString;
    if (entity.DType == SXMLEntity::EType::StartElement) {
        xmlString = "<" + entity.DNameData;
        for (const auto &attr : entity.DAttributes) {
            xmlString += " " + attr.first + "=\"" + attr.second + "\"";
        }
        xmlString += ">";
    } else if (entity.DType == SXMLEntity::EType::EndElement) {
        xmlString = "</" + entity.DNameData + ">";
    } else if (entity.DType == SXMLEntity::EType::CompleteElement) {
        xmlString = "<" + entity.DNameData;
        for (const auto &attr : entity.DAttributes) {
            xmlString += " " + attr.first + "=\"" + attr.second + "\"";
        }
        xmlString += "/>";
    } else if (entity.DType == SXMLEntity::EType::CharData) {
        xmlString = entity.DNameData;
    }

    if (!DImplementation->DDataSink->Write(std::vector<char>(xmlString.begin(), xmlString.end()))) {
        return false; 
    }


    return true;
}