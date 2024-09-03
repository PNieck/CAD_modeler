#include <CAD_modeler/model/systems/gregoryPatchesSystem.hpp>

#include <CAD_modeler/utilities/hashCombine.hpp>

#include <ecs/coordinator.hpp>

#include <graph/alg/cycleDetection.hpp>

#include <unordered_map>
#include <map>
#include <tuple>
#include <cassert>


void GregoryPatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<GregoryPatchesSystem>();
}


struct PatchEdge {
    Entity cornerCP1;
    Entity middleCP1;
    Entity middleCP2;
    Entity cornerCP2;
};


struct GraphEdge {
    int v1;
    int v2;
};


bool operator==(const GraphEdge& ge1, const GraphEdge& ge2) {
    return (ge1.v1 == ge2.v1 && ge1.v2 == ge2.v2) ||
           (ge1.v1 == ge2.v2 && ge1.v2 == ge2.v1);
}


// Hash function must be neutral to order of elements, because equal operator is too
struct GraphEdgeHash {
    size_t operator() (const GraphEdge& edge) const {
        return 
            std::hash<int>()(edge.v1) +
            std::hash<int>()(edge.v2);
        
    }
};


std::vector<std::vector<Entity>> GregoryPatchesSystem::FindHoleToFill(const std::vector<C0Patches>& patchesVec) const
{
    int graphSize = 0;
    std::map<Entity, int> cpToGraphVertexMap;
    std::unordered_map<GraphEdge, PatchEdge, GraphEdgeHash> edgeMap;
    std::vector<std::vector<Entity>> result;

    for (const auto& patches: patchesVec) {
        for (int row = 0; row < patches.PointsInRow(); row += 3) {
            for (int col = 0; col < patches.PointsInCol(); col += 3) {
                Entity cp = patches.GetPoint(row, col);

                if (!cpToGraphVertexMap.contains(cp)) {
                    cpToGraphVertexMap.insert({cp, graphSize++});
                }
            }
        }
    }

    Graph g(graphSize);

    for (const auto& patches: patchesVec) {
        for (int row = 0; row < patches.PointsInRow(); row += 3) {
            for (int col = 0; col < patches.PointsInCol(); col += 3) {
                if (col + 3 < patches.PointsInCol()) {
                    PatchEdge patchEdge;
                    patchEdge.cornerCP1 = patches.GetPoint(row, col);
                    patchEdge.middleCP1 = patches.GetPoint(row, col + 1);
                    patchEdge.middleCP2 = patches.GetPoint(row, col + 2);
                    patchEdge.cornerCP2 = patches.GetPoint(row, col + 3);

                    GraphEdge graphEdge;
                    graphEdge.v1 = cpToGraphVertexMap[patchEdge.cornerCP1];
                    graphEdge.v2 = cpToGraphVertexMap[patchEdge.cornerCP2];

                    edgeMap.insert({graphEdge, patchEdge});

                    g.AddEdge(graphEdge.v1, graphEdge.v2);
                }

                if (row + 3 < patches.PointsInRow()) {
                    PatchEdge patchEdge;
                    patchEdge.cornerCP1 = patches.GetPoint(row, col);
                    patchEdge.middleCP1 = patches.GetPoint(row + 1, col);
                    patchEdge.middleCP2 = patches.GetPoint(row + 2, col);
                    patchEdge.cornerCP2 = patches.GetPoint(row + 3, col);

                    GraphEdge graphEdge;
                    graphEdge.v1 = cpToGraphVertexMap[patchEdge.cornerCP1];
                    graphEdge.v2 = cpToGraphVertexMap[patchEdge.cornerCP2];

                    edgeMap.insert({graphEdge, patchEdge});

                    g.AddEdge(graphEdge.v1, graphEdge.v2);
                }

                
            }
        }
    }

    auto cycles = FindCyclesOfLength3(g);
    
    for (auto& cycle: cycles) {
        assert(cycle.size() == 3);

        std::vector<Entity> patchCycle;
        patchCycle.reserve(9);

        GraphEdge graphEdge;
        graphEdge.v1 = cycle[0];
        graphEdge.v2 = cycle[1];

        PatchEdge patchEdge = edgeMap[graphEdge];
        patchCycle.push_back(patchEdge.cornerCP1);
        patchCycle.push_back(patchEdge.middleCP1);
        patchCycle.push_back(patchEdge.middleCP2);
        patchCycle.push_back(patchEdge.cornerCP2);

        graphEdge.v1 = cycle[1];
        graphEdge.v2 = cycle[2];

        patchEdge = edgeMap[graphEdge];
        if (patchCycle[patchCycle.size() - 1] == patchEdge.cornerCP1) {
            patchCycle.push_back(patchEdge.middleCP1);
            patchCycle.push_back(patchEdge.middleCP2);
            patchCycle.push_back(patchEdge.cornerCP2);
        }
        else {
            patchCycle.push_back(patchEdge.middleCP2);
            patchCycle.push_back(patchEdge.middleCP1);
            patchCycle.push_back(patchEdge.cornerCP1);
        }

        graphEdge.v1 = cycle[2];
        graphEdge.v2 = cycle[0];

        patchEdge = edgeMap[graphEdge];
        if (patchCycle[patchCycle.size() - 1] == patchEdge.cornerCP1) {
            patchCycle.push_back(patchEdge.middleCP1);
            patchCycle.push_back(patchEdge.middleCP2);
        }
        else {
            patchCycle.push_back(patchEdge.middleCP2);
            patchCycle.push_back(patchEdge.middleCP1);
        }
        
        result.push_back(patchCycle);
    }

    return result;
}
