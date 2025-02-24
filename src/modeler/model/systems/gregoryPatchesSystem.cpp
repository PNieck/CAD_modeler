#include <CAD_modeler/model/systems/gregoryPatchesSystem.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/toUpdateSystem.hpp>

#include <CAD_modeler/model/components/gregoryPatchParameters.hpp>
#include <CAD_modeler/model/components/gregoryNetMesh.hpp>
#include <CAD_modeler/model/components/patchesDensity.hpp>

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <ecs/coordinator.hpp>

#include <graph/alg/cycleDetection.hpp>

#include <unordered_map>
#include <map>
#include <tuple>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <stack>
#include <memory>
#include <set>


using GregOuterPoint = GregoryPatchParameters::OuterPointSpec;
using GregInnerPoint = GregoryPatchParameters::InnerPointsSpec;


void GregoryPatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<GregoryPatchesSystem>();

    // TODO: move it
    coordinator.RegisterComponent<Hole>();

    coordinator.RegisterRequiredComponent<GregoryPatchesSystem, TriangleOfGregoryPatches>();
    coordinator.RegisterRequiredComponent<GregoryPatchesSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<GregoryPatchesSystem, Mesh>();
    coordinator.RegisterRequiredComponent<GregoryPatchesSystem, Hole>();
}


