#pragma once

#include "../components/c0Patches.hpp"
#include "../components/gregoryPatchParameters.hpp"

#include "utils/nameGenerator.hpp"
#include "shaders/shaderRepository.hpp"

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include <vector>


class GregoryPatchesSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    class Hole {
    public:
        static const int innerCpNb = 9;
        static const int outerCpNb = 12;

        inline Entity GetInnerControlPoint(int index) const
            { return innerCp[index]; }

        inline Entity GetOuterControlPoint(int index) const
            { return outerCp[index]; }

    private:
        Entity innerCp[innerCpNb];
        Entity outerCp[outerCpNb];

        friend class GregoryPatchesSystem;
    };

    std::vector<Hole> FindHolesToFill(const std::vector<C0Patches>& patches) const;

    void FillHole(const Hole& hole);

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    void RenderNet(const alg::Mat4x4& cameraMtx) const;

    std::vector<float> GenerateGregoryPatchVertices(const GregoryPatchParameters& params) const;
    std::vector<uint32_t> GenerateGregoryPatchIndices(const GregoryPatchParameters& params) const;

    std::vector<float> GenerateNetVertices(const GregoryPatchParameters& params) const;
    std::vector<uint32_t> GenerateNetIndices(const GregoryPatchParameters& params) const;
};
