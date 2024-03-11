#include "CSVBusSystem.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm> // Required for std::find

// Internal implementation of the CSV Bus System
struct CCSVBusSystem::SImplementation{

    // Internal struct representing a stop
    struct SStop : public CBusSystem::SStop{
        // Attributes
        CBusSystem::TStopID DID; // Stop ID
        CStreetMap::TNodeID DNodeID; // Node ID
        
        // Constructor
        SStop(CBusSystem::TStopID id, CStreetMap::TNodeID node_id){
            DID = id;
            DNodeID = node_id;
        }
        
        // Destructor
        ~SStop(){}
        
        // Overrides base class method to return stop ID
        CBusSystem::TStopID ID() const noexcept override{
            return DID;
        }
        
        // Overrides base class method to return node ID
        CStreetMap::TNodeID NodeID() const noexcept override{
            return DNodeID;
        }
    };

    // Internal struct representing a route
    struct SRoute : public CBusSystem::SRoute{
        // Attributes
        std::string DName; // Route name
        std::vector<CBusSystem::TStopID> DStopIDs; // IDs of stops in the route

        // Constructor
        SRoute(std::string name, std::vector<CBusSystem::TStopID> stops){
            DName = name;
            DStopIDs = stops;
        }
        
        // Destructor
        ~SRoute(){}

        // Overrides base class method to return route name
        std::string Name() const noexcept override{
            return DName;
        }

        // Overrides base class method to return number of stops in the route
        std::size_t StopCount() const noexcept override{
            return DStopIDs.size();
        }

        // Overrides base class method to return the stop ID at a given index
        CBusSystem::TStopID GetStopID(std::size_t index) const noexcept override{
            if(index < StopCount()) // Check if index is in range
                return DStopIDs[index]; // Return the stop ID at the given index
            return -1; // Return invalid stop ID if index is out of range
        }
        
        // Method to add a stop ID to the route
        void AddStopID(CBusSystem::TStopID &stop_id){
            // Check if the stop ID already exists in the vector
            if (std::find(DStopIDs.begin(), DStopIDs.end(), stop_id) == DStopIDs.end()) {
                // If not found, add it to the vector
                DStopIDs.push_back(stop_id);
            }
        }
    };

    // Maps that store routes and stops
    std::unordered_map<CBusSystem::TStopID, std::shared_ptr<SStop>> DStopIDtoStop; // Map of stop IDs to stop objects
    std::vector<std::shared_ptr<SStop>> DStopsByIndex; // Vector of stop objects

    std::unordered_map<std::string, std::shared_ptr<SRoute>> DRouteNametoRoute; // Map of route names to route objects
    std::vector<std::shared_ptr<SRoute>> DRoutesByIndex; // Vector of route objects

    // Constructor
    SImplementation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){
        std::vector<std::string> stop_row;
        std::vector<std::string> route_row;

        // Read first rows of both CSV files (contain headers)
        stopsrc->ReadRow(stop_row); // Format: stop_id,node_id
        routesrc->ReadRow(route_row); // Format: route,stop_id

        // Read stops from the CSV files
        while(stopsrc->ReadRow(stop_row)){ // Stops are one-to-one with nodes
            CBusSystem::TStopID stop_id = std::stoull(stop_row[0]);
            CStreetMap::TNodeID node_id = std::stoull(stop_row[1]);
            auto NewStop = std::make_shared<SStop>(stop_id, node_id); // Create new stop object
            DStopIDtoStop[stop_id] = NewStop; // Add to stop ID to stop object map
            DStopsByIndex.push_back(NewStop); // Add to vector of stops
        }

        // Read routes from the CSV files
        while(routesrc->ReadRow(route_row)){ // Routes are not one-to-one with stops
            std::string route_name = route_row[0]; // Route name
            CBusSystem::TStopID stop_id = std::stoull(route_row[1]); // Stop ID
            auto search = DRouteNametoRoute.find(route_name);
            if(search == DRouteNametoRoute.end()){ // New route!
                std::vector<CBusSystem::TStopID> stop_ids; // Create new vector of Stop IDs
                stop_ids.push_back(stop_id);
                auto NewRoute = std::make_shared<SRoute>(route_name, stop_ids); // Create new route object
                DRoutesByIndex.push_back(NewRoute); // Add to vector of routes
                DRouteNametoRoute[route_name] = NewRoute; // Add to route name to route object map
            }
            else // Add new stop to existing route
                search->second->AddStopID(stop_id); // Search already has the route we are looking for, add there
        }
    }

    // Method to get the number of stops
    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
    }
    
    // Method to get the number of routes
    std::size_t RouteCount() const noexcept{
        return DRoutesByIndex.size();
    }
    
    // Method to get a stop by index
    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        if(index < DStopsByIndex.size())
            return DStopsByIndex[index];
        return nullptr;
    }
    
    // Method to get a stop by ID
    std::shared_ptr<SStop> StopByID(CBusSystem::TStopID id) const noexcept{
        auto search = DStopIDtoStop.find(id);
        if(search != DStopIDtoStop.end())
            return search->second;
        return nullptr;
    }
    
    // Method to get a route by index
    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
        if(index < DRoutesByIndex.size())
            return DRoutesByIndex[index];
        return nullptr;
    }
    
    // Method to get a route by name
    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{
        auto search = DRouteNametoRoute.find(name);
        if(search != DRouteNametoRoute.end())
            return search->second;
        return nullptr;
    }
};

// Constructor for CSV Bus System
CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){
    DImplementation = std::make_unique<SImplementation>(stopsrc, routesrc);
}

// Destructor for CSV Bus System
CCSVBusSystem::~CCSVBusSystem(){
    // Empty
}

// Method to get the number of stops
std::size_t CCSVBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();
}

// Method to get the number of routes
std::size_t CCSVBusSystem::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

// Method to get a stop by index
std::shared_ptr<CCSVBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);
}

// Method to get a stop by ID
std::shared_ptr<CCSVBusSystem::SStop> CCSVBusSystem::StopByID(CBusSystem::TStopID id) const noexcept{
    return DImplementation->StopByID(id);
}

// Method to get a route by index
std::shared_ptr<CCSVBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);
}

// Method to get a route by name
std::shared_ptr<CCSVBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);
}
