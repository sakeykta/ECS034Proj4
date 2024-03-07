#include "DijkstraPathRouter.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>

struct CDijkstraPathRouter::SImplementation {
    struct Vertex {
        std::any tag;
        std::unordered_map<TVertexID, double> edges;
    };

    std::vector<Vertex> vertices;

    SImplementation() {}

    ~SImplementation() {}

    double Dijkstra(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) const {
        std::unordered_map<TVertexID, double> dist;
        std::unordered_map<TVertexID, TVertexID> prev;
        std::priority_queue<std::pair<double, TVertexID>, std::vector<std::pair<double, TVertexID>>, std::greater<std::pair<double, TVertexID>>> pq;

        for (TVertexID i = 0; i < vertices.size(); ++i) {
            dist[i] = std::numeric_limits<double>::max();
            prev[i] = InvalidVertexID;
        }
        dist[src] = 0;
        pq.push(std::make_pair(0, src));

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (u == dest) break;

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

        if (prev[dest] != InvalidVertexID) {
            TVertexID current = dest;
            while (current != InvalidVertexID) {
                path.push_back(current);
                current = prev[current];
            }
            std::reverse(path.begin(), path.end());
            return dist[dest];
        } else {
            return NoPathExists;
        }
    }
};

CDijkstraPathRouter::CDijkstraPathRouter() : DImplementation(std::make_unique<SImplementation>()) {}

CDijkstraPathRouter::~CDijkstraPathRouter() = default;

std::size_t CDijkstraPathRouter::VertexCount() const noexcept {
    return DImplementation->vertices.size();
}

CDijkstraPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept {
    DImplementation->vertices.push_back({tag});
    return DImplementation->vertices.size() - 1;
}

std::any CDijkstraPathRouter::GetVertexTag(CDijkstraPathRouter::TVertexID id) const noexcept {
    if (id < DImplementation->vertices.size()) {
        return DImplementation->vertices[id].tag;
    } else {
        return std::any();
    }
}

bool CDijkstraPathRouter::AddEdge(CDijkstraPathRouter::TVertexID src, CDijkstraPathRouter::TVertexID dest, double weight, bool bidir) noexcept {
    if (src < DImplementation->vertices.size() && dest < DImplementation->vertices.size() && weight >= 0) {
        DImplementation->vertices[src].edges[dest] = weight;
        if (bidir) {
            DImplementation->vertices[dest].edges[src] = weight;
        }
        return true;
    } else {
        return false;
    }
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept {
    return true;
}

double CDijkstraPathRouter::FindShortestPath(CDijkstraPathRouter::TVertexID src, CDijkstraPathRouter::TVertexID dest, std::vector<CDijkstraPathRouter::TVertexID> &path) noexcept {
    path.clear();
    if (src >= DImplementation->vertices.size() || dest >= DImplementation->vertices.size()) {
        return NoPathExists;
    }
    return DImplementation->Dijkstra(src, dest, path);
}