void GregoryPatchesSystem::Init(ShaderRepository *shadersRepo)
{
    this->shaderRepo = shadersRepo;
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
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


// TODO: move to other file
std::tuple<std::vector<Position>, std::vector<Position>> SubdivideBezierCurve(const std::vector<Position>& cps, float t)
{
    assert(cps.size() == 4);

    float oneMinutT = 1.f - t;
    Position tmp[3];

    std::vector<Position> result1(4), result2(4);

    result1[0] = cps[0];
    result2[3] = cps[3];

    tmp[0] = cps[0].vec * oneMinutT + cps[1].vec * t;
    tmp[1] = cps[1].vec * oneMinutT + cps[2].vec * t;
    tmp[2] = cps[2].vec * oneMinutT + cps[3].vec * t;
    result1[1] = tmp[0];
    result2[2] = tmp[2];

    tmp[0] = tmp[0].vec * oneMinutT + tmp[1].vec * t;
    tmp[1] = tmp[1].vec * oneMinutT + tmp[2].vec * t;
    result1[2] = tmp[0];
    result2[1] = tmp[1];

    tmp[0] = tmp[0].vec * oneMinutT + tmp[1].vec * t;
    result1[3] = tmp[0];
    result2[0] = tmp[0];

    return { result1, result2 };
}


// TODO: move to other file
alg::Vec3 DerivativeOfBezierCurve(const std::vector<Position>& cps, float t)
{
    assert(cps.size() == 4);

    constexpr int degree = 3;

    if (t == 0.f) {
        return static_cast<float>(degree) * (cps[1].vec - cps[0].vec);
    }
    else if (t == 1.0f) {
        return static_cast<float>(degree) * (cps[degree].vec - cps[degree - 1].vec);
    }

    // Calculating derivative using de Casteljau subdivision
    float oneMinutT = 1.f - t;
    alg::Vec3 tmp[3];

    tmp[0] = cps[0].vec * oneMinutT + cps[1].vec * t;
    tmp[1] = cps[1].vec * oneMinutT + cps[2].vec * t;
    tmp[2] = cps[2].vec * oneMinutT + cps[3].vec * t;

    tmp[0] = tmp[0] * oneMinutT + tmp[1] * t;
    tmp[1] = tmp[1] * oneMinutT + tmp[2] * t;

    return static_cast<float>(degree) * (tmp[1] - tmp[0]);
}


// TODO: move this function to other file
alg::Vec3 SquareBernsteinPolynomialValue(const alg::Vec3& p0, const alg::Vec3& p1, const alg::Vec3& p2, float u)
{
    float oneMinusU = 1.f - u;

    return oneMinusU * oneMinusU * p0 +
           2.f * u * oneMinusU * p1 +
           u * u * p2;
}


inline float DetOfMat2x2(float _00, float _01, float _10, float _11)
{
    return _00 * _11 - _10 * _01;
}


std::tuple<float, float> SolveSysOfLinEqWith3EqAnd3Unknowns(const alg::Vec3& result, const alg::Vec3& coef1, const alg::Vec3& coef2)
{
    float denominator = DetOfMat2x2(coef1.X(), coef2.X(), coef1.Y(), coef2.Y());
    if (denominator != 0.f)
        return {
            (result.X() * coef2.Y() - coef2.X() * result.Y()) / denominator,
            (result.Y() * coef1.X() - coef1.Y() * result.X()) / denominator
        };

    denominator = DetOfMat2x2(coef1.X(), coef2.X(), coef1.Z(), coef2.Z());
    if (denominator != 0.f)
        return {
            (result.X() * coef2.Z() - coef2.X() * result.Z()) / denominator,
            (result.Z() * coef1.X() - coef1.Z() * result.X()) / denominator
        };

    denominator = DetOfMat2x2(coef1.Y(), coef2.Y(), coef1.Z(), coef2.Z());
    if (denominator != 0.f)
        return {
            (result.Y() * coef2.Z() - coef2.Y() * result.Z()) / denominator,
            (result.Z() * coef1.Y() - coef1.Z() * result.Y()) / denominator
        };

    throw std::runtime_error("Cannot solve system of linear equations");
}


std::tuple<Position, Position> CalculateInnerPoints(
    const std::vector<Position>& curve,
    const alg::Vec3& a0,
    const alg::Vec3& b0,
    const alg::Vec3& a3,
    const alg::Vec3& b3
) {
    assert(curve.size() == 4);

    alg::Vec3 g0 = (a0 + b0) / 2.0f;
    alg::Vec3 g2 = (a3 + b3) / 2.0f;
    alg::Vec3 g1 = (g0 + g2) / 2.0f;

    alg::Vec3 c0 = curve[1].vec - curve[0].vec;
    alg::Vec3 c1 = curve[2].vec - curve[1].vec;
    alg::Vec3 c2 = curve[3].vec - curve[2].vec;

    float k0, h0, k1, h1;
    std::tie(k0, h0) = SolveSysOfLinEqWith3EqAnd3Unknowns(b0, g0, c0);
    assert((k0*g0 + h0*c0 - b0).LengthSquared() < 0.01f);

    std::tie(k1, h1) = SolveSysOfLinEqWith3EqAnd3Unknowns(b3, g2, c2);
    assert((k1*g2 + h1*c2 - b3).LengthSquared() < 0.01f);

    alg::Vec3 d1 = (k0 * 2.f/3.f + k1 * 1.f/3.f) * SquareBernsteinPolynomialValue(g0, g1, g2, 1.f/3.f) +
                   (h0 * 2.f/3.f + h1 * 1.f/3.f) * SquareBernsteinPolynomialValue(c0, c1, c2, 1.f/3.f);

    alg::Vec3 d2 = (k0 * 1.f/3.f + k1 * 2.f/3.f) * SquareBernsteinPolynomialValue(g0, g1, g2, 2.f/3.f) +
                   (h0 * 1.f/3.f + h1 * 2.f/3.f) * SquareBernsteinPolynomialValue(c0, c1, c2, 2.f/3.f);

    return { curve[1].vec + d1, curve[2].vec + d2 };
}


Entity GregoryPatchesSystem::FillHole(const GregoryPatchesSystem::Hole& hole)
{
    Entity entity = coordinator->CreateEntity();
    TriangleOfGregoryPatches triangle;
    std::set<Entity> cps;

    for (int i=0; i < hole.outerCpNb; i++)
        cps.insert(hole.GetOuterControlPoint(i));

    for (int i=0; i < hole.innerCpNb; i++)
        cps.insert(hole.GetInnerControlPoint(i));

    auto handler = std::make_shared<ControlPointMovedHandler>(entity, *coordinator);

    for (Entity cp: cps) {
        HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
        triangle.controlPointsHandlers.insert({cp, cpHandler});
    }

    triangle.deletionHandler = coordinator->Subscribe<TriangleOfGregoryPatches>(entity, deletionHandler);

    coordinator->AddComponent<Hole>(entity, hole);
    coordinator->AddComponent<TriangleOfGregoryPatches>(entity, triangle);
    coordinator->AddComponent<Mesh>(entity, Mesh());
    coordinator->AddComponent<PatchesDensity>(entity, PatchesDensity(5));

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(entity);

    return entity;
}


void GregoryPatchesSystem::ShowControlNet(Entity gregoryPatches)
{
    coordinator->EditComponent<TriangleOfGregoryPatches>(gregoryPatches,
        [this, gregoryPatches] (TriangleOfGregoryPatches& triangle) {
            GregoryNetMesh mesh; 

            mesh.Update(
                GenerateNetVertices(triangle),
                GenerateNetIndices()
            );

            coordinator->AddComponent(gregoryPatches, mesh);

            triangle.hasNet = true;
        }
    );  
}


void GregoryPatchesSystem::HideControlNet(Entity gregoryPatches)
{
    coordinator->DeleteComponent<GregoryNetMesh>(gregoryPatches);

    coordinator->EditComponent<TriangleOfGregoryPatches>(gregoryPatches,
        [] (TriangleOfGregoryPatches& triangle) {
            triangle.hasNet = false;
        }
    );
}


void GregoryPatchesSystem::Render(const alg::Mat4x4 &cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    UpdateEntities();

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetGregoryPatchShader();
    std::stack<Entity> netsToDraw;

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 20);

    for (auto const entity: entities) {
        auto const& triangle = coordinator->GetComponent<TriangleOfGregoryPatches>(entity);
        bool selection = selectionSystem->IsSelected(entity);

        if (triangle.hasNet)
            netsToDraw.push(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        auto const& density = coordinator->GetComponent<PatchesDensity>(entity);
        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }

    if (!netsToDraw.empty())
        RenderNet(netsToDraw, cameraMtx);
}


void GregoryPatchesSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        coordinator->EditComponent<TriangleOfGregoryPatches>(entity,
            [this, entity] (TriangleOfGregoryPatches& triangle) {
                FillPatchesParameters(triangle, entity);
            }
        );

        const auto& triangle = coordinator->GetComponent<TriangleOfGregoryPatches>(entity);
        UpdateMesh(entity, triangle);
    }
}


