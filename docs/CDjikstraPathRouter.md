# CDjikstraPathRouter
Inherits functionalities from CPathRouter and then implements Djikstra's algorithm

## Constructor
```cpp
CDijkstraPathRouter();
```
## Destructor
```cpp
~CDjikstraPathRouter(){};
```
## Member Function
### VertexCount
```cpp
virtual std::size_t VertexCount() const noexcept = 0;
```
Returns the number of vertexes

### AddVertex
```cpp
virtual TVertexID AddVertex(std::any tag) noexcept = 0;
```
Adds a vertex to a graph with a tag

### GetVertexTag
```cpp
virtual std::any GetVertexTag(TVertexID id) const noexcept = 0;
```
Returns the tag associated with a vertex

### AddEdge
```cpp
virtual bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) noexcept = 0;
```
Adds an edge to a graph

### PreCompute
```cpp
virtual bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept = 0;
```
Performs necessary calculations beforehand under a certain time limit

### FindShortestPath
```cpp
virtual double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept = 0;
```
Returns the value of the shortest path

