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
#include <set>
#include <queue>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h" // Graph implementation

std::vector<long long> Dijkstra(
    graph<long long, double> &G,
    long long startV,
    long long destV,
    std::map<long long, double> &distances)
{
    const double INF = std::numeric_limits<double>::max();

    std::vector<long long> visited;
    std::set<long long> visitedSet;

    std::vector<long long> graphVertices = G.getVertices();
    std::priority_queue<
        std::pair<double, long long>,
        std::vector<std::pair<double, long long>>,
        std::greater<std::pair<double, long long>>>
        unvisitedQueue;

    for (long long vertex : graphVertices)
    {
        unvisitedQueue.emplace((vertex == startV) ? 0 : INF, vertex);
        distances.emplace(vertex, (vertex == startV) ? 0 : INF);
    }

    std::pair<double, long long> currentV;
    double edgeWeight, altPathDistance;

    while (!unvisitedQueue.empty())
    {
        currentV = unvisitedQueue.top();
        unvisitedQueue.pop();

        // Terminate loop if top equals destination
        if (currentV.second == destV)
            break;

        if (currentV.first == INF)
            break;
        else if (visitedSet.count(currentV.second) > 0)
            continue;

        std::set<long long> neighbors = G.neighbors(currentV.second);

        for (auto neighbor : neighbors)
        {
            edgeWeight = 0.0;
            G.getWeight(currentV.second, neighbor, edgeWeight);
            altPathDistance = currentV.first + edgeWeight;

            if (altPathDistance < distances.find(neighbor)->second)
            {
                unvisitedQueue.emplace(altPathDistance, neighbor);
                distances[neighbor] = altPathDistance;
                visited.push_back(currentV.second);
                visitedSet.emplace(currentV.second);
            }
        }
    }
    return visited;
}

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

std::vector<std::string> splitStr(std::string x, char c)
{
    std::vector<std::string> res;
    for (size_t i = 0; i < x.size(); ++i)
    {
        if (x[i] == c)
        {
            res.push_back(x.substr(0, i));
            x = x.substr(i + 1);
        }
    }
    return res;
}

bool setBuildingInfo(
    std::map<std::string, BuildingInfo> &buildingsAbbreviation,
    std::map<std::string, BuildingInfo> &buildingsFullname,
    BuildingInfo &buildingInfo,
    std::string query)
{
    auto iterAbbrev = findBuilding(buildingsAbbreviation, query);
    if (iterAbbrev == buildingsAbbreviation.end())
    { // Abbreviation was not found, so we'll search for Fullname
        auto iterFname = findBuilding(buildingsFullname, query);
        if (iterFname != buildingsFullname.end())
        {
            buildingInfo = iterFname->second;
            return true;
        }
        else
        {
            // We will search for a partial match
            std::vector<std::string> words = splitStr(query, ' ');
            for (auto iter : buildingsFullname)
            {
                for (std::string w : words)
                {
                    std::vector<std::string> splitName = splitStr(iter.second.Fullname, ' ');
                    for (std::string sn : splitName)
                    {
                        if (w == sn)
                        {
                            buildingInfo = iter.second;
                            return true;
                        }
                    }
                }
            }
            return false;
        }
    }
    buildingInfo = iterAbbrev->second;
    return true;
}

Coordinates nearestNode(
    std::vector<FootwayInfo> &Footways,
    std::map<long long, Coordinates> &Nodes,
    Coordinates &c)
{
    double dist = std::numeric_limits<double>::max();
    Coordinates minDistCoord = c;

    for (FootwayInfo &f : Footways)
    {
        for (long long id : f.Nodes)
        {
            std::map<long long, Coordinates>::iterator it = Nodes.find(id);
            if (it != Nodes.end())
            {
                double dist2 = distBetween2Points(c.Lat, c.Lon, it->second.Lat, it->second.Lon);
                if (dist2 < dist)
                {
                    dist = dist2;
                    minDistCoord = it->second;
                }
            }
        }
    }
    return minDistCoord;
}