void GregoryPatchesSystem::UpdateMesh(Entity entity, const TriangleOfGregoryPatches &triangle) const
{
    coordinator->EditComponent<Mesh>(entity,
        [&triangle, this] (Mesh& mesh) {
            mesh.Update(
                GenerateGregoryPatchVertices(triangle),
                GenerateGregoryPatchIndices(triangle)
            );
        }
    );

    if (triangle.hasNet)
        coordinator->EditComponent<GregoryNetMesh>(entity,
            [&triangle, this] (GregoryNetMesh& netMesh) {
                netMesh.Update(
                    GenerateNetVertices(triangle),
                    GenerateNetIndices()
                );
            }
        );
}


void GregoryPatchesSystem::FillPatchesParameters(TriangleOfGregoryPatches& triangle, Entity entity) const
{
    auto const& hole = coordinator->GetComponent<Hole>(entity);

    std::vector<Position> curve1 {
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(0)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(1)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(2)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(3))
    };

    std::vector<Position> curve2 {
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(3)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(4)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(5)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(6))
    };

    std::vector<Position> curve3 {
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(6)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(7)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(8)),
        coordinator->GetComponent<Position>(hole.GetInnerControlPoint(0))
    };

    std::vector<Position> outerCurve1 {
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(0)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(1)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(2)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(3))
    };

    std::vector<Position> outerCurve2 {
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(4)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(5)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(6)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(7))
    };

    std::vector<Position> outerCurve3 {
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(8)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(9)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(10)),
        coordinator->GetComponent<Position>(hole.GetOuterControlPoint(11))
    };

    // Performing subdivision of surfaces
    auto sub1 = SubdivideBezierCurve(curve1, 0.5f);
    auto outerSub1 = SubdivideBezierCurve(outerCurve1, 0.5f);

    auto sub2 = SubdivideBezierCurve(curve2, 0.5f);
    auto outerSub2 = SubdivideBezierCurve(outerCurve2, 0.5f);

    auto sub3 = SubdivideBezierCurve(curve3, 0.5f);
    auto outerSub3 = SubdivideBezierCurve(outerCurve3, 0.5f);

    for (int i=1; i < 4; i++) {
        std::get<0>(outerSub1)[i] = 2.f * std::get<0>(sub1)[i].vec - std::get<0>(outerSub1)[i].vec;
        std::get<0>(outerSub2)[i] = 2.f * std::get<0>(sub2)[i].vec - std::get<0>(outerSub2)[i].vec;
        std::get<0>(outerSub3)[i] = 2.f * std::get<0>(sub3)[i].vec - std::get<0>(outerSub3)[i].vec;
    }

    for (int i = 1; i < 3; i++) {
        std::get<1>(outerSub1)[i] = 2.f * std::get<1>(sub1)[i].vec - std::get<1>(outerSub1)[i].vec;
        std::get<1>(outerSub2)[i] = 2.f * std::get<1>(sub2)[i].vec - std::get<1>(outerSub2)[i].vec;
        std::get<1>(outerSub3)[i] = 2.f * std::get<1>(sub3)[i].vec - std::get<1>(outerSub3)[i].vec;
    }

    const auto& p21 = std::get<0>(outerSub1)[3];
    const auto& p22 = std::get<0>(outerSub2)[3];
    const auto& p23 = std::get<0>(outerSub3)[3];

    const auto& p31 = std::get<0>(sub1)[3];
    const auto& p32 = std::get<0>(sub2)[3];
    const auto& p33 = std::get<0>(sub3)[3];

    Position q1 = (3.f * p21.vec - p31.vec) / 2.0f;
    Position q2 = (3.f * p22.vec - p32.vec) / 2.0f;
    Position q3 = (3.f * p23.vec - p33.vec) / 2.0f;

    Position p = (q1.vec + q2.vec + q3.vec) / 3.0f;

    Position p11 = (2.0f * q1.vec + p.vec) / 3.0f;
    Position p12 = (2.0f * q2.vec + p.vec) / 3.0f;
    Position p13 = (2.0f * q3.vec + p.vec) / 3.0f;

    // Go around the hole
    triangle.patch[0].SetOuterPoint(std::get<0>(sub1)[0], GregOuterPoint::_30);
    triangle.patch[0].SetOuterPoint(std::get<0>(sub1)[1], GregOuterPoint::_31);
    triangle.patch[0].SetOuterPoint(std::get<0>(sub1)[2], GregOuterPoint::_32);
    triangle.patch[0].SetOuterPoint(std::get<0>(sub1)[3], GregOuterPoint::_33);

    triangle.patch[1].SetOuterPoint(std::get<1>(sub1)[0], GregOuterPoint::_30);
    triangle.patch[1].SetOuterPoint(std::get<1>(sub1)[1], GregOuterPoint::_31);
    triangle.patch[1].SetOuterPoint(std::get<1>(sub1)[2], GregOuterPoint::_32);
    triangle.patch[1].SetOuterPoint(std::get<1>(sub1)[3], GregOuterPoint::_33);

    triangle.patch[1].SetOuterPoint(std::get<0>(sub2)[1], GregOuterPoint::_23);
    triangle.patch[1].SetOuterPoint(std::get<0>(sub2)[2], GregOuterPoint::_13);
    triangle.patch[1].SetOuterPoint(std::get<0>(sub2)[3], GregOuterPoint::_03);

    triangle.patch[2].SetOuterPoint(std::get<1>(sub2)[0], GregOuterPoint::_33);
    triangle.patch[2].SetOuterPoint(std::get<1>(sub2)[1], GregOuterPoint::_23);
    triangle.patch[2].SetOuterPoint(std::get<1>(sub2)[2], GregOuterPoint::_13);
    triangle.patch[2].SetOuterPoint(std::get<1>(sub2)[3], GregOuterPoint::_03);

    triangle.patch[2].SetOuterPoint(std::get<0>(sub3)[1], GregOuterPoint::_02);
    triangle.patch[2].SetOuterPoint(std::get<0>(sub3)[2], GregOuterPoint::_01);
    triangle.patch[2].SetOuterPoint(std::get<0>(sub3)[3], GregOuterPoint::_00);

    triangle.patch[0].SetOuterPoint(std::get<1>(sub3)[0], GregOuterPoint::_00);
    triangle.patch[0].SetOuterPoint(std::get<1>(sub3)[1], GregOuterPoint::_10);
    triangle.patch[0].SetOuterPoint(std::get<1>(sub3)[2], GregOuterPoint::_20);
    
    
    // Set inner points around the hole
    triangle.patch[0].SetInnerPoint(std::get<0>(outerSub1)[1], GregInnerPoint::neighbourOf31);
    triangle.patch[0].SetInnerPoint(std::get<0>(outerSub1)[2], GregInnerPoint::neighbourOf32);

    triangle.patch[1].SetInnerPoint(std::get<1>(outerSub1)[1], GregInnerPoint::neighbourOf31);
    triangle.patch[1].SetInnerPoint(std::get<1>(outerSub1)[2], GregInnerPoint::neighbourOf32);

    triangle.patch[1].SetInnerPoint(std::get<0>(outerSub2)[1], GregInnerPoint::neighbourOf23);
    triangle.patch[1].SetInnerPoint(std::get<0>(outerSub2)[2], GregInnerPoint::neighbourOf13);

    triangle.patch[2].SetInnerPoint(std::get<1>(outerSub2)[1], GregInnerPoint::neighbourOf23);
    triangle.patch[2].SetInnerPoint(std::get<1>(outerSub2)[2], GregInnerPoint::neighbourOf13);

    triangle.patch[2].SetInnerPoint(std::get<0>(outerSub3)[1], GregInnerPoint::neighbourOf02);
    triangle.patch[2].SetInnerPoint(std::get<0>(outerSub3)[2], GregInnerPoint::neighbourOf01);

    triangle.patch[0].SetInnerPoint(std::get<1>(outerSub3)[1], GregInnerPoint::neighbourOf10);
    triangle.patch[0].SetInnerPoint(std::get<1>(outerSub3)[2], GregInnerPoint::neighbourOf20);


    // Set the rest of outer points
    triangle.patch[0].SetOuterPoint(p21, GregOuterPoint::_23);
    triangle.patch[0].SetOuterPoint(p11, GregOuterPoint::_13);
    triangle.patch[0].SetOuterPoint(p, GregOuterPoint::_03);

    triangle.patch[1].SetOuterPoint(p21, GregOuterPoint::_20);
    triangle.patch[1].SetOuterPoint(p11, GregOuterPoint::_10);
    triangle.patch[1].SetOuterPoint(p, GregOuterPoint::_00);

    triangle.patch[1].SetOuterPoint(p22, GregOuterPoint::_02);
    triangle.patch[1].SetOuterPoint(p12, GregOuterPoint::_01);

    triangle.patch[2].SetOuterPoint(p22, GregOuterPoint::_32);
    triangle.patch[2].SetOuterPoint(p12, GregOuterPoint::_31);
    triangle.patch[2].SetOuterPoint(p, GregOuterPoint::_30);

    triangle.patch[2].SetOuterPoint(p23, GregOuterPoint::_10);
    triangle.patch[2].SetOuterPoint(p13, GregOuterPoint::_20);

    triangle.patch[0].SetOuterPoint(p23, GregOuterPoint::_01);
    triangle.patch[0].SetOuterPoint(p13, GregOuterPoint::_02);


    // Finish inner points
    alg::Vec3 b0 = std::get<0>(sub1)[2].vec - std::get<0>(sub1)[3].vec;
    alg::Vec3 a0 = b0;

    alg::Vec3 a3 = p.vec - p12.vec;
    alg::Vec3 b3 = p13.vec - p.vec;

    auto innerPts = CalculateInnerPoints({p31, p21, p11, p}, a0, b0, a3, b3);
    triangle.patch[0].SetInnerPoint(std::get<0>(innerPts), GregInnerPoint::neighbourOf23);
    triangle.patch[0].SetInnerPoint(std::get<1>(innerPts), GregInnerPoint::neighbourOf13);

    b0 = std::get<1>(sub1)[1].vec - std::get<1>(sub1)[0].vec;
    a0 = b0;

    b3 = p12.vec - p.vec;
    a3 = p.vec - p13.vec;

    innerPts = CalculateInnerPoints({p31, p21, p11, p}, a0, b0, a3, b3);
    triangle.patch[1].SetInnerPoint(std::get<0>(innerPts), GregInnerPoint::neighbourOf20);
    triangle.patch[1].SetInnerPoint(std::get<1>(innerPts), GregInnerPoint::neighbourOf10);

    b0 = std::get<0>(sub2)[2].vec - std::get<0>(sub2)[3].vec;
    a0 = b0;

    b3 = p11.vec - p.vec;
    a3 = p.vec - p13.vec;

    innerPts = CalculateInnerPoints({p32, p22, p12, p}, a0, b0, a3, b3);
    triangle.patch[1].SetInnerPoint(std::get<0>(innerPts), GregInnerPoint::neighbourOf02);
    triangle.patch[1].SetInnerPoint(std::get<1>(innerPts), GregInnerPoint::neighbourOf01);

    b0 = std::get<1>(sub2)[1].vec - std::get<1>(sub2)[0].vec;
    a0 = b0;

    b3 = p13.vec - p.vec;
    a3 = p.vec - p11.vec;

    innerPts = CalculateInnerPoints({p32, p22, p12, p}, a0, b0, a3, b3);
    triangle.patch[2].SetInnerPoint(std::get<0>(innerPts), GregInnerPoint::neighbourOf32);
    triangle.patch[2].SetInnerPoint(std::get<1>(innerPts), GregInnerPoint::neighbourOf31);

    b0 = std::get<0>(sub3)[2].vec - std::get<0>(sub3)[3].vec;
    a0 = b0;

    b3 = p12.vec - p.vec;
    a3 = p.vec - p11.vec;

    innerPts = CalculateInnerPoints({p33, p23, p13, p}, a0, b0, a3, b3);
    triangle.patch[2].SetInnerPoint(std::get<0>(innerPts), GregInnerPoint::neighbourOf10);
    triangle.patch[2].SetInnerPoint(std::get<1>(innerPts), GregInnerPoint::neighbourOf20);

    b0 = std::get<1>(sub3)[1].vec - std::get<1>(sub3)[0].vec;
    a0 = b0;

    b3 = p11.vec - p.vec;
    a3 = p.vec - p12.vec;

    innerPts = CalculateInnerPoints({p33, p23, p13, p}, a0, b0, a3, b3);
    triangle.patch[0].SetInnerPoint(std::get<0>(innerPts), GregInnerPoint::neighbourOf01);
    triangle.patch[0].SetInnerPoint(std::get<1>(innerPts), GregInnerPoint::neighbourOf02);
}


