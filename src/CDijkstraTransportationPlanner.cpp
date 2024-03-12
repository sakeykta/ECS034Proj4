#include "DijkstraTransportationPlanner.h"
#include "DijkstraPathRouter.h"
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <iostream>

struct CDijkstraTransportationPlanner::SImplementation{
    std::shared_ptr< CStreetMap > DStreetMap;
    std::shared_ptr< CBusSystem > DBusSystem;
    std::unordered_map< CStreetMap::TNodeID, CPathRouter::TVertexID > DNodeToVertexID;
    CDijkstraPathRouter DShortestPathRouter;
    CDijkstraPathRouter DFastestPathRouterBike;
    CDijkstraPathRouter DFastestPathRouterWalkBus;

    double walkSpeed;
    double bikeSpeed;
    double busSpeed;


    SImplementation(std::shared_ptr<SConfiguration> config){
        DStreetMap = config->StreetMap();
        DBusSystem = config->BusSystem();

        walkSpeed = config->WalkSpeed();
        bikeSpeed = config->BikeSpeed();
        busSpeed = config->DefaultSpeedLimit();

        for(size_t Index = 0; Index < DStreetMap->NodeCount(); Index++){
            auto Node = DStreetMap->NodeByIndex(Index);
            auto VertexID = DShortestPathRouter.AddVertex(Node->ID());
            
            DFastestPathRouterBike.AddVertex(Node->ID());
            DFastestPathRouterWalkBus.AddVertex(Node->ID());
            DNodeToVertexID[Node->ID()] = VertexID;
        }
        for(size_t Index = 0; Index < DStreetMap->WayCount(); Index++) {
            auto Way = DStreetMap->WayByIndex(Index);
            for (size_t NodeIndex = 1; NodeIndex < Way->NodeCount(); NodeIndex++) {
            auto PrevNode = DStreetMap->NodeByID(Way->GetNodeID(NodeIndex - 1));
            auto CurrNode = DStreetMap->NodeByID(Way->GetNodeID(NodeIndex));
            auto SrcVertexID = DNodeToVertexID[PrevNode->ID()];
            auto DestVertexID = DNodeToVertexID[CurrNode->ID()];

            // Convert latitudes and longitudes from degrees to radians
            double LatRad1 = PrevNode->Location().first * M_PI / 180.0;
            double LonRad1 = PrevNode->Location().second * M_PI / 180.0;
            double LatRad2 = CurrNode->Location().first * M_PI / 180.0;
            double LonRad2 = CurrNode->Location().second * M_PI / 180.0;

            // Compute deltas for latitude and longitude
            double DeltaLat = LatRad2 - LatRad1;
            double DeltaLon = LonRad2 - LonRad1;

            // Calculate components for the Haversine formula
            double a = pow(sin(DeltaLat / 2), 2) + cos(LatRad1) * cos(LatRad2) * pow(sin(DeltaLon / 2), 2);
            double c = 2 * atan2(sqrt(a), sqrt(1 - a));

            // Compute the distance in miles
            double DistanceMiles = 3959.88 * c;

            // Check for bidirectional attribute and add edge(s) accordingly
            bool Bidirectional = Way->GetAttribute("oneway") != "yes";
            DShortestPathRouter.AddEdge(SrcVertexID, DestVertexID, DistanceMiles, Bidirectional);
        }
        }
    }

    std::size_t NodeCount() const noexcept{
        return DStreetMap->NodeCount();
    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) noexcept{
        //returns nullptr for invalid index
        if(index >= DStreetMap->NodeCount()){
            return nullptr;
        }
        //create vector and store node IDs
        std::vector<CStreetMap::TNodeID> sortedNodeIDs;
        for(size_t i = 0; i < DStreetMap->NodeCount(); ++i){
            sortedNodeIDs.push_back(DStreetMap->NodeByIndex(i)->ID());
        }
        //CHATGPT USAGE, sorts nodes by ID
        std::sort(sortedNodeIDs.begin(), sortedNodeIDs.end());
        //retrieve node ID at given index
        auto nodeID = sortedNodeIDs[index];
        //return the node with the retrieved ID
        return DStreetMap->NodeByID(nodeID);

    }

