#pragma once

#include <vector>
#include <set>


class Graph {
public:
    Graph(int verticesCnt):
        adjacencyList(verticesCnt) {}

    inline void AddEdge(int v1, int v2) {
        adjacencyList[v1].insert(v2);
        adjacencyList[v2].insert(v1);
    }

    inline void DeleteEdge(int v1, int v2) {
        adjacencyList[v1].erase(v2);
        adjacencyList[v2].erase(v1);
    }

    inline const std::set<int>& GetNeighbors(int v) const
        { return adjacencyList[v]; }

    inline int GetVerticesCnt() const
        { return adjacencyList.size(); }

    inline bool HasEdge(int v1, int v2) const
        { return adjacencyList[v1].contains(v2); }

private:
    std::vector<std::set<int>> adjacencyList;
};
