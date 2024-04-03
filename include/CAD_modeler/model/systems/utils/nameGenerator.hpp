#pragma once

#include <string>


class NameGenerator {
public:
    inline std::string GenerateName(const std::string& prefix)
        { return prefix + std::to_string(nextId++); }

private:
    unsigned int nextId = 1;
};
