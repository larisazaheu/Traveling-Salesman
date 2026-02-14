#include "Graph.h"
#include <fstream>
#include <algorithm>
#include <cmath>

struct DSU {
    std::vector<int> parent;

    DSU(int n) {
        parent.resize(n);
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int Find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = Find(parent[i]);
    }

    void Unite(int i, int j) {
        int root_i = Find(i);
        int root_j = Find(j);
        if (root_i != root_j) parent[root_i] = root_j;
    }
};

void Graph::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    int n;
    file >> n;
    nodeCount = n; 

   
    cityNames.clear();
    cityCoordinates.clear();
    mstEdges.clear(); 
    tspRoute.clear();

    double inf = std::numeric_limits<double>::infinity();
    distanceMatrix.assign(n, std::vector<double>(n, inf));
    initialEdges.assign(n, std::vector<bool>(n, false));

    for (int i = 0; i < n; ++i) {
        std::string name;
        double lat, lon;
        file >> name >> lat >> lon;
        cityNames.push_back(name);
        cityCoordinates.push_back({ lat, lon });
        distanceMatrix[i][i] = 0; 
    }

    int m;
    file >> m;
    for (int i = 0; i < m; ++i) {
        int u, v;
        double w;
        file >> u >> v >> w;
        if (u >= 0 && u < n && v >= 0 && v < n) {
            distanceMatrix[u][v] = w;
            distanceMatrix[v][u] = w;
            initialEdges[u][v] = true;
            initialEdges[v][u] = true;
        }
    }
    file.close();
}

void Graph::ComputeFloydWarshall() {

    int n = (int)cityNames.size();
    double inf = std::numeric_limits<double>::infinity();

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
           
                if (distanceMatrix[i][k] != inf && distanceMatrix[k][j] != inf) {
                    if (distanceMatrix[i][k] + distanceMatrix[k][j] < distanceMatrix[i][j]) {
                        distanceMatrix[i][j] = distanceMatrix[i][k] + distanceMatrix[k][j];
                    }
                }
            }
        }
    }

}


void Graph::ComputeKruskalMST() {
    mstEdges.clear(); 
    adjacencyListMST.assign(nodeCount, std::vector<int>());
    std::vector<Edge> allEdges;
    double inf = std::numeric_limits<double>::infinity();

    for (int i = 0; i < nodeCount; ++i) {
        for (int j = i + 1; j < nodeCount; ++j) {
          
            if (distanceMatrix[i][j] != inf) {
                allEdges.push_back({ i, j, distanceMatrix[i][j] });
            }
        }
    }
    std::sort(allEdges.begin(), allEdges.end());

    DSU dsu(nodeCount);
    for (const auto& edge : allEdges) {
        if (dsu.Find(edge.u) != dsu.Find(edge.v)) {
            dsu.Unite(edge.u, edge.v);
            mstEdges.push_back(edge);
            adjacencyListMST[edge.u].push_back(edge.v);
            adjacencyListMST[edge.v].push_back(edge.u);
        }
    }
}

void Graph::GenerateTSPSolution() {
    std::vector<bool> visited(nodeCount, false);
    tspRoute.clear();
    PerformPreorder(0, visited);
    tspRoute.push_back(0);
}

void Graph::PerformPreorder(int u, std::vector<bool>& visited) {
    visited[u] = true;
    tspRoute.push_back(u);
    for (int v : adjacencyListMST[u]) {
        if (!visited[v]) PerformPreorder(v, visited);
    }
}