    double FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path) {
    if (DNodeToVertexID.find(src) == DNodeToVertexID.end() || DNodeToVertexID.find(dest) == DNodeToVertexID.end()) {
        return CPathRouter::NoPathExists;
    }

    std::vector<CPathRouter::TVertexID> ShortestPath;
    auto SourceVertexID = DNodeToVertexID[src];
    auto DestinationVertexID = DNodeToVertexID[dest];
    auto Distance = DShortestPathRouter.FindShortestPath(SourceVertexID, DestinationVertexID, ShortestPath);

    if (Distance == CPathRouter::NoPathExists) {
        path.clear();
        return CPathRouter::NoPathExists;
    }

    path.clear();
    for (auto VertexID : ShortestPath) {
        try {
            auto nodeID = std::any_cast<TNodeID>(DShortestPathRouter.GetVertexTag(VertexID));
            path.push_back(nodeID);
        } catch (const std::bad_any_cast& e) {
            std::cerr << "Error: Bad any_cast" << std::endl;
            return CPathRouter::NoPathExists;
        }
    }

    return Distance;
}



    double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path){

            std::vector < CPathRouter::TVertexID > FastestPath1;
            std::vector < CPathRouter::TVertexID > FastestPath2;

            if((!DStreetMap->NodeByID(src)) || (!DStreetMap->NodeByID(dest))){
                return CPathRouter::NoPathExists;
            }
            auto SourceVertexID = DNodeToVertexID[src];
            auto DestinationVertexID = DNodeToVertexID[dest];
            auto BikeFastestSpeed = DFastestPathRouterBike.FindShortestPath(SourceVertexID, DestinationVertexID, FastestPath1);
            auto WalkBusSpeed = DFastestPathRouterWalkBus.FindShortestPath(SourceVertexID, DestinationVertexID, FastestPath2);
            if((CPathRouter::NoPathExists ==  BikeFastestSpeed) && (CPathRouter::NoPathExists == WalkBusSpeed)){
                return CPathRouter::NoPathExists;
            }
            path.clear();
            if((BikeFastestSpeed > WalkBusSpeed) || (CPathRouter::NoPathExists == WalkBusSpeed)){
                for(auto VertexID : FastestPath1){
                    path.push_back(std::any_cast<TTripStep> (std::make_pair (ETransportationMode::Bike, DNodeToVertexID[VertexID])));
                }
            } else if((BikeFastestSpeed < WalkBusSpeed) || (CPathRouter::NoPathExists == BikeFastestSpeed)){
                for(auto VertexID : FastestPath2){
                    path.push_back(std::any_cast<TTripStep> (std::make_pair (ETransportationMode::Walk, DNodeToVertexID[VertexID])));
                }
            }
        }




    bool GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const{
        return true; //needs implementation
    }
};

CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config){
    DImplementation = std::make_unique<SImplementation>(config);
}

CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner(){

}

// Returns the number of nodes in the street map
std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept{
    return DImplementation->NodeCount();
}
// Returns the street map node specified by index if index is less than the
// NodeCount(). nullptr is returned if index is greater than or equal to
// NodeCount(). The nodes are sorted by Node ID.
std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept{
    return DImplementation->SortedNodeByIndex(index);
}
// Returns the distance in miles between the src and dest nodes of the
// shortest path if one exists. NoPathExists is returned if no path exists.
// The nodes of the shortest path are filled in the path parameter.
double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path){
    if (!DImplementation) {
        return CPathRouter::NoPathExists;
    }
    return DImplementation->FindShortestPath(src, dest, path);
}
// Returns the time in hours for the fastest path between the src and dest
// nodes of the if one exists. NoPathExists is returned if no path exists.
// The transportation mode and nodes of the fastest path are filled in the
// path parameter.
double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path){
    if (!DImplementation) {
        return CPathRouter::NoPathExists;
    }
    return DImplementation->FindFastestPath(src, dest, path);
}
// Returns true if the path description is created. Takes the trip steps path
// and converts it into a human readable set of steps.
bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const{
    return true;
}
