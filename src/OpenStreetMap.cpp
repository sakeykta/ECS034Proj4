#include "../include/OpenStreetMap.h"
#include "../include/StreetMap.h"
#include "../include/XMLReader.h"
#include <unordered_map>
#include <iostream>
#include <vector>

struct COpenStreetMap::SImplementation{

    // Stores a mapping from NodeID to SNode
    std::unordered_map<TNodeID, std::shared_ptr<CStreetMap::SNode> > DNodeIDToNode;
    // Stores all Nodes in a vector, each of which can be accessed by the Index
    std::vector< std::shared_ptr<CStreetMap::SNode> > DNodesByIndex;

    // Stores a mapping from the WayID to the SWay
    std::unordered_map<TWayID, std::shared_ptr<CStreetMap::SWay> > DWayIDToWay;
    // Stores all Ways in a vector, each of which can be accessed by the Index
    std::vector< std::shared_ptr<CStreetMap::SWay> > DWayByIndex;


    SImplementation(std::shared_ptr< CXMLReader > src){
        SXMLEntity TempEntity;

        while(src->ReadEntity(TempEntity,true)){
            if((TempEntity.DNameData == "osm") && (SXMLEntity::EType::EndElement == TempEntity.DType)){
                //reached end
                break;
            }

            else if ((TempEntity.DNameData == "node") && (SXMLEntity::EType::StartElement == TempEntity.DType)){
                //parse node
                TNodeID NewNodeID = std::stoull(TempEntity.AttributeValue("id"));

                // Create pair for location
                double Lat = std::stod(TempEntity.AttributeValue("lat"));
                double Lon = std::stod(TempEntity.AttributeValue("lon"));
                TLocation NewNodeLocation = std::make_pair(Lat,Lon);

                // Make the New Node
                auto NewNode = std::make_shared<SNode>(NewNodeID, NewNodeLocation);

                // Add to vector and unordered map
                DNodesByIndex.push_back(NewNode);
                DNodeIDToNode[NewNodeID] = NewNode;
                while(src->ReadEntity(TempEntity,true)){
                    // reached end of node
                    if((TempEntity.DNameData == "node") && (SXMLEntity::EType::EndElement == TempEntity.DType)){
                        break;
                    }
                    // Set the tag attributes if they exist
                    else if ((TempEntity.DNameData == "tag") && (SXMLEntity::EType::StartElement == TempEntity.DType)){
                        NewNode->SetAttribute(TempEntity.AttributeValue("k"), TempEntity.AttributeValue("v"));
                    }
                }
            }

            // Way tag encountered
            else if ((TempEntity.DNameData == "way") && (SXMLEntity::EType::StartElement == TempEntity.DType)){
                // Create New Way struct
                TWayID NewWayID = std::stoull(TempEntity.AttributeValue("id")); //get new id
                auto NewWay = std::make_shared<SWay>(NewWayID); // create new sway

                // push the NewWay to vector and unordered map
                DWayByIndex.push_back(NewWay);
                DWayIDToWay[NewWayID] = NewWay;
                while(src->ReadEntity(TempEntity, true)){
                    if((TempEntity.DNameData == "way") && (SXMLEntity::EType::EndElement == TempEntity.DType)){
                        break; // end of way
                    }
                    // Add Node references
                    else if ((TempEntity.DNameData == "nd") && (SXMLEntity::EType::StartElement == TempEntity.DType)){
                        NewWay->SetNode(std::stoull(TempEntity.AttributeValue("ref")));
                    }
                    // Set relevant attributes 
                    else if ((TempEntity.DNameData == "tag") && (SXMLEntity::EType::StartElement == TempEntity.DType)){
                        NewWay->SetAttribute(TempEntity.AttributeValue("k"), TempEntity.AttributeValue("v"));
                    }

                }
            }
        }
    }

    struct SNode : public CStreetMap::SNode{

        TNodeID DID;
        TLocation DLocation;
        std::unordered_map <std::string, std::string> DAttributes;
        std::vector <std::string> DAttributeKeys;

        // SNode Constructor
        SNode(TNodeID id, TLocation loc){
            DID = id;
            DLocation = loc;
        }

        // SNode Destructor
        ~ SNode(){
        }

        // Returns ID of Node
        TNodeID ID() const noexcept override{
            return DID;
        }

        // Returns Location with Latitude and Longitude
        TLocation Location() const noexcept override{
            return DLocation;
        }

        // Returns the number of Attributes for the Node
        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }

