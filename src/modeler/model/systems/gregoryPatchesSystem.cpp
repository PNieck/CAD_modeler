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
    /* Control Points from first row */
    Entity cornerCP1;
    Entity middleCP1;
    Entity middleCP2;
    Entity cornerCP2;

    /* Control Points from second row */

    /// @brief Control points which is located behind cornerCP1
    Entity innerCP1;

    /// @brief Control points which is located behind middlerCP1
    Entity innerCP2;

    /// @brief Control points which is located behind middlerCP2
    Entity innerCP3;

    /// @brief Control points which is located behind cornerCP2
    Entity innerCP4;
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
        return std::hash<int>()(edge.v1) + std::hash<int>()(edge.v2);
    }
};


std::vector<GregoryPatchesSystem::Hole> GregoryPatchesSystem::FindHolesToFill(const std::vector<C0Patches>& patchesVec) const
{
    int graphSize = 0;
    std::map<Entity, int> cpToGraphVertexMap;
    std::unordered_map<GraphEdge, PatchEdge, GraphEdgeHash> edgeMap;
    std::vector<GregoryPatchesSystem::Hole> result;

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

                    int innerRow = row + 1 < patches.PointsInRow() ? row + 1 : row - 1;

                    patchEdge.innerCP1 = patches.GetPoint(innerRow, col);
                    patchEdge.innerCP2 = patches.GetPoint(innerRow, col + 1);
                    patchEdge.innerCP3 = patches.GetPoint(innerRow, col + 2);
                    patchEdge.innerCP4 = patches.GetPoint(innerRow, col + 3);

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

                    int innerCol = col + 1 < patches.PointsInCol() ? col + 1 : col - 1;

                    patchEdge.innerCP1 = patches.GetPoint(row, innerCol);
                    patchEdge.innerCP2 = patches.GetPoint(row + 1, innerCol);
                    patchEdge.innerCP3 = patches.GetPoint(row + 2, innerCol);
                    patchEdge.innerCP4 = patches.GetPoint(row + 3, innerCol);

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
    result.reserve(cycles.size());
    
    for (auto& cycle: cycles) {
        assert(cycle.size() == 3);

        GregoryPatchesSystem::Hole hole;
        int innerCpCounter = 0;
        int outerCpCounter = 0;

        GraphEdge graphEdge;
        graphEdge.v1 = cycle[0];
        graphEdge.v2 = cycle[1];

        PatchEdge patchEdge = edgeMap[graphEdge];
        hole.innerCp[innerCpCounter++] = patchEdge.cornerCP1;
        hole.innerCp[innerCpCounter++] = patchEdge.middleCP1;
        hole.innerCp[innerCpCounter++] = patchEdge.middleCP2;
        hole.innerCp[innerCpCounter++] = patchEdge.cornerCP2;

        hole.outerCp[outerCpCounter++] = patchEdge.innerCP1;
        hole.outerCp[outerCpCounter++] = patchEdge.innerCP2;
        hole.outerCp[outerCpCounter++] = patchEdge.innerCP3;
        hole.outerCp[outerCpCounter++] = patchEdge.innerCP4;

        graphEdge.v1 = cycle[1];
        graphEdge.v2 = cycle[2];

        patchEdge = edgeMap[graphEdge];
        if (hole.innerCp[innerCpCounter - 1] == patchEdge.cornerCP1) {
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP1;
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP2;
            hole.innerCp[innerCpCounter++] = patchEdge.cornerCP2;

            hole.outerCp[outerCpCounter++] = patchEdge.innerCP1;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP2;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP3;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP4;
        }
        else {
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP2;
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP1;
            hole.innerCp[innerCpCounter++] = patchEdge.cornerCP1;

            hole.outerCp[outerCpCounter++] = patchEdge.innerCP4;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP3;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP2;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP1;
        }

        graphEdge.v1 = cycle[2];
        graphEdge.v2 = cycle[0];

        patchEdge = edgeMap[graphEdge];
        if (hole.innerCp[innerCpCounter - 1] == patchEdge.cornerCP1) {
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP1;
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP2;

            hole.outerCp[outerCpCounter++] = patchEdge.innerCP1;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP2;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP3;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP4;
        }
        else {
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP2;
            hole.innerCp[innerCpCounter++] = patchEdge.middleCP1;

            hole.outerCp[outerCpCounter++] = patchEdge.innerCP4;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP3;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP2;
            hole.outerCp[outerCpCounter++] = patchEdge.innerCP1;
        }
        
        result.push_back(hole);
    }

    return result;
}
