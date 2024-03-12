#include "BusSystemIndexer.h"
#include "XMLReader.h"
#include "DSVReader.h"
#include "StringUtils.h"
#include "StringDataSource.h"
#include "CSVBusSystem.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>

struct CBusSystemIndexer::SImplementation {
	// https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key
	
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1,T2> &p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);

			// Mainly for demonstration purposes, i.e. works but is overly simple
			// In the real world, use sth. like boost.hash_combine
			return h1 ^ h2;  
		}
	};

	std::shared_ptr<CBusSystem> BusSystem;
	std::vector<std::shared_ptr<SStop> > sortedStops;
	std::vector<std::shared_ptr<SRoute> > sortedRoutes;
	std::unordered_map < TNodeID, std::shared_ptr<SStop> > NodeIDtoStop;
	std::unordered_map< std::pair< TNodeID, TNodeID >, std::unordered_set< std::shared_ptr<SRoute> >, pair_hash > SrcDestToRoutes;

	static bool StopIDCompare(std::shared_ptr<SStop> left, std::shared_ptr<SStop> right) {
		return left->ID() < right->ID();
	}

	static bool RouteNameCompare(std::shared_ptr<SRoute> left, std::shared_ptr<SRoute> right) {
		return left->Name() < right->Name();
	}

	SImplementation(std::shared_ptr<CBusSystem> bussystem) {
		
		BusSystem = bussystem;
		
		// Populate with stops
		for (size_t i = 0; i < BusSystem->StopCount(); i++) {
			auto currentStop = (BusSystem->StopByIndex(i));
			sortedStops.push_back(currentStop);
			NodeIDtoStop[currentStop->NodeID()] = currentStop;
		}

		// Sort stops
		std::sort(sortedStops.begin(), sortedStops.end(), StopIDCompare);

		// Populate with routes
		for (size_t i = 0; i < BusSystem->RouteCount(); i++) {
			auto currentRoute = BusSystem->RouteByIndex(i);
			sortedRoutes.push_back(currentRoute);
			
			// Connect stops in the route
			for (size_t stopindex = 1; stopindex < currentRoute->StopCount(); stopindex++) {
				auto src = BusSystem->StopByID(currentRoute->GetStopID(stopindex - 1))->NodeID();
				auto dest = BusSystem->StopByID(currentRoute->GetStopID(stopindex))->NodeID();

				auto searchkey = std::make_pair(src, dest);
				auto search = SrcDestToRoutes.find(searchkey);

				// Checks if route connecting src & dest stops already exists
				if (search != SrcDestToRoutes.end()) 
					search->second.insert(currentRoute);
				else
					SrcDestToRoutes[searchkey] = {currentRoute};
			}
		}

		// Sort routes
		std::sort(sortedRoutes.begin(), sortedRoutes.end(), RouteNameCompare);
	}

	std::size_t StopCount() const noexcept {
		return BusSystem->StopCount();
	}

	std::size_t RouteCount() const noexcept {
		return BusSystem->RouteCount();
	}

	std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept {
		if (0 <= index < sortedStops.size()) 
			return sortedStops[index];

		return nullptr;
	}

	std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept {
		if (0 <= index < sortedRoutes.size())
			return sortedRoutes[index];
		
		return nullptr;
	}

	std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept {
		auto search = NodeIDtoStop.find(id);
		if (search != NodeIDtoStop.end()) 
			return search->second;
		
		return nullptr;
	}

	bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute> > &routes) {	
		auto searchkey = std::make_pair(src, dest);
		auto search = SrcDestToRoutes.find(searchkey);

		if (search != SrcDestToRoutes.end()) {
			routes = search->second;
			return true;
		} 	
		return false;
	}

	bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) {
		auto searchkey = std::make_pair(src, dest);
		auto search = SrcDestToRoutes.find(searchkey);
		
		return search != SrcDestToRoutes.end();
	}
};

CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem) {
	DImplementation = std::make_unique<SImplementation>(bussystem);
}

CBusSystemIndexer::~CBusSystemIndexer() {}

std::size_t CBusSystemIndexer::StopCount() const noexcept {
	return DImplementation->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept {
	return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept {
	return DImplementation->SortedStopByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept {
	return DImplementation->SortedRouteByIndex(index);
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::StopByNodeID(CStreetMap::TNodeID id) const noexcept {
	return DImplementation->StopByNodeID(id);
}

bool CBusSystemIndexer::RoutesByNodeIDs(CStreetMap::TNodeID src, CStreetMap::TNodeID dest, std::unordered_set<std::shared_ptr<CBusSystem::SRoute> > &routes) const noexcept {
	return DImplementation->RoutesByNodeIDs(src, dest, routes);
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(CStreetMap::TNodeID src, CStreetMap::TNodeID dest) const noexcept {
	return DImplementation->RouteBetweenNodeIDs(src, dest);
}
