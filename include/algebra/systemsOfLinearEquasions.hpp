#pragma once

#include <vector>
#include <cassert>
#include <optional>


namespace alg
{
    template <typename ScalarType, typename VectorType>
    std::optional<std::vector<VectorType>> SolveSystemOfLinearEquationsWithTridiagonalMtx(
        std::vector<ScalarType>& superdiagonalElems,
        std::vector<ScalarType>& diagonalElems,
        std::vector<ScalarType>& subdiagonalElems,
        std::vector<VectorType>& resultsElems
    ) {
        assert(superdiagonalElems.size() == diagonalElems.size() - 1);
        assert(diagonalElems.size() == subdiagonalElems.size() + 1);
        assert(diagonalElems.size() == resultsElems.size());

        // Thomas algorithm

        auto size = diagonalElems.size();
        std::vector<VectorType> result(size);

        for (int i=0; i < size - 1; ++i) {
            if (diagonalElems[i] == 0)
                return std::nullopt;

            float coeff = subdiagonalElems[i]/diagonalElems[i];
            diagonalElems[i+1] -= coeff * superdiagonalElems[i];
            resultsElems[i+1] -= coeff * resultsElems[i];
        }

        result[size - 1] = resultsElems[size - 1] / diagonalElems[size - 1];

        for (int i = size - 2; i >= 0; --i) {
            if (diagonalElems[i] == 0)
                return std::nullopt;

            result[i] = (resultsElems[i] - superdiagonalElems[i] * result[i+1]) / diagonalElems[i];
        }

        return result;
    }

}
