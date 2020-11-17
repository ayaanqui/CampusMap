# CampusMap

##### _This project is part of the UIC's [CS 251](https://www.cs.uic.edu/~i251/) (Data Structures) class, by [Professor Joseph Hummel](https://cs.uic.edu/profiles/joe-hummel/)._

CampusMap is a console app that helps the user navigate UIC's campus. The project uses Dijkstra’s algorithm to find the shortest weighted path from a given starting location to a given ending location. This project is based on [OpenStreetMap](https://www.openstreetmap.org/) to navigate through the UIC campus, and uses [TinyXML](http://www.grinninglizard.com/tinyxml/) to parse the `.xml` files.

## Working with the project

### Clone the project

```
git clone https://github.com/ayaanqui/uic-campus-map.git
```

### Compile the project

#### Linux and Mac

```
make build
```

#### Windows

```
g++ -O2 -std=c++11 -Wall main.cpp dist.cpp osm.cpp tinyxml2.cpp -o program.exe
```

_Ignore warnings._ This will create a new file in your local project directory, named `program.exe`

### Run program

#### Linux and Mac

```
make run
```

#### Windows

```
.\program.exe
```

Once the program starts input `map.osm` as the file name
