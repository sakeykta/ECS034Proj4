#include "XMLReader.h"
#include "XMLEntity.h"
#include <expat.h>
#include <queue>
#include <iostream>
struct CXMLReader::SImplementation{
    std::shared_ptr < CDataSource > DDataSource;
    XML_Parser DXMLParser;
    std::queue<SXMLEntity> DEntityQueue;

void StartElementHandler(const std::string &name, const std::vector<std::string> &attrs){
    SXMLEntity TempEntity;
    TempEntity.DNameData = name;
    TempEntity.DType = SXMLEntity::EType::StartElement;

    // Debug: Print the name of the element
    std::cout << "StartElementHandler: Element name: " << name << std::endl;

    // Debug: Print the attributes
    std::cout << "StartElementHandler: Attributes:" << std::endl;
    for (size_t i = 0; i < attrs.size(); i += 2) {
        std::string attributeName = attrs[i];
        std::string attributeValue = attrs[i + 1];
        std::cout << "  " << attributeName << " = " << attributeValue << std::endl;
    }

    // Set attributes for the entity
    for(size_t i = 0; i < attrs.size(); i += 2){
        TempEntity.SetAttribute(attrs[i], attrs[i+1]); // set attributes using given attributes

        // Debug: Print each attribute as it's being set
        std::cout << "StartElementHandler: Set attribute: " << attrs[i] << " = " << attrs[i+1] << std::endl;
    }

    // Push the entity to the queue
    DEntityQueue.push(TempEntity); // push newly made entity to queue

    // Debug: Print a message indicating the entity has been pushed to the queue
    std::cout << "StartElementHandler: Entity pushed to queue." << std::endl;
}


    void EndElementHandler(const std::string &name){
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::EndElement; // the entity type will signal that it is the end of the objects
        DEntityQueue.push(TempEntity);
    }
    
    void CharacterDataHandler(const std::string &cdata){
        SXMLEntity TempEntity;
        TempEntity.DNameData = cdata;
        TempEntity.DType = SXMLEntity::EType::CharData;
        if(!DEntityQueue.empty() && DEntityQueue.back().DType ==  SXMLEntity::EType::CharData){ // checking if previous entity is chardata as well (merge)
        // also, used https://cplusplus.com/reference/queue/queue/back/ here
            DEntityQueue.back().DNameData += cdata; // merging
        }
        else // push entity as normal
            DEntityQueue.push(TempEntity);
    }
    
    static void StartElementHandlerCallback(void *context, const XML_Char *name, const XML_Char **atts){
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        std::vector<std::string> Attributes;
        auto AttrPtr = atts;
        while(*AttrPtr){
            Attributes.push_back(*AttrPtr);
            AttrPtr++;
        } // format given attributes to call StartElementHandler()
        ReaderObject->StartElementHandler(name,Attributes);
    }

    static void EndElementHandlerCallback(void *context, const XML_Char *name){
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        ReaderObject->EndElementHandler(name);
    }

    static void CharacterDataHandlerCallback(void *context, const XML_Char *s, int len){
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        ReaderObject->CharacterDataHandler(std::string(s, len));
    }

    SImplementation(std::shared_ptr< CDataSource > src){ // link functions we made using XML BIFs
        DDataSource = src;
        DXMLParser = XML_ParserCreate(NULL);
        XML_SetStartElementHandler(DXMLParser, StartElementHandlerCallback);
        XML_SetEndElementHandler(DXMLParser, EndElementHandlerCallback);
        XML_SetCharacterDataHandler(DXMLParser, CharacterDataHandlerCallback);
        XML_SetUserData(DXMLParser,this);
    }

    bool End() const{
        return DEntityQueue.empty() && DDataSource->End(); // both are true, then we are done reading
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        std::vector<char> DataBuffer;
        while(DEntityQueue.empty() && !DDataSource->End()){ // second condition handles empty src
            if(DDataSource->Read(DataBuffer,256)) // if there are at least 256 chars, read them all
                XML_Parse(DXMLParser, DataBuffer.data(), DataBuffer.size(), DataBuffer.size() < 256);
            else // otherwise, read what's left
                XML_Parse(DXMLParser, DataBuffer.data(),0,true);
        }
        if(DEntityQueue.empty()) // nothing was read
            return false;
        entity = DEntityQueue.front();
        DEntityQueue.pop(); // queue operations
        return true;
    }
};

 CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){
    DImplementation = std::make_unique< SImplementation > (src);
 }  

 CXMLReader::~CXMLReader(){

 }     

bool CXMLReader::End() const{
    return DImplementation->End();
}
        
bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity,skipcdata);
}
