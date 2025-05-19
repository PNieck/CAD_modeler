#pragma once

#include <algebra/vec2.hpp>

#include <memory>


bool IsSteepLine(const alg::IVec2& p1, const alg::IVec2& p2);


class LineDrawer {
public:
    LineDrawer(const alg::IVec2& p1, const alg::IVec2& p2);
    virtual ~LineDrawer() = default;

    virtual bool NextPoint() = 0;

    [[nodiscard]]
    const alg::IVec2& ActualPoint() const
        { return actualPoint; }

protected:
    alg::IVec2 endPoint;
    alg::IVec2 actualPoint;

    alg::IVec2 delta;

private:
    static alg::IVec2 EndPoint(const alg::IVec2& p1, const alg::IVec2& p2);
};


std::unique_ptr<LineDrawer> GetLineDrawer(const alg::IVec2& p1, const alg::IVec2& p2);
