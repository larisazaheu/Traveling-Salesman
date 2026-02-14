#include <vector>
#include <string>
#include <map>

struct Point {
    double x, y;
};

struct Edge {
    int u, v;
    double weight;
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class Graph {
public:
    void LoadFromFile(const std::string& filename);
    void ComputeFloydWarshall();
    void ComputeKruskalMST();
    void GenerateTSPSolution();

private:
    void PerformPreorder(int u, std::vector<bool>& visited);

public:
    std::vector<Point> cityCoordinates;
    std::vector<std::string> cityNames;
    std::vector<std::vector<double>> distanceMatrix;
    std::vector<Edge> mstEdges;
    std::vector<int> tspRoute;
    std::vector<std::vector<bool>> initialEdges;
    int nodeCount;

private: 
    std::vector<std::vector<int>> adjacencyListMST;
};

