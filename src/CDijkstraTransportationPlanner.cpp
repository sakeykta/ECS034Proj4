#include "DijkstraPathRouter.h"
#include "StreetMap.h"
#include "DijkstraTransportationPlanner.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

struct CDijkstraTransportationPlanner::SImplementation {
    std::shared_ptr<CTransportationPlanner::SConfiguration> DConfig;
    std::shared_ptr<CPathRouter> DPathRouter;

    SImplementation(std::shared_ptr<CTransportationPlanner::SConfiguration> config)
        : DConfig(config) {
        // Initialize path router
        DPathRouter = std::make_shared<CDijkstraPathRouter>();
        Precompute();
    }

    // Precompute paths using Dijkstra's algorithm
    void Precompute() {
        auto StreetMap = DConfig->StreetMap();
        for (std::size_t SrcIndex = 0; SrcIndex < StreetMap->NodeCount(); ++SrcIndex) {
            for (std::size_t DestIndex = 0; DestIndex < StreetMap->NodeCount(); ++DestIndex) {
                auto SrcNode = StreetMap->NodeByIndex(SrcIndex);
                auto DestNode = StreetMap->NodeByIndex(DestIndex);
                auto SrcID = SrcNode->ID();
                auto DestID = DestNode->ID();
                std::vector<CPathRouter::TVertexID> Path;
                if (SrcID != DestID) {
                    double distance = CalculateDistance(SrcNode, DestNode);
                    DPathRouter->AddEdge(SrcID, DestID, distance);
                }
            }
        }
    }

    // Method to calculate distance between two nodes in the street map
    double CalculateDistance(std::shared_ptr<CStreetMap::SNode> src, std::shared_ptr<CStreetMap::SNode> dest) {
        // Example implementation (replace with actual distance calculation)
        return 0.0; // Placeholder
    }
};

CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config)
    : DImplementation(std::make_unique<SImplementation>(config)) {}

CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner() = default;

std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept {
    return DImplementation->DConfig->StreetMap()->NodeCount();
}

std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept {
    return DImplementation->DConfig->StreetMap()->NodeByIndex(index);
}



double CDijkstraTransportationPlanner::FindShortestPath(CTransportationPlanner::TNodeID src, CTransportationPlanner::TNodeID dest, std::vector<CTransportationPlanner::TNodeID> &path) {
    // Use the correct path type for path storage
    std::vector<CPathRouter::TVertexID> convertedPath;
    double distance = DImplementation->DPathRouter->FindShortestPath(src, dest, convertedPath);
    if (distance != CPathRouter::NoPathExists) {
        // Convert the path to the expected type (if necessary)
        path.resize(convertedPath.size()); // Ensure path has enough capacity
        std::copy(convertedPath.begin(), convertedPath.end(), path.begin());
    } else {
        path.clear();
    }
    return distance;
}

double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path) {
    // Not implemented in this version
    return CPathRouter::NoPathExists;
}

bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const {
    // Not implemented in this version
    return false;
}
