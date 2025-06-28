#pragma once


#include "../../components/intersectionCurve.hpp"

#include "surface.hpp"


namespace interSys
{
    inline bool PointInDomain(Surface& s, const float u, const float v) {
        return s.MinU() <= u && s.MaxU() >= u && s.MinV() <= v && s.MaxV() >= v;
    }


    inline bool PointInDomains(Surface& s1, Surface& s2, const IntersectionPoint& p) {
        return PointInDomain(s1, p.U1(), p.V1()) && PointInDomain(s2, p.U2(), p.V2());
    }
}
