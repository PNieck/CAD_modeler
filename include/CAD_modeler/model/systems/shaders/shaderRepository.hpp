#pragma once

#include "stdShader.hpp"
#include "gridShader.hpp"


class ShaderRepository {
public:
    inline const StdShader& GetStdShader() const
        { return stdShader; }

    inline const GridShader& GetGridShader() const
        { return gridShader; }

private:
    StdShader stdShader;
    GridShader gridShader;
};
