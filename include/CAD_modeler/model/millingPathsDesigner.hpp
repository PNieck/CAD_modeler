#pragma  once

#include "model.hpp"

#include "systems/gridSystem.hpp"
#include "systems/pointsSystem.hpp"
#include "systems/c0PatchesSystem.hpp"
#include "systems/c0PatchesRenderSystem.hpp"
#include "systems/c2PatchesSystem.hpp"
#include "systems/c2PatchesRenderSystem.hpp"
#include "systems/nameSystem.hpp"
#include "systems/intersectionsSystem.hpp"
#include "systems/interpolationCurvesRenderingSystem.hpp"
#include "systems/equidistanceC2SurfaceSystem.hpp"
#include "systems/polylineSystem.hpp"

#include "components/millingCutter.hpp"

#include "millingPathsDesigner/materialParameters.hpp"
#include "millingPathsDesigner/millingSettings.hpp"
#include "millingPathsDesigner/broadPhaseHeightMap.hpp"

#include "../utilities/circularVector.hpp"


class MillingPathsDesigner: public Model {
public:
    MillingPathsDesigner(int viewportWidth, int viewportHeight);

    void LoadModel(const std::string& filePath);

    void Update();

    void GenerateBroadPhase();

    void GenerateBasePhase();

    void GenerateMainPhase();

protected:
    void RenderSystemsObjects(
        const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, float nearPlane, float farPlane
    ) const override;

private:
    std::shared_ptr<GridSystem> gridSystem;

    std::shared_ptr<PointsSystem> pointsSystem;

    std::shared_ptr<C0PatchesSystem> c0PatchesSystem;
    std::shared_ptr<C0PatchesRenderSystem> c0PatchesRenderSystem;

    std::shared_ptr<C2PatchesSystem> c2PatchesSystem;
    std::shared_ptr<C2PatchesRenderSystem> c2PatchesRenderSystem;

    std::shared_ptr<EquidistanceC2System> equidistanceC2System;

    std::shared_ptr<NameSystem> nameSystem;

    std::shared_ptr<IntersectionSystem> intersectionSystem;
    std::shared_ptr<InterpolationCurvesRenderingSystem> interpolationCurvesRendering;

    std::shared_ptr<PolylineSystem> polylineSystem;

    Entity base = -1;
    MaterialParameters materialParameters;
    MillingSettings millingSettings;

    BroadPhaseHeightMap GenerateBroadPhaseHeightMap();
    float MinYCutterPos(const BroadPhaseHeightMap& heightMap, const MillingCutter& cutter, float cutterX, float cutterZ) const;

    std::vector<Position> FindBoundary(const MillingCutter& cutter);

    CircularVector<Position> BoundaryPoints(Entity entity, float dist);

    Position BoundaryPoint(const IntersectionPoint& p, const C2Patches& patches, float dist) const;
};
