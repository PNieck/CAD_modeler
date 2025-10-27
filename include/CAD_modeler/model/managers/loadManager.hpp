#pragma once

#include "ecs/coordinator.hpp"

#include <nlohmann/json.hpp>


class LoadManager {
public:
    enum ToLoad {
        Points = 1 << 0,
        Tori = 1 << 1,
        C0Curves = 1 << 2,
        C2Curves = 1 << 3,
        InterpolationCurves = 1 << 4,
        C0Surfaces = 1 << 5,
        C2Surfaces = 1 << 6,

        AllCurves = C0Curves | C2Curves | InterpolationCurves,
        AllSurfaces = Tori | C0Surfaces | C2Surfaces,

        All = Points | AllCurves | AllSurfaces
    };

    void Load(const std::string& path, Coordinator& coordinator, ToLoad options = All);

private:
    using json = nlohmann::json;
    using IdFromFile = int;

    std::unordered_map<IdFromFile, Entity> pointsIdsMap;



    class PointsToParse {
    public:
        PointsToParse(json& data, ToLoad options);

        bool ParseAll() const
            { return parseAll; }

        bool ShouldParse(IdFromFile id) const;

    private:
        bool parseAll = false;
        std::unordered_set<IdFromFile> idsToParse;

        bool InsertRange(auto begin, auto end, int pointsCnt);
    };

    void LoadPoints(json& data, Coordinator& coordinator, const PointsToParse& pointsToParse);
    void LoadTori(json& data, Coordinator& coordinator);
    void LoadC0Curves(json& data, Coordinator& coordinator);
    void LoadC2Curves(json& data, Coordinator& coordinator);
    void LoadInterpolationCurves(json& data, Coordinator& coordinator);
    void LoadC0Patches(json& data, Coordinator& coordinator);
    void LoadC2Patches(json& data, Coordinator& coordinator);

    std::vector<Entity> LoadControlPoints(json& data);
};
