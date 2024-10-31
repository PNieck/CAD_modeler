#pragma once

#include <map>


class MillingWarningsRepo {
public:
    enum WarningType {
        MillingUnderTheBase = 1 << 0,
        MillingTooDeep      = 1 << 1,
        MillingStraightDown = 1 << 2
    };

    void AddWarning(int commandId, WarningType warningType);

    void Clear()
        { warnings.clear(); }

    [[nodiscard]]
    const auto& GetWarnings() const
        { return warnings; }

    [[nodiscard]]
    bool Empty() const
        { return warnings.empty(); }

private:
    std::map<int, int> warnings;
};