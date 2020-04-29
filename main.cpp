//
// Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07: open street maps, graphs, and Dijkstra's alg
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip> // setprecision
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h" // Graph implementation

void addNodes(std::map<long long, Coordinates> &Nodes, graph<long long, double> &G)
{
    for (auto node : Nodes)
        G.addVertex(node.first);
}

void addEdges(std::vector<FootwayInfo> &Footways, std::map<long long, Coordinates> &Nodes, graph<long long, double> &G)
{
    for (FootwayInfo &footway : Footways)
    {
        for (size_t i = 0; i < footway.Nodes.size() - 1; ++i)
        {
            auto node1 = Nodes.find(footway.Nodes[i]);
            auto node2 = Nodes.find(footway.Nodes[i + 1]);

            G.addEdge(
                node1->first,
                node2->first,
                distBetween2Points(node1->second.Lat, node1->second.Lon, node2->second.Lat, node2->second.Lon));
            G.addEdge(
                node2->first,
                node1->first,
                distBetween2Points(node2->second.Lat, node2->second.Lon, node1->second.Lat, node1->second.Lon));
        }
    }
}

void addBuildings(
    std::vector<BuildingInfo> &Buildings,
    std::map<std::string, BuildingInfo> &buildingsAbbreviation,
    std::map<std::string, BuildingInfo> &buildingsFullname)
{
    for (BuildingInfo building : Buildings)
    {
        buildingsAbbreviation.emplace(building.Abbrev, building);
        buildingsFullname.emplace(building.Fullname.substr(0, building.Fullname.find('(') - 1), building);
    }
}

std::map<std::string, BuildingInfo>::iterator findBuilding(
    std::map<std::string, BuildingInfo> &buildings,
    std::string query)
{
    return buildings.find(query);
}

int main()
{
    std::map<long long, Coordinates> Nodes; // maps a Node ID to it's coordinates (lat, lon)
    std::vector<FootwayInfo> Footways;      // info about each footway, in no particular order
    std::vector<BuildingInfo> Buildings;    // info about each building, in no particular order
    tinyxml2::XMLDocument xmldoc;

    std::cout << "** Navigating UIC open street map **" << std::endl;
    std::cout << endl;
    std::cout << std::setprecision(8);

    std::string def_filename = "map.osm";
    std::string filename;

    std::cout << "Enter map filename> ";
    getline(std::cin, filename);

    if (filename == "")
        filename = def_filename;

    // Load XML-based map file
    if (!LoadOpenStreetMap(filename, xmldoc))
    {
        cout << "**Error: unable to load open street map." << endl;
        cout << endl;
        return 0;
    }

    // Read the nodes, which are the various known positions on the map:
    int nodeCount = ReadMapNodes(xmldoc, Nodes);

    // Read the footways, which are the walking paths:
    int footwayCount = ReadFootways(xmldoc, Footways);

    // Read the university buildings:
    int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

    // Stats
    assert(nodeCount == Nodes.size());
    assert(footwayCount == Footways.size());
    assert(buildingCount == Buildings.size());

    std::cout << std::endl;
    std::cout << "# of nodes: " << Nodes.size() << std::endl;
    std::cout << "# of footways: " << Footways.size() << std::endl;
    std::cout << "# of buildings: " << Buildings.size() << std::endl;

    graph<long long, double> G;
    addNodes(Nodes, G); // Add all nodes to graph
    addEdges(Footways, Nodes, G);

    std::map<std::string, BuildingInfo> buildingsAbbreviation, buildingsFullname;
    addBuildings(Buildings, buildingsAbbreviation, buildingsFullname);

    std::cout << "# of vertices: " << G.NumVertices() << std::endl;
    std::cout << "# of edges: " << G.NumEdges() << std::endl;
    std::cout << std::endl;

    // Navigation from building to building
    std::string startBuilding, destBuilding;

    std::cout << "Enter start (partial name or abbreviation), or #> ";
    std::getline(std::cin, startBuilding);

    while (startBuilding != "#")
    {
        std::cout << "Enter destination (partial name or abbreviation)> ";
        std::getline(cin, destBuilding);

        auto startIterAbbrev = findBuilding(buildingsAbbreviation, startBuilding);
        auto startIterFname = findBuilding(buildingsFullname, startBuilding);

        auto destIterAbbrev = findBuilding(buildingsAbbreviation, destBuilding);
        auto destIterFname = findBuilding(buildingsFullname, destBuilding);

        bool startFound = false;
        bool destFound = false;

        if (startIterAbbrev == buildingsAbbreviation.end() && startIterFname == buildingsFullname.end())
            std::cout << "Start building not found" << std::endl;
        else
            startFound = true;
        if (destIterAbbrev == buildingsAbbreviation.end() && destIterFname == buildingsFullname.end())
            std::cout << "Destination building not found" << std::endl;
        else
            destFound = true;

        if (startFound && destFound)
            std::cout << "Let's start Dijkstra's algorithm!!" << std::endl;

        // Restart...
        std::cout << std::endl;
        std::cout << "Enter start (partial name or abbreviation), or #> ";
        std::getline(cin, startBuilding);
    }

    //
    // done:
    //
    std::cout << "** Done **" << std::endl;

    return 0;
}
