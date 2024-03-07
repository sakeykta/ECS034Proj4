#include <OpenStreetMap.h>
#include <BusSystem.h>
#include <CSVBusSystem.h>
#include <DSVReader.h>
#include <DSVWriter.h>
#include <StreetMap.h>
#include <StringDataSink.h>
#include <StringDataSource.h>
#include <StringUtils.h>
#include <stdlib.h>
#include <unordered_map>
#include <iostream>

// Structure to hold implementation details
struct CCSVBusSystem::SImplementation {
    std::vector<std::shared_ptr<CBusSystem::SStop>> Stops; 
    std::unordered_map<CBusSystem::TStopID, std::shared_ptr<CBusSystem::SStop>> StopsById; 
    std::vector<std::shared_ptr<CBusSystem::SRoute>> Routes;
    std::unordered_map<CBusSystem::TStopID, std::shared_ptr<CBusSystem::SRoute>> RoutesById; 
    
    // Struct representing a stop
    struct SStop : public CBusSystem::SStop {
        CBusSystem::TStopID id; 
        CStreetMap::TNodeID nodeID; 

        // Constructor
        SStop(CBusSystem::TStopID id, CStreetMap::TNodeID nodeID) : id(id), nodeID(nodeID) {}

        // Methods to retrieve stop information
        CBusSystem::TStopID ID() const noexcept override { return id; }
        CStreetMap::TNodeID NodeID() const noexcept override { return nodeID; }
    };

    // Struct representing a route
    struct SRoute : public CBusSystem::SRoute {
        std::string name; 
        std::vector<CBusSystem::TStopID> stopIDs; 

        // Constructor
        SRoute(const std::string& name, const std::vector<CBusSystem::TStopID>& stopIDs) : name(name), stopIDs(stopIDs) {}

        // Methods to retrieve route information
        std::string Name() const noexcept override { return name; }
        std::size_t StopCount() const noexcept override { return stopIDs.size(); }
        CBusSystem::TStopID GetStopID(std::size_t index) const noexcept override {
            if (index < stopIDs.size()) return stopIDs[index];
            else return CBusSystem::InvalidStopID;
        }
    };
};

// Destructor for CCSVBusSystem
CCSVBusSystem::~CCSVBusSystem() {}

// Method to get the count of stops
std::size_t CCSVBusSystem::StopCount() const noexcept {
    return DImplementation->Stops.size();
}

// Method to get the count of routes
std::size_t CCSVBusSystem::RouteCount() const noexcept {
    return DImplementation->Routes.size();
}

// Method to get a stop by its index
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Stops.size()) {
        return DImplementation->Stops[index]; 
    } else {
        return nullptr;
    }
}

// Method to get a stop by its ID
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(CBusSystem::TStopID id) const noexcept {
    auto it = DImplementation->StopsById.find(id);
    if (it != DImplementation->StopsById.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

// Method to get a route by its index
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Routes.size()) {
        return DImplementation->Routes[index]; 
    } else {
        return nullptr; 
    }
}

// Method to get a route by its name
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string& name) const noexcept {
    std::string nameLower = StringUtils::Lower(name);
    for (const auto& route : DImplementation->Routes) {
        if (StringUtils::Lower(route->Name()) == nameLower) {}
    }
    return nullptr; 
}

// Constructor for CCSVBusSystem
CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) : DImplementation(std::make_unique<SImplementation>()) {
    std::vector<std::string> row;

    while (!stopsrc->End()) {
        if (stopsrc->ReadRow(row)) {
            try {
                TStopID id = std::stoull(row[0]);
                CStreetMap::TNodeID nodeID = std::stoull(row[1]); 
                auto stop = std::make_shared<SImplementation::SStop>(id, nodeID); 
                DImplementation->Stops.push_back(stop);
                DImplementation->StopsById[id] = stop;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument error: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range error: " << e.what() << std::endl;
            }
        }
    }
}