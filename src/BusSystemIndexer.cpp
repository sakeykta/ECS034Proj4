#include "BusSystemIndexer.h"
#include <vector>
#include <unordered_map>
#include <algorithm> // Include for std::sort
#include <functional>

struct pair_hash {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& p) const {
        std::size_t seed = 0;
        std::hash<T> hash1;
        std::hash<U> hash2;
        seed ^= hash1(p.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash2(p.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

struct CBusSystemIndexer::SImplementation {
    std::shared_ptr<CBusSystem> DBusSystem;
    std::vector<std::shared_ptr<SStop>> DSortedStops;
    std::unordered_map<TNodeID, std::shared_ptr<SStop>> DNodeIDToStop;
    std::unordered_map<std::pair<TNodeID, TNodeID>, std::unordered_set<std::shared_ptr<SRoute>>, pair_hash> DSrcDestToRoutes;

    static bool StopIDCompare(std::shared_ptr<SStop> left, std::shared_ptr<SStop> right) {
        return left->ID() < right->ID();
    }

    SImplementation(std::shared_ptr<CBusSystem> bussystem) : DBusSystem(bussystem) {
        for (size_t Index = 0; Index < DBusSystem->StopCount(); Index++) {
            auto CurrentStop = DBusSystem->StopByIndex(Index);
            DSortedStops.push_back(CurrentStop);
            DNodeIDToStop[CurrentStop->NodeID()] = CurrentStop;
        }
        std::sort(DSortedStops.begin(), DSortedStops.end(), StopIDCompare);
        for (size_t Index = 0; Index < DBusSystem->RouteCount(); Index++) {
            auto CurrentRoute = DBusSystem->RouteByIndex(Index);
            for (size_t StopIndex = 1; StopIndex < CurrentRoute->StopCount(); StopIndex++) {
                auto SourceNodeID = DBusSystem->StopByID(CurrentRoute->GetStopID(StopIndex - 1))->NodeID();
                auto DestinationNodeID = DBusSystem->StopByID(CurrentRoute->GetStopID(StopIndex))->NodeID();
                auto SearchKey = std::make_pair(SourceNodeID, DestinationNodeID);

                auto Search = DSrcDestToRoutes.find(SearchKey);
                if (Search != DSrcDestToRoutes.end()) {
                    Search->second.insert(CurrentRoute);
                } else {
                    DSrcDestToRoutes[SearchKey] = {CurrentRoute};
                }
            }
        }
    }

    std::size_t StopCount() const noexcept {
        return DBusSystem->StopCount();
    }

    std::size_t RouteCount() const noexcept {
        return DBusSystem->RouteCount();
    }
};

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem) : DImplementation(std::make_unique<SImplementation>(bussystem)) {
}

CBusSystemIndexer::~CBusSystemIndexer() = default;

std::size_t CBusSystemIndexer::StopCount() const noexcept {
    return DImplementation->DBusSystem->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept {
    return DImplementation->DBusSystem->RouteCount();
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept {
    return DImplementation->DSortedStops[index];
}

std::shared_ptr<CBusSystemIndexer::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept {
    return DImplementation->DBusSystem->RouteByIndex(index);
}

std::shared_ptr<CBusSystemIndexer::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept {
    return DImplementation->DNodeIDToStop.find(id) != DImplementation->DNodeIDToStop.end() ? DImplementation->DNodeIDToStop[id] : nullptr;
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept {
    std::unordered_set<std::shared_ptr<SRoute>> routes;
    return RoutesByNodeIDs(src, dest, routes);
}


bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>>& routes) const noexcept {
    auto search = DImplementation->DSrcDestToRoutes.find(std::make_pair(src, dest));

    if (search != DImplementation->DSrcDestToRoutes.end()) {
        routes.insert(search->second.begin(), search->second.end());
        return true;
    }

    return false;
}
