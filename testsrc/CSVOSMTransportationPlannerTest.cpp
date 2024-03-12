#include "TransportationPlannerCommandLine.h"
#include "DijkstraTransportationPlanner.h"
#include "OpenStreetMap.h"
#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringUtils.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <cmath>
#include <unordered_set>
#include <iomanip>
#include <sstream>

struct CTransportationPlannerCommandLine::SImplementation {
    std::shared_ptr<CDataSource> commandSrc;
    std::shared_ptr<CDataSink> outSink;
    std::shared_ptr<CDataSink> errSink;
    std::shared_ptr<CDataFactory> results;
    std::shared_ptr<CTransportationPlanner> planner;

    std::vector<CTransportationPlanner::TTripStep> lastPath;
    double lastPathTime;
    std::pair<COpenStreetMap::TNodeID, COpenStreetMap::TNodeID> lastPathSrcDest;
    
    SImplementation(std::shared_ptr<CDataSource> c, std::shared_ptr<CDataSink> o, std::shared_ptr<CDataSink> e, std::shared_ptr<CDataFactory> r, std::shared_ptr<CTransportationPlanner> p) {
        commandSrc = c;
        outSink = o;
        errSink = e;
        results = r;
        planner = p;
    }

    std::string ConvertCoordinates(std::shared_ptr<COpenStreetMap::SNode> node) const noexcept {
        std::string direction;
        
        // Calculating latitude
        double lat = node->Location().first;
        if (lat < 0) {
            direction = "S";
            lat *= -1;
        } else 
            direction = "N";

        int degrees = std::floor(lat);
        double temp = (lat - degrees) * 60;
        int minutes = std::floor(temp);
        int seconds = std::round((temp - minutes) * 60);

        std::string latDMS = std::to_string(degrees) + "d " + std::to_string(minutes) + "' " + std::to_string(seconds) + "\" " + direction;

        // Calculating longitude
        double lon = node->Location().second;
        if (lon < 0) {
            direction = "W";
            lon *= -1;
        } else 
            direction = "E";
        
        degrees = std::floor(lon);
        temp = (lon - degrees) * 60;
        minutes = std::floor(temp);
        seconds = std::round((temp - minutes) * 60);
        
        std::string lonDMS = std::to_string(degrees) + "d " + std::to_string(minutes) + "' " + std::to_string(seconds) + "\" " + direction;

        return latDMS + ", " + lonDMS;
    }

    std::string ConvertHours(double hours) const noexcept {
        int hr = std::floor(hours);
        double remaining = (hours - hr) * 60;

        int min = std::floor(remaining);
        int sec = std::round((remaining - min) * 60);

        std::string retstr;
        if (hr)
            retstr += " " + std::to_string(hr) + " hr";
        
        if (min)
            retstr += " " + std::to_string(min) + " min";
        
        if (sec)
            retstr += " " + std::to_string(sec) + " sec";

        return retstr;
    }

    std::vector<char> ConvertStr(std::string str) const noexcept {
        return std::vector<char>(str.begin(), str.end());
    }

