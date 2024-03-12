#include <memory>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include "BusSystem.h" // Assuming this is the header file for CBusSystem
#include "StreetMap.h" // Assuming this is the header file for CStreetMap
// You may need to include other necessary headers

class CBusSystemIndexer {
private:
    struct SImplementation {
        std::shared_ptr<CBusSystem> m_busSystem; // Pointer to the CBusSystem
        std::vector<std::shared_ptr<CBusSystem::SStop>> m_sortedStops; // Sorted vector of stops by ID
        std::vector<std::shared_ptr<CBusSystem::SRoute>> m_sortedRoutes; // Sorted vector of routes by Name

        void IndexStops();
        void IndexRoutes();
    };
    std::unique_ptr<SImplementation> DImplementation;

public:
    using TNodeID = CStreetMap::TNodeID;
    using TStopID = CBusSystem::TStopID;
    using SStop = CBusSystem::SStop;
    using SRoute = CBusSystem::SRoute;

    CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem);
    ~CBusSystemIndexer();

    std::size_t StopCount() const noexcept;
    std::size_t RouteCount() const noexcept;
    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept;
    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept;
    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept;
    bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>>& routes) const noexcept;
    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept;
};

// Implement member functions here

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem)
    : DImplementation(std::make_unique<SImplementation>()) {
    DImplementation->m_busSystem = bussystem;
    // Initialize the indexer with the provided bus system
    DImplementation->IndexStops();
    DImplementation->IndexRoutes();
}

CBusSystemIndexer::~CBusSystemIndexer() = default;

std::size_t CBusSystemIndexer::StopCount() const noexcept {
    return DImplementation->m_sortedStops.size();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept {
    return DImplementation->m_sortedRoutes.size();
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->m_sortedStops.size())
        return DImplementation->m_sortedStops[index];
    else
        return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->m_sortedRoutes.size())
        return DImplementation->m_sortedRoutes[index];
    else
        return nullptr;
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept {
    auto it = std::find_if(DImplementation->m_sortedStops.begin(), DImplementation->m_sortedStops.end(),
                           [id](const std::shared_ptr<SStop>& stop) {
                               return stop->NodeID() == id;
                           });
    if (it != DImplementation->m_sortedStops.end())
        return *it;
    else
        return nullptr;
}

bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest,
                                        std::unordered_set<std::shared_ptr<CBusSystem::SRoute>>& routes) const noexcept {
    for (const auto& route : DImplementation->m_sortedRoutes) {
        // Check if both src and dest nodes exist in the route
        bool hasSrc = false;
        bool hasDest = false;
        for (std::size_t i = 0; i < route->StopCount(); ++i) {
            if (route->GetStopID(i) == src)
                hasSrc = true;
            if (route->GetStopID(i) == dest)
                hasDest = true;
        }
        if (hasSrc && hasDest) {
            routes.insert(route);
        }
    }
    return !routes.empty();
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept {
    for (const auto& route : DImplementation->m_sortedRoutes) {
        // Check if both src and dest nodes exist in the route
        bool hasSrc = false;
        bool hasDest = false;
        for (std::size_t i = 0; i < route->StopCount(); ++i) {
            if (route->GetStopID(i) == src)
                hasSrc = true;
            if (route->GetStopID(i) == dest)
                hasDest = true;
        }
        if (hasSrc && hasDest) {
            return true;
        }
    }
    return false;
}

void CBusSystemIndexer::SImplementation::IndexStops() {
    m_sortedStops.clear();
    std::unordered_set<int> uniqueStopIDs; // To store unique stop IDs
    for (std::size_t i = 0; i < m_busSystem->StopCount(); ++i) {
        auto stop = m_busSystem->StopByIndex(i);
        int stopID = stop->ID();
        if (uniqueStopIDs.find(stopID) == uniqueStopIDs.end()) {
            std::cout << "Added stop with ID: " << stopID << ", Node ID: " << stop->NodeID() << std::endl; // Debug print
            m_sortedStops.push_back(stop);
            uniqueStopIDs.insert(stopID);
        }
    }

    // Print contents of uniqueStopIDs set
    std::cout << "Unique stop IDs:";
    for (auto id : uniqueStopIDs) {
        std::cout << " " << id;
    }
    std::cout << std::endl;

    std::sort(m_sortedStops.begin(), m_sortedStops.end(),
              [](const std::shared_ptr<CBusSystem::SStop>& lhs, const std::shared_ptr<CBusSystem::SStop>& rhs) {
                  return lhs->ID() < rhs->ID();
              });
}

void CBusSystemIndexer::SImplementation::IndexRoutes() {
    m_sortedRoutes.clear();
    for (std::size_t i = 0; i < m_busSystem->RouteCount(); ++i) {
        auto route = m_busSystem->RouteByIndex(i);
        std::string routeSignature = ""; // Signature representing the route's stop sequence
        
        // Construct route signature by appending each stop's ID
        for (std::size_t j = 0; j < route->StopCount(); ++j) {
            int stopID = route->GetStopID(j);
            routeSignature += std::to_string(stopID) + "-";
        }

        // Print route signature for debugging
        std::cout << "Route signature: " << routeSignature << std::endl;

        // Add the route to m_sortedRoutes
        m_sortedRoutes.push_back(route);
    }

    // Sort routes alphabetically by name
    std::sort(m_sortedRoutes.begin(), m_sortedRoutes.end(),
              [](const std::shared_ptr<CBusSystem::SRoute>& lhs, const std::shared_ptr<CBusSystem::SRoute>& rhs) {
                  return lhs->Name() < rhs->Name();
              });
}


