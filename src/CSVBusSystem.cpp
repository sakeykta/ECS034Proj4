#include "CSVBusSystem.h"
#include "DataSource.h"
#include "StringDataSource.h"
#include <iostream>
#include <memory>
#include <vector>

struct CCSVBusSystem::SImplementation {

    struct SStop : public CBusSystem::SStop{
        TStopID stopID;
        CStreetMap::TNodeID nodeID;
		
		SStop(TStopID s, CStreetMap::TNodeID n) {
			stopID = s;
			nodeID = n;
		}
		~SStop(){};

        TStopID ID() const noexcept{
            return stopID;
        }

        CStreetMap::TNodeID NodeID() const noexcept{
            return nodeID;
		}
    };

    struct SRoute : public CBusSystem::SRoute{
        ~SRoute(){};
        std::string name;
        std::vector<TStopID> routestops;

		SRoute(std::string n, TStopID s) {
			name = n;
			routestops.push_back(s);
		}

        std::string Name() const noexcept{
            return name;
        }

        std::size_t StopCount() const noexcept{
            return routestops.size();
        }

        TStopID GetStopID(std::size_t index) const noexcept{
            if (0 <= index && index < routestops.size())
				return routestops[index];
			else	
				return CBusSystem::InvalidStopID;
		}  
    };

    std::vector< std::shared_ptr<SRoute> > routes;
	std::vector< std::shared_ptr<SStop> > stops;

    SImplementation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){
        FillStops(stopsrc);
		FillRoutes(routesrc);
    }

	void FillStops(std::shared_ptr<CDSVReader> stopsrc) {
		std::vector < std::string > currStop;
		long s, n;
		while (stopsrc->ReadRow(currStop)) {
			try {
				s = std::stol(currStop[0]);
				n = std::stol(currStop[1]);
			} catch(std::invalid_argument) {
				currStop = {};
				continue;
			}
			auto stop = std::make_shared<SStop>(s, n);
			stops.push_back(std::move(stop));
			currStop = {};
		}
	}

	void FillRoutes(std::shared_ptr<CDSVReader> routesrc) {
		std::vector < std::string > currRoute;
		long s;
		bool found;

		while (routesrc->ReadRow(currRoute)) {
			try {
				s = std::stol(currRoute[1]);
			} catch(std::invalid_argument) {
				currRoute = {};
				continue;
			}
			
			found = false;
			for (int i = 0; i < routes.size(); i++) {
				if (currRoute[0] == routes[i]->Name()) {
					routes[i]->routestops.push_back(s);
					found = true;
					break;
				}
			}
			
			if (!found) {
				auto route = std::make_shared<SRoute>(currRoute[0], s);
				routes.push_back(std::move(route));
			}	
		
			currRoute = {};
		} 
	}

    std::size_t StopCount() const noexcept{
        return stops.size();
    }

    std::size_t RouteCount() const noexcept{
        return routes.size();
    }

    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
		if (0 <= index && index < StopCount())
			return stops[index];
		
		return nullptr;
    }

    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
		for (int i = 0; i < StopCount(); i++) {
			if (stops[i]->ID() == id)
				return stops[i];		
		}
		return nullptr;
    }

    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
		if (0 <= index && index < RouteCount())
			return routes[index];
		
		return nullptr;
    }
	
    std::shared_ptr<SRoute> RouteByName(const std::string &name)const noexcept{
        for (int i = 0; i < RouteCount(); i++) {
			if (!name.compare(routes[i]->Name()))
				return routes[i];
		}
		
		return nullptr;
    }
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr< CDSVReader > stopsrc, std::shared_ptr< CDSVReader > routesrc){
	DImplementation = std::make_unique<SImplementation>(stopsrc, routesrc);
}


CCSVBusSystem::~CCSVBusSystem(){

}

std::size_t CCSVBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();

}

std::size_t CCSVBusSystem::RouteCount() const noexcept{
    return DImplementation->RouteCount();

}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);

}
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);

}
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);

}
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name)const noexcept{
    return DImplementation->RouteByName(name);
}