        // Gets an attribute key based on the index
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if (index < DAttributeKeys.size()){
                return DAttributeKeys[index];
            }
            return std::string();
        }
        
        // Returns True if Node has attribute
        bool HasAttribute(const std::string &key) const noexcept override{
            auto Search = DAttributes.find(key);
            return DAttributes.end() != Search; // if equal to end, we didn't find it
        }

        // Returns value given attribute key
        std::string GetAttribute(const std::string &key) const noexcept override{
            auto Search = DAttributes.find(key);
            if(DAttributes.end() != Search){
                return Search->second;
            }
            return std::string();
        }

        // Sets attribute to DAttributes (vector and map) while parsing
        void SetAttribute(const std::string &key, const std::string &value){
            DAttributeKeys.push_back(key);
            DAttributes[key] = value;
        }
    };

    struct SWay : public CStreetMap::SWay{
        TWayID DID;
        std::vector <CStreetMap::TNodeID> DNodes;
        std::unordered_map <std::string, std::string> DAttributes;
        std::vector <std::string> DAttributeKeys;

        // Constructor for SWay
        SWay(TWayID id){
            DID = id;
        }
        
        // Destructor for SWay
        ~SWay(){

        }

        //Returns the Way ID
        TWayID ID() const noexcept override{
            return DID;
        }

        // Returns the number of nodes in the way
        std::size_t NodeCount() const noexcept override{
            return DNodes.size();
        }

        // Returns the ID given an index in the way
        TNodeID GetNodeID(std::size_t index) const noexcept override{
            if (index < DNodes.size()){
                return DNodes[index];
            }
            return InvalidNodeID;
        }

        // Returns the number of attributes for the way
        std::size_t AttributeCount() const noexcept override{
            return DAttributeKeys.size();
        }

        // Gets an attributes key given an index in the way
        std::string GetAttributeKey(std::size_t index) const noexcept override{
            if (index < DAttributeKeys.size()){
                return DAttributeKeys[index];
            }
            return std::string();
        }

        // Returns True if the way has attribute with key "key"
        bool HasAttribute(const std::string &key) const noexcept override{
            auto Search = DAttributes.find(key);
            return DAttributes.end() != Search; 
        }

        // Returns value of attribute given key
        std::string GetAttribute(const std::string &key) const noexcept override{
            auto Search = DAttributes.find(key);
            if(DAttributes.end() != Search){
                return Search->second;
            }
            return std::string();
        }

        // Updates map and vector while parsing way for attributes
        void SetAttribute(const std::string &key, const std::string &value){
            DAttributeKeys.push_back(key);
            DAttributes[key] = value;
        }

        // Updates vector of nodes while parsing way for nodes
        void SetNode(const CStreetMap::TNodeID &NodeID){
            DNodes.push_back(NodeID);
        }
    };

    // Returns total number of nodes parsed
    std::size_t NodeCount() const{
        return DNodesByIndex.size();
    }

    // Returns total number of ways parsed
    std::size_t WayCount() const{
        return DWayByIndex.size();  
    }

    // Returns a node given an Index
    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const{
        if(index < DNodesByIndex.size()){
            return DNodesByIndex[index];
        }
        return nullptr;
    }

    // Returns a node given an id
    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const{
        auto Search = DNodeIDToNode.find(id);
        if(DNodeIDToNode.end() != Search){
            return Search->second;
        }
        return nullptr;
    }

    // Returns a Way given an index
    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const {
        if(index < DWayByIndex.size()){
            return DWayByIndex[index];
        }
        return nullptr;
    }

    // Returns a Way given an id
    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const{

        auto Search = DWayIDToWay.find(id);
        if(DWayIDToWay.end() != Search){
            return Search->second;
        }
        return nullptr;
    }
};

// COpenStreetMap member functions
// Constructor for the Open Street Map
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src){
    DImplementation = std::make_unique< SImplementation > (src);
}

// Destructor for the Open Street Map
COpenStreetMap::~COpenStreetMap(){

}

// Returns the number of nodes in the map
std::size_t COpenStreetMap::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}

// Returns the number of ways in the map
std::size_t COpenStreetMap::WayCount() const noexcept{
    return DImplementation->WayCount();
}

// Returns the SNode associated with index, returns nullptr if index is
// larger than or equal to NodeCount()
std::shared_ptr<COpenStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept{
    return DImplementation->NodeByIndex(index);
}
// Returns the SNode with the id of id, returns nullptr if doesn't exist
std::shared_ptr<COpenStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept{
    return DImplementation->NodeByID(id);
}

// Returns the SWay associated with index, returns nullptr if index is
// larger than or equal to WayCount()
std::shared_ptr<COpenStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept{
    return DImplementation->WayByIndex(index);
}

// Returns the SWay with the id of id, returns nullptr if doesn't exist
std::shared_ptr<COpenStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept{
    return DImplementation->WayByID(id);
}
