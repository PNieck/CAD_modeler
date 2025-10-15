#include <optimization/conjugateGradientMethod.hpp>

#include <optimization/utils.hpp>

#include <iostream>


std::optional<std::vector<float>> opt::ConjugateGradientMethod(
    FunctionToOptimize& fun,
    LineSearchMethod& lineSearch,
    const std::vector<float> &initSol,
    const unsigned int maxIt,
    StopCondition& stopCondition
) {
    std::vector solution(initSol);

    // At first a search direction is equal to gradient with minus sign
    std::vector<float> searchDir = fun.Gradient(solution);
    for (float & i : searchDir)
        i = -i;

    for (unsigned int it = 0; it < maxIt; it++) {
        std::cout << "It " << it << " conjugate\n";

        if (stopCondition.ShouldStop(fun, solution))
            return solution;

        const float step = lineSearch.Search(fun, solution, searchDir);

        std::vector<float> oldSolution = solution;

        // Update solution
        for (size_t i=0; i < solution.size(); i++)
            solution[i] += step * searchDir[i];

        // Update a search direction
        
        std::vector<float> oldGradient = fun.Gradient(oldSolution);
        std::vector<float> newGradient = fun.Gradient(solution);

        const float beta = LengthSquared(newGradient) / LengthSquared(oldGradient);

        for (size_t i=0; i < searchDir.size(); i++)
            searchDir[i] = -newGradient[i] + beta * searchDir[i];
        
    }

    return std::nullopt;
}
