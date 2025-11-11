#pragma  once

#include "model.hpp"

#include "systems/gridSystem.hpp"
#include "systems/pointsSystem.hpp"
#include "systems/c0PatchesSystem.hpp"
#include "systems/c0PatchesRenderSystem.hpp"
#include "systems/c2PatchesSystem.hpp"
#include "systems/c2PatchesRenderSystem.hpp"
#include "systems/nameSystem.hpp"

#include "millingPathsDesigner/materialParameters.hpp"
#include "millingPathsDesigner/millingSettings.hpp"

#include <CAD_modeler/utilities/flatVec2D.hpp>


class MillingPathsDesigner: public Model {
public:
    MillingPathsDesigner(int viewportWidth, int viewportHeight);

    void LoadModel(const std::string& filePath);

    void Update();

    void GenerateBroadPhase();

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

    std::shared_ptr<NameSystem> nameSystem;

    Entity base = -1;
    MaterialParameters materialParameters;
    MillingSettings millingSettings;

    FlatVec2D<float> GenerateBroadPhaseHeightMap();
};
