#include "DijkstraPathRouter.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>

// Implementation of Dijkstra's algorithm
struct CDijkstraPathRouter::SImplementation {
    // Structure to represent a vertex in the graph
    struct Vertex {
        std::any tag; // A tag associated with the vertex
        std::unordered_map<TVertexID, double> edges; // Adjacency list representing edges and their weights
    };

    std::vector<Vertex> vertices; // Vector to store all vertices

    // Constructor
    SImplementation() {}

    // Destructor
    ~SImplementation() {}

    // Dijkstra's algorithm to find the shortest path
    double Dijkstra(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) const {
        // Data structures for distance and previous vertex
        std::unordered_map<TVertexID, double> dist;
        std::unordered_map<TVertexID, TVertexID> prev;
        std::priority_queue<std::pair<double, TVertexID>, std::vector<std::pair<double, TVertexID>>, std::greater<std::pair<double, TVertexID>>> pq;

        // Initialization of distance and previous
        for (TVertexID i = 0; i < vertices.size(); ++i) {
            dist[i] = std::numeric_limits<double>::max();
            prev[i] = InvalidVertexID;
        }
        dist[src] = 0;
        pq.push(std::make_pair(0, src));

        // Dijkstra's main loop
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (u == dest) break; // Destination reached

            if (d <= dist[u]) {
                for (const auto &[v, weight] : vertices[u].edges) {
                    double alt = dist[u] + weight;
                    if (alt < dist[v]) {
                        dist[v] = alt;
                        prev[v] = u;
                        pq.push(std::make_pair(alt, v));
                    }
                }
            }
        }

        // Reconstructing the shortest path
        if (prev[dest] != InvalidVertexID) {
            TVertexID current = dest;
            while (current != InvalidVertexID) {
                path.push_back(current);
                current = prev[current];
            }
            std::reverse(path.begin(), path.end());
            return dist[dest]; // Returning the shortest distance
        } else {
            return NoPathExists; // No path exists between source and destination
        }
    }
};

// Constructor
CDijkstraPathRouter::CDijkstraPathRouter() : DImplementation(std::make_unique<SImplementation>()) {}

// Destructor
CDijkstraPathRouter::~CDijkstraPathRouter() = default;

// Returns the number of vertices in the graph
std::size_t CDijkstraPathRouter::VertexCount() const noexcept {
    return DImplementation->vertices.size();
}

// Adds a vertex to the graph with the given tag
CDijkstraPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept {
    DImplementation->vertices.push_back({tag}); // Add vertex with provided tag
    return DImplementation->vertices.size() - 1;
}

// Returns the tag associated with the vertex ID
std::any CDijkstraPathRouter::GetVertexTag(CDijkstraPathRouter::TVertexID id) const noexcept {
    if (id < DImplementation->vertices.size()) {
        return DImplementation->vertices[id].tag;
    } else {
        return std::any(); // Return an empty tag if vertex ID is invalid
    }
}

// Adds an edge between source and destination vertices with given weight
bool CDijkstraPathRouter::AddEdge(CDijkstraPathRouter::TVertexID src, CDijkstraPathRouter::TVertexID dest, double weight, bool bidir) noexcept {
    if (src < DImplementation->vertices.size() && dest < DImplementation->vertices.size() && weight >= 0) {
        // Add edge from src to dest
        DImplementation->vertices[src].edges[dest] = weight;
        if (bidir) {
            // If bidirectional, add edge from dest to src
            DImplementation->vertices[dest].edges[src] = weight;
        }
        return true; // Edge added successfully
    } else {
        return false; // Failed to add edge
    }
}

// Placeholder for any precomputation tasks (not implemented)
bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept {
    return true; // Placeholder return value
}

// Finds the shortest path between source and destination vertices
double CDijkstraPathRouter::FindShortestPath(CDijkstraPathRouter::TVertexID src, CDijkstraPathRouter::TVertexID dest, std::vector<CDijkstraPathRouter::TVertexID> &path) noexcept {
    path.clear(); // Clear the path vector
    if (src >= DImplementation->vertices.size() || dest >= DImplementation->vertices.size()) {
        return NoPathExists; // Invalid source or destination vertex
    }
    return DImplementation->Dijkstra(src, dest, path); // Perform Dijkstra's algorithm
}
