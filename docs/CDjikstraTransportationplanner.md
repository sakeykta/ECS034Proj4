# CDjikstraTransportationPlanner
Inherits functionalities from CTransportationPlanner and then implements Djikstra's algorithm

## Constructor
```cpp
CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config);
```
## Destructor
```cpp
~CDjikstraTransportationPlanner(){};
```
## Member Functions
### NodeCount
```cpp
std::size_t NodeCount() const noexcept override;
```
Returns the amound of Nodes in the system

### SortedNodeByIndex
```cpp
std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept override;
```
Retrieves the node at a specific index

### FindShortestPath
```cpp
double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) override;
```
Finds the shortest path between given source and destination node. Fills the path vector as it goes along

### FindFastestPath
```cpp
double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) override;
```
Finds the fastest path which accounts for travel time. Fills the path vector as it goes along.

### GetPathDescription
```cpp
bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const override;
```
Returns a readable description of the path provided. If succesfully generated, returns true