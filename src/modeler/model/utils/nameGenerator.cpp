#include <CAD_modeler/model/utils/nameGenerator.hpp>


std::string NameGenerator::GenerateName(const std::string &prefix)
{
    auto it = prefixIds.find(prefix);
    if (it != prefixIds.end())
        return prefix + std::to_string(++(it->second));

    prefixIds.insert({prefix, 1});

    return prefix + "1";
}
