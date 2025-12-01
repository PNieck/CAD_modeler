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
#include "systems/equidistanceC0SurfaceSystem.hpp"
#include "systems/polylineSystem.hpp"

#include "components/millingCutter.hpp"

#include "millingPathsDesigner/materialParameters.hpp"
#include "millingPathsDesigner/millingSettings.hpp"
#include "millingPathsDesigner/modelHeightMap.hpp"
#include "millingPathsDesigner/millingMachinePathsBuilder.hpp"

#include "../utilities/circularVector.hpp"

#include <algebra/vec2.hpp>


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

    std::shared_ptr<EquidistanceC2SurfaceSystem> equidistanceC2System;
    std::shared_ptr<EquidistanceC0SurfaceSystem> equidistanceC0System;

    std::shared_ptr<NameSystem> nameSystem;

    std::shared_ptr<IntersectionSystem> intersectionSystem;
    std::shared_ptr<InterpolationCurvesRenderingSystem> interpolationCurvesRendering;

    std::shared_ptr<PolylineSystem> polylineSystem;

    Entity base = -1;
    MaterialParameters materialParameters;
    MillingSettings millingSettings;

    ModelHeightMap GenerateHeightMap(size_t xResolution, size_t zResolution);
    ModelHeightMap GenerateBroadPhaseHeightMap();

    static float MinYCutterPos(const ModelHeightMap& heightMap, const MillingCutter& cutter, float cutterX, float cutterZ);

    std::vector<Position> FindModelBoundary(float dist);
    CircularVector<Position> ModelBoundaryPoints(Entity entity, float dist);
    Position ModelBoundaryPoint(const IntersectionPoint& p, const C2Patches& patches, float dist) const;

    void GeneratePathsForLeftFin(MillingMachinePathsBuilder& builder, const MillingCutter& cutter);
    void GeneratePathsForRightFin(MillingMachinePathsBuilder& builder, const MillingCutter& cutter);
    void GeneratePathsForTorso(MillingMachinePathsBuilder& builder, const MillingCutter& cutter);
    void GeneratePathsForLeftEye(MillingMachinePathsBuilder& builder, const MillingCutter& cutter);
    void GeneratePathsForRightEye(MillingMachinePathsBuilder& builder, const MillingCutter& cutter);
    void GeneratePathsForUpperFin(MillingMachinePathsBuilder& builder, const MillingCutter& cutter);

    std::vector<alg::Vec2> GetBoundaryCurveForLeftFin(const MillingCutter& cutter, Entity torsoOffset, Entity leftFinOffset);
    std::vector<alg::Vec2> GetBoundaryCurveForRightFin(const MillingCutter& cutter, Entity torsoOffset, Entity rightFinOffset);
    std::vector<alg::Vec2> GetBoundaryCurveForTorso(const MillingCutter& cutter, const std::unordered_map<std::string, Entity>& offsetSurfaces);
    std::vector<alg::Vec2> GetBoundaryCurveForTorsoUpperFinIntersection(const MillingCutter& cutter, const std::unordered_map<std::string, Entity>& offsetSurfaces);
    std::vector<alg::Vec2> GetBoundaryCurveForLeftEye(const MillingCutter& cutter, Entity torsoOffset, Entity leftEyeOffset);
    std::vector<alg::Vec2> GetBoundaryCurveForRightEye(const MillingCutter& cutter, Entity torsoOffset, Entity rightEyeOffset);
    std::vector<alg::Vec2> GetBoundaryCurveForUpperFin(const MillingCutter& cutter, Entity torsoOffset, Entity upperFinOffset, Entity upperFin);

    std::vector<std::vector<alg::Vec2>> GetTorsoHoles(const MillingCutter& cutter, const std::unordered_map<std::string, Entity>& offsetSurfaces);

    static std::vector<alg::Vec2> GetPointsVec(const IntersectionCurve& curve);
    void NormalizeUV(Entity entity, float& u, float& v) const;
    void InterCurveToFileNormalized(const std::string& fileName, const IntersectionCurve& curve, Entity e);

    static std::vector<alg::Vec2> ConnectInsidePointToBoundary(const std::vector<alg::Vec2>& insidePoints, const std::vector<alg::Vec2>& boundary);
    static std::vector<alg::Vec2> BoundaryPointsFromInternalPoint(const alg::Vec2& lastPoint, const std::vector<alg::Vec2>& boundary);

    Position GlobalPosition(Entity entity, const alg::Vec2& paramPoint, const MillingCutter& cutter) const;

    void AddPointsToBuilder(const std::vector<alg::Vec2>& points, MillingMachinePathsBuilder& builder, const MillingCutter& cutter, Entity entity, size_t start, size_t end) const;
    void AddPointsToBuilder(const std::vector<alg::Vec2>& points, MillingMachinePathsBuilder& builder, const MillingCutter& cutter, Entity entity, const std::vector<size_t>& indices) const;
};