void printBuildingInfo(BuildingInfo &building)
{
    std::cout << " " << building.Fullname << std::endl;
    std::cout << " (" << building.Coords.Lat << ", " << building.Coords.Lon << ")" << std::endl;
}

void printNearestNode(Coordinates &coord)
{
    std::cout << " " << coord.ID << std::endl;
    std::cout << " (" << coord.Lat << ", " << coord.Lon << ")" << std::endl;
}

std::stack<long long> getPath(
    graph<long long, double> &G,
    std::vector<long long> &nodes,
    long long start, long long dest)
{
    std::stack<long long> shortestPath;
    std::set<long long> visitedNodes;
    shortestPath.push(dest);

    while (shortestPath.top() != start)
    {
        std::queue<long long> S;
        for (long long vert : nodes)
        {
            double weight = 0.0;
            if (visitedNodes.count(vert) == 0 && G.getWeight(vert, shortestPath.top(), weight))
            {
                S.push(vert);
                visitedNodes.insert(vert);
            }
        }

        if (!S.empty())
            shortestPath.push(S.front());
        else
            break;
    }
    return shortestPath;
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
    size_t nodeCount = ReadMapNodes(xmldoc, Nodes);

    // Read the footways, which are the walking paths:
    size_t footwayCount = ReadFootways(xmldoc, Footways);

    // Read the university buildings:
    size_t buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

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

    BuildingInfo startBuildingInfo, destBuildingInfo;

    while (startBuilding != "#")
    {
        std::cout << "Enter destination (partial name or abbreviation)> ";
        std::getline(cin, destBuilding);

        bool startFound = setBuildingInfo(buildingsAbbreviation, buildingsFullname, startBuildingInfo, startBuilding);
        bool destFound = setBuildingInfo(buildingsAbbreviation, buildingsFullname, destBuildingInfo, destBuilding);

        if (!startFound)
            std::cout << "Start building not found..." << std::endl;
        if (!destFound)
            std::cout << "Destination building not found..." << std::endl;

        if (startFound && destFound)
        {
            std::cout << "Starting point: " << std::endl;
            printBuildingInfo(startBuildingInfo);
            std::cout << "Destination point: " << std::endl;
            printBuildingInfo(destBuildingInfo);

            std::cout << std::endl;

            // Nearest nodes
            std::cout << "Nearest start node: " << std::endl;
            Coordinates startCoord = nearestNode(Footways, Nodes, startBuildingInfo.Coords);
            printNearestNode(startCoord);
            std::cout << "Nearest destination node: " << std::endl;
            Coordinates destCoord = nearestNode(Footways, Nodes, destBuildingInfo.Coords);
            printNearestNode(destCoord);

            std::cout << std::endl;

            // Dijksra's algorithm...
            std::cout << "Navigating with Dijkstra..." << std::endl;

            std::map<long long, double> distances;
            std::vector<long long> nodes = Dijkstra(G, startCoord.ID, destCoord.ID, distances);

            auto shortestPath = getPath(G, nodes, startCoord.ID, destCoord.ID);

            if (shortestPath.top() == destCoord.ID)
                std::cout << "Sorry, destination unreachable" << std::endl;
            else
            {
                std::cout << "Distance to dest: " << distances.find(destCoord.ID)->second << " miles" << std::endl;

                std::cout << "Path: ";
                while (!shortestPath.empty())
                {
                    std::cout << shortestPath.top();
                    if (shortestPath.top() != destCoord.ID)
                        std::cout << "->";
                    shortestPath.pop();
                }
                std::cout << std::endl;
            }
        }

        // Restart...
        std::cout << std::endl;
        std::cout << "Enter start (partial name or abbreviation), or #> ";
        std::getline(cin, startBuilding);
    }
    std::cout << "** Done **" << std::endl;
    return 0;
}
