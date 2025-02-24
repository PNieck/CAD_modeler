#pragma once

#include <string>
#include <unordered_map>


class NameGenerator {
public:
    std::string GenerateName(const std::string& prefix);

private:
    std::unordered_map<std::string, unsigned int> prefixIds;
};