void GregoryPatchesSystem::RenderNet(std::stack<Entity>& cps, const alg::Mat4x4 &cameraMtx) const
{
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    while (!cps.empty()) {
        Entity entity = cps.top();
        cps.pop();

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<GregoryNetMesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void AddPosition(std::vector<float>& vec, const Position& pos)
{
    vec.push_back(pos.GetX());
    vec.push_back(pos.GetY());
    vec.push_back(pos.GetZ());
}


void AddPatchToVertices(std::vector<float>& result, const GregoryPatchParameters& params)
{
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_00));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_01));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_02));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_03));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_13));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_23));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_33));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_32));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_31));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_30));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_20));
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_10));


    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf01));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf02));

    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf13));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf23));

    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf32));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf31));

    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf20));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf10));
}


std::vector<float> GregoryPatchesSystem::GenerateGregoryPatchVertices(const TriangleOfGregoryPatches& triangle) const
{
    std::vector<float> result;
    constexpr int size = (GregoryPatchParameters::InnerPointsNb + GregoryPatchParameters::OuterPointsNb) * triangle.ParamsCnt * 3;
    result.reserve(size);

    for (int i=0; i < triangle.ParamsCnt; i++)
        AddPatchToVertices(result, triangle.patch[i]);

    assert(result.size() == size);

    return result;
}


