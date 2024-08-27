#include <graph/alg/cycleDetection.hpp>


std::vector<std::vector<int>> FindCyclesOfLength3(const Graph &graph)
{
    // TODO: write more efficient version

    std::vector<std::vector<int>> result;

    for (int v=0; v < graph.GetVerticesCnt(); v++) {
        for (int u=v+1; u < graph.GetVerticesCnt(); u++) {
            for (int w=u+1; w < graph.GetVerticesCnt(); w++) {
                if (graph.HasEdge(v, u) && graph.HasEdge(u, w) && graph.HasEdge(w, v))
                    result.push_back({v, u, w});
            }
        }
    }

    return result;
}