    bool ProcessCommands() {
        
        while (true) { 
            outSink->Write(ConvertStr("> "));
            
            char c = ' ';
            std::string commandstr;
            while (c != '\n') {
                commandSrc->Get(c);
                commandstr += c;
            }

            std::vector<std::string> command = StringUtils::Split(commandstr, " ");

            for (size_t i = 0; i < command.size(); i++) 
                command[i] = StringUtils::Replace(command[i], "\n", "");

            // HELP
            if (command[0] == "help") {

                outSink->Write(ConvertStr("------------------------------------------------------------------------\n"));
                outSink->Write(ConvertStr("help     Display this help menu\n"));
                outSink->Write(ConvertStr("exit     Exit the program\n"));
                outSink->Write(ConvertStr("count    Output the number of nodes in the map\n"));
                outSink->Write(ConvertStr("node     Syntax \"node [0, count)\" \n"));
                outSink->Write(ConvertStr("         Will output node ID and Lat/Lon for node\n"));
                outSink->Write(ConvertStr("fastest  Syntax \"fastest start end\" \n"));
                outSink->Write(ConvertStr("         Calculates the time for fastest path from start to end\n"));
                outSink->Write(ConvertStr("shortest Syntax \"shortest start end\" \n"));
                outSink->Write(ConvertStr("         Calculates the distance for the shortest path from start to end\n"));
                outSink->Write(ConvertStr("save     Saves the last calculated path to file\n"));
                outSink->Write(ConvertStr("print    Prints the steps for the last calculated path\n"));
            }

            // EXIT
            else if (command[0] == "exit") {
                break;
            }

            // COUNT
            else if (command[0] == "count") {
                std::string numNodes = std::to_string(planner->NodeCount());
                outSink->Write(ConvertStr(numNodes + " nodes\n"));
            }
            
            // NODE
            else if (command[0] == "node") {
                
                if (command.size() < 2) {
                    errSink->Write(ConvertStr("Invalid node command, see help.\n"));
                    continue;
                }
                
                size_t index;
                try {
                    index = std::stol(command[1]);
                } catch(std::invalid_argument) {
                    errSink->Write(ConvertStr("Invalid node parameter, see help.\n"));
                    continue;
                }
                
                if (0 <= index < planner->NodeCount()) {
                    auto node = planner->SortedNodeByIndex(index);
                    auto id = std::to_string(node->ID());
                    auto coordinates = ConvertCoordinates(node);

                    outSink->Write(ConvertStr("Node " + std::to_string(index) + ": id = " + id + " is at " + coordinates + "\n"));
                } else {
                    errSink->Write(ConvertStr("Invalid node parameter, see help.\n"));
                }
            }
            
            // FASTEST
            else if (command[0] == "fastest") {
                
                if (command.size() < 3) {
                    errSink->Write(ConvertStr("Invalid fastest command, see help.\n"));
                    continue;
                }

                COpenStreetMap::TNodeID src;
                COpenStreetMap::TNodeID dest;
                try {
                    src = std::stol(command[1]);
                    dest = std::stol(command[2]);
                } catch(std::invalid_argument) {
                    errSink->Write(ConvertStr("Invalid fastest parameter, see help.\n"));
                    continue;
                }
                
                lastPath.clear();
                lastPathTime = planner->FindFastestPath(src, dest, lastPath);

                if (lastPathTime == CPathRouter::NoPathExists)
                    errSink->Write(ConvertStr("Unable to find fastest path between " + std::to_string(src) + " to " + std::to_string(dest) + ".\n"));
                
                else {
                    lastPathSrcDest.first = src;
                    lastPathSrcDest.second = dest;

                    outSink->Write(ConvertStr("Fastest path takes" + ConvertHours(lastPathTime) + ".\n"));
                }
            } 
            
            // SHORTEST
            else if (command[0] == "shortest") {

                if (command.size() < 3) {
                    errSink->Write(ConvertStr("Invalid shortest command, see help.\n"));
                    continue;
                }
                
                COpenStreetMap::TNodeID src;
                COpenStreetMap::TNodeID dest;
                try {
                    src = std::stol(command[1]);
                    dest = std::stol(command[2]);
                } catch(std::invalid_argument) {
                    errSink->Write(ConvertStr("Invalid shortest parameter, see help.\n"));
                    continue;
                }

                std::vector < COpenStreetMap::TNodeID > path;
                auto miles = planner->FindShortestPath(src, dest, path);

                if (miles == CPathRouter::NoPathExists)
                    errSink->Write(ConvertStr("Unable to find shortest path between " + std::to_string(src) + " to " + std::to_string(dest) + ".\n"));

                else {
                    // https://stackoverflow.com/questions/29200635/convert-float-to-string-with-precision-number-of-decimal-digits-specified
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(1) << miles;
                    std::string dist = stream.str();

                    outSink->Write(ConvertStr("Shortest path is " + dist + " mi.\n"));
                }
                
            }

            // SAVE
            else if (command[0] == "save") {
                
                if (lastPath.size() == 0) 
                    errSink->Write(ConvertStr("No valid path to save, see help.\n"));
                
                else {
                    std::string fileName = std::to_string(lastPathSrcDest.first) + "_" + std::to_string(lastPathSrcDest.second) + "_" + std::to_string(lastPathTime) + "hr.csv";
                    auto saveSink = results->CreateSink(fileName);
                    saveSink->Write(ConvertStr("mode,node_id\n"));

                    // Loop to write all steps
                    std::string type, output;
                    for (size_t i = 0; i < lastPath.size(); i++) {
                        if (lastPath[i].first == CTransportationPlanner::ETransportationMode::Bus)
                            type = "Bus";
                        else if (lastPath[i].first == CTransportationPlanner::ETransportationMode::Bike)
                            type = "Bike";
                        else if (lastPath[i].first == CTransportationPlanner::ETransportationMode::Walk)
                            type = "Walk";
                        
                        output += type + "," + std::to_string(lastPath[i].second);
                        if (i != lastPath.size() - 1)
                            output += "\n";
                    }
                    saveSink->Write(ConvertStr(output));
                    outSink->Write(ConvertStr("Path saved to <results>/" + fileName + "\n"));
                }
            }

            // PRINT
            else if (command[0] == "print") {

                if (lastPath.size() == 0) 
                    errSink->Write(ConvertStr("No valid path to print, see help.\n"));
                
                else {
                    std::vector <std::string> desc;
                    planner->GetPathDescription(lastPath, desc);
                    
                    for (size_t i = 0; i < desc.size(); i++) 
                        outSink->Write(ConvertStr(desc[i] + "\n"));
                }
            }

            // INVALID COMMAND
            else
                errSink->Write(ConvertStr("Unknown command \"" + command[0] + "\" type help for help.\n"));
            
        }
        return true;
    }
};

CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner) {
    DImplementation = std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner);
}

CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine() {}

bool CTransportationPlannerCommandLine::ProcessCommands() {
    return DImplementation->ProcessCommands();
}