void AddPatchToIndices(std::vector<uint32_t>& result,const GregoryPatchParameters& params, int startIdx)
{
    for (int i=0; i < params.InnerPointsNb + params.OuterPointsNb; i++)
        result.push_back(startIdx + i);

    for (int i=3; i < params.OuterPointsNb; i++)
        result.push_back(startIdx + i);

    result.push_back(startIdx + 0);
    result.push_back(startIdx + 1);
    result.push_back(startIdx + 2);

    for (int i=14; i < params.InnerPointsNb + params.OuterPointsNb; i++)
        result.push_back(startIdx + i);

    result.push_back(startIdx + 12);
    result.push_back(startIdx + 13);
}


std::vector<uint32_t> GregoryPatchesSystem::GenerateGregoryPatchIndices(const TriangleOfGregoryPatches& triangle) const
{
    std::vector<uint32_t> result;
    constexpr int size = (GregoryPatchParameters::InnerPointsNb + GregoryPatchParameters::OuterPointsNb) * triangle.ParamsCnt * 2;
    result.reserve(size);

    AddPatchToIndices(result, triangle.patch[0], 0);
    AddPatchToIndices(result, triangle.patch[1], 20);
    AddPatchToIndices(result, triangle.patch[1], 40);

    assert(result.size() == size);

    return result;
}


