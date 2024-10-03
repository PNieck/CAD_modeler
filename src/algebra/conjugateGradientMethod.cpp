#include <algebra/conjugateGradientMethod.hpp>

#include <cmath>
#include <cassert>
#include <functional>


float LengthSquared(const std::vector<float>& vec)
{
    float result = 0.f;

    for (const float value: vec) {
        result += value * value;
    }

    return result;
}


float DotProduct(const std::vector<float>& vec1, const std::vector<float>& vec2)
{
    assert(vec1.size() == vec2.size());

    float result = 0.f;

    for (int i=0; i < vec1.size(); i++) {
        result += vec1[i] * vec2[i];
    }

    return result;
}


std::vector<float> FindMinimumInOneDirection2(
    alg::FunctionToOptimize &function, const std::vector<float> &initSol, const std::vector<float>& dir, float initStep) {

    float eps = 0.000001;

    std::vector<float> oldSol(initSol.begin(), initSol.end());
    std::vector<float> newSol(initSol.begin(), initSol.end());

    float step = initStep;

    do {
        //std::swap(oldSol, newSol);
        std::vector<float> tmp = oldSol;
        oldSol = newSol;
        newSol = tmp;

        for (int i=0; i < initSol.size(); i++)
            newSol[i] = oldSol[i] + step * dir[i];
    } while (function.Value(newSol) < function.Value(oldSol));

    step /= 2.f;

    while (step > eps) {
        for (int i=0; i < initSol.size(); i++)
            newSol[i] = oldSol[i] + step * dir[i];

        if (function.Value(newSol) < function.Value(oldSol)) {
            std::vector<float> tmp = oldSol;
            oldSol = newSol;
            newSol = tmp;
        }

        step /= 2.f;
    }

    return oldSol;
}

std::vector<float> FindMinimumInOneDirection(
    alg::FunctionToOptimize &function, const std::vector<float> &initSol, const std::vector<float>& dir, float initStep)
{
    float n = 0.1f;
    float beta = 0.2f;
    std::vector<float> solution(initSol.size());

    float dot = DotProduct(function.Gradient(initSol), dir);

    for (int i=0; ; i++) {
        float coef = std::powf(beta, i) * initStep;

        for (int j=0; j<initSol.size(); j++) {
            solution[j] = initSol[j] + coef * dir[j];
        }

        if (function.Value(solution) - function.Value(initSol) <= n * coef * dot)
            return solution;
    }
}


#include <iostream>

std::optional<std::vector<float>> alg::ConjugationGradientMethod(
    FunctionToOptimize &functionToOptimise, const std::vector<float> &solutionEstimation, float step, float eps)
{
    std::vector<float> oldSolution(solutionEstimation.begin(), solutionEstimation.end());
    std::vector<float> searchDir(solutionEstimation.size());

    // At first search direction is equal to gradient with minus sign
    std::vector<float> gradient = functionToOptimise.Gradient(solutionEstimation);
    for (int i=0; i < solutionEstimation.size(); i++)
        searchDir[i] = -gradient[i];

    while (true) {
        std::vector<float> newSolution = FindMinimumInOneDirection2(functionToOptimise, oldSolution, searchDir, step);

        float v = LengthSquared(functionToOptimise.Gradient(newSolution));
        float funVal = functionToOptimise.Value(newSolution);
        std::cout << "Grad len sq: " << v << " function value: " << funVal << std::endl;
        if (v < eps)
            return newSolution;

        float oldSolLen = LengthSquared(oldSolution);
        float newSolLen = LengthSquared(newSolution);
        float coef = newSolLen / oldSolLen;

        gradient = functionToOptimise.Gradient(newSolution);

        for (int j=0; j < solutionEstimation.size(); j++)
            searchDir[j] = -gradient[j] + coef * searchDir[j];

        //std::swap(newSolution, oldSolution);
        std::vector<float> tmp = oldSolution;
        oldSolution = newSolution;
        newSolution = tmp;
    }
}
