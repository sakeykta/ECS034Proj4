#include "TransportationPlannerCommandLine.h"
#include "DijkstraTransportationPlanner.h"
#include "OpenStreetMap.h"
#include "DSVReader.h"
#include "DSVWriter.h"
#include "StandardDataSource.h"
#include "StandardDataSink.h"
#include "StandardErrorDataSink.h"
#include "StringUtils.h"
#include "FileDataFactory.h"
#include "FileDataSink.h"
#include "FileDataSource.h"
#include "CSVBusSystem.h"
#include "TransportationPlannerConfig.h"
#include <memory>
#include <algorithm>
#include <iostream>

int main(){
    std::vector<std::string> Arguments;
    const std::string OSMFilename = "city.osm";
    const std::string StopFilename = "stops.csv";
    const std::string RouteFilename = "routes.csv";

    auto DataFactory = std::make_shared<CFileDataFactory>("./data");
    auto StopReader = std::make_shared<CDSVReader>(DataFactory->CreateSource(StopFilename),',');
    auto RouteReader = std::make_shared<CDSVReader>(DataFactory->CreateSource(RouteFilename),',');
    auto BusSystem = std::make_shared<CCSVBusSystem>(StopReader, RouteReader);
    auto XMLReader = std::make_shared<CXMLReader>(DataFactory->CreateSource(OSMFilename));
    auto StreetMap = std::make_shared<COpenStreetMap>(XMLReader);
    auto PlannerConfig = std::make_shared<STransportationPlannerConfig>(StreetMap, BusSystem);
	
	auto In = std::make_shared<CStandardDataSource>();
	auto Out = std::make_shared<CStandardDataSink>();
	auto Err = std::make_shared<CStandardErrorDataSink>();
	auto ResultsFactory = std::make_shared<CFileDataFactory>("./results");
	auto Planner = std::make_shared<CDijkstraTransportationPlanner>(PlannerConfig);
	
	auto commandline = std::make_shared<CTransportationPlannerCommandLine>(In, Out, Err, ResultsFactory, Planner);
	commandline->ProcessCommands();

}
