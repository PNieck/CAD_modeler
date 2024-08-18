#include <CAD_modeler/utilities/hashCombine.hpp>


size_t stdh::hashCombine(size_t lhs, size_t rhs)
{
    // source: boost::hash_combine

    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}
