# CampusMap

CampusMap is a console app that helps the user navigate UIC's campus. The project uses Dijkstra’s algorithm to find the shortest weighted path from a given starting location to a given ending location. This project is based on [OpenStreetMap](https://www.openstreetmap.org/) to navigate through the UIC campus, and uses [TinyXML](http://www.grinninglizard.com/tinyxml/) to parse the `.xml` files.

## Working with the project

### Clone the project

```
git clone https://github.com/ayaanqui/CampusMap.git
```

### Compile the project

```
g++ -O2 -std=c++11 -Wall main.cpp dist.cpp osm.cpp tinyxml2.cpp -o program
```

This will create a new file in your local project directory, named `program`

### Run program

```
./program
```

Ignore the warnings. And input `map.osm` as the file name
