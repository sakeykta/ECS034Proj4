#include "XMLReader.h"
#include "StringDataSource.h"
#include <expat.h>
#include <iostream>
#include <queue> 

// Structure to hold implementation details
struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;
    XML_Parser DXMLParser;
    std::queue<SXMLEntity> DEntityQueue;
    bool DInCDATA;

    // Constructor
    SImplementation(std::shared_ptr<CDataSource> src) : DDataSource(src), DInCDATA(false) {
        DXMLParser = XML_ParserCreate(NULL);
        XML_SetUserData(DXMLParser, this);
        XML_SetElementHandler(DXMLParser, StartElementHandlerCallback, EndElementHandlerCallback);
        XML_SetCharacterDataHandler(DXMLParser, CharacterDataHandlerCallback);
    }

    // Destructor
    ~SImplementation() {
        XML_ParserFree(DXMLParser);
    }

    // Handler for XML start elements
    static void XMLCALL StartElementHandlerCallback(void *userData, const XML_Char *name, const XML_Char **atts) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(userData);
        ReaderObject->StartElementHandler(name, atts);
    }

    // Handler for XML end elements
    static void XMLCALL EndElementHandlerCallback(void *userData, const XML_Char *name) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(userData);
        ReaderObject->EndElementHandler(name);
    }

    // Handler for XML character data
    static void XMLCALL CharacterDataHandlerCallback(void *userData, const XML_Char *s, int len) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(userData);
        ReaderObject->CharacterDataHandler(s, len);
    }

    // Handler for XML start elements
    void StartElementHandler(const XML_Char *name, const XML_Char **atts) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::StartElement;

        for (int i = 0; atts[i]; i += 2) {
            TempEntity.SetAttribute(atts[i], atts[i + 1]);
        }

        DEntityQueue.push(TempEntity);
    }

    // Handler for XML end elements
    void EndElementHandler(const XML_Char *name) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::EndElement;
        DEntityQueue.push(TempEntity);
    }

    // Handler for XML character data
    void CharacterDataHandler(const XML_Char *s, int len) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = std::string(s, len);
        TempEntity.DType = SXMLEntity::EType::CharData;
        DEntityQueue.push(TempEntity);
    }

    // Check if end of data source is reached
    bool End() const {
        return DEntityQueue.empty() && DDataSource->End();
    }

    // Read XML entity from data source
    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        if (DInCDATA) {
            if (DEntityQueue.front().DType == SXMLEntity::EType::EndElement) {
                DInCDATA = false;
                DEntityQueue.pop();
            } else {
                entity = DEntityQueue.front();
                DEntityQueue.pop();
                return true;
            }
        }

        while (DEntityQueue.empty() && !DDataSource->End()) {
            std::vector<char> DataBuffer;
            if (DDataSource->Read(DataBuffer, 256)) {
                XML_Parse(DXMLParser, DataBuffer.data(), DataBuffer.size(), false);
            } else {
                XML_Parse(DXMLParser, nullptr, 0, true);
            }
        }

        if (!DEntityQueue.empty()) {
            entity = DEntityQueue.front();
            DEntityQueue.pop();
            if (!skipcdata && entity.DType == SXMLEntity::EType::CharData) {
                DInCDATA = true;
            }
            return true;
        }

        return false;
    }
};

// Constructor
CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src) {
    DImplementation = std::make_unique<SImplementation>(src);
}

// Destructor
CXMLReader::~CXMLReader() {
}

// Check if end of data source is reached
bool CXMLReader::End() const {
    return DImplementation->End();
}

// Read XML entity from data source
bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}
