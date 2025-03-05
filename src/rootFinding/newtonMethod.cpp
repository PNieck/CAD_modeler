#include <rootFinding/newtonMethod.hpp>

#include <iostream>


std::optional<alg::Vec4> root::NewtonMethod(FunctionToFindRoot &fun, const alg::Vec4 &initSol, float eps, int maxIter) {
    alg::Vec4 newSol = initSol;
    int i = 0;

    do {
        if (++i > maxIter)
            return std::nullopt;

        alg::Vec4 oldSol = newSol;

        auto jacInv = fun.Jacobian(oldSol).Inverse();
        if (!jacInv.has_value())
            return std::nullopt;

        newSol = oldSol - fun.Value(oldSol) * jacInv.value();

        std::cout << i << " Function value: " << fun.Value(newSol) << " len sq: " << fun.Value(newSol).LengthSquared() << std::endl;
    } while (fun.Value(newSol).LengthSquared() > eps);

    return newSol;
}
