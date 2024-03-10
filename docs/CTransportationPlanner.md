# CTransportationPlanner
Serves as a base clas for planning transportation routes. Defines all the necessary functionalities used later


## Destructor
```cpp
~CDjikstraPathRouter(){};
```
## SConfiguration Struct
### Streetmap
```cpp
virtual std::shared_ptr<CStreetMap> StreetMap() const noexcept = 0;
```
Accesses the streetmap through a shared pointer

### BusSystem
```cpp
virtual std::shared_ptr<CBusSystem> BusSystem() const noexcept = 0;
```
Accesses the bus system through a shared pointer

### WalkSpeed
```cpp
virtual double WalkSpeed() const noexcept = 0;
```
Returns the walk speed in double format

### BikeSpeed
```cpp
virtual double BikeSpeed() const noexcept = 0;
```
Retrieves the biking speed in double format

### DefaultSpeedLimit
```cpp
virtual double DefaultSpeedLimit() const noexcept = 0;
```
Retreives the defauly speed limit for vehicles

### BusStopTime
```cpp
virtual double BusStopTime() const noexcept = 0;
```
Returns how long it takes for a bus to stop at the bustops

### PrecomputeTime
```cpp
virtual int PrecomputeTime() const noexcept = 0;
```
Gives the maximum tied allowed for precomputation, helps avoid infinite calculations

## Member Functions
### NodeCount
```cpp
virtual std::size_t NodeCount() const noexcept = 0;
```
Returns the amound of Nodes in the system

### SortedNodeByIndex
```cpp
virtual std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept = 0;
```
Retrieves the node at a specific index

### FindShortestPath
```cpp
virtual double FindShortestPath(TNodeID src, TNodeID dest, std::vector< TNodeID > &path) = 0;
```
Finds the shortest path between given source and destination node. Fills the path vector as it goes along

### FindFastestPath
```cpp
virtual double FindFastestPath(TNodeID src, TNodeID dest, std::vector< TTripStep > &path) = 0;
```
Finds the fastest path which accounts for travel time. Fills the path vector as it goes along.

### GetPathDescription
```cpp
virtual bool GetPathDescription(const std::vector< TTripStep > &path, std::vector< std::string > &desc) const = 0;
```
Returns a readable description of the path provided. If succesfully generated, returns true