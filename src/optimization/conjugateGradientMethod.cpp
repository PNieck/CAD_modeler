#include <optimization/conjugateGradientMethod.hpp>

#include <iostream>


float LengthSquared(const std::vector<float>& vec)
{
    float result = 0.f;

    for (const float value: vec) {
        result += value * value;
    }

    return result;
}


std::optional<std::vector<float>> opt::ConjugateGradientMethod(
    FunctionToOptimize& fun, LineSearchMethod& lineSearch, const std::vector<float> &initSol, const float eps, const unsigned int maxIt)
{
    std::vector<float> solution(initSol);

    // At first search direction is equal to gradient with minus sign
    std::vector<float> searchDir = fun.Gradient(solution);
    for (float & i : searchDir)
        i = -i;

    for (unsigned int it = 0; it < maxIt; it++) {
        std::cout << "It " << it << " conjugate\n";

        if (fun.Value(solution) < eps)
            return solution;

        const float step = lineSearch.Search(fun, solution, searchDir);

        std::vector<float> oldSolution = solution;

        // Update solution
        for (int i=0; i < solution.size(); i++)
            solution[i] += step * searchDir[i];

        // Update search direction
        
        std::vector<float> oldGradient = fun.Gradient(oldSolution);
        std::vector<float> newGradient = fun.Gradient(solution);

        const float beta = LengthSquared(newGradient) / LengthSquared(oldGradient);

        for (int i=0; i < searchDir.size(); i++)
            searchDir[i] = -newGradient[i] + beta * searchDir[i];
        
    }

    return std::nullopt;
}
