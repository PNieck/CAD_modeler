#include <CAD_modeler/utilities/lineDrawer.hpp>


bool IsSteepLine(const alg::IVec2 &p1, const alg::IVec2 &p2)
{
    return std::abs(p1.Y() - p2.Y()) > std::abs(p1.X() - p2.X());
}


LineDrawer::LineDrawer(const alg::IVec2 &p1, const alg::IVec2 &p2):
    endPoint(EndPoint(p1, p2)),
    actualPoint(p1 == endPoint ? p2 : p1),
    delta(endPoint - actualPoint)
{}


class SteepLineDrawer final : public LineDrawer {
public:
    SteepLineDrawer(const alg::IVec2 &p1, const alg::IVec2 &p2):
        LineDrawer(p1, p2), xi(1), d()
    {
        if (delta.X() < 0) {
            xi = -1;
            delta.X() = -delta.X();
        }

        d = 2*(delta.X() - delta.Y());
    }

    bool NextPoint() override {
        if (actualPoint.Y() == endPoint.Y())
            return false;

        actualPoint.Y() += 1;

        if (d > 0) {
            actualPoint.X() += xi;
            d += 2*(delta.X() - delta.Y());
        }
        else
            d += 2*delta.X();

        return true;
    }

private:
    int xi;
    int d;
};


class GentleLineDrawer final : public LineDrawer {
public:
    GentleLineDrawer(const alg::IVec2 &p1, const alg::IVec2 &p2):
        LineDrawer(p1, p2), yi(1), d()
    {
        if (delta.Y() < 0) {
            yi = -1;
            delta.Y() = -delta.Y();
        }

        d = 2*delta.Y() - delta.X();
    }

    bool NextPoint() override {
        if (actualPoint.X() == endPoint.X())
            return false;

        actualPoint.X() += 1;

        if (d > 0) {
            actualPoint.Y() += yi;
            d += 2*(delta.Y() - delta.X());
        }
        else
            d += 2*delta.Y();

        return true;
    }

private:
    int yi;
    int d;
};


alg::IVec2 LineDrawer::EndPoint(const alg::IVec2 &p1, const alg::IVec2 &p2) {
    if (IsSteepLine(p1, p2)) {
        if (p1.Y() > p2.Y())
            return p1;

        return p2;
    }

    if (p1.X() > p2.X())
        return p1;

    return p2;

}


std::unique_ptr<LineDrawer> GetLineDrawer(const alg::IVec2 &p1, const alg::IVec2 &p2)
{
    if (IsSteepLine(p1, p2)) {
        return std::make_unique<SteepLineDrawer>(p1, p2);
    }

    return std::make_unique<GentleLineDrawer>(p1, p2);
}
