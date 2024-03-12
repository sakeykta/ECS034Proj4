#include "XMLReader.h"
#include <expat.h>
#include <iostream>
#include <queue>

struct CXMLReader::SImplementation{
    std::shared_ptr< CDataSource > DDataSource;
    XML_Parser DXMLParser;
    std::queue<SXMLEntity> DEntityQueue;

    void StartElementHandler(const std::string &name, const std::vector< std::string > &attrs){
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::StartElement;
        for(size_t i = 0; i < attrs.size(); i += 2){
            TempEntity.SetAttribute(attrs[i],attrs[i+1]);
        }
        DEntityQueue.push(TempEntity);
    }

    void EndElementHandler(const std::string &name){
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::EndElement;
        DEntityQueue.push(TempEntity);
    }

    void CharacterDataHandler(const std::string &cdata){
        SXMLEntity TempEntity;
        TempEntity.DNameData = cdata;
        TempEntity.DType = SXMLEntity::EType::CharData;
        DEntityQueue.push(TempEntity);
    }

    static void StartElementHandlerCallback(void *context, const XML_Char *name, const XML_Char **atts){
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        std::vector< std::string > Attributes;
        auto AttrPtr = atts;
        while(*AttrPtr){
            Attributes.push_back(*AttrPtr);
            AttrPtr++;
        }
        ReaderObject->StartElementHandler(name, Attributes);
    }

    static void EndElementHandlerCallBack(void *context, const XML_Char *name){
        SImplementation *ReaderObject = static_cast<SImplementation *> (context);
        ReaderObject->EndElementHandler(name);

    }

    static void CharacterDataHandlerCallback( void *context, const XML_Char *s, int len){
        SImplementation *ReaderObject = static_cast<SImplementation *> (context);
        ReaderObject->CharacterDataHandler(std::string(s,len));
    }

    SImplementation(std::shared_ptr< CDataSource > src){
        // TODO Write the code
        DDataSource = src;
        DXMLParser = XML_ParserCreate(NULL);
        XML_SetStartElementHandler(DXMLParser, StartElementHandlerCallback);
        XML_SetEndElementHandler(DXMLParser, EndElementHandlerCallBack);
        XML_SetCharacterDataHandler(DXMLParser, CharacterDataHandlerCallback);
        XML_SetUserData(DXMLParser, this);
    };

    bool End() const{
        return DDataSource->End();
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        // Read from the source if necessary
        // Pass to XML_Parse Function
        // Return entity

        std::vector<char> DataBuffer;

        while(DEntityQueue.empty()){

			if (End())
				return false;

            if(DDataSource->Read(DataBuffer, 2048))
                XML_Parse(DXMLParser,DataBuffer.data(), DataBuffer.size(), DataBuffer.size() < 2048);
            
            else
                XML_Parse(DXMLParser, DataBuffer.data(), 0, true);
            
        }

        if(DEntityQueue.empty())
            return false;
        

        entity = DEntityQueue.front();

		if (skipcdata) {
			while (entity.DType == SXMLEntity::EType::CharData) {
				DEntityQueue.pop();
				entity = DEntityQueue.front();
			}
		}

        DEntityQueue.pop();
        return true;
    }
};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){
    DImplementation = std::make_unique< SImplementation > (src);
}

CXMLReader::~CXMLReader() {}

bool CXMLReader::End() const{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity, skipcdata);
}