void AddSingleGregoryPatchToNetVertices(std::vector<float>& result, const GregoryPatchParameters& params)
{
    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_00));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_01));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf01));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_02));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf02));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_03));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_13));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf13));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_23));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf23));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_33));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_32));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf32));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_31));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf31));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_30));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_20));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf20));

    AddPosition(result, params.GetOuterPoint(GregOuterPoint::_10));
    AddPosition(result, params.GetInnerPoint(GregInnerPoint::neighbourOf10));
}


std::vector<float> GregoryPatchesSystem::GenerateNetVertices(const TriangleOfGregoryPatches& triangles) const
{
    std::vector<float> result;
    constexpr int size = (2 * GregoryPatchParameters::InnerPointsNb + GregoryPatchParameters::OuterPointsNb) * 3 * triangles.ParamsCnt;
    result.reserve(size);

    for (int i=0; i < triangles.ParamsCnt; i++)
        AddSingleGregoryPatchToNetVertices(result, triangles.patch[i]);

    // TODO: FIX ASSERT
    //assert(result.size() == size);

    return result;
}


void AddSingleGregoryPatchToNetIndices(std::vector<uint32_t>& result, int startIdx)
{
    std::vector<uint32_t> newIndices {
        0, 
        1, 2, 1,
        3, 4, 3,
        5,
        6, 7, 6,
        8, 9, 8,
        10, 
        11, 12, 11,
        13, 14, 13,
        15,
        16, 17, 16,
        18, 19, 18,
        0
    };

    if (startIdx != 0)
        std::for_each(newIndices.begin(), newIndices.end(), [startIdx] (uint32_t& index) { index += startIdx; });

    for (auto idx: newIndices)
        result.push_back(idx);
}


std::vector<uint32_t> GregoryPatchesSystem::GenerateNetIndices() const
{
    std::vector<uint32_t> result;

    AddSingleGregoryPatchToNetIndices(result, 0);
    result.push_back(std::numeric_limits<uint32_t>::max());
    AddSingleGregoryPatchToNetIndices(result, 20);
    result.push_back(std::numeric_limits<uint32_t>::max());
    AddSingleGregoryPatchToNetIndices(result, 40);

    return result; 
}


void GregoryPatchesSystem::DeletionHandler::HandleEvent(Entity entity, const TriangleOfGregoryPatches &component, EventType eventType)
{
    (void)entity;

    if (eventType != EventType::ComponentDeleted)
        return;

    for (auto handler: component.controlPointsHandlers) {
        Entity cp = handler.first;
        HandlerId handlerId = handler.second;

        coordinator.Unsubscribe<Position>(cp, handlerId);
    }
}


void GregoryPatchesSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    (void)entity;
    (void)component;

    if (eventType == EventType::ComponentDeleted) {
        coordinator.DestroyEntity(targetObject);
        return;
    }

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}
